#pragma once

#include <bitvec.h>
#include <network_connection.pb.h>
#include <serversideclient.h>

class CBaseEntity;
class INetChannelInfo;
class IPlayerInfo;
class CSteamID;
class CCommand;
struct edict_t;

enum class TargetType
{
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

class CPlayer
{
	friend class CPlayerManager;

public:
	CPlayer();
	~CPlayer();

public:
	void Initialize(std::string name, std::string ip, CPlayerSlot slot);
	void Connect();
	void Disconnect();
	void Authorize();

public:
	const std::string& GetName() const;
	void SetName(std::string name);
	INetChannelInfo* GetNetInfo() const;
	const CSteamID* GetSteamId() const;
	void SetSteamId(const CSteamID* steam_id);
	bool IsConnected() const;
	bool IsFakeClient() const;
	bool IsAuthorized() const;
	bool IsInGame() const;
	void Kick();
	const char* GetKeyValue(const char* key) const;
	const std::string& GetIpAddress() const;
	float GetTimeConnected() const;
	float GetLatency() const;
	bool IsAuthStringValidated() const;

public:
	std::string m_name;
	std::string m_authId;
	std::string m_ipAddress;
	bool m_bConnected{};
	bool m_bFakeClient{};
	bool m_bInGame{};
	bool m_bAuthorized{};
	CPlayerSlot m_iSlot{-1};
	const CSteamID* m_steamId;
};

class CPlayerManager
{
	friend class CPlayer;

public:
	CPlayerManager();

	void OnLevelShutdown();
	bool OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason);
	bool OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet);
	void OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer);
	void OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid);
	void OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID);
	void OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID);
	void OnClientActive(CPlayerSlot slot, bool bLoadGame) const;
	void OnClientAuthorized(CPlayer* player) const;

	void RunAuthChecks();

	int NumPlayers() const;
	int MaxClients() const;

	CPlayer* GetPlayerBySlot(CPlayerSlot slot) const;
	CPlayerSlot GetSlotFromUserId(uint16 userid) const;
	CPlayer* GetPlayerFromUserId(uint16 userid) const;
	CPlayer* GetPlayerFromSteamId(uint64 steamid) const;

	TargetType TargetPlayerString(int caller, const char* target, std::vector<int>& clients);

private:
	void InvalidatePlayer(CPlayer* pPlayer);

	std::array<CPlayer, MAXPLAYERS + 2> m_players;
	bool m_bRefuseConnection{};
	int m_playerCount{};
	int m_listenClient{};
	float m_lastAuthCheckTime{};
};

extern CPlayerManager g_PlayerManager;