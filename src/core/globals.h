#pragma once

#include <playerslot.h>
#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <variant.h>

#define CS_TEAM_NONE 0
#define CS_TEAM_SPECTATOR 1
#define CS_TEAM_T 2
#define CS_TEAM_CT 3

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4
#define HUD_PRINTALERT		6

#define MAXPLAYERS 64

class IGameEventSystem;
class IGameEventManager2;
class IEngineSound;
class INetworkGameServer;
class CGlobalVars;
class IVEngineServer2;
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

extern SourceMM::IMetamodListener* g_pMetamodListener;
extern CCoreConfig* g_pCoreConfig;
extern CGameConfig* g_pGameConfig;

namespace globals
{
	void Initialize(std::string coreConfig, std::string gameConfig);
	void Terminate();

} // namespace globals

namespace DynLibUtils
{
	class CModule;
}

namespace modules
{
	extern DynLibUtils::CModule* engine;
	extern DynLibUtils::CModule* tier0;
	extern DynLibUtils::CModule* server;
	extern DynLibUtils::CModule* schemasystem;
	extern DynLibUtils::CModule* filesystem;
	extern DynLibUtils::CModule* vscript;
	extern DynLibUtils::CModule* networksystem;
} // namespace modules

class IEntityFindFilter;
class CEntitySystem;
class CCSPlayerPawn;
class IRecipientFilter;
struct EmitSound_t;
struct SndOpEventGuid_t;
class CEntityKeyValues;
class CBaseModelEntity;
class CGameRules;
class CTakeDamageInfo;
class INetworkStringTable;

namespace addresses
{
	inline IGameEventListener2* (*GetLegacyGameEventListener)(CPlayerSlot slot);

	//inline void (*TracePlayerBBox)(const Vector& start, const Vector& end, const bbox_t& bounds, CTraceFilterS2* filter, trace_t_s2& pm);

	inline void (*CCSPlayerController_SwitchTeam)(CCSPlayerController* controller, int team);

	inline void (*CBasePlayerController_SetPawn)(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool, bool);

	inline CEntityInstance* (*CGameEntitySystem_FindEntityByClassName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName);

	inline CEntityInstance* (*CGameEntitySystem_FindEntityByName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName, CEntityInstance* pSearchingEntity, CEntityInstance* pActivator, CEntityInstance* pCaller, IEntityFindFilter* pFilter);

	inline CEntityInstance* (*CreateEntityByName)(const char* className, int iForceEdictIndex);

	inline void (*DispatchSpawn)(CEntityInstance* pEntity, CEntityKeyValues* pEntityKeyValues);

	inline void (*SetGroundEntity)(CBaseEntity* ent, CBaseEntity* ground, CBaseEntity* unk3);

	inline void (*CBaseModelEntity_SetModel)(CBaseModelEntity* pModel, const char* szModel);

	inline void (*UTIL_Remove)(CEntityInstance*);

	inline void (*CEntitySystem_AddEntityIOEvent)(CEntitySystem* pEntitySystem, CEntityInstance* pTarget, const char* pszInput, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* value, float flDelay, int outputID);

	inline void (*CEntityInstance_AcceptInput)(CEntityInstance* pThis, const char* pInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* value, int nOutputID);

	inline void (*CGameRules_TerminateRound)(CGameRules* pGameRules, float delay, unsigned int reason, int64 a4, unsigned int a5);

	inline void (*CEntityIdentity_SetEntityName)(CEntityIdentity* pEntity, const char* pName);

	inline void (*CBaseEntity_EmitSoundParams)(CBaseEntity* pEntity, const char* pszSound, int nPitch, float flVolume, float flDelay);

	inline void (*CBaseEntity_SetParent)(CBaseEntity* pEntity, CBaseEntity* pNewParent, CUtlStringToken nBoneOrAttachName, matrix3x4a_t* pOffsetTransform);

	inline SndOpEventGuid_t(*CBaseEntity_EmitSoundFilter)(IRecipientFilter& filter, CEntityIndex ent, const EmitSound_t& params);

	inline void (*CBaseEntity_SetMoveType)(CBaseEntity* pThis, MoveType_t nMoveType, MoveCollide_t nMoveCollide);

	inline void (*CTakeDamageInfo_Constructor)(CTakeDamageInfo* pThis, CBaseEntity* pInflictor, CBaseEntity* pAttacker, CBaseEntity* pAbility, const Vector* vecDamageForce, const Vector* vecDamagePosition, float flDamage, int bitsDamageType, int iCustomDamage, void* a10);

	inline void (*CNetworkStringTable_DeleteAllStrings)(INetworkStringTable* pThis);

	//inline void (*TracePlayerBBox)(const Vector& start, const Vector& end, const bbox_t& bounds, CTraceFilterS2* filter, trace_t_s2& pm);
} // namespace addresses