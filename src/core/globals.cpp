
#include "globals.h"
#include "core_config.h"
#include "game_config.h"
#include "module.h"
#include "utils.h"
#include "virtual.h"

#include <ISmmPlugin.h>
#include <engine/IEngineSound.h>
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
	variable = (decltype(variable))(void*)gameConfig->ResolveSignature(name)

namespace modules
{
	DynLibUtils::CModule* engine = nullptr;
	DynLibUtils::CModule* tier0 = nullptr;
	DynLibUtils::CModule* server = nullptr;
	DynLibUtils::CModule* schemasystem = nullptr;
	DynLibUtils::CModule* filesystem = nullptr;
	DynLibUtils::CModule* vscript = nullptr;
} // namespace modules

template <class T>
T* FindInterface(const DynLibUtils::CModule* module, const char* name)
{
	auto fn = module->GetFunctionByName("CreateInterface");
	if (!fn)
	{
		g_Logger.ErrorFormat("Could not find CreateInterface in %s at \"%s\"\n", module->GetModuleName().data(), module->GetModulePath().data());
	}

	void* pInterface = fn.CCast<CreateInterfaceFn>()(name, nullptr);
	if (!pInterface)
	{
		g_Logger.ErrorFormat("Could not find interface: %s in at \"%s\"\n", name, module->GetModuleName().data(), module->GetModulePath().data());
	}

	return (T*)pInterface;
}

template <typename CharT, std::size_t N>
constexpr auto replace(const CharT (&str)[N], CharT oldChar, CharT newChar) {
	std::basic_string<CharT> result(str, N - 1);  // Constructing std::basic_string from array
	for (auto& c : result) {
		if (c == oldChar) {
			c = newChar;
		}
	}
	return result;
}

namespace globals
{
	IMetamodListener* g_MetamodListener = nullptr;
	CCoreConfig* g_CoreConfig = nullptr;
	CGameConfig* g_GameConfig = nullptr;

	void Initialize()
	{
		modules::engine = new DynLibUtils::CModule(Plat_GetGameDirectory() + replace(CS2SDK_ROOT_BINARY BINARY_MODULE_PREFIX "engine2", '/', '\\'));
		modules::tier0 = new DynLibUtils::CModule(Plat_GetGameDirectory() + replace(CS2SDK_ROOT_BINARY BINARY_MODULE_PREFIX "tier0", '/', '\\'));
		modules::server = new DynLibUtils::CModule(Plat_GetGameDirectory() + replace(CS2SDK_GAME_BINARY BINARY_MODULE_PREFIX "server", '/', '\\'));
		modules::schemasystem = new DynLibUtils::CModule(Plat_GetGameDirectory() + replace(CS2SDK_ROOT_BINARY BINARY_MODULE_PREFIX "schemasystem", '/', '\\'));
		modules::filesystem = new DynLibUtils::CModule(Plat_GetGameDirectory() + replace(CS2SDK_ROOT_BINARY BINARY_MODULE_PREFIX "filesystem_stdio", '/', '\\'));
		modules::vscript = new DynLibUtils::CModule(Plat_GetGameDirectory() + replace(CS2SDK_ROOT_BINARY BINARY_MODULE_PREFIX "vscript", '/', '\\'));

		g_pCVar = FindInterface<ICvar>(modules::tier0, CVAR_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSchemaSystem2 = FindInterface<CSchemaSystem>(modules::schemasystem, SCHEMASYSTEM_INTERFACE_VERSION);
		g_pSource2Server = FindInterface<ISource2Server>(modules::server, SOURCE2SERVER_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSource2GameClients = FindInterface<IServerGameClients>(modules::server, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
		g_pGameResourceServiceServer = FindInterface<IGameResourceServiceServer>(modules::engine, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

		g_pEngineServer2 = FindInterface<IVEngineServer2>(modules::engine, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
		g_pFullFileSystem = FindInterface<IFileSystem>(modules::filesystem, FILESYSTEM_INTERFACE_VERSION);
		g_gameEventSystem = FindInterface<IGameEventSystem>(modules::engine, GAMEEVENTSYSTEM_INTERFACE_VERSION);
		g_pNetworkServerService = FindInterface<INetworkServerService>(modules::engine, NETWORKSERVERSERVICE_INTERFACE_VERSION);
		//g_pEngineSound = FindInterface<IEngineSound>(modules::engine, IENGINESOUND_SERVER_INTERFACE_VERSION);
		g_pNetworkMessages = FindInterface<INetworkMessages>(modules::engine, NETWORKMESSAGES_INTERFACE_VERSION);

		ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);

		char confError[255] = "";
		g_CoreConfig = new CCoreConfig(utils::ConfigsDirectory() + "core.txt");
		if (!g_CoreConfig->Initialize(confError))
		{
			g_Logger.ErrorFormat("Could not read \"%s\": %s\n", g_CoreConfig->GetPath().c_str(), confError);
		}

		g_GameConfig = g_GameConfigManager.LoadGameConfigFile("cs2sdk.games.txt");

		DynLibUtils::CModule plugify("plugify");

		using IMetamodListenerFn = IMetamodListener* (*)();
		auto Plugify_ImmListener = plugify.GetFunctionByName("Plugify_ImmListener");
		g_MetamodListener = Plugify_ImmListener.CCast<IMetamodListenerFn>()();

		g_gameEventManager = (IGameEventManager2*)(CALL_VIRTUAL(uintptr_t, g_GameConfig->GetOffset("GameEventManager"), g_pSource2Server) - 8);

		// load more if needed
		//RESOLVE_SIG(g_GameConfig, "NetworkStateChanged", addresses::NetworkStateChanged);
		//RESOLVE_SIG(g_GameConfig, "StateChanged", addresses::StateChanged);
		RESOLVE_SIG(g_GameConfig, "UTIL_ClientPrintAll", addresses::UTIL_ClientPrintAll);
		RESOLVE_SIG(g_GameConfig, "ClientPrint", addresses::ClientPrint);
		// RESOLVE_SIG(g_GameConfig, "SetGroundEntity", addresses::SetGroundEntity);
		// RESOLVE_SIG(g_GameConfig, "CCSPlayerController_SwitchTeam", addresses::CCSPlayerController_SwitchTeam);
		// RESOLVE_SIG(g_GameConfig, "CBasePlayerController_SetPawn", addresses::CBasePlayerController_SetPawn);
		// RESOLVE_SIG(g_GameConfig, "CBaseModelEntity_SetModel", addresses::CBaseModelEntity_SetModel);
		// RESOLVE_SIG(g_GameConfig, "UTIL_Remove", addresses::UTIL_Remove);
		// RESOLVE_SIG(g_GameConfig, "CEntitySystem_AddEntityIOEvent", addresses::CEntitySystem_AddEntityIOEvent);
		// RESOLVE_SIG(g_GameConfig, "CEntityInstance_AcceptInput", addresses::CEntityInstance_AcceptInput);
		RESOLVE_SIG(g_GameConfig, "CGameEntitySystem_FindEntityByClassName", addresses::CGameEntitySystem_FindEntityByClassName);
		 RESOLVE_SIG(g_GameConfig, "CGameEntitySystem_FindEntityByName", addresses::CGameEntitySystem_FindEntityByName);
		// RESOLVE_SIG(g_GameConfig, "CGameRules_TerminateRound", addresses::CGameRules_TerminateRound);
		RESOLVE_SIG(g_GameConfig, "CreateEntityByName", addresses::CreateEntityByName);
		//RESOLVE_SIG(g_GameConfig, "DispatchSpawn", addresses::DispatchSpawn);
		// RESOLVE_SIG(g_GameConfig, "CEntityIdentity_SetEntityName", addresses::CEntityIdentity_SetEntityName);
		// RESOLVE_SIG(g_GameConfig, "CBaseEntity_EmitSoundParams", addresses::CBaseEntity_EmitSoundParams);
		// RESOLVE_SIG(g_GameConfig, "CBaseEntity_SetParent", addresses::CBaseEntity_SetParent);
		RESOLVE_SIG(g_GameConfig, "LegacyGameEventListener", addresses::GetLegacyGameEventListener);
	}

	void Terminate()
	{
		delete g_CoreConfig;
		delete modules::engine;
		delete modules::tier0;
		delete modules::server;
		delete modules::schemasystem;
		delete modules::filesystem;
		delete modules::vscript;
	}

} // namespace globals