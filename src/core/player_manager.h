#pragma once

#include <bitvec.h>
#include <network_connection.pb.h>

class CBaseEntity;
class INetChannelInfo;
class IPlayerInfo;
class CSteamID;
class CCommand;
struct edict_t;

enum ListenOverride : uint8_t
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

typedef uint8_t VoiceFlag_t;

class CPlayer
{
	friend class PlayerManager;

public:
	CPlayer();
	~CPlayer();

public:
	void Initialize(std::string name, std::string ip, CPlayerSlot slot);
	void Connect();
	void Disconnect();
	IPlayerInfo* GetPlayerInfo() const;
	bool WasCountedAsInGame() const;
	int GetUserId();
	bool IsAuthStringValidated() const;
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
	void PrintToConsole(const char* message) const;
	void PrintToChat(const char* message) const;
	void PrintToHint(const char* message) const;
	void PrintToCenter(const char* message) const;
	QAngle GetAbsAngles() const;
	Vector GetAbsOrigin() const;
	bool IsAlive() const;
	bool IsInGame() const;
	void Kick(const char* kickReason);
	const char* GetWeaponName() const;
	void ChangeTeam(int team) const;
	int GetTeam() const;
	int GetArmor() const;
	int GetFrags() const;
	int GetDeaths() const;
	const char* GetKeyValue(const char* key) const;
	Vector GetMaxSize() const;
	Vector GetMinSize() const;
	int GetMaxHealth() const;
	const std::string& GetIpAddress() const;
	const char* GetModelName() const;
	int GetUserId() const;
	float GetTimeConnected() const;
	float GetLatency() const;
	void SetListen(CPlayerSlot slot, ListenOverride listen);
	void SetVoiceFlags(VoiceFlag_t flags);
	VoiceFlag_t GetVoiceFlags();
	ListenOverride GetListen(CPlayerSlot slot) const;

public:
	std::string m_name;
	std::string m_authId;
	// IPlayerInfo* m_info{nullptr};
	bool m_bConnected{};
	bool m_bFakeClient{};
	bool m_bInGame{};
	bool m_bAuthorized{};
	int m_userId{1};
	CPlayerSlot m_slot{-1};
	const CSteamID* m_steamId;
	std::string m_ipAddress;
	VoiceFlag_t m_voiceFlag = 0;
	std::array<ListenOverride, MAXPLAYERS + 2> m_listenMap = {};
	CPlayerBitVec m_selfMutes = {};
};

class PlayerManager
{
	friend class CPlayer;

public:
	PlayerManager();

	void OnLevelShutdown();
	bool OnClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason);
	bool OnClientConnect_Post(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason, bool origRet);
	void OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer);
	void OnClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid);
	void OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID);
	void OnClientDisconnect_Post(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID);
	void OnClientActive(CPlayerSlot slot, bool bLoadGame) const;
	void OnClientCommand(CPlayerSlot slot, const CCommand& args) const;
	void OnClientAuthorized(CPlayer* player) const;

	int ListenClient() const;
	void RunAuthChecks();

	int NumPlayers() const;
	int MaxClients() const;
	CPlayer* GetPlayerBySlot(int client) const;
	// CPlayer* GetClientOfUserId(int user_id) const;

private:
	void InvalidatePlayer(CPlayer* pPlayer);

	std::array<CPlayer, MAXPLAYERS + 2> m_players;
	int m_maxClients{};
	int m_playerCount{};
	std::map<int, int> m_userIdLookup;
	int m_listenClient{};
	bool m_bListenServer{};
	bool m_refuseConnection{};
	float m_lastAuthCheckTime{};
};

extern PlayerManager g_PlayerManager;