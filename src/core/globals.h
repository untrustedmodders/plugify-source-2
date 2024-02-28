#pragma once

#include <playerslot.h>
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

#define MAXPLAYERS 64

class IGameEventSystem;
class IGameEventManager2;
class IEngineSound;
class INetworkGameServer;
class CGlobalVars;
class IVEngineServer2;
class CGameResourceService;
class CSchemaSystem;
class CGameEntitySystem;
class IGameEventListener2;

class CEntityInstance;
class CBasePlayerController;
class CCSPlayerController;
class CBaseEntity;

extern IGameEventSystem* g_gameEventSystem;
extern IGameEventManager2* g_gameEventManager;
extern INetworkGameServer* g_pNetworkGameServer;
extern CGlobalVars* gpGlobals;
extern IVEngineServer2* g_pEngineServer2;
extern CSchemaSystem* g_pSchemaSystem2;
extern CGameEntitySystem* g_pEntitySystem;
extern IEngineSound* g_pEngineSound;

class CCoreConfig;
class CGameConfig;

namespace SourceMM
{
	class IMetamodListener;
}

namespace DynLibUtils
{
	class CModule;
}

namespace globals
{
	extern SourceMM::IMetamodListener* g_MetamodListener;
	extern CCoreConfig* g_CoreConfig;
	extern CGameConfig* g_GameConfig;

	void Initialize();
	void Terminate();

} // namespace globals

namespace modules
{
	extern DynLibUtils::CModule* engine;
	extern DynLibUtils::CModule* tier0;
	extern DynLibUtils::CModule* server;
	extern DynLibUtils::CModule* schemasystem;
	extern DynLibUtils::CModule* filesystem;
	extern DynLibUtils::CModule* vscript;
} // namespace modules

class CEntityInstance;
class CEntityIdentity;
class CBasePlayerController;
class CCSPlayerController;
class CCSPlayerPawn;
class CBaseModelEntity;
class Z_CBaseEntity;
class CGameConfig;
class CEntitySystem;
class IEntityFindFilter;
class CGameRules;
class CEntityKeyValues;

namespace addresses
{
	inline IGameEventListener2* (*GetLegacyGameEventListener)(CPlayerSlot slot);
	inline void (*UTIL_ClientPrintAll)(int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4);
	inline void (*ClientPrint)(CBasePlayerController* player, int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4);
	inline void (*NetworkStateChanged)(int64 chainEntity, int64 offset, int64 a3);
	inline void (*StateChanged)(void* networkTransmitComponent, CEntityInstance* ent, int64 offset, int16 a4, int16 a5);
	// inline void (*SetGroundEntity)(CBaseEntity* ent, CBaseEntity* ground);
	// inline void (*CCSPlayerController_SwitchTeam)(CCSPlayerController* pController, uint32 team);
	// inline void (*CBasePlayerController_SetPawn)(CBasePlayerController* pController, CCSPlayerPawn* pPawn, bool a3, bool a4);
	// inline void (*CBaseModelEntity_SetModel)(CBaseModelEntity* pModel, const char* szModel);
	// inline void (*UTIL_Remove)(CEntityInstance*);
	// inline void (*CEntitySystem_AddEntityIOEvent)(CEntitySystem* pEntitySystem, CEntityInstance* pTarget, const char* pszInput, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* value, float flDelay, int outputID);
	// inline void (*CEntityInstance_AcceptInput)(CEntityInstance* pThis, const char* pInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* value, int nOutputID);
	// inline CBaseEntity* (*CGameEntitySystem_FindEntityByClassName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName);
	// inline CBaseEntity* (*CGameEntitySystem_FindEntityByName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName, CEntityInstance* pSearchingEntity, CEntityInstance* pActivator, CEntityInstance* pCaller, IEntityFindFilter* pFilter);
	// inline void (*CGameRules_TerminateRound)(CGameRules* pGameRules, float delay, unsigned int reason, int64 a4, unsigned int a5);
	// inline CBaseEntity* (*CreateEntityByName)(const char* className, int iForceEdictIndex);
	// inline void (*DispatchSpawn)(CBaseEntity* pEntity, CEntityKeyValues* pEntityKeyValues);
	// inline void (*CEntityIdentity_SetEntityName)(CEntityIdentity* pEntity, const char* pName);
	// inline void (*CBaseEntity_EmitSoundParams)(CBaseEntity* pEntity, const char* pszSound, int nPitch, float flVolume, float flDelay);
	// inline void (*CBaseEntity_SetParent)(CBaseEntity* pEntity, CBaseEntity* pNewParent, CUtlStringToken nBoneOrAttachName, matrix3x4a_t* pOffsetTransform);
} // namespace addresses