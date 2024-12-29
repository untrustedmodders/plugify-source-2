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
	if (!utils::IsPlayerSlot(GetPlayerSlot())) {
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
				GetOnClientAuthorizedListenerManager().Notify(player.GetPlayerSlot(), player.GetSteamId().ConvertToUint64());
				return;
			}
			default:
				break;
		}
	}
}

thread_local bool s_refuseConnection;

bool CPlayerManager::OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID) {
	CPlayer* pPlayer = ToPlayer(slot);
	if (pPlayer) {
		pPlayer->Init(slot);
		pPlayer->SetUnauthenticatedSteamID(xuid);

		if (pPlayer->IsConnected()) {
			OnClientDisconnect(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID);
			OnClientDisconnect_Post(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID);
		}

		s_refuseConnection = false;

		for (size_t i = 0; i < GetOnClientConnectListenerManager().GetCount(); ++i) {
			s_refuseConnection |= !GetOnClientConnectListenerManager().Notify(i, slot, pszName, pszNetworkID);
		}

		return s_refuseConnection;
	}

	return true;
}

bool CPlayerManager::OnClientConnect_Post(CPlayerSlot slot, bool bOrigRet) {
	CPlayer* pPlayer = ToPlayer(slot);
	if (pPlayer) {
		if (s_refuseConnection) {
			bOrigRet = false;
		}

		if (bOrigRet) {
			GetOnClientConnect_PostListenerManager().Notify(slot);
		} else {
			pPlayer->Reset();
		}
	}

	return bOrigRet;
}

void CPlayerManager::OnClientConnected(CPlayerSlot slot) {
	GetOnClientConnectedListenerManager().Notify(slot);
}

void CPlayerManager::OnClientPutInServer(CPlayerSlot slot, char const* pszName) {
	CPlayer* pPlayer = ToPlayer(slot);
	if (pPlayer) {
		if (!pPlayer->IsConnected()) {
			if (!OnClientConnect(slot, pszName, 0, "127.0.0.1"))
				return;

			GetOnClientConnect_PostListenerManager().Notify(slot);
		}

		GetOnClientPutInServerListenerManager().Notify(slot);
	}
}

void CPlayerManager::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason) {
	GetOnClientDisconnectListenerManager().Notify(slot, reason);
}

void CPlayerManager::OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason) {
	GetOnClientDisconnect_PostListenerManager().Notify(slot, reason);

	CPlayer* pPlayer = ToPlayer(slot);
	if (pPlayer) {
		pPlayer->Reset();
	}
}

void CPlayerManager::OnClientActive(CPlayerSlot slot, bool bLoadGame) const {
	GetOnClientActiveListenerManager().Notify(slot, bLoadGame);
}

void CPlayerManager::OnLevelShutdown() {
	for (int i = 0; i <= MaxClients(); ++i) {
		CPlayer& player = m_players[i];
		CPlayerSlot slot = player.GetPlayerSlot();
		if (utils::IsPlayerSlot(slot)) {
			OnClientDisconnect(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID);
			OnClientDisconnect_Post(slot, ENetworkDisconnectionReason::NETWORK_DISCONNECT_INVALID);
		}
	}
}

int CPlayerManager::MaxClients() {
	return gpGlobals ? gpGlobals->maxClients : -1;
}

CPlayer* CPlayerManager::ToPlayer(CServerSideClientBase* pClient) const {
	return ToPlayer(pClient->GetPlayerSlot());
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
		return const_cast<CPlayer*>(&m_players.at(slot));
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
		if (g_pEngineServer->GetPlayerUserId(slot) == userID) {
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
				clients.push_back(slot);
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