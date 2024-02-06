
#include "globals.h"
#include "module.h"
#include "utils.h"
#include "gameconfig.h"
#include "fwd.h"

#include "interfaces/cgameresourceserviceserver.h"
#include "interfaces/cschemasystem.h"

IGameEventSystem* g_gameEventSystem = nullptr;
IGameEventManager2* g_gameEventManager = nullptr;
CGlobalVars* gpGlobals = nullptr;
IVEngineServer2* g_pEngineServer2 = nullptr;
CGameResourceService* pGameResourceServiceServer = nullptr;
CSchemaSystem* pSchemaSystem = nullptr;
CGameEntitySystem* g_pEntitySystem = nullptr;

#define RESOLVE_SIG(gameConfig, name, variable) \
	variable = (decltype(variable))gameConfig->ResolveSignature(name)

namespace modules {
	CModule* engine = nullptr;
	CModule* tier0 = nullptr;
	CModule* server = nullptr;
	CModule* schemasystem = nullptr;
	CModule* filesystem = nullptr;
	CModule* vscript = nullptr;
}

namespace globals {
	IMetamodListener* g_MetamodListener = nullptr;
	CGameConfig* g_GameConfig = nullptr;

	void Initialize() {
		modules::engine = new modules::CModule(CS2SDK_ROOT_BINARY, "engine2");
		modules::tier0 = new modules::CModule(CS2SDK_ROOT_BINARY, "tier0");
		modules::server = new modules::CModule(CS2SDK_GAME_BINARY, "server");
		modules::schemasystem = new modules::CModule(CS2SDK_ROOT_BINARY, "schemasystem");
		modules::filesystem = new modules::CModule(CS2SDK_ROOT_BINARY, "filesystem_stdio");
		modules::vscript = new modules::CModule(CS2SDK_ROOT_BINARY, "vscript");

		pGameResourceServiceServer = (CGameResourceService*)modules::engine->FindInterface(GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
		g_pCVar = (ICvar*)modules::tier0->FindInterface(CVAR_INTERFACE_VERSION);
		g_pSource2GameEntities = (ISource2GameEntities*)modules::server->FindInterface(SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		pSchemaSystem = (CSchemaSystem*)modules::schemasystem->FindInterface(SCHEMASYSTEM_INTERFACE_VERSION);
		g_pSource2Server = (ISource2Server*)modules::server->FindInterface(SOURCE2SERVER_INTERFACE_VERSION);
		g_pSource2GameEntities = (ISource2GameEntities*)modules::server->FindInterface(SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSource2GameClients = (IServerGameClients*)modules::server->FindInterface(SOURCE2GAMECLIENTS_INTERFACE_VERSION);

		g_pEngineServer2 = (IVEngineServer2*)modules::engine->FindInterface(SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
		g_pFullFileSystem = (IFileSystem*)modules::filesystem->FindInterface(FILESYSTEM_INTERFACE_VERSION);
		g_gameEventSystem = (IGameEventSystem*)modules::engine->FindInterface(GAMEEVENTSYSTEM_INTERFACE_VERSION);
		g_pNetworkServerService = (INetworkServerService*)modules::engine->FindInterface(NETWORKSERVERSERVICE_INTERFACE_VERSION);

		ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);

		gpGlobals = GetGameGlobals();

		g_GameConfig = g_GameConfigManager.LoadGameConfigFile("cs2sdk.games.txt");

		auto plugify = CAssembly::FindModule(utils::GetBinDirectory() + BINARY_MODULE_PREFIX "plugify" BINARY_MODULE_SUFFIX);

		using IMetamodListenerFn = IMetamodListener* (*)();
		auto Plugify_ImmListener = plugify->GetFunction<IMetamodListenerFn>("Plugify_ImmListener");
		g_MetamodListener = Plugify_ImmListener();

		g_gameEventManager = (IGameEventManager2*)(CALL_VIRTUAL(uintptr_t, g_GameConfig->GetOffset("GameEventManager"), g_pSource2Server) - 8);

		// load more if needed
		//RESOLVE_SIG(g_GameConfig, "NetworkStateChanged", addresses::NetworkStateChanged);
		//RESOLVE_SIG(g_GameConfig, "StateChanged", addresses::StateChanged);
		//RESOLVE_SIG(g_GameConfig, "UTIL_ClientPrintAll", addresses::UTIL_ClientPrintAll);
		//RESOLVE_SIG(g_GameConfig, "ClientPrint", addresses::ClientPrint);
		//RESOLVE_SIG(g_GameConfig, "SetGroundEntity", addresses::SetGroundEntity);
		//RESOLVE_SIG(g_GameConfig, "CCSPlayerController_SwitchTeam", addresses::CCSPlayerController_SwitchTeam);
		//RESOLVE_SIG(g_GameConfig, "UTIL_Remove", addresses::UTIL_Remove);
	}

	void Terminate() {
		delete modules::engine;
		delete modules::tier0;
		delete modules::server;
		delete modules::schemasystem;
		delete modules::filesystem;
		delete modules::vscript;
	}

	CGlobalVars* GetGameGlobals() {
		INetworkGameServer* server = g_pNetworkServerService->GetIGameServer();

		if (!server)
			return nullptr;

		return server->GetGlobals();
	}
}