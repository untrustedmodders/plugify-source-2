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

PlayerManager g_PlayerManager;

CCSPlayerController* Player::GetController() const {
	auto entIndex = GetEntityIndex();
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(entIndex));
	if (!ent) {
		return nullptr;
	}

	return ent->IsController() ? static_cast<CCSPlayerController*>(ent) : nullptr;
}

CBasePlayerPawn* Player::GetCurrentPawn() const {
	auto controller = GetController();
	return controller ? controller->GetCurrentPawn() : nullptr;
}

CCSPlayerPawn* Player::GetPlayerPawn() const {
	auto controller = GetController();
	return controller ? controller->GetPlayerPawn() : nullptr;
}

CCSObserverPawn* Player::GetObserverPawn() const {
	auto controller = GetController();
	return controller ? reinterpret_cast<CCSObserverPawn*>(controller->GetObserverPawn()) : nullptr;
}

CServerSideClientBase* Player::GetClient() const {
	return utils::GetClientBySlot(GetPlayerSlot());
}

bool Player::IsAuthenticated() const {
	auto client = GetClient();
	return client && client->IsConnected() && !client->IsFakeClient() && g_pEngineServer2->IsClientFullyAuthenticated(GetPlayerSlot());
}

bool Player::IsConnected() const {
	auto client = GetClient();
	return client && client->IsConnected();
}

bool Player::IsInGame() const {
	auto client = GetClient();
	return client && client->IsActive();
}

bool Player::IsFakeClient() const {
	auto client = GetClient();
	return client && client->IsFakeClient();
}

bool Player::IsSourceTV() const {
	auto client = GetClient();
	return client && client->IsHLTV();
}

bool Player::IsAlive() const {
	auto pawn = GetPlayerPawn();
	return pawn && pawn->IsAlive();
}

bool Player::IsValidClient() const {
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

const char* Player::GetName() const {
	auto client = GetClient();
	return client ? client->GetClientName() : "<blank>";
}

const char* Player::GetIpAddress() const {
	auto client = GetClient();
	return client ? client->GetNetChannel()->GetRemoteAddress().ToString(true) : nullptr;
}

CSteamID Player::GetSteamId(bool validated) const {
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

INetChannelInfo* Player::GetNetInfo() const {
	return g_pEngineServer2->GetPlayerNetInfo(m_iSlot);
}

float Player::GetTimeConnected() const {
	if (!IsConnected() || IsFakeClient()) {
		return 0;
	}

	return GetNetInfo()->GetTimeConnected();
}

float Player::GetLatency() const {
	return GetNetInfo()->GetAvgLatency();
}

void Player::Kick(const char* internalReason, ENetworkDisconnectionReason reason) const {
	g_pEngineServer2->KickClient(GetPlayerSlot(), internalReason, reason);
}

void PlayerManager::OnSteamAPIActivated() {
	if (m_callbackRegistered)
		return;

	m_callbackRegistered = true;
	m_CallbackValidateAuthTicketResponse.Register(this, &PlayerManager::OnValidateAuthTicket);
}

void PlayerManager::OnValidateAuthTicket(ValidateAuthTicketResponse_t* pResponse) {
	if (pResponse->m_eAuthSessionResponse != k_EAuthSessionResponseOK)
		return;

	for (const Player& player : m_players) {
		CSteamID steamID = player.GetSteamId();
		if (steamID == pResponse->m_SteamID) {
			GetOnClientAuthenticatedListenerManager().Notify(player.GetPlayerSlot(), steamID.ConvertToUint64());
			return;
		}
	}
}

thread_local bool s_refuseConnection;

bool PlayerManager::OnClientConnect(CPlayerSlot slot, const char* name, uint64 xuid, const char* networkID) {
	Player* player = ToPlayer(slot);
	if (player) {
		player->Init(slot, xuid);

		s_refuseConnection = false;

		for (size_t i = 0; i < GetOnClientConnectListenerManager().GetCount(); ++i) {
			s_refuseConnection |= !GetOnClientConnectListenerManager().Notify(i, slot, name, networkID);
		}

		return s_refuseConnection;
	}

	return true;
}

bool PlayerManager::OnClientConnect_Post(CPlayerSlot slot, bool origRet) {
	Player* player = ToPlayer(slot);
	if (player) {
		if (s_refuseConnection) {
			origRet = false;
		}

		if (origRet) {
			GetOnClientConnect_PostListenerManager().Notify(slot);
		} else {
			player->Reset();
		}
	}

	return origRet;
}

void PlayerManager::OnClientConnected(CPlayerSlot slot) {
	GetOnClientConnectedListenerManager().Notify(slot);
}

void PlayerManager::OnClientPutInServer(CPlayerSlot slot, char const* name) {
	Player* player = ToPlayer(slot);
	if (player) {
		// For bots only
		if (player->GetPlayerSlot() == CPlayerSlot{-1}) {
			player->Init(slot, 0);
		}

		GetOnClientPutInServerListenerManager().Notify(slot);
	}
}

void PlayerManager::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason) {
	GetOnClientDisconnectListenerManager().Notify(slot, reason);
}

void PlayerManager::OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason) {
	GetOnClientDisconnect_PostListenerManager().Notify(slot, reason);

	Player* player = ToPlayer(slot);
	if (player) {
		player->Reset();
	}
}

void PlayerManager::OnClientActive(CPlayerSlot slot, bool bLoadGame) const {
	GetOnClientActiveListenerManager().Notify(slot, bLoadGame);
}

int PlayerManager::MaxClients() {
	return gpGlobals ? gpGlobals->maxClients : -1;
}

Player* PlayerManager::ToPlayer(CServerSideClientBase* client) const {
	return ToPlayer(client->GetPlayerSlot());
}

Player* PlayerManager::ToPlayer(CPlayerPawnComponent* component) const {
	return ToPlayer(component->GetPawn());
}

Player* PlayerManager::ToPlayer(CBasePlayerController* controller) const {
	if (!controller) {
		return nullptr;
	}

	return ToPlayer(CPlayerSlot(controller->GetPlayerSlot()));
}

Player* PlayerManager::ToPlayer(CBasePlayerPawn* pawn) const {
	if (!pawn) {
		return nullptr;
	}

	CBasePlayerController* controller = pawn->GetController();
	if (!controller) {
		return nullptr;
	}

	return ToPlayer(controller);
}

Player* PlayerManager::ToPlayer(CPlayerSlot slot) const {
	if (utils::IsPlayerSlot(slot)) {
		return const_cast<Player*>(&m_players.at(slot));
	}

	return nullptr;
}

Player* PlayerManager::ToPlayer(CEntityIndex entIndex) const {
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

Player* PlayerManager::ToPlayer(CPlayerUserId userID) const {
	for (int slot = 0; slot < MaxClients(); slot++) {
		if (g_pEngineServer2->GetPlayerUserId(slot) == userID) {
			return ToPlayer(CPlayerSlot(slot));
		}
	}

	return nullptr;
}

Player* PlayerManager::ToPlayer(CSteamID steamid, bool validate) const {
	auto steam64 = steamid.ConvertToUint64();
	for (auto& player: GetOnlinePlayers()) {
		if (player->GetSteamId64(validate) == steam64) {
			return player;
		}
	}

	return nullptr;
}

std::vector<Player*> PlayerManager::GetOnlinePlayers() const {
	std::vector<Player*> players;
	players.reserve(MAXPLAYERS);
	for (auto& player: m_players) {
		if (utils::IsPlayerSlot(player.GetPlayerSlot())) {
			players.emplace_back(const_cast<Player*>(&player));
		}
	}

	return players;
}

TargetType PlayerManager::TargetPlayerString(int caller, std::string_view target, plg::vector<int>& clients) {
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
		clients.emplace_back(caller);
	} else if (targetType == TargetType::ALL) {
		for (int i = 0; i < MaxClients(); ++i) {
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			clients.emplace_back(i);
		}
	} else if (targetType >= TargetType::SPECTATOR) {
		for (int i = 0; i < MaxClients(); ++i) {
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			if (player->m_iTeamNum() != (targetType == TargetType::T ? CS_TEAM_T : targetType == TargetType::CT ? CS_TEAM_CT : CS_TEAM_SPECTATOR))
				continue;

			clients.emplace_back(i);
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

			clients.emplace_back(i);
		}
	} else if (target.starts_with('#')) {
		if (auto slot = utils::string_to_int<int>(target.substr(1))) {
			targetType = TargetType::PLAYER;
			CBasePlayerController* player = utils::GetController(*slot);
			if (player && player->IsController() && player->IsConnected()) {
				clients.emplace_back(*slot);
			}
		}
	} else {
		for (int i = 0; i < MaxClients(); ++i) {
			if (!m_players[i].IsConnected())
				continue;

			CBasePlayerController* player = utils::GetController(i);

			if (!player || !player->IsController() || !player->IsConnected())
				continue;

			std::string_view playerName(player->GetPlayerName());
			if (playerName.find(playerName) != std::string_view::npos) {
				targetType = TargetType::PLAYER;
				clients.emplace_back(i);
				break;
			}
		}
	}

	return targetType;
}