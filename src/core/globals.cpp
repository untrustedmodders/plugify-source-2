
#include "globals.h"
#include "core_config.h"
#include "game_config.h"
#include "module.h"

#include <core/sdk/utils.h>
#include <core/sdk/virtual.h>

#include <ISmmPlugin.h>
#include <igameevents.h>
#include <engine/igameeventsystem.h>

IGameEventSystem* g_gameEventSystem = nullptr;
IGameEventManager2* g_gameEventManager = nullptr;
INetworkGameServer* g_pNetworkGameServer = nullptr;
CGlobalVars* gpGlobals = nullptr;
IVEngineServer2* g_pEngineServer2 = nullptr;
CSchemaSystem* g_pSchemaSystem2 = nullptr;
CGameEntitySystem* g_pEntitySystem = nullptr;
IEngineSound* g_pEngineSound = nullptr;

#define RESOLVE_SIG(gameConfig, name, variable) \
	variable = (decltype(variable))(void*)(gameConfig)->ResolveSignature(name)

namespace modules
{
	CModule* engine = nullptr;
	CModule* tier0 = nullptr;
	CModule* server = nullptr;
	CModule* schemasystem = nullptr;
	CModule* filesystem = nullptr;
	CModule* vscript = nullptr;
	CModule* networksystem = nullptr;
} // namespace modules

IMetamodListener* g_pMetamodListener = nullptr;
CCoreConfig* g_pCoreConfig = nullptr;
CGameConfig* g_pGameConfig = nullptr;

template <class T>
T* FindInterface(const CModule* module, const char* name)
{
	auto CreateInterface = module->GetFunctionByName("CreateInterface");
	if (!CreateInterface)
	{
		g_Logger.LogFormat(LS_ERROR, "Could not find CreateInterface in %s at \"%s\"\n", module->GetModuleName().data(), module->GetModulePath().data());
		return nullptr;
	}

	void* pInterface = CreateInterface.CCast<CreateInterfaceFn>()(name, nullptr);
	if (!pInterface)
	{
		g_Logger.LogFormat(LS_ERROR, "Could not find interface %s in %s at \"%s\"\n", name, module->GetModuleName().data(), module->GetModulePath().data());
		return nullptr;
	}

	return reinterpret_cast<T*>(pInterface);
}

// we cannot return a char array from a function, therefore we need a wrapper
template<size_t N>
struct String {
	char c[N];
};

template<size_t N>
constexpr auto ModulePath(const char (&str)[N])
{
	constexpr auto length = N - 1;
	String<N> result = {};
	for (size_t i = 0; i < length; ++i)
	{
		result.c[i] = str[i];
#if CS2SDK_PLATFORM_WINDOWS
		if (result.c[i] == '/')
			result.c[i] = '\\';
#endif
	}
	result.c[length] = '\0';
	return result;
}

namespace globals
{
	void Initialize(std::string coreConfig, std::string gameConfig)
	{
		modules::engine = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_ROOT_BINARY CS2SDK_LIBRARY_PREFIX "engine2").c);
		modules::tier0 = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_ROOT_BINARY CS2SDK_LIBRARY_PREFIX "tier0").c);
		modules::server = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_GAME_BINARY CS2SDK_LIBRARY_PREFIX "server").c);
		modules::schemasystem = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_ROOT_BINARY CS2SDK_LIBRARY_PREFIX "schemasystem").c);
		modules::filesystem = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_ROOT_BINARY CS2SDK_LIBRARY_PREFIX "filesystem_stdio").c);
		modules::vscript = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_ROOT_BINARY CS2SDK_LIBRARY_PREFIX "vscript").c);
		modules::networksystem = new CModule(utils::GameDirectory() + ModulePath(CS2SDK_ROOT_BINARY CS2SDK_LIBRARY_PREFIX "networksystem").c);

		g_pCVar = FindInterface<ICvar>(modules::tier0, CVAR_INTERFACE_VERSION);
		g_pSchemaSystem2 = FindInterface<CSchemaSystem>(modules::schemasystem, SCHEMASYSTEM_INTERFACE_VERSION);
		g_pSource2Server = FindInterface<ISource2Server>(modules::server, SOURCE2SERVER_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSource2GameClients = FindInterface<IServerGameClients>(modules::server, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
		g_pGameResourceServiceServer = FindInterface<IGameResourceService>(modules::engine, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

		g_pEngineServer2 = FindInterface<IVEngineServer2>(modules::engine, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
		g_pFullFileSystem = FindInterface<IFileSystem>(modules::filesystem, FILESYSTEM_INTERFACE_VERSION);
		g_gameEventSystem = FindInterface<IGameEventSystem>(modules::engine, GAMEEVENTSYSTEM_INTERFACE_VERSION);
		g_pNetworkServerService = FindInterface<INetworkServerService>(modules::engine, NETWORKSERVERSERVICE_INTERFACE_VERSION);
		//g_pEngineSound = FindInterface<IEngineSound>(modules::engine, IENGINESOUND_SERVER_INTERFACE_VERSION);
		g_pNetworkMessages = FindInterface<INetworkMessages>(modules::networksystem, NETWORKMESSAGES_INTERFACE_VERSION);

		ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);

		CModule plugify("plugify");

		using IMetamodListenerFn = IMetamodListener* (*)();
		auto Plugify_ImmListener = plugify.GetFunctionByName("Plugify_ImmListener");
		g_pMetamodListener = Plugify_ImmListener.CCast<IMetamodListenerFn>()();

		g_gameEventManager = static_cast<IGameEventManager2*>(CALL_VIRTUAL(void*, int(93), g_pSource2Server));
		if (g_gameEventManager == nullptr)
		{
			g_Logger.Log(LS_ERROR, "GameEventManager not found!");
		}

		g_pCoreConfig = new CCoreConfig(std::move(coreConfig));
		g_pCoreConfig->Initialize();
		g_pGameConfig = new CGameConfig("cs2", std::move(gameConfig));
		g_pGameConfig->Initialize();

		// load more if needed
		RESOLVE_SIG(g_pGameConfig, "LegacyGameEventListener", addresses::GetLegacyGameEventListener);
		RESOLVE_SIG(g_pGameConfig, "CCSPlayerController_SwitchTeam", addresses::CCSPlayerController_SwitchTeam);
		RESOLVE_SIG(g_pGameConfig, "CBasePlayerController_SetPawn", addresses::CBasePlayerController_SetPawn);
		RESOLVE_SIG(g_pGameConfig, "CBaseModelEntity_SetModel", addresses::CBaseModelEntity_SetModel);
		RESOLVE_SIG(g_pGameConfig, "CGameEntitySystem_FindEntityByClassName", addresses::CGameEntitySystem_FindEntityByClassName);
		RESOLVE_SIG(g_pGameConfig, "CGameEntitySystem_FindEntityByName", addresses::CGameEntitySystem_FindEntityByName);
		RESOLVE_SIG(g_pGameConfig, "CreateEntityByName", addresses::CreateEntityByName);
		RESOLVE_SIG(g_pGameConfig, "DispatchSpawn", addresses::DispatchSpawn);
		RESOLVE_SIG(g_pGameConfig, "UTIL_Remove", addresses::UTIL_Remove);
		//RESOLVE_SIG(g_pGameConfig, "CEntitySystem_AddEntityIOEvent", addresses::CEntitySystem_AddEntityIOEvent);
		//RESOLVE_SIG(g_pGameConfig, "CEntityInstance_AcceptInput", addresses::CEntityInstance_AcceptInput);
		//RESOLVE_SIG(g_pGameConfig, "CGameRules_TerminateRound", addresses::CGameRules_TerminateRound);
		RESOLVE_SIG(g_pGameConfig, "CEntityIdentity_SetEntityName", addresses::CEntityIdentity_SetEntityName);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_EmitSoundParams", addresses::CBaseEntity_EmitSoundParams);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_SetParent", addresses::CBaseEntity_SetParent);
		//RESOLVE_SIG(g_pGameConfig, "DispatchParticleEffect", addresses::DispatchParticleEffect);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_EmitSoundFilter", addresses::CBaseEntity_EmitSoundFilter);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_SetMoveType", addresses::CBaseEntity_SetMoveType);
		//RESOLVE_SIG(g_pGameConfig, "CTakeDamageInfo", addresses::CTakeDamageInfo_Constructor);
		//RESOLVE_SIG(g_pGameConfig, "CNetworkStringTable_DeleteAllStrings", addresses::CNetworkStringTable_DeleteAllStrings);
		//RESOLVE_SIG(g_pGameConfig, "TracePlayerBBox", addresses::TracePlayerBBox);
	}

	void Terminate()
	{
		delete g_pGameConfig;
		delete g_pCoreConfig;
		delete modules::engine;
		delete modules::tier0;
		delete modules::server;
		delete modules::schemasystem;
		delete modules::filesystem;
		delete modules::vscript;
	}

} // namespace globals