#pragma once

namespace SourceMM
{
	class IMetamodListener;
}

class IGameEventSystem;
class IGameEventManager2;
class CGlobalVars;
class IVEngineServer2;
class CGameResourceService;
class CSchemaSystem;
class CGameEntitySystem;

class CEntityInstance;
class CBasePlayerController;
class CCSPlayerController;
class CBaseEntity;

extern IGameEventSystem* g_gameEventSystem;
extern IGameEventManager2* g_gameEventManager;
extern CGlobalVars* gpGlobals;
extern IVEngineServer2* g_pEngineServer2;
extern CGameResourceService* pGameResourceServiceServer;
extern CSchemaSystem* pSchemaSystem;
extern CGameEntitySystem* g_pEntitySystem;

class CGameConfig;

namespace DynLibUtils
{
	class CModule;
}

namespace globals
{
	extern SourceMM::IMetamodListener* g_MetamodListener;
	extern CGameConfig* g_GameConfig;

	void Initialize();
	void Terminate();

	CGlobalVars* GetGameGlobals();
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

namespace addresses
{
	inline void (*NetworkStateChanged)(int64_t chainEntity, int64_t offset, int64_t a3);
	inline void (*StateChanged)(void* networkTransmitComponent, CEntityInstance* ent, int64_t offset, int16_t a4, int16_t a5);
	inline void (*UTIL_ClientPrintAll)(int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4);
	inline void (*ClientPrint)(CBasePlayerController* player, int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4);
	inline void (*SetGroundEntity)(CBaseEntity* ent, CBaseEntity* ground);
	inline void (*CCSPlayerController_SwitchTeam)(CCSPlayerController* pController, uint32_t team);
	inline void (*UTIL_Remove)(CEntityInstance*);
} // namespace addresses