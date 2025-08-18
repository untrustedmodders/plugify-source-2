#pragma once

#include <playerslot.h>
#include <steam_api.h>
#include <plugify/cpp_plugin.hpp>
#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <variant.h>

#define CS_TEAM_NONE 0
#define CS_TEAM_SPECTATOR 1
#define CS_TEAM_T 2
#define CS_TEAM_CT 3

#define HUD_PRINTNOTIFY 1
#define HUD_PRINTCONSOLE 2
#define HUD_PRINTTALK 3
#define HUD_PRINTCENTER 4
#define HUD_PRINTALERT 6

#define MAXPLAYERS 64

class CAppSystemDict;
class IAppSystem;
class IGameEventSystem;
class IGameEventManager2;
class CNetworkGameServerBase;
class CGlobalVars;
class CGameEntitySystem;
class IGameEventListener2;
class CEntitySystem;

class CEntityInstance;
class CBasePlayerController;
class CBaseEntity;
class CHostStateMgr;
struct CHostStateRequest;
class CGameRules;
#if defined(CS2)
class CCSGameRules;
class CCSGameRulesProxy;
using CBaseGameRules = CCSGameRules;
using CBaseGameRulesProxy = CCSGameRulesProxy;
#elif defined(DEADLOCK)
class CCitadelGameRules;
class CCitadelGameRulesProxy;
using CBaseGameRules = CCitadelGameRules;
using CBaseGameRulesProxy = CCitadelGameRulesProxy;
#endif

inline IGameEventSystem* g_pGameEventSystem = nullptr;
inline IGameEventManager2* g_pGameEventManager = nullptr;
inline CAppSystemDict* g_pCurrentAppSystem = nullptr;
inline CNetworkGameServerBase* g_pNetworkGameServer = nullptr;
inline CGlobalVars* gpGlobals = nullptr;
inline CGameEntitySystem* g_pGameEntitySystem = nullptr;
inline CBaseGameRules* g_pGameRules = nullptr;
inline CBaseGameRulesProxy* g_pGameRulesProxy = nullptr;
//inline ISteamHTTP* g_http = nullptr;
inline CSteamGameServerAPIContext g_SteamAPI = {};

class CoreConfig;
class GameConfig;

namespace SourceMM {
	class IMetamodListener;
}

extern SourceMM::IMetamodListener* g_pMetamodListener;
extern std::unique_ptr<CoreConfig> g_pCoreConfig;
extern std::unique_ptr<GameConfig> g_pGameConfig;

namespace globals {
	void Initialize(std::unordered_map<std::string, fs::path> paths);
	void Terminate();

	PlatModule_t FindModule(std::string_view name);
	IAppSystem* FindInterface(std::string_view name);
	void* QueryInterface(std::string_view module, std::string_view name);
}// namespace globals

class CPlayer_WeaponServices;
class IEntityFindFilter;
class IRecipientFilter;
struct EmitSound_t;
struct SndOpEventGuid_t;
class CEntityKeyValues;
class CBaseModelEntity;
class CTakeDamageInfo;
class INetworkStringTable;
class CBasePlayerPawn;
class CBasePlayerWeapon;
class CServerSideClient;
class CServerSideClientBase;
class CNetMessage;
class CCSWeaponBaseVData;

namespace addresses {
	inline IGameEventListener2* (*GetLegacyGameEventListener)(CPlayerSlot slot);

	inline void (*CBasePlayerController_SetPawn)(CBasePlayerController* controller, CBasePlayerPawn* pawn, bool, bool, bool);

	inline CEntityInstance* (*CGameEntitySystem_FindEntityByClassName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName);

	inline CEntityInstance* (*CGameEntitySystem_FindEntityByName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName, CEntityInstance* pSearchingEntity, CEntityInstance* pActivator, CEntityInstance* pCaller, IEntityFindFilter* pFilter);

	inline CEntityInstance* (*CreateEntityByName)(const char* className, int iForceEdictIndex);

	inline void (*DispatchSpawn)(CEntityInstance* pEntity, CEntityKeyValues* pEntityKeyValues);

	inline void (*CBaseModelEntity_SetModel)(CBaseModelEntity* pModel, const char* szModel);

	inline void (*UTIL_Remove)(CEntityInstance*);

	inline void (*CEntityInstance_AcceptInput)(CEntityInstance* pThis, const char* pInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* value, int nOutputID);

	inline void (*CEntityIdentity_SetEntityName)(CEntityIdentity* pEntity, const char* pName);

	inline void (*CBaseEntity_EmitSoundParams)(CBaseEntity* pEntity, const char* pszSound, int nPitch, float flVolume, float flDelay);

	inline void (*CBaseEntity_SetParent)(CBaseEntity* pEntity, CBaseEntity* pNewParent, CUtlStringToken nBoneOrAttachName, matrix3x4a_t* pOffsetTransform);

	inline SndOpEventGuid_t (*CBaseEntity_EmitSoundFilter)(IRecipientFilter& filter, CEntityIndex ent, const EmitSound_t& params);

	inline void (*CBaseEntity_SetMoveType)(CBaseEntity* pThis, MoveType_t nMoveType, MoveCollide_t nMoveCollide);

	inline void (*CTakeDamageInfo_Constructor)(CTakeDamageInfo* pThis, CBaseEntity* pInflictor, CBaseEntity* pAttacker, CBaseEntity* pAbility, const Vector* vecDamageForce, const Vector* vecDamagePosition, float flDamage, int bitsDamageType, int iCustomDamage, void* a10);

	inline void (*CNetworkStringTable_DeleteAllStrings)(INetworkStringTable* pThis);

	// cs go

	inline void (*CCSPlayer_WeaponServices_RemoveItem)(CPlayer_WeaponServices* player, CBasePlayerWeapon* weapon);

	inline void (*CCSPlayerController_SwitchTeam)(CBasePlayerController* controller, int team);

	inline void (*CGameRules_TerminateRound)(CGameRules* pGameRules, float delay, unsigned int reason, int64 a4, unsigned int a5);

	inline CCSWeaponBaseVData* (*GetCSWeaponDataFromKey)(int, const char*);

}// namespace addresses

using HostStateRequestFn = void* (*)(CHostStateMgr *pMgrDoNotUse, CHostStateRequest* pRequest);
using ReplyConnectionFn = void (*)(CNetworkGameServerBase *server, CServerSideClient* client);
using SendNetMessageFn = void* (*)(const CServerSideClientBase*, const CNetMessage* data, uint8 bufType);

inline HostStateRequestFn g_pfnSetPendingHostStateRequest;
inline ReplyConnectionFn g_pfnReplyConnection;
inline SendNetMessageFn g_pfnSendNetMessage;