
#include "globals.h"
#include "fwd.h"
#include "gameconfig.h"
#include "module.h"
#include "virtual.h"
#include "utils.h"

IGameEventSystem *g_gameEventSystem = nullptr;
IGameEventManager2 *g_gameEventManager = nullptr;
CGlobalVars *gpGlobals = nullptr;
IVEngineServer2 *g_pEngineServer2 = nullptr;
CGameResourceService *pGameResourceServiceServer = nullptr;
CSchemaSystem *pSchemaSystem = nullptr;
CGameEntitySystem *g_pEntitySystem = nullptr;

#define RESOLVE_SIG(gameConfig, name, variable) \
	variable = (decltype(variable))gameConfig->ResolveSignature(name)

namespace modules
{
	DynLibUtils::CModule *engine = nullptr;
	DynLibUtils::CModule *tier0 = nullptr;
	DynLibUtils::CModule *server = nullptr;
	DynLibUtils::CModule *schemasystem = nullptr;
	DynLibUtils::CModule *filesystem = nullptr;
	DynLibUtils::CModule *vscript = nullptr;
} // namespace modules

template<class T>
T* FindInterface(const DynLibUtils::CModule* module, const char* name) {
	auto fn = module->GetFunctionByName("CreateInterface");
	if (!fn) {
		Log_Error(LOG_GENERAL, "Could not find CreateInterface in %s at \"%s\"\n", module->GetModuleName(), module->GetModulePath());
	}
	
	void* pInterface = fn.CCast<CreateInterfaceFn>()(name, nullptr);
	if (!pInterface) {
		Log_Error(LOG_GENERAL, "Could not find interface: %s in %s at \"%s\"\n", name, module->GetModuleName(), module->GetModulePath());
	}

	return (T*) pInterface;
}

namespace globals
{
	IMetamodListener *g_MetamodListener = nullptr;
	CGameConfig *g_GameConfig = nullptr;

	void Initialize()
	{
		modules::engine = new DynLibUtils::CModule("engine2");
		modules::tier0 = new DynLibUtils::CModule("tier0");
		modules::server = new DynLibUtils::CModule("server");
		modules::schemasystem = new DynLibUtils::CModule("schemasystem");
		modules::filesystem = new DynLibUtils::CModule("filesystem_stdio");
		modules::vscript = new DynLibUtils::CModule("vscript");

		pGameResourceServiceServer = FindInterface<CGameResourceService>(modules::engine, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
		g_pCVar = FindInterface<ICvar>(modules::tier0, CVAR_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		pSchemaSystem = FindInterface<CSchemaSystem>(modules::schemasystem, SCHEMASYSTEM_INTERFACE_VERSION);
		g_pSource2Server = FindInterface<ISource2Server>(modules::server, SOURCE2SERVER_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSource2GameClients = FindInterface<IServerGameClients>(modules::server, SOURCE2GAMECLIENTS_INTERFACE_VERSION);

		g_pEngineServer2 = FindInterface<IVEngineServer2>(modules::engine, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
		g_pFullFileSystem = FindInterface<IFileSystem>(modules::filesystem, FILESYSTEM_INTERFACE_VERSION);
		g_gameEventSystem = FindInterface<IGameEventSystem>(modules::engine, GAMEEVENTSYSTEM_INTERFACE_VERSION);
		g_pNetworkServerService = FindInterface<INetworkServerService>(modules::engine, NETWORKSERVERSERVICE_INTERFACE_VERSION);

		ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);

		gpGlobals = GetGameGlobals();

		g_GameConfig = g_GameConfigManager.LoadGameConfigFile("cs2sdk.games.txt");

		DynLibUtils::CModule plugify("plugify");

		using IMetamodListenerFn = IMetamodListener *(*)();
		auto Plugify_ImmListener = plugify.GetFunctionByName("Plugify_ImmListener");
		g_MetamodListener = Plugify_ImmListener.CCast<IMetamodListenerFn>()();

		g_gameEventManager = (IGameEventManager2 *)(CALL_VIRTUAL(uintptr_t, g_GameConfig->GetOffset("GameEventManager"), g_pSource2Server) - 8);

		// load more if needed
		// RESOLVE_SIG(g_GameConfig, "NetworkStateChanged", addresses::NetworkStateChanged);
		// RESOLVE_SIG(g_GameConfig, "StateChanged", addresses::StateChanged);
		// RESOLVE_SIG(g_GameConfig, "UTIL_ClientPrintAll", addresses::UTIL_ClientPrintAll);
		// RESOLVE_SIG(g_GameConfig, "ClientPrint", addresses::ClientPrint);
		// RESOLVE_SIG(g_GameConfig, "SetGroundEntity", addresses::SetGroundEntity);
		// RESOLVE_SIG(g_GameConfig, "CCSPlayerController_SwitchTeam", addresses::CCSPlayerController_SwitchTeam);
		// RESOLVE_SIG(g_GameConfig, "UTIL_Remove", addresses::UTIL_Remove);
	}

	void Terminate()
	{
		delete modules::engine;
		delete modules::tier0;
		delete modules::server;
		delete modules::schemasystem;
		delete modules::filesystem;
		delete modules::vscript;
	}

	CGlobalVars *GetGameGlobals()
	{
		INetworkGameServer *server = g_pNetworkServerService->GetIGameServer();

		if (!server)
			return nullptr;

		return server->GetGlobals();
	}
} // namespace globals