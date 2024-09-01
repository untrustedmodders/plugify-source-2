#include "player_manager.h"
#include "listeners.h"
#include "timer_system.h"

#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/utils.h>

#include <eiface.h>
#include <game/server/iplayerinfo.h>
#include <inetchannelinfo.h>
#include <iserver.h>

CPlayer::CPlayer() = default;
CPlayer::~CPlayer() = default;

void CPlayer::Initialize(std::string name, std::string ip, CPlayerSlot slot)
{
	m_bConnected = true;
	m_iSlot = slot;
	m_name = std::move(name);
	m_ipAddress = std::move(ip);
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
		return g_pEngineServer2->IsClientFullyAuthenticated(m_iSlot);
	}
	return false;
}

void CPlayer::Authorize()
{
	m_bAuthorized = true;
}

void CPlayer::SetName(std::string name)
{
	m_name = std::move(name);
}

INetChannelInfo* CPlayer::GetNetInfo() const
{
	return g_pEngineServer2->GetPlayerNetInfo(m_iSlot);
}

bool CPlayer::IsInGame() const
{
	return m_bInGame;
}

void CPlayer::Kick()
{
	g_pEngineServer2->DisconnectClient(m_iSlot, NETWORK_DISCONNECT_KICKED);
}

const char* CPlayer::GetKeyValue(const char* key) const
{
	return g_pEngineServer2->GetClientConVarValue(m_iSlot, key);
}

const std::string& CPlayer::GetIpAddress() const
{
	return m_ipAddress;
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
	return GetNetInfo()->GetAvgLatency();
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
	m_bFakeClient = false;
	m_bAuthorized = false;
	m_ipAddress.clear();
}

const CSteamID* CPlayer::GetSteamId() const
{
	return m_steamId;
}

void CPlayer::SetSteamId(const CSteamID* steam_id)
{
	m_steamId = steam_id;
}

uint64 CPlayer::GetAdminFlags() const
{
	return m_iAdminFlags;
}

void CPlayer::SetAdminFlags(uint64 adminFlags)
{
	m_iAdminFlags = adminFlags;
}

bool CPlayer::IsAdminFlagSet(uint64 flag)
{
	return !flag || (m_iAdminFlags & flag);
}

CPlayerManager::CPlayerManager() = default;

void CPlayerManager::RunAuthChecks()
{
	if (CTimerSystem::GetTickedTime() - m_lastAuthCheckTime < 0.5)
	{
		return;
	}

	m_lastAuthCheckTime = static_cast<float>(CTimerSystem::GetTickedTime());

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

void CPlayerManager::OnClientAuthorized(CPlayer* player) const
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientAuthorized] - %s, %lli\n", player->GetName().c_str(), player->GetSteamId()->ConvertToUint64());

	GetOnClientAuthorizedListenerManager().Notify(player->m_iSlot.Get(), player->GetSteamId()->ConvertToUint64());
}

bool CPlayerManager::OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason)
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientConnect] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (pPlayer->IsConnected())
	{
		OnClientDisconnect(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, pszName, xuid, pszNetworkID);
		OnClientDisconnect_Post(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, pszName, xuid, pszNetworkID);
	}

	pPlayer->Initialize(pszName, pszNetworkID, slot);

	m_bRefuseConnection = false;

	for (size_t i = 0; i < GetOnClientConnectListenerManager().GetCount(); ++i)
	{
		m_bRefuseConnection |= !GetOnClientConnectListenerManager().Notify(i, client, pszName, pszNetworkID);
	}

	return m_bRefuseConnection;
}

bool CPlayerManager::OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet)
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientConnect_Post] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (m_bRefuseConnection)
		origRet = false;

	if (origRet)
	{
		GetOnClientConnect_PostListenerManager().Notify(pPlayer->m_iSlot.Get());
	}
	else
	{
		InvalidatePlayer(pPlayer);
	}

	return origRet;
}

void CPlayerManager::OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer)
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientConnected] - %d, %s, %s, %s\n", slot.Get(), pszName, pszNetworkID, pszAddress);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];
	//pPlayer->SetUnauthenticatedSteamId(new CSteamID(xuid));

	GetOnClientConnectedListenerManager().Notify(pPlayer->m_iSlot.Get());
}

void CPlayerManager::OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid)
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientPutInServer] - %d, %s, %d\n", slot.Get(), pszName, type);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (!pPlayer->IsConnected())
	{
		pPlayer->m_bFakeClient = true;

		/*CBufferStringGrowable<255> buffer;
		if (!OnClientConnect(slot, pszName, 0, "127.0.0.1", false, &buffer))
		{
			pPlayer->Kick();
			return;
		}*/

		GetOnClientConnect_PostListenerManager().Notify(pPlayer->m_iSlot.Get());
	}

	pPlayer->Connect();
	m_playerCount++;

	// pPlayer->m_info = playerinfo->GetPlayerInfo(pEntity);

	GetOnClientPutInServerListenerManager().Notify(pPlayer->m_iSlot.Get());
}

void CPlayerManager::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID)
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientDisconnect] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (pPlayer->IsConnected())
	{
		GetOnClientDisconnectListenerManager().Notify(pPlayer->m_iSlot.Get(), (int)reason);
	}

	if (pPlayer->IsInGame())
	{
		m_playerCount--;
	}
}

void CPlayerManager::OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID)
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientDisconnect_Post] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];
	if (!pPlayer->IsConnected())
	{
		/* We don't care, prevent a double call */
		return;
	}

	InvalidatePlayer(pPlayer);

	GetOnClientDisconnect_PostListenerManager().Notify(pPlayer->m_iSlot.Get(), (int)reason);
}

void CPlayerManager::OnClientActive(CPlayerSlot slot, bool bLoadGame) const
{
	g_Logger.LogFormat(LS_DEBUG, "[OnClientActive] - %d\n", slot.Get());

	GetOnClientActiveListenerManager().Notify(slot.Get(), bLoadGame);
}

void CPlayerManager::OnLevelShutdown()
{
	g_Logger.Log(LS_DEBUG, "[OnLevelShutdown]\n");

	for (int i = 0; i <= MaxClients(); ++i)
	{
		auto& player = m_players[i];
		if (player.IsConnected())
		{
			OnClientDisconnect(player.m_iSlot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, player.GetName().c_str(), 0, player.GetIpAddress().c_str());
			OnClientDisconnect_Post(player.m_iSlot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, player.GetName().c_str(), 0, player.GetIpAddress().c_str());
		}
	}

	m_playerCount = 0;
}

int CPlayerManager::NumPlayers() const
{
	return m_playerCount;
}

int CPlayerManager::MaxClients() const
{
	return gpGlobals->maxClients;
}

CPlayer* CPlayerManager::GetPlayerBySlot(CPlayerSlot slot) const
{
	int client = slot.Get();

	if (client > MaxClients() || client < 0)
	{
		return nullptr;
	}

	return const_cast<CPlayer*>(&m_players[client]);
}

// In userids, the lower byte is always the player slot
CPlayerSlot CPlayerManager::GetSlotFromUserId(uint16 userid) const
{
	return CPlayerSlot(userid & 0xFF);
}

CPlayer* CPlayerManager::GetPlayerFromUserId(uint16 userid) const
{
	uint8 client = userid & 0xFF;

	if (client >= MaxClients())
	{
		return nullptr;
	}

	return const_cast<CPlayer*>(&m_players[client]);
}

CPlayer* CPlayerManager::GetPlayerFromSteamId(uint64 steamid) const
{
	for (int i = 0; i <= MaxClients(); ++i)
	{
		auto& player = m_players[i];
		if (player.IsConnected() && player.IsAuthorized() && player.GetSteamId()->ConvertToUint64() == steamid)
		{
			return const_cast<CPlayer*>(&player);
		}
	}

	return nullptr;
}

TargetType CPlayerManager::TargetPlayerString(int caller, const char* target, std::vector<int>& clients)
{
	TargetType targetType = TargetType::NONE;
	if (!V_stricmp(target, "@me"))
		targetType = TargetType::SELF;
	else if (!V_stricmp(target, "@all"))
		targetType = TargetType::ALL;
	else if (!V_stricmp(target, "@t"))
		targetType = TargetType::T;
	else if (!V_stricmp(target, "@ct"))
		targetType = TargetType::CT;
	else if (!V_stricmp(target, "@spec"))
		targetType = TargetType::SPECTATOR;
	else if (!V_stricmp(target, "@random"))
		targetType = TargetType::RANDOM;
	else if (!V_stricmp(target, "@randomt"))
		targetType = TargetType::RANDOM_T;
	else if (!V_stricmp(target, "@randomct"))
		targetType = TargetType::RANDOM_CT;

	clients.clear();
	
	if (targetType == TargetType::SELF && caller != -1)
	{
		clients.push_back(caller);
	}
	else if (targetType == TargetType::ALL)
	{
		for (int i = 0; i < gpGlobals->maxClients; i++)
		{
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			clients.push_back(i + 1);
		}
	}
	else if (targetType >= TargetType::SPECTATOR)
	{
		for (int i = 0; i < gpGlobals->maxClients; i++)
		{
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (player->m_iTeamNum() != (targetType == TargetType::T ? CS_TEAM_T : targetType == TargetType::CT ? CS_TEAM_CT : CS_TEAM_SPECTATOR))
				continue;

			clients.push_back(i + 1);
		}
	}
	else if (targetType >= TargetType::RANDOM && targetType <= TargetType::RANDOM_CT)
	{
		int attempts = 0;

		while (clients.empty() == 0 && attempts < 10000)
		{
			int slot = rand() % (gpGlobals->maxClients - 1);

			// Prevent infinite loop
			attempts++;

			if (!m_players[slot].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(slot);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (targetType >= TargetType::RANDOM_T && (player->m_iTeamNum() != (targetType == TargetType::RANDOM_T ? CS_TEAM_T : CS_TEAM_CT)))
				continue;

			clients.push_back(slot + 1);
		}
	}
	else if (*target == '#')
	{
		int userid = V_StringToUint16(target + 1, -1);

		if (userid != -1)
		{
			targetType = TargetType::PLAYER;
			CBasePlayerController* player = utils::GetController(GetSlotFromUserId(userid).Get());
			if (player && player->IsController() && player->IsConnected())
			{
				clients.push_back(GetSlotFromUserId(userid).Get() + 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < gpGlobals->maxClients; i++)
		{
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (V_stristr(player->GetPlayerName(), target))
			{
				targetType = TargetType::PLAYER;
				clients.push_back(i + 1);
				break;
			}
		}
	}

	return targetType;
}

void CPlayerManager::InvalidatePlayer(CPlayer* pPlayer)
{
	pPlayer->Disconnect();
}

CPlayerManager g_PlayerManager;