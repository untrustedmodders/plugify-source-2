#pragma once

#include <bitvec.h>
#include <network_connection.pb.h>
#include <serversideclient.h>
#include <steam/isteamuser.h>
#include <steam/steam_api_common.h>
#include <steam/steamclientpublic.h>

enum class TargetType {
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

class CPlayer {
public:
	CPlayer() = default;
	~CPlayer() = default;

	void Init(int slot) {
		m_iSlot = slot;
	}

	void Reset() {
		m_iSlot = -1;
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

	void SetUnauthenticatedSteamID(uint64 xuid) {
		m_unauthenticatedSteamID = CSteamID(xuid);
	}

	const CSteamID& GetUnauthenticatedSteamId64() const {
		return m_unauthenticatedSteamID;
	}

	CPlayerSlot GetPlayerSlot() const {
		return m_iSlot;
	}

	CEntityIndex GetEntityIndex() const {
		return m_iSlot + 1;
	}

private:
	int m_iSlot{-1};
	CSteamID m_unauthenticatedSteamID{};
};

class CPlayerManager {
public:
	CPlayerManager() = default;
	~CPlayerManager() = default;

	CPlayer* ToPlayer(CServerSideClientBase* pClient) const;
	CPlayer* ToPlayer(CPlayerPawnComponent* component) const;
	CPlayer* ToPlayer(CBasePlayerController* controller) const;
	CPlayer* ToPlayer(CBasePlayerPawn* pawn) const;
	CPlayer* ToPlayer(CPlayerSlot slot) const;
	CPlayer* ToPlayer(CEntityIndex entIndex) const;
	CPlayer* ToPlayer(CPlayerUserId userID) const;
	CPlayer* ToPlayer(CSteamID steamid, bool validate = false) const;

	//void OnSteamAPIActivated();
	bool OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID);
	bool OnClientConnect_Post(CPlayerSlot slot, bool bOrigRet);
	void OnClientConnected(CPlayerSlot slot);
	void OnClientPutInServer(CPlayerSlot slot, char const* pszName);
	void OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason);
	void OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason);
	void OnClientActive(CPlayerSlot slot, bool bLoadGame) const;
	void OnLevelShutdown();
	void RunAuthChecks();

	//STEAM_GAMESERVER_CALLBACK_MANUAL(CPlayerManager, OnValidateAuthTicket, ValidateAuthTicketResponse_t, m_CallbackValidateAuthTicketResponse);

	std::vector<CPlayer*> GetOnlinePlayers() const;
	static int MaxClients();

	TargetType TargetPlayerString(int caller, std::string_view target, plg::vector<int>& clients);

protected:
	std::array<CPlayer, MAXPLAYERS> m_players;
	double m_lastAuthCheckTime;
};

extern CPlayerManager g_PlayerManager;