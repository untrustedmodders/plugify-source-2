#pragma once

#include <bitvec.h>
#include <network_connection.pb.h>
#include <serversideclient.h>
#include <steam/isteamuser.h>
#include <steam/steam_api_common.h>
#include <steam/steamclientpublic.h>

enum class TargetType : int {
	NONE,
	PLAYER,
	SELF,
	RANDOM,
	RANDOM_T,
	RANDOM_CT,
	ALL,
	SPECTATOR,
	T,
	CT,
};

class CPlayerPawnComponent;
class CCSObserverPawn;

class Player {
public:
	Player() = default;
	~Player() = default;

	void Init(int slot, uint64 xuid) {
		m_slot = slot;
		m_unauthenticatedSteamID = CSteamID(xuid);
	}

	void Reset() {
		m_slot = -1;
		m_unauthenticatedSteamID = k_steamIDNil;
	}

	CCSPlayerController* GetController() const;
	CBasePlayerPawn* GetCurrentPawn() const;
	CCSPlayerPawn* GetPlayerPawn() const;
	CCSObserverPawn* GetObserverPawn() const;
	CServerSideClientBase* GetClient() const;

	// Returns true if this client has been fully authenticated by Steam
	bool IsAuthenticated() const;
	bool IsConnected() const;
	bool IsInGame() const;
	bool IsFakeClient() const;
	bool IsSourceTV() const;
	bool IsAlive() const;
	bool IsValidClient() const;
	const char* GetName() const;
	const char* GetIpAddress() const;
	CSteamID GetSteamId(bool validated = false) const;
	INetChannelInfo* GetNetInfo() const;
	float GetTimeConnected() const;
	float GetLatency() const;

	void Kick(const char* internalReason, ENetworkDisconnectionReason reason) const;

	uint64 GetSteamId64(bool validated = false) const {
		return GetSteamId(validated).ConvertToUint64();
	}

	CPlayerSlot GetPlayerSlot() const {
		return m_slot;
	}

	CEntityIndex GetEntityIndex() const {
		return m_slot + 1;
	}

private:
	int m_slot{-1};
	CSteamID m_unauthenticatedSteamID{k_steamIDNil};
};

class PlayerManager {
public:
	PlayerManager() = default;
	~PlayerManager() = default;

	Player* ToPlayer(CServerSideClientBase* client) const;
	Player* ToPlayer(CPlayerPawnComponent* component) const;
	Player* ToPlayer(CBasePlayerController* controller) const;
	Player* ToPlayer(CBasePlayerPawn* pawn) const;
	Player* ToPlayer(CPlayerSlot slot) const;
	Player* ToPlayer(CEntityIndex entIndex) const;
	Player* ToPlayer(CPlayerUserId userID) const;
	Player* ToPlayer(CSteamID steamid, bool validate = false) const;

	void OnSteamAPIActivated();
	bool OnClientConnect(CPlayerSlot slot, const char* name, uint64 xuid, const char* networkID);
	bool OnClientConnect_Post(CPlayerSlot slot, bool origRet);
	void OnClientConnected(CPlayerSlot slot);
	void OnClientPutInServer(CPlayerSlot slot, char const* name);
	void OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason);
	void OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason);
	void OnClientActive(CPlayerSlot slot, bool bLoadGame) const;

	STEAM_GAMESERVER_CALLBACK_MANUAL(PlayerManager, OnValidateAuthTicket, ValidateAuthTicketResponse_t, m_CallbackValidateAuthTicketResponse);

	std::vector<Player*> GetOnlinePlayers() const;
	static int MaxClients();

	TargetType TargetPlayerString(int caller, std::string_view target, plg::vector<int>& clients);

protected:
	std::array<Player, MAXPLAYERS + 1> m_players{};
	bool m_callbackRegistered{};
};

extern PlayerManager g_PlayerManager;