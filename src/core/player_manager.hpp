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

enum ListenOverride : int8_t {
	Listen_Default = 0,
	Listen_Mute,
	Listen_Hear
};

enum VoiceFlagValue {
	Speak_Normal = 0,
	Speak_Muted = 1 << 0,
	Speak_All = 1 << 1,
	Speak_ListenAll = 1 << 2,
	Speak_Team = 1 << 3,
	Speak_ListenTeam = 1 << 4,
};

using VoiceFlag_t = uint8_t;
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

	void OnSteamAPIActivated();
	bool OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason);
	bool OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet);
	void OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer);
	void OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid);
	void OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID);
	void OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID);
	void OnClientActive(CPlayerSlot slot, bool bLoadGame) const;
	void OnLevelShutdown();

	STEAM_GAMESERVER_CALLBACK_MANUAL(CPlayerManager, OnValidateAuthTicket, ValidateAuthTicketResponse_t, m_CallbackValidateAuthTicketResponse);

	std::vector<CPlayer*> GetOnlinePlayers() const;
	static int MaxClients();

	TargetType TargetPlayerString(int caller, std::string_view target, plg::vector<int>& clients);

protected:
	std::array<CPlayer, MAXPLAYERS> m_players;
	std::bitset<MAXPLAYERS> m_refuseConnection{};
};

extern CPlayerManager g_PlayerManager;