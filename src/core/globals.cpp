
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

namespace cs2sdk {
	namespace modules {
		CModule* engine = nullptr;
		CModule* tier0 = nullptr;
		CModule* server = nullptr;
		CModule* schemasystem = nullptr;
		CModule* filesystem = nullptr;
		CModule* vscript = nullptr;
	}

	namespace globals {
		IMetamodListener* metamodListener = nullptr;
		CGameConfig* gameConfig = nullptr;

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

			gameConfig = new CGameConfig("csgo", utils::GamedataDirectory() + "cs2sdk.games.txt");

			if (!gameConfig->Initialize()) {
				Log_Error(LOG_GENERAL, "Could not read \"%s\".", gameConfig->GetPath().c_str());
				delete gameConfig;
			}

			auto plugify = CAssembly::FindModule(utils::GetBinDirectory() + BINARY_MODULE_PREFIX "plugify" BINARY_MODULE_SUFFIX);

			using IMetamodListenerFn = IMetamodListener* (*)();
			auto func = plugify->GetFunction<IMetamodListenerFn>("Plugify_ImmListener");
			metamodListener = func();

			g_gameEventManager = (IGameEventManager2*)(CALL_VIRTUAL(uintptr_t, 91, g_pSource2Server) - 8);

			// load more if needed
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
}