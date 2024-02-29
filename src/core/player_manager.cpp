#include "player_manager.h"
#include "listeners.h"
#include "timer_system.h"

#include <eiface.h>
#include <game/server/iplayerinfo.h>
#include <inetchannelinfo.h>
#include <iserver.h>

CPlayer::CPlayer() = default;
CPlayer::~CPlayer() = default;

void CPlayer::Initialize(std::string name, std::string ip, CPlayerSlot slot)
{
	m_bConnected = true;
	m_slot = slot;
	m_name = std::move(name);
	m_ipAddress = std::move(ip);
}

IPlayerInfo* CPlayer::GetPlayerInfo() const
{
	return m_info;
}

const std::string& CPlayer::GetName() const
{
	return m_name;
}

bool CPlayer::IsConnected() const
{
	return m_bConnected;
}

bool CPlayer::IsFakeClient() const
{
	return m_bFakeClient;
}

bool CPlayer::IsAuthorized() const
{
	return m_bAuthorized;
}

bool CPlayer::IsAuthStringValidated() const
{
	if (!IsFakeClient())
	{
		return g_pEngineServer2->IsClientFullyAuthenticated(m_slot);
	}
	return false;
}

void CPlayer::Authorize()
{
	m_bAuthorized = true;
}

void CPlayer::PrintToConsole(const char* message) const
{
	if (!m_bConnected || m_bFakeClient)
	{
		return;
	}

	INetChannelInfo* pNetChan = g_pEngineServer2->GetPlayerNetInfo(m_slot);
	if (pNetChan == nullptr)
	{
		return;
	}

	g_pEngineServer2->ClientPrintf(m_slot, message);
}

void CPlayer::PrintToChat(const char* message) const
{
	// globals::user_message_manager.SendMessageToChat(m_i_index, message);
}

void CPlayer::PrintToHint(const char* message) const
{
	// globals::user_message_manager.SendHintMessage(m_i_index, message);
}

void CPlayer::PrintToCenter(const char* message) const
{
	// globals::user_message_manager.SendCenterMessage(m_i_index, message);
}

void CPlayer::SetName(std::string name)
{
	m_name = std::move(name);
}

INetChannelInfo* CPlayer::GetNetInfo() const
{
	return g_pEngineServer2->GetPlayerNetInfo(m_slot);
}

bool CPlayer::WasCountedAsInGame() const
{
	return m_bInGame;
}

int CPlayer::GetUserId()
{
	if (m_userId == -1)
	{
		m_userId = g_pEngineServer2->GetPlayerUserId(m_slot).Get();
	}

	return m_userId;
}

bool CPlayer::IsInGame() const
{
	return m_bInGame;
}

void CPlayer::Kick(const char* kickReason)
{
	char buffer[255];
	sprintf(buffer, "kickid %d %s\n", GetUserId(), kickReason);
	g_pEngineServer2->ServerCommand(buffer);
}

const char* CPlayer::GetWeaponName() const
{
	return m_info->GetWeaponName();
}

void CPlayer::ChangeTeam(int team) const
{
	m_info->ChangeTeam(team);
}

int CPlayer::GetTeam() const
{
	return m_info->GetTeamIndex();
}

int CPlayer::GetArmor() const
{
	return m_info->GetArmorValue();
}

int CPlayer::GetFrags() const
{
	return m_info->GetFragCount();
}

int CPlayer::GetDeaths() const
{
	return m_info->GetDeathCount();
}

const char* CPlayer::GetKeyValue(const char* key) const
{
	return g_pEngineServer2->GetClientConVarValue(m_slot, key);
}

Vector CPlayer::GetMaxSize() const
{
	return m_info->GetPlayerMaxs();
}

Vector CPlayer::GetMinSize() const
{
	return m_info->GetPlayerMins();
}

int CPlayer::GetMaxHealth() const
{
	return m_info->GetMaxHealth();
}

const std::string& CPlayer::GetIpAddress() const
{
	return m_ipAddress;
}

const char* CPlayer::GetModelName() const
{
	return m_info->GetModelName();
}

int CPlayer::GetUserId() const
{
	return m_userId;
}

float CPlayer::GetTimeConnected() const
{
	if (!IsConnected() || IsFakeClient())
	{
		return 0;
	}

	return GetNetInfo()->GetTimeConnected();
}

float CPlayer::GetLatency() const
{
	return GetNetInfo()->GetLatency(FLOW_INCOMING) + GetNetInfo()->GetLatency(FLOW_OUTGOING);
}

void CPlayer::SetListen(CPlayerSlot slot, ListenOverride listen)
{
	m_listenMap[slot.Get()] = listen;
}

void CPlayer::SetVoiceFlags(VoiceFlag_t flags)
{
	m_voiceFlag = flags;
}

VoiceFlag_t CPlayer::GetVoiceFlags()
{
	return m_voiceFlag;
}

ListenOverride CPlayer::GetListen(CPlayerSlot slot) const
{
	return m_listenMap[slot.Get()];
}

void CPlayer::Connect()
{
	if (m_bInGame)
	{
		return;
	}

	m_bInGame = true;
}

void CPlayer::Disconnect()
{
	m_bConnected = false;
	m_bInGame = false;
	m_name.clear();
	m_info = nullptr;
	m_bFakeClient = false;
	m_userId = -1;
	m_bAuthorized = false;
	m_ipAddress.clear();
	m_selfMutes.ClearAll();
	std::fill(m_listenMap.begin(), m_listenMap.end(), Listen_Default);
	m_voiceFlag = 0;
}

QAngle CPlayer::GetAbsAngles() const
{
	return m_info->GetAbsAngles();
}

Vector CPlayer::GetAbsOrigin() const
{
	return m_info->GetAbsOrigin();
}

bool CPlayer::IsAlive() const
{
	if (!IsInGame())
	{
		return false;
	}

	return !m_info->IsDead();
}

const CSteamID* CPlayer::GetSteamId() const
{
	return m_steamId;
}

void CPlayer::SetSteamId(const CSteamID* steam_id)
{
	m_steamId = steam_id;
}

PlayerManager::PlayerManager() = default;

void PlayerManager::RunAuthChecks()
{
	if (TimerSystem::GetTickedTime() - m_lastAuthCheckTime < 0.5)
	{
		return;
	}

	m_lastAuthCheckTime = static_cast<float>(TimerSystem::GetTickedTime());

	for (int i = 0; i <= MaxClients(); i++)
	{
		auto& player = m_players[i];
		if (player.IsConnected())
		{
			if (player.IsAuthorized() || player.IsFakeClient())
				continue;

			if (g_pEngineServer2->IsClientFullyAuthenticated(i))
			{
				player.Authorize();
				player.SetSteamId(g_pEngineServer2->GetClientSteamID(i));
				OnClientAuthorized(&player);
			}
		}
	}
}

void PlayerManager::OnClientAuthorized(CPlayer* player) const
{
	g_Logger.MessageFormat("[OnClientAuthorized] - %s, %lli\n", player->GetName().c_str(), player->GetSteamId()->ConvertToUint64());

	GetOnClientAuthorizedListenerManager().Notify(player->m_slot.Get(), player->GetSteamId()->ConvertToUint64());
}

bool PlayerManager::OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason)
{
	g_Logger.MessageFormat("[OnClientConnect] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (pPlayer->IsConnected())
	{
		OnClientDisconnect(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, pszName, xuid, pszNetworkID);
		OnClientDisconnect_Post(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, pszName, xuid, pszNetworkID);
	}

	pPlayer->Initialize(pszName, pszNetworkID, slot);

	m_refuseConnection = false;

	for (size_t i = 0; i < GetOnClientConnectListenerManager().GetCount(); ++i)
	{
		m_refuseConnection |= !GetOnClientConnectListenerManager().Notify(i, client, pszName, pszNetworkID);
	}

	m_userIdLookup[g_pEngineServer2->GetPlayerUserId(slot).Get()] = client;

	return m_refuseConnection;
}

bool PlayerManager::OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet)
{
	g_Logger.MessageFormat("[OnClientConnect_Post] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (m_refuseConnection)
		origRet = false;

	if (origRet)
	{
		GetOnClientConnect_PostListenerManager().Notify(pPlayer->m_slot.Get());

		if (!pPlayer->IsFakeClient() && m_bListenServer && !strncmp(pszNetworkID, "127.0.0.1", 9))
		{
			m_listenClient = client;
		}
	}
	else
	{
		InvalidatePlayer(pPlayer);
	}

	return origRet;
}

void PlayerManager::OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer)
{
	g_Logger.MessageFormat("[OnClientConnected] - %d, %s, %s, %s\n", slot.Get(), pszName, pszNetworkID, pszAddress);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	GetOnClientConnectedListenerManager().Notify(pPlayer->m_slot.Get());
}

void PlayerManager::OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid)
{
	g_Logger.MessageFormat("[OnClientPutInServer] - %d, %s, %d\n", slot.Get(), pszName, type);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (!pPlayer->IsConnected())
	{
		pPlayer->m_bFakeClient = true;

		if (!OnClientConnect(slot, pszName, 0, "127.0.0.1", false, new CBufferStringGrowable<255>()))
		{
			pPlayer->Kick("Bot rejected");
			return;
		}

		GetOnClientConnect_PostListenerManager().Notify(pPlayer->m_slot.Get());
	}

	pPlayer->Connect();
	m_playerCount++;

	// pPlayer->m_info = playerinfo->GetPlayerInfo(pEntity);

	GetOnClientPutInServerListenerManager().Notify(pPlayer->m_slot.Get());
}

void PlayerManager::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID)
{
	g_Logger.MessageFormat("[OnClientDisconnect] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (pPlayer->IsConnected())
	{
		GetOnClientDisconnectListenerManager().Notify(pPlayer->m_slot.Get(), (int)reason);
	}

	if (pPlayer->WasCountedAsInGame())
	{
		m_playerCount--;
	}
}

void PlayerManager::OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID)
{
	g_Logger.MessageFormat("[OnClientDisconnect_Post] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];
	if (!pPlayer->IsConnected())
	{
		/* We don't care, prevent a double call */
		return;
	}

	InvalidatePlayer(pPlayer);

	GetOnClientDisconnect_PostListenerManager().Notify(pPlayer->m_slot.Get(), (int)reason);
}

void PlayerManager::OnClientCommand(CPlayerSlot slot, const CCommand& args) const
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(slot.Get());
	if (!pPlayer)
		return;

	if (args.ArgC() > 1 && stricmp(args.Arg(0), "vban") == 0)
	{
		// clients just refuse to send vban for indexes over 32 and all 4 fields are just the same number, so we only get the first one
		// for (int i = 1; (i < args.ArgC()) && (i < 3); i++) {
		unsigned int mask = 0;
		sscanf(args.Arg(1), "%x", &mask);

		pPlayer->m_selfMutes.SetDWord(0, mask);
		//}
	}
}

void PlayerManager::OnClientActive(CPlayerSlot slot, bool bLoadGame) const
{
	g_Logger.MessageFormat("[OnClientActive] - %d\n", slot.Get());

	GetOnClientActiveListenerManager().Notify(slot.Get(), bLoadGame);
}

void PlayerManager::OnLevelShutdown()
{
	g_Logger.Message("[OnLevelShutdown]\n");

	for (int i = 0; i <= MaxClients(); ++i)
	{
		auto& player = m_players[i];
		if (player.IsConnected())
		{
			OnClientDisconnect(player.m_slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, player.GetName().c_str(), 0, player.GetIpAddress().c_str());
			OnClientDisconnect_Post(player.m_slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, player.GetName().c_str(), 0, player.GetIpAddress().c_str());
		}
	}

	m_playerCount = 0;
}

int PlayerManager::ListenClient() const
{
	return m_listenClient;
}

int PlayerManager::NumPlayers() const
{
	return m_playerCount;
}

int PlayerManager::MaxClients() const
{
	return gpGlobals->maxClients;
}

CPlayer* PlayerManager::GetPlayerBySlot(int client) const
{
	if (client > MaxClients() || client < 0)
	{
		return nullptr;
	}

	return const_cast<CPlayer*>(&m_players[client]);
}

// CPlayer *PlayerManager::GetClientOfUserId(int user_id) const
//{
//     if (user_id < 0 || user_id > USHRT_MAX)
//     {
//         return nullptr;
//     }
//
//     int client = m_userIdLookup[user_id];
//
//     /* Verify the userid.  The cache can get messed up with older
//      * Valve engines.
//      * check before invoking this backwards compat code.
//      */
//     if (client)
//     {
//         CPlayer *player = GetPlayerByIndex(client);
//         if (player && player->IsConnected())
//         {
//             int realUserId = ExcUseridFromEdict(player->GetEdict());
//             if (realUserId == user_id)
//             {
//                 return player;
//             }
//         }
//     }
//
//     /* If we can't verify the userid, we have to do a manual loop */
//     CPlayer *player;
//     auto index = ExcIndexFromUserid(user_id);
//     player = GetPlayerByIndex(index);
//     if (player && player->IsConnected())
//     {
//         m_userIdLookup[user_id] = index;
//         return player;
//     }
//
//     return nullptr;
// }

void PlayerManager::InvalidatePlayer(CPlayer* pPlayer)
{
	auto userid = g_pEngineServer2->GetPlayerUserId(pPlayer->m_slot);
	if (userid.Get() != -1)
		m_userIdLookup.erase(userid.Get());

	pPlayer->Disconnect();
}

PlayerManager g_PlayerManager;