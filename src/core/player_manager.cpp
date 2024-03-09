#include "player_manager.h"
#include "listeners.h"
#include "timer_system.h"

#include <core/sdk/entity/cbaseplayercontroller.h>
#include <utils/utils.h>

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

int CPlayer::GetUserId()
{
	if (m_iUserId == -1)
	{
		m_iUserId = g_pEngineServer2->GetPlayerUserId(m_iSlot).Get();
	}

	return m_iUserId;
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
	m_bFakeClient = false;
	m_iUserId = -1;
	m_bAuthorized = false;
	m_ipAddress.clear();
	m_selfMutes.ClearAll();
	std::fill(m_listenMap.begin(), m_listenMap.end(), Listen_Default);
	m_voiceFlag = 0;
}

const CSteamID* CPlayer::GetSteamId() const
{
	return m_steamId;
}

void CPlayer::SetSteamId(const CSteamID* steam_id)
{
	m_steamId = steam_id;
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
	g_Logger.MessageFormat("[OnClientAuthorized] - %s, %lli\n", player->GetName().c_str(), player->GetSteamId()->ConvertToUint64());

	GetOnClientAuthorizedListenerManager().Notify(player->m_iSlot.Get(), player->GetSteamId()->ConvertToUint64());
}

bool CPlayerManager::OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason)
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

bool CPlayerManager::OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet)
{
	g_Logger.MessageFormat("[OnClientConnect_Post] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (m_refuseConnection)
		origRet = false;

	if (origRet)
	{
		GetOnClientConnect_PostListenerManager().Notify(pPlayer->m_iSlot.Get());

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

void CPlayerManager::OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer)
{
	g_Logger.MessageFormat("[OnClientConnected] - %d, %s, %s, %s\n", slot.Get(), pszName, pszNetworkID, pszAddress);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	GetOnClientConnectedListenerManager().Notify(pPlayer->m_iSlot.Get());
}

void CPlayerManager::OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid)
{
	g_Logger.MessageFormat("[OnClientPutInServer] - %d, %s, %d\n", slot.Get(), pszName, type);

	int client = slot.Get();
	CPlayer* pPlayer = &m_players[client];

	if (!pPlayer->IsConnected())
	{
		pPlayer->m_bFakeClient = true;

		CBufferStringGrowable<255> buffer;
		if (!OnClientConnect(slot, pszName, 0, "127.0.0.1", false, &buffer))
		{
			pPlayer->Kick("Bot rejected");
			return;
		}

		GetOnClientConnect_PostListenerManager().Notify(pPlayer->m_iSlot.Get());
	}

	pPlayer->Connect();
	m_playerCount++;

	// pPlayer->m_info = playerinfo->GetPlayerInfo(pEntity);

	GetOnClientPutInServerListenerManager().Notify(pPlayer->m_iSlot.Get());
}

void CPlayerManager::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID)
{
	g_Logger.MessageFormat("[OnClientDisconnect] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

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
	g_Logger.MessageFormat("[OnClientDisconnect_Post] - %d, %s, %s\n", slot.Get(), pszName, pszNetworkID);

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

void CPlayerManager::OnClientCommand(CPlayerSlot slot, const CCommand& args) const
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

void CPlayerManager::OnClientActive(CPlayerSlot slot, bool bLoadGame) const
{
	g_Logger.MessageFormat("[OnClientActive] - %d\n", slot.Get());

	GetOnClientActiveListenerManager().Notify(slot.Get(), bLoadGame);
}

void CPlayerManager::OnLevelShutdown()
{
	g_Logger.Message("[OnLevelShutdown]\n");

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

int CPlayerManager::ListenClient() const
{
	return m_listenClient;
}

int CPlayerManager::NumPlayers() const
{
	return m_playerCount;
}

int CPlayerManager::MaxClients() const
{
	return gpGlobals->maxClients;
}

CPlayer* CPlayerManager::GetPlayerBySlot(int client) const
{
	if (client > MaxClients() || client < 0)
	{
		return nullptr;
	}

	return const_cast<CPlayer*>(&m_players[client]);
}

// In userids, the lower byte is always the player slot
CPlayerSlot CPlayerManager::GetSlotFromUserId(uint16 userid)
{
	return CPlayerSlot(userid & 0xFF);
}

CPlayer* CPlayerManager::GetPlayerFromUserId(uint16 userid)
{
	uint8 client = userid & 0xFF;

	if (client >= MaxClients())
	{
		return nullptr;
	}

	return const_cast<CPlayer*>(&m_players[client]);
}

CPlayer* CPlayerManager::GetPlayerFromSteamId(uint64 steamid)
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

ETargetType CPlayerManager::TargetPlayerString(int caller, const char* target, std::vector<int>& clients)
{
	ETargetType targetType = ETargetType::NONE;
	if (!V_stricmp(target, "@me"))
		targetType = ETargetType::SELF;
	else if (!V_stricmp(target, "@all"))
		targetType = ETargetType::ALL;
	else if (!V_stricmp(target, "@t"))
		targetType = ETargetType::T;
	else if (!V_stricmp(target, "@ct"))
		targetType = ETargetType::CT;
	else if (!V_stricmp(target, "@spec"))
		targetType = ETargetType::SPECTATOR;
	else if (!V_stricmp(target, "@random"))
		targetType = ETargetType::RANDOM;
	else if (!V_stricmp(target, "@randomt"))
		targetType = ETargetType::RANDOM_T;
	else if (!V_stricmp(target, "@randomct"))
		targetType = ETargetType::RANDOM_CT;

	clients.clear();
	
	if (targetType == ETargetType::SELF && caller != -1)
	{
		clients.push_back(caller);
	}
	else if (targetType == ETargetType::ALL)
	{
		for (int i = 0; i < gpGlobals->maxClients; i++)
		{
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			clients.push_back(i);
		}
	}
	else if (targetType >= ETargetType::SPECTATOR)
	{
		for (int i = 0; i < gpGlobals->maxClients; i++)
		{
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (player->m_iTeamNum() != (targetType == ETargetType::T ? CS_TEAM_T : targetType == ETargetType::CT ? CS_TEAM_CT : CS_TEAM_SPECTATOR))
				continue;

			clients.push_back(i);
		}
	}
	else if (targetType >= ETargetType::RANDOM && targetType <= ETargetType::RANDOM_CT)
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

			if (targetType >= ETargetType::RANDOM_T && (player->m_iTeamNum() != (targetType == ETargetType::RANDOM_T ? CS_TEAM_T : CS_TEAM_CT)))
				continue;

			clients.push_back(slot);
		}
	}
	else if (*target == '#')
	{
		int userid = V_StringToUint16(target + 1, -1);

		if (userid != -1)
		{
			targetType = ETargetType::PLAYER;
			CBasePlayerController* player = utils::GetController(GetSlotFromUserId(userid).Get());
			if (player && player->IsController() && player->IsConnected())
			{
				clients.push_back(GetSlotFromUserId(userid).Get());
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
				targetType = ETargetType::PLAYER;
				clients.push_back(i);
				break;
			}
		}
	}

	return targetType;
}

void CPlayerManager::InvalidatePlayer(CPlayer* pPlayer)
{
	auto userid = g_pEngineServer2->GetPlayerUserId(pPlayer->m_iSlot);
	if (userid.Get() != -1)
		m_userIdLookup.erase(userid.Get());

	pPlayer->Disconnect();
}

CPlayerManager g_PlayerManager;