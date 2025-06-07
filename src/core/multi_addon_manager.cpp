#include "multi_addon_manager.hpp"

#include <netmessages.h>
#include <networkbasetypes.pb.h>
#include <networksystem/inetworkserializer.h>
#include <networksystem/inetworkmessages.h>
#include <igameeventsystem.h>
#include <serversideclient.h>
#include <filesystem.h>
#include <gameevents.pb.h>
#include <steam/steam_gameserver.h>
#include <iserver.h>
#include <hoststate.h>

#include "core_config.hpp"
#include "game_config.hpp"
#include "sdk/utils.h"

CConVar<bool> s2_addon_mount_download("s2_addon_mount_download", FCVAR_NONE, "Whether to download an addon upon mounting even if it's installed", false);
CConVar<bool> s2_block_disconnect_messages("s2_block_disconnect_messages", FCVAR_NONE, "Whether to block \"loop shutdown\" disconnect messages", false);
CConVar<bool> s2_cache_clients_with_addons("s2_cache_clients_with_addons", FCVAR_NONE, "Whether to cache clients addon download list, this will prevent reconnects on mapchange/rejoin", false);
CConVar<double> s2_cache_clients_duration("s2_cache_clients_duration", FCVAR_NONE, "How long to cache clients' downloaded addons list in seconds, pass 0 for forever.", 0.0);
CConVar<double> s2_extra_addons_timeout("s2_extra_addons_timeout", FCVAR_NONE, "How long until clients are timed out in between connects for extra addons in seconds, requires s2_extra_addons to be used", 10.0);

struct ClientAddonInfo {
	uint64 steamID64;
	double lastActiveTime;
	std::vector<uint64_t> addonsToLoad;
	std::vector<uint64_t> downloadedAddons;
	uint64_t currentPendingAddon;
};

std::unordered_map<uint64, ClientAddonInfo> g_ClientAddons;

CConVar<CUtlString> s2_extra_addons("s2_extra_addons", FCVAR_NONE, "The workshop IDs of extra addons separated by commas, addons will be downloaded (if not present) and mounted", CUtlString(""),
	[](CConVar<CUtlString> *cvar, CSplitScreenSlot slot, const CUtlString *new_val, const CUtlString *old_val)
	{
		g_MultiAddonManager.m_extraAddons = utils::string_to_vector<uint64_t>(new_val->Get());
		g_MultiAddonManager.RefreshAddons();
	});


CConVar<CUtlString> s2_client_extra_addons("s2_client_extra_addons", FCVAR_NONE, "The workshop IDs of extra client addons that will be applied to all clients, separated by commas", CUtlString(""),
	[](CConVar<CUtlString> *cvar, CSplitScreenSlot slot, const CUtlString *new_val, const CUtlString *old_val)
	{
		g_MultiAddonManager.m_globalClientAddons = utils::string_to_vector<uint64_t>(new_val->Get());
	});

std::string MultiAddonManager::BuildAddonPath(uint64_t addon) {
    // The workshop on a dedicated server is stored relative to the working directory for whatever reason
    static CBufferStringGrowable<MAX_PATH> s_sWorkingDir;
    ExecuteOnce(g_pFullFileSystem->GetSearchPath("EXECUTABLE_PATH", GET_SEARCH_PATH_ALL, s_sWorkingDir, 1));

    return std::format("{}steamapps/workshop/content/730/{}/{}_dir.vpk", s_sWorkingDir.Get(), addon, addon);
}

bool MultiAddonManager::MountAddon(uint64_t addon, bool addToTail) {
	if (!addon)
		return false;

	if (addon == m_currentWorkshopMap) {
		S2_LOGF(LS_MESSAGE, "{}: Addon {} is already mounted by the server\n", __func__, addon);
		return false;
	}

	uint32 addonState = g_SteamAPI.SteamUGC()->GetItemState(addon);

	if (addonState & k_EItemStateLegacyItem) {
		S2_LOGF(LS_MESSAGE, "{}: Addon {} is not compatible with Source 2, skipping\n", __func__, addon);
		return false;
	}

	if (!(addonState & k_EItemStateInstalled)) {
		S2_LOGF(LS_MESSAGE, "{}: Addon {} is not installed, queuing a download\n", __func__, addon);
		DownloadAddon(addon, true, true);
		return false;
	}

	if (s2_addon_mount_download.Get()) {
		// Queue a download anyway in case the addon got an update and the server desires this, but don't reload the map when done
		DownloadAddon(addon, false, true);
	}

	auto path = BuildAddonPath(addon);

	constexpr std::string_view current = "_dir.vpk";
	constexpr std::string_view legacy = ".vpk";

	if (!g_pFullFileSystem->FileExists(path.c_str())) {
		// This might be a legacy addon (before multi-chunk was introduced), try again without the _dir
		path.replace(path.size() - current.size(), current.size(), legacy);

		if (!g_pFullFileSystem->FileExists(path.c_str())) {
			S2_LOGF(LS_WARNING, "{}: Addon {} not found at {}\n", __func__, addon, path);
			return false;
		}
	} else {
		// We still need it without _dir anyway because the filesystem will append suffixes if needed
		path.replace(path.size() - current.size(), current.size(), legacy);
	}

	if (std::find(m_mountedAddons.begin(), m_mountedAddons.end(), addon) != m_mountedAddons.end()) {
		S2_LOGF(LS_WARNING, "{}: Addon {} is already mounted\n", __func__, addon);
		return false;
	}

	S2_LOGF(LS_MESSAGE, "Adding search path: {}\n", path);

	g_pFullFileSystem->AddSearchPath(path.c_str(), "GAME", addToTail ? PATH_ADD_TO_TAIL : PATH_ADD_TO_HEAD);
	m_mountedAddons.emplace_back(addon);

	return true;
}

bool MultiAddonManager::UnmountAddon(uint64_t addon, bool remove) {
	if (!addon)
		return false;

	auto path = BuildAddonPath(addon);

	if (!g_pFullFileSystem->RemoveSearchPath(path.c_str(), "GAME"))
		return false;

	if (remove) {
		std::erase(m_mountedAddons, addon);
	}

	S2_LOGF(LS_MESSAGE, "Removing search path: {}\n", path);

	return true;
}

void MultiAddonManager::PrintDownloadProgress() const {
	if (m_downloadQueue.Count() == 0)
		return;

	uint64 iBytesDownloaded = 0;
	uint64 iTotalBytes = 0;

	if (!g_SteamAPI.SteamUGC()->GetItemDownloadInfo(m_downloadQueue.Head(), &iBytesDownloaded, &iTotalBytes) || !iTotalBytes)
		return;

	constexpr int MB = 1024 * 1024;
	double flMBDownloaded = static_cast<double>(iBytesDownloaded) / MB;
	double flTotalMB = static_cast<double>(iTotalBytes) / MB;

	double flProgress = static_cast<double>(iBytesDownloaded) / static_cast<double>(iTotalBytes);
	flProgress *= 100.f;

	S2_LOGF(LS_MESSAGE, "Downloading addon {}: {:.2f}/{:.2f} MB ({:.2f}%)\n", m_downloadQueue.Head(), flMBDownloaded, flTotalMB, flProgress);
}

// important adds downloads to the pending list, which will reload the current map once the list is exhausted
// force will initiate a download even if the addon already exists and is updated
// Internally, downloads are queued up and processed one at a time
bool MultiAddonManager::DownloadAddon(uint64_t addon, bool important, bool force) {
	if (!g_SteamAPI.SteamUGC()) {
		S2_LOGF(LS_ERROR, "{}: Cannot download addons as the Steam API is not initialized\n", __func__);
		return false;
	}

	if (addon == 0) {
		S2_LOGF(LS_WARNING, "{}: Invalid addon {}\n", __func__, addon);
		return false;
	}

	if (m_downloadQueue.Check(addon)) {
		S2_LOGF(LS_WARNING, "{}: Addon {} is already queued for download!\n", __func__, addon);
		return false;
	}

	uint32 itemState = g_SteamAPI.SteamUGC()->GetItemState(addon);

	if (!force && (itemState & k_EItemStateInstalled)) {
		S2_LOGF(LS_MESSAGE, "Addon {} is already installed\n", addon);
		return true;
	}

	if (!g_SteamAPI.SteamUGC()->DownloadItem(addon, false)) {
		S2_LOGF(LS_WARNING, "{}: Addon download for {} failed to start, addon ID is invalid or server is not logged on Steam\n",
		      __func__, addon);
		return false;
	}

	if (important && m_importantDownloads.Find(addon) == -1)
		m_importantDownloads.AddToTail(addon);

	m_downloadQueue.Insert(addon);

	S2_LOGF(LS_MESSAGE, "Addon download started for {}\n", addon);

	return true;
}

void MultiAddonManager::RefreshAddons(bool reloadMap) {
	if (!g_SteamAPI.SteamUGC())
		return;

	S2_LOGF(LS_MESSAGE, "Refreshing addons ({})\n", utils::vector_to_string(m_extraAddons));

	// Remove our paths first in case addons were switched
	for (const auto& addon : m_mountedAddons) {
		UnmountAddon(addon, false);
	}
	m_mountedAddons.clear();

	bool refuseMounting = false;

	for (const auto& addon : m_extraAddons) {
		refuseMounting |= !MountAddon(addon);
	}

	if (!refuseMounting && reloadMap)
		ReloadMap();
}

void MultiAddonManager::ClearAddons() {
	m_extraAddons.clear();

	for (const auto& addon : m_mountedAddons) {
		UnmountAddon(addon, false);
	}
	m_mountedAddons.clear();
}

void MultiAddonManager::ReloadMap() {
	std::string cmd;

	// Using the concommand here as g_pEngineServer->ChangeLevel somehow doesn't unmount workshop maps and we wanna be clean
	if (m_currentWorkshopMap == k_PublishedFileIdInvalid)
		cmd = std::format("changelevel {}", gpGlobals->mapname.ToCStr());
	else
		cmd = std::format("host_workshop_map {}", m_currentWorkshopMap);

	g_pEngineServer->ServerCommand(cmd.c_str());
}

void MultiAddonManager::OnAddonDownloaded(DownloadItemResult_t* result) {
	if (result->m_eResult == k_EResultOK)
		S2_LOGF(LS_MESSAGE, "Addon {} downloaded successfully\n", static_cast<uint64_t>(result->m_nPublishedFileId));
	else
		S2_LOGF(LS_WARNING, "Addon {} download failed with status {}\n", static_cast<uint64_t>(result->m_nPublishedFileId), static_cast<int>(result->m_eResult));

	// This download isn't triggered by us, don't do anything
	if (!m_downloadQueue.Check(result->m_nPublishedFileId))
		return;

	m_downloadQueue.RemoveAtHead();

	bool found = m_importantDownloads.FindAndRemove(result->m_nPublishedFileId);

	// That was the last important download, now reload the map
	if (found && m_importantDownloads.Count() == 0) {
		S2_LOGF(LS_MESSAGE, "All addon downloads finished, reloading map {}\n", gpGlobals->mapname.ToCStr());
		ReloadMap();
	}
}

bool MultiAddonManager::AddAddon(uint64_t addon, bool refresh) {
	if (std::find(m_extraAddons.begin(), m_extraAddons.end(), addon) != m_extraAddons.end()) {
		S2_LOGF(LS_WARNING, "Addon {} is already in the list!\n", addon);
		return false;
	}

	S2_LOGF(LS_MESSAGE, "Adding {} to addon list\n", addon);

	m_extraAddons.emplace_back(addon);

	// Update the convar to reflect the new addon list, but don't trigger the callback
	//s2_extra_addons.GetConVarData()->Value(0)->m_StringValue = utils::vector_to_string(m_extraAddons).c_str();
	S2_LOGF(LS_MESSAGE, "Clearing client cache due to addons changing");

	if (refresh) {
		RefreshAddons();
	}

	return true;
}

bool MultiAddonManager::RemoveAddon(uint64_t addon, bool refresh) {
	auto it = std::find(m_extraAddons.begin(), m_extraAddons.end(), addon);

	if (it == m_extraAddons.end()) {
		S2_LOGF(LS_WARNING, "Addon {} is not in the list!\n", addon);
		return false;
	}

	S2_LOGF(LS_MESSAGE, "Removing {} from addon list\n", addon);

	m_extraAddons.erase(it);

	// Update the convar to reflect the new addon list, but don't trigger the callback
	//s2_extra_addons.GetConVarData()->Value(0)->m_StringValue = utils::vector_to_string(m_extraAddons).c_str();

	S2_LOGF(LS_MESSAGE, "Clearing client cache due to addons changing");

	if (refresh) {
		RefreshAddons();
	}

	return true;
}

void MultiAddonManager::OnSteamAPIActivated() {
	if (m_callbackRegistered)
		return;

	m_callbackRegistered = true;
	m_CallbackDownloadItemResult.Register(this, &MultiAddonManager::OnAddonDownloaded);

	S2_LOG(LS_MESSAGE, "Refreshing addons to check for updates\n");
	RefreshAddons(true);
}

void MultiAddonManager::OnStartupServer() {
	// Remove empty paths added when there are 2+ addons, they screw up file writes
	g_pFullFileSystem->RemoveSearchPath("", "GAME");
	g_pFullFileSystem->RemoveSearchPath("", "DEFAULT_WRITE_PATH");

	// This has to be done here to replicate the behavior on clients, where they mount addons in the string order
	// So if the current map is ID 1 and extra addons are IDs 2 and 3, they would be mounted in that order with ID 3 at the top
	// Note that the actual map VPK(s) and any sub-maps like team_select will be even higher, but those usually don't contain any assets that concern us
	RefreshAddons();
}

CNETMsg_SignonState_t* GetAddonSignonStateMessage(uint64_t addon) {
	if (!gpGlobals)
		return nullptr;

	INetworkMessageInternal* pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("SignonState");
	CNETMsg_SignonState_t* pMsg = pNetMsg->AllocateMessage()->As<CNETMsg_SignonState_t>();
	pMsg->set_spawn_count(gpGlobals->serverCount);
	pMsg->set_signon_state(SIGNONSTATE_CHANGELEVEL);
	pMsg->set_addons(addon ? std::to_string(addon) : "");
	CUtlClientVector& clients = *utils::GetClientList();
	pMsg->set_num_server_players(clients.Count());
	for (int i = 0; i < clients.Count(); i++) {
		auto client = clients.Element(i);
		char const *szNetworkId = g_pEngineServer->GetPlayerNetworkIDString(client->GetPlayerSlot());
		pMsg->add_players_networkids(szNetworkId);
	}

	return pMsg;
}

bool MultiAddonManager::HasUGCConnection() {
	return g_SteamAPI.SteamUGC() != nullptr;
}

void MultiAddonManager::AddClientAddon(uint64_t addon, uint64 steamID64, bool refresh) {
	if (!steamID64) {
		if (std::find(m_globalClientAddons.begin(), m_globalClientAddons.end(), addon) != m_globalClientAddons.end()) {
			S2_LOGF(LS_WARNING, "Addon {} is already in the list!\n", addon);
			return;
		}

		m_globalClientAddons.emplace_back(addon);
		//s2_client_extra_addons.GetConVarData()->Value(0)->m_StringValue = utils::vector_to_string(m_globalClientAddons).c_str();
	} else {
		ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];

		if (std::find(clientInfo.addonsToLoad.begin(), clientInfo.addonsToLoad.end(), addon) != clientInfo.addonsToLoad.end()) {
			S2_LOGF(LS_WARNING, "Addon {} is already in the list!\n", addon);
			return;
		}

		clientInfo.addonsToLoad.emplace_back(addon);
	}

	if (refresh) {
		CUtlClientVector& clients = *utils::GetClientList();
		auto pMsg = GetAddonSignonStateMessage(addon);
		if (!pMsg) {
			S2_LOGF(LS_WARNING, "Failed to create signon state message for {}\n", addon);
			return;
		}
		FOR_EACH_VEC(clients, i) {
			CServerSideClientBase* client = clients[i];
			if (steamID64 == 0 || client->GetClientSteamID().ConvertToUint64() == steamID64) {
				// Client is already loading, telling them to reload now will actually just disconnect them. ("Received signon %i when at %i\n" in client console)
				if (client->GetSignonState() == SIGNONSTATE_CHANGELEVEL)
					break;
				// Client still has addons to load anyway, they don't need to be told to reload
				if (g_ClientAddons[steamID64].currentPendingAddon != 0)
					break;

				ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];

				std::vector<uint64_t> addons = GetClientAddons(steamID64);

				for (const auto& downloadedAddon : clientInfo.downloadedAddons) {
					if (auto it = std::find(addons.begin(), addons.end(), downloadedAddon); it != addons.end()) {
						addons.erase(it);
					}
				}

				if (addons.empty())
					break;

				clientInfo.currentPendingAddon = addons.front();

				client->GetNetChannel()->SendNetMessage(pMsg, BUF_RELIABLE);

				if (steamID64)
					break;
			}
		}
		delete pMsg;
	}
}

void MultiAddonManager::RemoveClientAddon(uint64_t addon, uint64 steamID64) {
	if (!steamID64) {
		auto it = std::find(m_globalClientAddons.begin(), m_globalClientAddons.end(), addon);
		if (it != m_globalClientAddons.end()) {
			m_globalClientAddons.erase(it);
		}
		//s2_client_extra_addons.GetConVarData()->Value(0)->m_StringValue = utils::vector_to_string(m_globalClientAddons).c_str();
	} else {
		ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];
		auto it = std::find(clientInfo.addonsToLoad.begin(), clientInfo.addonsToLoad.end(), addon);
		if (it != clientInfo.addonsToLoad.end()) {
			clientInfo.addonsToLoad.erase(it);
		}
	}
}

void MultiAddonManager::ClearClientAddons(uint64 steamID64) {
	if (!steamID64) {
		m_globalClientAddons.clear();
		//s2_client_extra_addons.GetConVarData()->Value(0)->m_StringValue = utils::vector_to_string(m_globalClientAddons).c_str();
	} else {
		ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];
		clientInfo.addonsToLoad.clear();
	}
}

std::vector<uint64_t> MultiAddonManager::GetClientAddons(uint64 steamID64) {
	// The list of mounted addons should never contain the workshop map.
	auto addons = m_mountedAddons;

	if (m_currentWorkshopMap) {
		addons.insert(addons.begin(), m_currentWorkshopMap);
	}

	// Also make sure we don't have duplicates.
	for (const auto& addon : m_globalClientAddons) {
		if (std::find(addons.begin(), addons.end(), addon) == addons.end()) {
			addons.emplace_back(addon);
		}
	}

	// If we specify a client steamID64, check for the addons exclusive to this client as well.
	if (steamID64) {
		auto& addonsToLoad = g_ClientAddons[steamID64].addonsToLoad;
		for (const auto& addon : addonsToLoad) {
			if (std::find(addonsToLoad.begin(), addonsToLoad.end(), addon) == addonsToLoad.end()) {
				addonsToLoad.emplace_back(addon);
			}
		}
	}

	return addons;
}

CON_COMMAND_F(s2_add_client_addon, "Add a workshop ID to the global client-only addon list", FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "Usage: {} <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.AddClientAddon(*addon);
	}
}

CON_COMMAND_F(s2_remove_client_addon, "Remove a workshop ID from the global client-only addon list", FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "Usage: {} <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.RemoveClientAddon(*addon);
	}
}


CON_COMMAND_F(s2_add_addon, "Add a workshop ID to the extra addon list", FCVAR_LINKED_CONCOMMAND | FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "Usage: {} <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.AddAddon(*addon);
	}
}

CON_COMMAND_F(s2_remove_addon, "Remove a workshop ID from the extra addon list",  FCVAR_LINKED_CONCOMMAND | FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "Usage: {} <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.RemoveAddon(*addon);
	}
}

CON_COMMAND_F(s2_download_addon, "Download an addon manually", FCVAR_GAMEDLL | FCVAR_RELEASE | FCVAR_SPONLY) {
	if (args.ArgC() != 2) {
		S2_LOGF(LS_WARNING, "Usage: {} <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.DownloadAddon(*addon, false, true);
	}
}

CON_COMMAND_F(s2_print_searchpaths, "Print search paths", FCVAR_GAMEDLL | FCVAR_RELEASE | FCVAR_SPONLY) {
	g_pFullFileSystem->PrintSearchPaths();
}

CON_COMMAND_F(s2_print_searchpaths_client, "Print search paths client-side, only usable if you're running the plugin on a listenserver", FCVAR_CLIENTDLL) {
	g_pFullFileSystem->PrintSearchPaths();
}

void MultiAddonManager::OnHostStateRequest(CHostStateMgr* mgrDoNotUse, CHostStateRequest* request) {
	// When IVEngineServer::ChangeLevel is called by the plugin or the server code,
	// (which happens at the end of a map), the server-defined addon does not change.
	// Also, host state requests coming from that function will always have "ChangeLevel" in its KV's name.
	// We can use this information to always be aware of what the original addon is.

	if (!request->m_pKV) {
		m_currentWorkshopMap = 0;
	} else {
		std::string_view name = request->m_pKV->GetName();
		if (name == "ChangeLevel") {
			if (name == "map_workshop") {
				if (auto addon = utils::string_to_int<uint64_t>(request->m_pKV->GetString("customgamemode", ""))) {
					m_currentWorkshopMap = *addon;
				}
			} else {
				m_currentWorkshopMap = 0;
			}
		}
	}

	if (g_MultiAddonManager.m_extraAddons.empty()) {
		g_pfnSetPendingHostStateRequest(mgrDoNotUse, request);
		return;
	}

	// Rebuild the addon list. We always start with the original addon.
	if (m_currentWorkshopMap == 0) {
		request->m_Addons = utils::vector_to_string(g_MultiAddonManager.m_extraAddons).c_str();
	} else {
		// Don't add the same addon twice. Hopefully no server owner is diabolical enough to do things like `map de_dust2 customgamemode=1234,5678`.
		std::vector<uint64_t> newAddons = g_MultiAddonManager.m_extraAddons;
		auto it = std::find(newAddons.begin(), newAddons.end(), m_currentWorkshopMap);

		// If the element is found and is not already at the end
		if (it != newAddons.end() && it != newAddons.begin() + static_cast<ptrdiff_t>(newAddons.size() - 1)) {
			// Rotate the elements such that the found element moves to the end
			std::rotate(it, it + 1, newAddons.end());
		}

		request->m_Addons = utils::vector_to_string(newAddons).c_str();
	}

	g_pfnSetPendingHostStateRequest(mgrDoNotUse, request);
}

void MultiAddonManager::OnReplyConnection(INetworkGameServer* server, CServerSideClient* client) {
	uint64 steamID64 = client->GetClientSteamID().ConvertToUint64();

	// Clear cache if necessary.
	ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];
	if (s2_cache_clients_with_addons.Get() && s2_cache_clients_duration.Get() != 0 && Plat_FloatTime() - clientInfo.lastActiveTime > s2_cache_clients_duration.Get()) {
		S2_LOGF(LS_MESSAGE, "{}: Client {} has not connected for a while, clearing the cache\n", __func__, steamID64);
		clientInfo.currentPendingAddon = 0;
		clientInfo.downloadedAddons.clear();
	}
	clientInfo.lastActiveTime = Plat_FloatTime();

	// Server copies the CUtlString from CNetworkGameServer to this client.
	static int offset = g_pGameConfig->GetOffset("ServerAddons");
	CUtlString* addons = reinterpret_cast<CUtlString*>(reinterpret_cast<uintptr_t>(server) + offset);
	CUtlString originalAddons = *addons;

	// Figure out which addons the client should be loading.
	std::vector<uint64_t> clientAddons = g_MultiAddonManager.GetClientAddons(steamID64);
	if (clientAddons.empty()) {
		// No addons to send. This means the list of original addons is empty as well.
		assert(originalAddons.IsEmpty());
		g_pfnReplyConnection(server, client);
		return;
	}

	auto& downloadedAddons = clientInfo.downloadedAddons;

	// Handle the first addon here. The rest should be handled in the SendNetMessage hook.
	if (std::find(downloadedAddons.begin(), downloadedAddons.end(), clientAddons[0]) == downloadedAddons.end()) {
		clientInfo.currentPendingAddon = clientAddons[0];
	}

	*addons = utils::vector_to_string<uint64_t>(clientAddons).c_str();

	S2_LOGF(LS_MESSAGE, "{}: Sending addons {} to steamID64 {}\n", __func__, addons->Get(), steamID64);
	g_pfnReplyConnection(server, client);

	*addons = originalAddons;
}

void* MultiAddonManager::OnSendNetMessage(CServerSideClient* client, CNetMessage* data, NetChannelBufType_t bufType) {
	uint64 steamID64 = client->GetClientSteamID().ConvertToUint64();
	ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];

	// If we are sending a message to the client, that means the client is still active.
	clientInfo.lastActiveTime = Plat_FloatTime();

	INetworkSerializerPB* serializerPB = data->GetSerializerPB();
	if (!serializerPB || serializerPB->GetNetMessageInfo()->m_MessageId != net_SignonState) {
		return g_pfnSendNetMessage(client, data, bufType);
	}

	auto pMsg = data->As<CNETMsg_SignonState_t>();

	std::vector<uint64_t> addons = g_MultiAddonManager.GetClientAddons(steamID64);

	if (pMsg->signon_state() == SIGNONSTATE_CHANGELEVEL) {
		// When switching to another map, the signon message might contain more than 1 addon.
		// This puts the client in limbo because client doesn't know how to handle multiple addons at the same time.
		std::vector<uint64_t> addonsList = utils::string_to_vector<uint64_t>(pMsg->addons());
		if (addonsList.size() > 1) {
			// If there's more than one addon, ensure that it takes the first addon (which should be the workshop map or the first custom addon)
			pMsg->set_addons(std::to_string(addonsList.front()));
			// Since the client will download the addon contained inside this messsage, we might as well add it to the list of client's downloaded addons.
			clientInfo.currentPendingAddon = addonsList.front();
		} else if (addonsList.size() == 1) {
			// Nothing to do here, the rest of the required addons can be sent later.
			if (auto addon = utils::string_to_int<uint64_t>(pMsg->addons())) {
				clientInfo.currentPendingAddon = *addon;
			}
		}

		return g_pfnSendNetMessage(client, data, bufType);
	}

	for (const auto& downloadedAddon : clientInfo.downloadedAddons) {
		if (auto it = std::find(addons.begin(), addons.end(), downloadedAddon); it != addons.end()) {
			addons.erase(it);
		}
	}

	// Check if client has downloaded everything.
	if (addons.empty()) {
		return g_pfnSendNetMessage(client, data, bufType);
	}

	// Otherwise, send the next addon to the client.
	S2_LOGF(LS_MESSAGE, "{}: Number of addons remaining to download for {}: {}\n", __func__, steamID64, addons.size());
	clientInfo.currentPendingAddon = addons.front();
	pMsg->set_addons(std::to_string(addons.front()));
	pMsg->set_signon_state(SIGNONSTATE_CHANGELEVEL);

	return g_pfnSendNetMessage(client, data, bufType);
}

void MultiAddonManager::OnGameFrame() {
	static double s_time = 0.0;

	// Print download progress every second
	double currentTime = Plat_FloatTime();
	if (currentTime - s_time > 1.0) {
		s_time = currentTime;
		PrintDownloadProgress();
	}
}

void MultiAddonManager::OnPostEvent(INetworkMessageInternal* message, CNetMessage* data, uint64_t* clients) {
	if (s2_block_disconnect_messages.Get() && message->GetNetMessageInfo()->m_MessageId == GE_Source1LegacyGameEvent) {
		auto pMsg = data->As<CMsgSource1LegacyGameEvent_t>();

		static int sDisconnectId = g_pGameEventManager->LookupEventId("player_disconnect");

		if (pMsg->eventid() == sDisconnectId) {
			IGameEvent* pEvent = g_pGameEventManager->UnserializeEvent(*pMsg);

			// This will prevent "loop shutdown" messages in the chat when clients reconnect
			// As far as we're aware, there are no other cases where this reason is used
			if (pEvent->GetInt("reason") == NETWORK_DISCONNECT_LOOPSHUTDOWN) {
				*reinterpret_cast<uint64*>(clients) = 0;
			}
		}
	}
}

void MultiAddonManager::OnClientConnect(CPlayerSlot slot, const char* name, uint64 steamID64, const char* networkID) {
	std::vector<uint64_t> addons = GetClientAddons(steamID64);
	// We don't have an extra addon set so do nothing here, also don't do anything if we're a listenserver
	if (addons.empty())
		return;

	ClientAddonInfo& clientInfo = g_ClientAddons[steamID64];

	if (clientInfo.currentPendingAddon != 0) {
		if (Plat_FloatTime() - clientInfo.lastActiveTime > s2_extra_addons_timeout.Get()) {
			S2_LOGF(LS_MESSAGE, "{}: Client {} has reconnected after the timeout or did not receive the addon message, will not add addon {} to the downloaded list\n", __func__, steamID64, clientInfo.currentPendingAddon);
		} else {
			S2_LOGF(LS_MESSAGE, "{}: Client {} has connected within the interval with the pending addon {}, will send next addon in SendNetMessage hook\n", __func__, steamID64, clientInfo.currentPendingAddon);
			clientInfo.downloadedAddons.emplace_back(clientInfo.currentPendingAddon);
		}
		// Reset the current pending addon anyway, SendNetMessage tells us which addon to download next.
		clientInfo.currentPendingAddon = 0;
	}
	g_ClientAddons[steamID64].lastActiveTime = Plat_FloatTime();
}

void MultiAddonManager::OnClientDisconnect(CPlayerSlot slot, const char* name, uint64 steamID64, const char* networkID) {
	// Mark the disconnection time for caching purposes.
	g_ClientAddons[steamID64].lastActiveTime = Plat_FloatTime();
}

void MultiAddonManager::OnClientActive(CPlayerSlot slot, bool loadGame, const char* name, uint64 steamID64) {
	// When the client reaches this stage, they should already have all the necessary addons downloaded, so we can safely remove the downloaded addons list here.
	if (!s2_cache_clients_with_addons.Get()) {
		g_ClientAddons[steamID64].downloadedAddons.clear();
	}
}

MultiAddonManager g_MultiAddonManager;