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

enum ListenOverride : int8_t
{
	Listen_Default = 0,
	Listen_Mute,
	Listen_Hear
};

enum VoiceFlagValue
{
	Speak_Normal = 0,
	Speak_Muted = 1 << 0,
	Speak_All = 1 << 1,
	Speak_ListenAll = 1 << 2,
	Speak_Team = 1 << 3,
	Speak_ListenTeam = 1 << 4,
};

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

using VoiceFlag_t = uint8_t;

class CPlayer
{
	friend class CPlayerManager;

public:
	CPlayer();
	~CPlayer();

public:
	void Initialize(plg::string name, plg::string ip, CPlayerSlot slot);
	void Connect();
	void Disconnect();
	void Authorize();

public:
	const plg::string& GetName() const;
	void SetName(plg::string name);
	INetChannelInfo* GetNetInfo() const;
	const CSteamID* GetSteamId() const;
	void SetSteamId(const CSteamID* steam_id);
	uint64 GetAdminFlags() const;
	void SetAdminFlags(uint64 adminFlags);
	bool IsAdminFlagSet(uint64 flag);
	bool IsConnected() const;
	bool IsFakeClient() const;
	bool IsAuthorized() const;
	bool IsInGame() const;
	void Kick();
	const char* GetKeyValue(const char* key) const;
	const plg::string& GetIpAddress() const;
	float GetTimeConnected() const;
	float GetLatency() const;
	void SetListen(CPlayerSlot slot, ListenOverride listen);
	void SetVoiceFlags(VoiceFlag_t flags);
	VoiceFlag_t GetVoiceFlags() const;
	ListenOverride GetListen(CPlayerSlot slot) const;
	bool IsAuthStringValidated() const;

public:
	plg::string m_name;
	plg::string m_authId;
	plg::string m_ipAddress;
	bool m_bConnected{};
	bool m_bFakeClient{};
	bool m_bInGame{};
	bool m_bAuthorized{};
	CPlayerSlot m_iSlot{-1};
	const CSteamID* m_steamId;
	uint64 m_iAdminFlags{};
	VoiceFlag_t m_voiceFlag{};
	std::array<ListenOverride, MAXPLAYERS + 2> m_listenMap{};
	CPlayerBitVec m_selfMutes{};
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
	static int MaxClients();

	CPlayer* GetPlayerBySlot(CPlayerSlot slot) const;
	CPlayerSlot GetSlotFromUserId(uint16 userid) const;
	CPlayer* GetPlayerFromUserId(uint16 userid) const;
	CPlayer* GetPlayerFromSteamId(uint64 steamid) const;

	TargetType TargetPlayerString(int caller, const char* target, plg::vector<int>& clients);

private:
	void InvalidatePlayer(CPlayer* pPlayer);

	std::array<CPlayer, MAXPLAYERS + 2> m_players;
	bool m_bRefuseConnection{};
	int m_playerCount{};
	int m_listenClient{};
	float m_lastAuthCheckTime{};
};

extern CPlayerManager g_PlayerManager;