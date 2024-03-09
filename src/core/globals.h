#pragma once

#include <playerslot.h>
#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <variant.h>

#define CS_TEAM_NONE 0
#define CS_TEAM_SPECTATOR 1
#define CS_TEAM_T 2
#define CS_TEAM_CT 3

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

extern SourceMM::IMetamodListener* g_pMetamodListener;
extern CCoreConfig* g_pCoreConfig;
extern CGameConfig* g_pGameConfig;

namespace DynLibUtils
{
	class CModule;
}

namespace globals
{
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

class IEntityFindFilter;
class CEntitySystem;
class CCSPlayerPawn;
class IRecipientFilter;
struct EmitSound_t;
struct CTraceFilterS2;
struct SndOpEventGuid_t;
struct bbox_t;
struct trace_t_s2;
class CEntityKeyValues;
class CEntityKeyValues;
class CBaseEntity2;

namespace addresses
{
	inline IGameEventListener2* (*GetLegacyGameEventListener)(CPlayerSlot slot);
	inline SndOpEventGuid_t (*EmitSound)(IRecipientFilter& filter, CEntityIndex ent, const EmitSound_t& params);
	inline void (*TracePlayerBBox)(const Vector& start, const Vector& end, const bbox_t& bounds, CTraceFilterS2* filter, trace_t_s2& pm);
	inline void (*CCSPlayerController_SwitchTeam)(CCSPlayerController* controller, int team);
	inline void (*CBasePlayerController_SetPawn)(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool, bool);
	inline CBaseEntity2* (*CGameEntitySystem_FindEntityByClassName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName);
	inline CBaseEntity2* (*CGameEntitySystem_FindEntityByName)(CEntitySystem* pEntitySystem, CEntityInstance* pStartEntity, const char* szName, CEntityInstance* pSearchingEntity, CEntityInstance* pActivator, CEntityInstance* pCaller, IEntityFindFilter* pFilter);
	inline CBaseEntity2* (*CreateEntityByName)(const char* className, int iForceEdictIndex);
	inline void (*DispatchSpawn)(CBaseEntity2* pEntity, CEntityKeyValues* pEntityKeyValues);


} // namespace addresses