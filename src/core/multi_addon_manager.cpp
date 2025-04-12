#include "multi_addon_manager.hpp"

#include <networkbasetypes.pb.h>
#include <networksystem/inetworkserializer.h>
#include <networksystem/inetworkmessages.h>
#include <igameeventsystem.h>
#include <serversideclient.h>
#include <filesystem.h>
#include <gameevents.pb.h>
#include <steam/steam_gameserver.h>
#include <iserver.h>

#include "core_config.hpp"
#include "sdk/utils.h"

struct ClientJoinInfo {
	uint64 xuid;
	double timestamp;
	size_t addon;
};

std::vector<ClientJoinInfo> g_ClientsPendingAddon; // List of clients who are still downloading addons
std::unordered_set<uint64> g_ClientsWithAddons; // List of clients who already downloaded everything so they don't get reconnects on mapchange/rejoin

void MultiAddonManager::BuildAddonPath(uint64_t addon, char* buf, size_t len, bool legacy) {
    // The workshop on a dedicated server is stored relative to the working directory for whatever reason
    static CBufferStringGrowable<MAX_PATH> s_sWorkingDir;
    ExecuteOnce(g_pFullFileSystem->GetSearchPath("EXECUTABLE_PATH", GET_SEARCH_PATH_ALL, s_sWorkingDir, 1));

    V_snprintf(buf, static_cast<int>(len), "%ssteamapps/workshop/content/730/%lu/%lu%s.vpk", s_sWorkingDir.Get(), addon, addon, legacy ? "" : "_dir");
}

bool MultiAddonManager::MountAddon(uint64_t addon, bool addToTail) {
	if (!addon)
		return false;

	uint32 addonState = g_SteamAPI.SteamUGC()->GetItemState(addon);

	if (!(addonState & k_EItemStateInstalled)) {
		S2_LOGF(LS_MESSAGE, "%s: Addon %lu is not installed, queuing a download\n", __func__, addon);
		DownloadAddon(addon, true, true);
		return false;
	}

	if (g_pCoreConfig->AddonMountDownload) {
		// Queue a download anyway in case the addon got an update and the server desires this, but don't reload the map when done
		DownloadAddon(addon, false, true);
	}

	char pszPath[MAX_PATH];
	BuildAddonPath(addon, pszPath, sizeof(pszPath));

	if (!g_pFullFileSystem->FileExists(pszPath)) {
		// This might be a legacy addon (before multi-chunk was introduced), try again without the _dir
		BuildAddonPath(addon, pszPath, sizeof(pszPath), true);

		if (!g_pFullFileSystem->FileExists(pszPath)) {
			S2_LOGF(LS_WARNING, "%s: Addon %lu not found at %s\n", __func__, addon, pszPath);
			return false;
		}
	} else {
		// We still need it without _dir anyway because the filesystem will append suffixes if needed
		BuildAddonPath(addon, pszPath, sizeof(pszPath), true);
	}

	if (std::find(m_mountedAddons.begin(), m_mountedAddons.end(), addon) != m_mountedAddons.end()) {
		S2_LOGF(LS_WARNING, "%s: Addon %lu is already mounted\n", __func__, addon);
		return false;
	}

	S2_LOGF(LS_MESSAGE, "Adding search path: %s\n", pszPath);

	g_pFullFileSystem->AddSearchPath(pszPath, "GAME", addToTail ? PATH_ADD_TO_TAIL : PATH_ADD_TO_HEAD);
	m_mountedAddons.emplace_back(addon);

	return true;
}

bool MultiAddonManager::UnmountAddon(uint64_t addon, bool remove) {
	if (!addon)
		return false;

	char path[MAX_PATH];
	BuildAddonPath(addon, path, sizeof(path));

	if (!g_pFullFileSystem->RemoveSearchPath(path, "GAME"))
		return false;

	if (remove) {
		std::erase(m_mountedAddons, addon);
	}

	S2_LOGF(LS_MESSAGE, "Removing search path: %s\n", path);

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

	S2_LOGF(LS_MESSAGE, "Downloading addon %lu: %.2f/%.2f MB (%.2f%%)\n", m_downloadQueue.Head(), flMBDownloaded, flTotalMB, flProgress);
}

// important adds downloads to the pending list, which will reload the current map once the list is exhausted
// force will initiate a download even if the addon already exists and is updated
// Internally, downloads are queued up and processed one at a time
bool MultiAddonManager::DownloadAddon(uint64_t addon, bool important, bool force) {
	if (!g_SteamAPI.SteamUGC()) {
		S2_LOGF(LS_ERROR, "%s: Cannot download addons as the Steam API is not initialized\n", __func__);
		return false;
	}

	if (addon == 0) {
		S2_LOGF(LS_WARNING, "%s: Invalid addon %lu\n", __func__, addon);
		return false;
	}

	if (m_downloadQueue.Check(addon)) {
		S2_LOGF(LS_WARNING, "%s: Addon %lu is already queued for download!\n", __func__, addon);
		return false;
	}

	uint32 itemState = g_SteamAPI.SteamUGC()->GetItemState(addon);

	if (!force && (itemState & k_EItemStateInstalled)) {
		S2_LOGF(LS_MESSAGE, "Addon %lu is already installed\n", addon);
		return true;
	}

	if (!g_SteamAPI.SteamUGC()->DownloadItem(addon, false)) {
		S2_LOGF(LS_WARNING, "%s: Addon download for %lu failed to start, addon ID is invalid or server is not logged on Steam\n",
		      __func__, addon);
		return false;
	}

	if (important && m_importantDownloads.Find(addon) == -1)
		m_importantDownloads.AddToTail(addon);

	m_downloadQueue.Insert(addon);

	S2_LOGF(LS_MESSAGE, "Addon download started for %lu\n", addon);

	return true;
}

void MultiAddonManager::RefreshAddons(bool reloadMap) {
	if (!g_SteamAPI.SteamUGC())
		return;

	S2_LOGF(LS_MESSAGE, "Refreshing addons (%s)\n", utils::vector_to_string(m_extraAddons).c_str());

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
	char cmd[MAX_PATH];

	// Using the concommand here as g_pEngineServer->ChangeLevel somehow doesn't unmount workshop maps and we wanna be clean
	if (m_currentWorkshopMap == k_PublishedFileIdInvalid)
		V_snprintf(cmd, sizeof(cmd), "changelevel %s", gpGlobals->mapname.ToCStr());
	else
		V_snprintf(cmd, sizeof(cmd), "host_workshop_map %lu", m_currentWorkshopMap);

	g_pEngineServer2->ServerCommand(cmd);
}

void MultiAddonManager::OnAddonDownloaded(DownloadItemResult_t* result) {
	if (result->m_eResult == k_EResultOK)
		S2_LOGF(LS_MESSAGE, "Addon %lu downloaded successfully\n", result->m_nPublishedFileId);
	else
		S2_LOGF(LS_WARNING, "Addon %lu download failed with status %i\n", result->m_nPublishedFileId, result->m_eResult);

	// This download isn't triggered by us, don't do anything
	if (!m_downloadQueue.Check(result->m_nPublishedFileId))
		return;

	m_downloadQueue.RemoveAtHead();

	bool found = m_importantDownloads.FindAndRemove(result->m_nPublishedFileId);

	// That was the last important download, now reload the map
	if (found && m_importantDownloads.Count() == 0) {
		S2_LOGF(LS_MESSAGE, "All addon downloads finished, reloading map %s\n", gpGlobals->mapname);
		ReloadMap();
	}
}

bool MultiAddonManager::AddAddon(uint64_t addon, bool refresh) {
	if (std::find(m_extraAddons.begin(), m_extraAddons.end(), addon) != m_extraAddons.end()) {
		S2_LOGF(LS_WARNING, "Addon %lu is already in the list!\n", addon);
		return false;
	}

	S2_LOGF(LS_MESSAGE, "Adding %lu to addon list\n", addon);

	m_extraAddons.emplace_back(addon);

	g_ClientsWithAddons.clear();
	S2_LOGF(LS_MESSAGE, "Clearing client cache due to addons changing");

	if (refresh)
		RefreshAddons();

	return true;
}

bool MultiAddonManager::RemoveAddon(uint64_t addon, bool refresh) {
	auto it = std::find(m_extraAddons.begin(), m_extraAddons.end(), addon);

	if (it == m_extraAddons.end()) {
		S2_LOGF(LS_WARNING, "Addon %lu is not in the list!\n", addon);
		return false;
	}

	S2_LOGF(LS_MESSAGE, "Removing %lu from addon list\n", addon);

	m_extraAddons.erase(it);

	g_ClientsWithAddons.clear();
	S2_LOGF(LS_MESSAGE, "Clearing client cache due to addons changing");

	if (refresh)
		RefreshAddons();

	return true;
}

void MultiAddonManager::OnSteamAPIActivated() {
	if (m_callbackRegistered)
		return;

	m_callbackRegistered = true;
	m_CallbackDownloadItemResult.Register(this, &MultiAddonManager::OnAddonDownloaded);

	S2_LOGF(LS_MESSAGE, "Refreshing addons to check for updates\n");
	RefreshAddons(true);
}

void MultiAddonManager::OnStartupServer() {
	g_ClientsPendingAddon.clear();
	g_MultiAddonManager.m_extraAddons = g_pCoreConfig->ExtraAddons;

	// Remove empty paths added when there are 2+ addons, they screw up file writes
	g_pFullFileSystem->RemoveSearchPath("", "GAME");
	g_pFullFileSystem->RemoveSearchPath("", "DEFAULT_WRITE_PATH");

	// This has to be done here to replicate the behavior on clients, where they mount addons in the string order
	// So if the current map is ID 1 and extra addons are IDs 2 and 3, they would be mounted in that order with ID 3 at the top
	// Note that the actual map VPK(s) and any sub-maps like team_select will be even higher, but those usually don't contain any assets that concern us
	RefreshAddons();
}

CON_COMMAND_F(s2_extra_addons, "The workshop IDs of extra addons separated by commas, addons will be downloaded (if not present) and mounted",  FCVAR_LINKED_CONCOMMAND | FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "%s %s\n", args[0], utils::vector_to_string(g_MultiAddonManager.m_extraAddons).c_str());
		return;
	}

	g_ClientsWithAddons.clear();
	S2_LOGF(LS_MESSAGE, "Clearing client cache due to addons changing");

	g_MultiAddonManager.m_extraAddons = utils::string_to_vector<uint64_t>(args[1]);

	g_MultiAddonManager.RefreshAddons();
}

CON_COMMAND_F(s2_add_addon, "Add a workshop ID to the extra addon list", FCVAR_LINKED_CONCOMMAND | FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "Usage: %s <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.AddAddon(*addon);
	}
}

CON_COMMAND_F(s2_remove_addon, "Remove a workshop ID from the extra addon list",  FCVAR_LINKED_CONCOMMAND | FCVAR_SPONLY) {
	if (args.ArgC() < 2) {
		S2_LOGF(LS_WARNING, "Usage: %s <ID>\n", args[0]);
		return;
	}

	if (auto addon = utils::string_to_int<uint64_t>(args[1])) {
		g_MultiAddonManager.RemoveAddon(*addon);
	}
}

CON_COMMAND_F(s2_download_addon, "Download an addon manually", FCVAR_GAMEDLL | FCVAR_RELEASE | FCVAR_SPONLY) {
	if (args.ArgC() != 2) {
		S2_LOGF(LS_WARNING, "Usage: %s <ID>\n", args[0]);
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

bool MultiAddonManager::OnHostStateRequest(CUtlString* addonString) {
	if (m_extraAddons.empty())
		return false;

	auto vecAddons = utils::string_to_vector<uint64_t>(addonString->Get());

	// Clear the string just in case it wasn't somehow, like when reloading the map
	addonString->Clear();

	plg::string extraAddonString = utils::vector_to_string(m_extraAddons);

	// If it's empty or the first addon in the string is ours, it means we're on a default map
	if (vecAddons.empty() || std::find(m_extraAddons.begin(), m_extraAddons.end(), vecAddons[0]) != m_extraAddons.end()) {
		S2_LOGF(LS_MESSAGE, "%s: setting addon string to \"%s\"\n", __func__, extraAddonString.c_str());
		addonString->Set(extraAddonString.c_str());
		ClearCurrentWorkshopMap();
	} else {
		S2_LOGF(LS_MESSAGE, "%s: appending \"%s\" to addon string \"%lu\"\n", __func__, extraAddonString.c_str(), vecAddons[0]);
		addonString->Format("%lu,%s", vecAddons[0], extraAddonString.c_str());
		SetCurrentWorkshopMap(vecAddons[0]);
	}

	return true;
}

bool MultiAddonManager::OnSendNetMessage(CServerSideClient* client, CNetMessage* data, NetChannelBufType_t bufType) {
	NetMessageInfo_t* info = data->GetNetMessage()->GetNetMessageInfo();

	// 7 for signon messages
	if (info->m_MessageId != net_SignonState || m_extraAddons.empty())
		return false;

	auto msg = data->ToPB<CNETMsg_SignonState>();

	// If the server is changing maps then we don't want to send all addons in this message
	// Because for SOME reason, this can put the client in a state of limbo after the 25/07/2024 update
	// Also we send the workshop map here because the client MUST have it in order to remain in the server
	// This is what prompts clients to download the next map after all
	uint64_t map = GetCurrentWorkshopMap();

	if (msg->signon_state() == SIGNONSTATE_CHANGELEVEL)
		msg->set_addons(std::to_string(map == k_PublishedFileIdInvalid ? m_extraAddons[k_PublishedFileIdInvalid] : map));

	// If we're on a default map send the first addon
	auto xuid = client->GetClientSteamID().ConvertToUint64();
	auto it = std::find_if(g_ClientsPendingAddon.begin(), g_ClientsPendingAddon.end(), [xuid](const ClientJoinInfo& client) {
		return client.xuid == xuid;
	});

	if (it != g_ClientsPendingAddon.end()) {
		// This only happens after the client connects, therefore the signon message is for SIGNONSTATE_PRESPAWN
		// and we can proceed to send addons as usual
		S2_LOGF(LS_MESSAGE, "%s: Sending addon %lu to client %lu\n", __func__, m_extraAddons[it->addon], it->xuid);

		msg->set_addons(std::to_string(m_extraAddons[it->addon]));
		msg->set_signon_state(SIGNONSTATE_CHANGELEVEL);

		it->timestamp = Plat_FloatTime();
	}

	return true;
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
	if (g_pCoreConfig->BlockDisconnectMessages && message->GetNetMessageInfo()->m_MessageId == GE_Source1LegacyGameEvent) {
		auto msg = data->ToPB<CMsgSource1LegacyGameEvent>();

		static int DisconnectId = g_pGameEventManager->LookupEventId("player_disconnect");
		if (msg->eventid() == DisconnectId) {
			IGameEvent* event = g_pGameEventManager->UnserializeEvent(*msg);

			// This will prevent "loop shutdown" messages in the chat when clients reconnect
			// As far as we're aware, there are no other cases where this reason is used
			if (event->GetInt("reason") == NETWORK_DISCONNECT_LOOPSHUTDOWN) {
				*clients = 0;
			}
		}
	}
}

void MultiAddonManager::OnClientConnect(CPlayerSlot slot, const char* name, uint64 xuid, const char* networkID) {
	// We don't have an extra addon set so do nothing here, also don't do anything if we're a listenserver
	if (m_extraAddons.empty())
		return;

	// If we're on a default map and we have only 1 addon, no need to do any of this
	// The client will be prompted to download upon receiving C2S_CONNECTION with an addon
	if (m_extraAddons.size() == 1 && m_currentWorkshopMap == k_PublishedFileIdInvalid)
		return;

	S2_LOGF(LS_MESSAGE, "Client %s (%lu) connected:\n", name, xuid);

	if (g_pCoreConfig->CacheClientsWithAddons && g_ClientsWithAddons.contains(xuid)) {
		S2_LOGF(LS_MESSAGE, "new connection but client was cached earlier so allowing immediately\n");
		return;
	}

	// Store the client's ID temporarily as they will get reconnected once an extra addon is sent
	// This gets checked for in SendNetMessage so we don't repeatedly send the changelevel signon state for the same addon
	// The only caveat to this is that there's no way for us to verify if the client has actually downloaded the extra addon
	// as they're fully disconnected while downloading it, so the best we can do is use a timeout interval
	auto it = std::find_if(g_ClientsPendingAddon.begin(), g_ClientsPendingAddon.end(), [xuid](const ClientJoinInfo& client) {
		return client.xuid == xuid;
	});

	if (it == g_ClientsPendingAddon.end()) {
		// Client joined for the first time or after a timeout
		S2_LOGF(LS_MESSAGE, "first connection, sending addon %lu\n", m_extraAddons[k_PublishedFileIdInvalid]);
		g_ClientsPendingAddon.emplace_back(xuid, 0.0, k_PublishedFileIdInvalid);
	} else if (Plat_FloatTime() - it->timestamp < g_pCoreConfig->RejoinTimeout) {
		// Client reconnected within the timeout interval
		// If they already have the addon this happens almost instantly after receiving the signon message with the addon
		if (++it->addon < m_extraAddons.size()) {
			S2_LOGF(LS_MESSAGE, "reconnected within the interval, sending next addon %lu\n", m_extraAddons[it->addon]);
		} else {
			S2_LOGF(LS_MESSAGE, "reconnected within the interval and has all addons, allowing\n");
			g_ClientsPendingAddon.erase(it);

			if (g_pCoreConfig->CacheClientsWithAddons) {
				g_ClientsWithAddons.insert(xuid);
			}
		}
	} else {
		S2_LOGF(LS_MESSAGE, "reconnected after the timeout or did not receive the addon message, will resend addon %lu\n", m_extraAddons[it->addon]);
	}
}

MultiAddonManager g_MultiAddonManager;