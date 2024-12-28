#include "player_manager.hpp"
#include "listeners.hpp"
#include "timer_system.hpp"

#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/entity/ccsplayercontroller.h>
#include <core/sdk/utils.h>

#include <eiface.h>
#include <game/server/iplayerinfo.h>
#include <inetchannelinfo.h>
#include <iserver.h>

CPlayerManager g_PlayerManager;

CCSPlayerController* CPlayer::GetController() const {
	auto entIndex = GetEntityIndex();
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(entIndex));
	if (!ent) {
		return nullptr;
	}

	return ent->IsController() ? static_cast<CCSPlayerController*>(ent) : nullptr;
}

CBasePlayerPawn* CPlayer::GetCurrentPawn() const {
	auto controller = GetController();
	return controller ? controller->GetCurrentPawn() : nullptr;
}

CCSPlayerPawn* CPlayer::GetPlayerPawn() const {
	auto controller = GetController();
	return controller ? controller->GetPlayerPawn() : nullptr;
}

CCSObserverPawn* CPlayer::GetObserverPawn() const {
	auto controller = GetController();
	return controller ? reinterpret_cast<CCSObserverPawn*>(controller->GetObserverPawn()) : nullptr;
}

CServerSideClientBase* CPlayer::GetClient() const {
	return utils::GetClientBySlot(GetPlayerSlot());
}

bool CPlayer::IsAuthenticated() const {
	auto client = GetClient();
	return client && client->IsConnected() && !client->IsFakeClient() && g_pEngineServer->IsClientFullyAuthenticated(GetPlayerSlot());
}

bool CPlayer::IsConnected() const {
	auto client = GetClient();
	return client && client->IsConnected();
}

bool CPlayer::IsInGame() const {
	auto client = GetClient();
	return client && client->IsActive();
}

bool CPlayer::IsFakeClient() const {
	auto client = GetClient();
	return client && client->IsFakeClient();
}

bool CPlayer::IsSourceTV() const {
	auto client = GetClient();
	return client && client->IsHLTV();
}

bool CPlayer::IsAlive() const {
	auto pawn = GetPlayerPawn();
	return pawn && pawn->IsAlive();
}

bool CPlayer::IsValidClient() const {
	if (!utils::IsPlayerSlot(m_iSlot)) {
		return false;
	}

	CServerSideClientBase* client = GetClient();
	if (!client) {
		return false;
	}

	auto controller = utils::GetController(client->GetPlayerSlot());
	return controller && controller->IsController() && client->IsConnected() && client->IsInGame() && !client->IsHLTV();
}

const char* CPlayer::GetName() const {
	auto controller = GetController();
	return controller ? controller->GetPlayerName() : "<blank>";
}

const char* CPlayer::GetIpAddress() const {
	auto client = GetClient();
	return client ? client->GetNetChannel()->GetRemoteAddress().ToString(true) : nullptr;
}

CSteamID CPlayer::GetSteamId(bool validated) const {
	bool authenticated = IsAuthenticated();
	if (validated && !authenticated) {
		return k_steamIDNil;
	}

	CServerSideClientBase* client = GetClient();
	if (client && authenticated) {
		return client->GetClientSteamID();
	}

	return m_unauthenticatedSteamID;
}

INetChannelInfo* CPlayer::GetNetInfo() const {
	return g_pEngineServer2->GetPlayerNetInfo(m_iSlot);
}

float CPlayer::GetTimeConnected() const {
	if (!IsConnected() || IsFakeClient()) {
		return 0;
	}

	return GetNetInfo()->GetTimeConnected();
}

float CPlayer::GetLatency() const {
	return GetNetInfo()->GetAvgLatency();
}

void CPlayer::Kick(const char* internalReason, ENetworkDisconnectionReason reason) const {
	g_pEngineServer->KickClient(GetPlayerSlot(), internalReason, reason);
}

void CPlayerManager::OnSteamAPIActivated() {
	m_CallbackValidateAuthTicketResponse.Register(this, &CPlayerManager::OnValidateAuthTicket);
}

void CPlayerManager::OnValidateAuthTicket(ValidateAuthTicketResponse_t* pResponse) {
	uint64 iSteamId = pResponse->m_SteamID.ConvertToUint64();

	g_Logger.LogFormat(LS_DEBUG, "OnValidateAuthTicket %s: SteamID=%llu Response=%d\n", iSteamId, pResponse->m_eAuthSessionResponse);

	for (const CPlayer& player : m_players) {
		if (!player.IsConnected() || player.IsFakeClient() || !(player.GetUnauthenticatedSteamId64() == iSteamId))
			continue;

		switch (pResponse->m_eAuthSessionResponse) {
			case k_EAuthSessionResponseOK: {
				GetOnClientAuthorizedListenerManager().Notify(player.GetPlayerSlot().Get(), player.GetSteamId().ConvertToUint64());
				return;
			}
			default:
				break;
		}
	}
}

bool CPlayerManager::OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason) {
	int client = slot.Get();
	CPlayer& player = m_players[client];

	if (player.IsConnected()) {
		OnClientDisconnect(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, pszName, xuid, pszNetworkID);
		OnClientDisconnect_Post(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, pszName, xuid, pszNetworkID);
	}

	player.Init(client);
	player.SetUnauthenticatedSteamID(xuid);

	m_refuseConnection.reset(client);

	for (size_t i = 0; i < GetOnClientConnectListenerManager().GetCount(); ++i) {
		bool connected = GetOnClientConnectListenerManager().Notify(i, client, pszName, pszNetworkID);
		if (!connected) {
			m_refuseConnection.set(client);
		}
	}

	return !m_refuseConnection.test(client);
}

bool CPlayerManager::OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet) {
	int client = slot.Get();
	CPlayer& player = m_players[client];

	if (m_refuseConnection.test(client)) {
		m_refuseConnection.reset(client);
		origRet = false;
	}

	if (origRet) {
		GetOnClientConnect_PostListenerManager().Notify(player.GetPlayerSlot().Get());
	} else {
		player.Reset();
	}

	return origRet;
}

void CPlayerManager::OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer) {
	int client = slot.Get();
	CPlayer& player = m_players[client];
	//player.SetUnauthenticatedSteamId(xuid);

	GetOnClientConnectedListenerManager().Notify(player.GetPlayerSlot().Get());
}

void CPlayerManager::OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid) {
	int client = slot.Get();
	CPlayer& player = m_players[client];

	if (!player.IsConnected()) {
		if (!OnClientConnect(slot, pszName, 0, "127.0.0.1", false, new CBufferStringGrowable<255>())) {
			/* :TODO: kick the bot if it's rejected */
			return;
		}

		GetOnClientConnect_PostListenerManager().Notify(player.GetPlayerSlot().Get());
	}

	GetOnClientPutInServerListenerManager().Notify(player.GetPlayerSlot().Get());
}

void CPlayerManager::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID) {
	int client = slot.Get();
	CPlayer& player = m_players[client];

	if (player.IsConnected()) {
		GetOnClientDisconnectListenerManager().Notify(player.GetPlayerSlot().Get(), (int) reason);
	}
}

void CPlayerManager::OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID) {
	int client = slot.Get();
	CPlayer& player = m_players[client];
	if (!player.IsConnected()) {
		/* We don't care, prevent a double call */
		return;
	}

	GetOnClientDisconnect_PostListenerManager().Notify(player.GetPlayerSlot().Get(), (int) reason);

	player.Reset();
}

void CPlayerManager::OnClientActive(CPlayerSlot slot, bool bLoadGame) const {
	GetOnClientActiveListenerManager().Notify(slot.Get(), bLoadGame);
}

void CPlayerManager::OnLevelShutdown() {
	for (int i = 0; i <= MaxClients(); ++i) {
		auto& player = m_players[i];
		if (player.IsConnected()) {
			CPlayerSlot slot = player.GetPlayerSlot();
			OnClientDisconnect(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, player.GetName(), 0, player.GetIpAddress());
			OnClientDisconnect_Post(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID, player.GetName(), 0, player.GetIpAddress());
		}
	}
}

int CPlayerManager::MaxClients() {
	return gpGlobals ? gpGlobals->maxClients : -1;
}

CPlayer* CPlayerManager::ToPlayer(CServerSideClientBase* pClient) const {
	return ToPlayer(CPlayerSlot(pClient->GetPlayerSlot()));
}

CPlayer* CPlayerManager::ToPlayer(CPlayerPawnComponent* component) const {
	return ToPlayer(component->GetPawn());
}

CPlayer* CPlayerManager::ToPlayer(CBasePlayerController* controller) const {
	if (!controller) {
		return nullptr;
	}

	return ToPlayer(CPlayerSlot(controller->GetPlayerSlot()));
}

CPlayer* CPlayerManager::ToPlayer(CBasePlayerPawn* pawn) const {
	if (!pawn) {
		return nullptr;
	}

	CBasePlayerController* controller = pawn->GetController();
	if (!controller) {
		return nullptr;
	}

	return ToPlayer(controller);
}

CPlayer* CPlayerManager::ToPlayer(CPlayerSlot slot) const {
	if (utils::IsPlayerSlot(slot)) {
		return const_cast<CPlayer*>(&m_players.at(slot.Get()));
	}

	return nullptr;
}

CPlayer* CPlayerManager::ToPlayer(CEntityIndex entIndex) const {
	CBaseEntity* ent = static_cast<CBaseEntity*>(GameEntitySystem()->GetEntityInstance(entIndex));
	if (!ent) {
		return nullptr;
	}

	if (ent->IsPawn()) {
		return ToPlayer(static_cast<CBasePlayerPawn*>(ent));
	}

	if (ent->IsController()) {
		return ToPlayer(static_cast<CBasePlayerController*>(ent));
	}

	return nullptr;
}

CPlayer* CPlayerManager::ToPlayer(CPlayerUserId userID) const {
	for (int slot = 0; slot < MaxClients(); slot++) {
		if (g_pEngineServer->GetPlayerUserId(CPlayerSlot(slot)) == userID.Get()) {
			return ToPlayer(CPlayerSlot(slot));
		}
	}

	return nullptr;
}

CPlayer* CPlayerManager::ToPlayer(CSteamID steamid, bool validate) const {
	auto steam64 = steamid.ConvertToUint64();
	for (auto& player: GetOnlinePlayers()) {
		if (player->GetSteamId64(validate) == steam64) {
			return player;
		}
	}

	return nullptr;
}

std::vector<CPlayer*> CPlayerManager::GetOnlinePlayers() const {
	std::vector<CPlayer*> players;
	players.reserve(MAXPLAYERS);
	for (auto& player: m_players) {
		if (utils::IsPlayerSlot(player.GetPlayerSlot())) {
			players.emplace_back(const_cast<CPlayer*>(&player));
		}
	}

	return players;
}

TargetType CPlayerManager::TargetPlayerString(int caller, std::string_view target, plg::vector<int>& clients) {
	TargetType targetType = TargetType::NONE;

	if (target == "@me")
		targetType = TargetType::SELF;
	else if (target == "@all")
		targetType = TargetType::ALL;
	else if (target == "@t")
		targetType = TargetType::T;
	else if (target == "@ct")
		targetType = TargetType::CT;
	else if (target == "@spec")
		targetType = TargetType::SPECTATOR;
	else if (target == "@random")
		targetType = TargetType::RANDOM;
	else if (target == "@randomt")
		targetType = TargetType::RANDOM_T;
	else if (target == "@randomct")
		targetType = TargetType::RANDOM_CT;

	clients.clear();

	if (targetType == TargetType::SELF && caller != -1) {
		clients.push_back(caller);
	} else if (targetType == TargetType::ALL) {
		for (int i = 0; i < MaxClients(); ++i) {
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			clients.push_back(i);
		}
	} else if (targetType >= TargetType::SPECTATOR) {
		for (int i = 0; i < MaxClients(); ++i) {
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (player->m_iTeamNum() != (targetType == TargetType::T ? CS_TEAM_T : targetType == TargetType::CT ? CS_TEAM_CT
																												: CS_TEAM_SPECTATOR))
				continue;

			clients.push_back(i);
		}
	} else if (targetType >= TargetType::RANDOM && targetType <= TargetType::RANDOM_CT) {
		int attempts = 0;

		while (clients.empty() == 0 && attempts < 10000) {
			int i = rand() % (MaxClients() - 1);

			// Prevent infinite loop
			attempts++;

			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (targetType >= TargetType::RANDOM_T && (player->m_iTeamNum() != (targetType == TargetType::RANDOM_T ? CS_TEAM_T : CS_TEAM_CT)))
				continue;

			clients.push_back(i);
		}
	} else if (target.starts_with('#')) {
		int userid = V_StringToUint16(target.substr(1).data(), -1);

		if (userid != -1) {
			targetType = TargetType::PLAYER;
			CPlayerSlot slot = utils::GetSlotFromUserId(userid);
			CBasePlayerController* player = utils::GetController(slot);
			if (player && player->IsController() && player->IsConnected()) {
				clients.push_back(slot.Get());
			}
		}
	} else {
		for (int i = 0; i < MaxClients(); ++i) {
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (V_stristr(player->GetPlayerName(), target.data())) {
				targetType = TargetType::PLAYER;
				clients.push_back(i);
				break;
			}
		}
	}

	return targetType;
}