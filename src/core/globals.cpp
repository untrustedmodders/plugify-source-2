
#include "globals.h"
#include "core_config.h"
#include "game_config.h"
#include "module.h"

#include <utils/utils.h>
#include <utils/virtual.h>

#include <ISmmPlugin.h>
#include <igameevents.h>
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
IUniformRandomStream* random = nullptr;

#define RESOLVE_SIG(gameConfig, name, variable) \
	variable = (decltype(variable))(void*)(gameConfig)->ResolveSignature(name)

namespace modules
{
	DynLibUtils::CModule* engine = nullptr;
	DynLibUtils::CModule* tier0 = nullptr;
	DynLibUtils::CModule* server = nullptr;
	DynLibUtils::CModule* schemasystem = nullptr;
	DynLibUtils::CModule* filesystem = nullptr;
	DynLibUtils::CModule* vscript = nullptr;
	DynLibUtils::CModule* networksystem = nullptr;
} // namespace modules

IMetamodListener* g_pMetamodListener = nullptr;
CCoreConfig* g_pCoreConfig = nullptr;
CGameConfig* g_pGameConfig = nullptr;

template <class T>
T* FindInterface(const DynLibUtils::CModule* module, const char* name)
{
	auto fn = module->GetFunctionByName("CreateInterface");
	if (!fn)
	{
		g_Logger.ErrorFormat("Could not find CreateInterface in %s at \"%s\"\n", module->GetModuleName().data(), module->GetModulePath().data());
		return nullptr;
	}

	void* pInterface = fn.CCast<CreateInterfaceFn>()(name, nullptr);
	if (!pInterface)
	{
		g_Logger.ErrorFormat("Could not find interface: %s in at \"%s\"\n", name, module->GetModuleName().data(), module->GetModulePath().data());
		return nullptr;
	}

	return (T*)pInterface;
}

// we cannot return a char array from a function, therefore we need a wrapper
template<size_t N>
struct String {
	char c[N];
};

template<size_t N>
constexpr auto copy(const char (&str)[N])
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

#define BINARY_PATH(path, name) copy(path BINARY_MODULE_PREFIX name)

namespace globals
{
	void Initialize()
	{
		std::string gameDir(Plat_GetGameDirectory());

		constexpr String enginePath = BINARY_PATH(CS2SDK_ROOT_BINARY, "engine2");
		constexpr String tier0Path = BINARY_PATH(CS2SDK_ROOT_BINARY, "tier0");
		constexpr String serverPath = BINARY_PATH(CS2SDK_GAME_BINARY, "server");
		constexpr String schemasystemPath = BINARY_PATH(CS2SDK_ROOT_BINARY, "schemasystem");
		constexpr String filesystemPath = BINARY_PATH(CS2SDK_ROOT_BINARY, "filesystem_stdio");
		constexpr String vscriptPath = BINARY_PATH(CS2SDK_ROOT_BINARY, "vscript");
		constexpr String networksystemPath = BINARY_PATH(CS2SDK_ROOT_BINARY, "networksystem");

		modules::engine = new DynLibUtils::CModule(gameDir + enginePath.c);
		modules::tier0 = new DynLibUtils::CModule(gameDir + tier0Path.c);
		modules::server = new DynLibUtils::CModule(gameDir + serverPath.c);
		modules::schemasystem = new DynLibUtils::CModule(gameDir + schemasystemPath.c);
		modules::filesystem = new DynLibUtils::CModule(gameDir + filesystemPath.c);
		modules::vscript = new DynLibUtils::CModule(gameDir + vscriptPath.c);
		modules::networksystem = new DynLibUtils::CModule(gameDir + networksystemPath.c);

		g_pCVar = FindInterface<ICvar>(modules::tier0, CVAR_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSchemaSystem2 = FindInterface<CSchemaSystem>(modules::schemasystem, SCHEMASYSTEM_INTERFACE_VERSION);
		g_pSource2Server = FindInterface<ISource2Server>(modules::server, SOURCE2SERVER_INTERFACE_VERSION);
		g_pSource2GameEntities = FindInterface<ISource2GameEntities>(modules::server, SOURCE2GAMEENTITIES_INTERFACE_VERSION);
		g_pSource2GameClients = FindInterface<IServerGameClients>(modules::server, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
		g_pGameResourceServiceServer = FindInterface<IGameResourceServiceServer>(modules::engine, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
		//random = FindInterface<IUniformRandomStream>(modules::engine, VENGINE_SERVER_RANDOM_INTERFACE_VERSION);

		g_pEngineServer2 = FindInterface<IVEngineServer2>(modules::engine, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
		g_pFullFileSystem = FindInterface<IFileSystem>(modules::filesystem, FILESYSTEM_INTERFACE_VERSION);
		g_gameEventSystem = FindInterface<IGameEventSystem>(modules::engine, GAMEEVENTSYSTEM_INTERFACE_VERSION);
		g_pNetworkServerService = FindInterface<INetworkServerService>(modules::engine, NETWORKSERVERSERVICE_INTERFACE_VERSION);
		//g_pEngineSound = FindInterface<IEngineSound>(modules::engine, IENGINESOUND_SERVER_INTERFACE_VERSION);
		g_pNetworkMessages = FindInterface<INetworkMessages>(modules::networksystem, NETWORKMESSAGES_INTERFACE_VERSION);

		printf("--------------%s\n", typeid(*g_pSource2Server).name());

		ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);

		char confError[255] = "";
		g_pCoreConfig = new CCoreConfig(utils::ConfigsDirectory() + "core.txt");
		if (!g_pCoreConfig->Initialize(confError))
		{
			g_Logger.ErrorFormat("Could not read \"%s\": %s\n", g_pCoreConfig->GetPath().c_str(), confError);
		}

		g_pGameConfig = g_pGameConfigManager.LoadGameConfigFile("cs2sdk.games.txt");
		if (!g_pGameConfig)
		{
			g_Logger.Error("cs2sdk.games.txt not found!");
		}

		DynLibUtils::CModule plugify("plugify");

		using IMetamodListenerFn = IMetamodListener* (*)();
		auto Plugify_ImmListener = plugify.GetFunctionByName("Plugify_ImmListener");
		g_pMetamodListener = Plugify_ImmListener.CCast<IMetamodListenerFn>()();

		g_gameEventManager = static_cast<IGameEventManager2*>(CALL_VIRTUAL(IToolGameEventAPI*, int(93), g_pSource2Server));
		if (g_gameEventManager == nullptr)
		{
			g_Logger.Error("GameEventManager not found!");
		}

		// load more if needed
		RESOLVE_SIG(g_pGameConfig, "LegacyGameEventListener", addresses::GetLegacyGameEventListener);
		RESOLVE_SIG(g_pGameConfig, "CCSPlayerController_SwitchTeam", addresses::CCSPlayerController_SwitchTeam);
		RESOLVE_SIG(g_pGameConfig, "CBasePlayerController_SetPawn", addresses::CBasePlayerController_SetPawn);
		RESOLVE_SIG(g_pGameConfig, "CGameEntitySystem_FindEntityByClassName", addresses::CGameEntitySystem_FindEntityByClassName);
		RESOLVE_SIG(g_pGameConfig, "CGameEntitySystem_FindEntityByName", addresses::CGameEntitySystem_FindEntityByName);
		RESOLVE_SIG(g_pGameConfig, "CreateEntityByName", addresses::CreateEntityByName);
		RESOLVE_SIG(g_pGameConfig, "DispatchSpawn", addresses::DispatchSpawn);
	}

	void Terminate()
	{
		delete g_pCoreConfig;
		delete modules::engine;
		delete modules::tier0;
		delete modules::server;
		delete modules::schemasystem;
		delete modules::filesystem;
		delete modules::vscript;
	}

} // namespace globals