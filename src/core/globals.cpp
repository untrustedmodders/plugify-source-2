
#include "globals.hpp"
#include "core_config.hpp"
#include "game_config.hpp"
#include "hook_holder.hpp"

#include <core/sdk/utils.h>
#include <core/sdk/virtual.h>

#include <dynlibutils/module.hpp>

#include <IEngineSound.h>
#include <ISmmPlugin.h>
#include <engine/igameeventsystem.h>
#include <igameevents.h>
#include <igamesystemfactory.h>

#define RESOLVE_SIG(gameConfig, name, variable) \
	variable = (gameConfig)->ResolveSignature(name).RCast<decltype(variable)>(); \
	if (!(variable)) { \
		S2_LOGF(LS_ERROR, "Failed to resolve signature for {}\n", #name); \
		return; \
	} \

IMetamodListener* g_pMetamodListener = nullptr;
std::unique_ptr<CoreConfig> g_pCoreConfig = nullptr;
std::unique_ptr<GameConfig> g_pGameConfig = nullptr;

namespace globals {
	void Initialize(std::unordered_map<std::string, fs::path> paths) {
		g_pCoreConfig = std::make_unique<CoreConfig>(plg::vector<plg::string>{
				(paths["base"] / "settings.jsonc").generic_string(),
				(paths["configs"] / "settings.jsonc").generic_string(),
				(paths["data"] / "settings.jsonc").generic_string()
		});
		if (!g_pCoreConfig->Initialize()) {
			S2_LOG(LS_ERROR, "Failed to load settings configuration!\n");
			return;
		}
		g_pGameConfig = std::make_unique<GameConfig>("csgo", plg::vector<plg::string>{
				(paths["base"] / "gamedata.jsonc").generic_string(),
				(paths["configs"] / "gamedata.jsonc").generic_string(),
				(paths["data"] / "gamedata.jsonc").generic_string()
		});
		if (!g_pGameConfig->Initialize()) {
			S2_LOG(LS_ERROR, "Failed to load gamedata configuration!\n");
			return;
		}

		CAppSystemDict** p_ppCurrentAppSystem = g_pGameConfig->GetAddress("&s_pCurrentAppSystem").RCast<CAppSystemDict**>();
		if (!p_ppCurrentAppSystem) {
			S2_LOG(LS_ERROR, "s_pCurrentAppSystem not found!\n");
			return;
		}
		g_pCurrentAppSystem = *p_ppCurrentAppSystem;

		IGameEventManager2** p_ppGameEventManager = g_pGameConfig->GetAddress("&s_GameEventManager").RCast<IGameEventManager2**>();
		if (!p_ppGameEventManager) {
			S2_LOG(LS_ERROR, "s_GameEventManager not found!\n");
			return;
		}
		g_pGameEventManager = *p_ppGameEventManager;

		g_pCVar = static_cast<ICvar*>(QueryInterface("tier0", CVAR_INTERFACE_VERSION));
		g_pSchemaSystem = static_cast<ISchemaSystem*>(QueryInterface("schemasystem", SCHEMASYSTEM_INTERFACE_VERSION));
		g_pSource2Server = static_cast<ISource2Server*>(QueryInterface("server", SOURCE2SERVER_INTERFACE_VERSION));
		g_pSource2GameEntities = static_cast<ISource2GameEntities*>(QueryInterface("server", SOURCE2GAMEENTITIES_INTERFACE_VERSION));
		g_pSource2GameClients = static_cast<ISource2GameClients*>(QueryInterface("server", SOURCE2GAMECLIENTS_INTERFACE_VERSION));
		g_pGameResourceServiceServer = static_cast<IGameResourceService*>(QueryInterface("engine2", GAMERESOURCESERVICESERVER_INTERFACE_VERSION));
		g_pEngineServiceMgr = static_cast<IEngineServiceMgr*>(QueryInterface("engine2", ENGINESERVICEMGR_INTERFACE_VERSION));

		g_pEngineServer = static_cast<IVEngineServer2*>(QueryInterface("engine2", SOURCE2ENGINETOSERVER_INTERFACE_VERSION));
		g_pFullFileSystem = static_cast<IFileSystem*>(QueryInterface("filesystem", FILESYSTEM_INTERFACE_VERSION));
		g_pGameEventSystem = static_cast<IGameEventSystem*>(QueryInterface("engine2", GAMEEVENTSYSTEM_INTERFACE_VERSION));
		g_pNetworkServerService = static_cast<INetworkServerService*>(QueryInterface("engine", NETWORKSERVERSERVICE_INTERFACE_VERSION));
		g_pNetworkMessages = static_cast<INetworkMessages*>(QueryInterface("networksystem", NETWORKMESSAGES_INTERFACE_VERSION));

		ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);

		Module plugify("plugify");

		using IMetamodListenerFn = IMetamodListener* (*) ();
		auto Plugify_ImmListener = plugify.GetFunctionByName("Plugify_ImmListener");
		if (Plugify_ImmListener) {
			g_pMetamodListener = Plugify_ImmListener.CCast<IMetamodListenerFn>()();
		}

		// load more if needed
		RESOLVE_SIG(g_pGameConfig, "LegacyGameEventListener", addresses::GetLegacyGameEventListener);
		RESOLVE_SIG(g_pGameConfig, "CCSPlayer_WeaponServices_RemoveItem", addresses::CCSPlayer_WeaponServices_RemoveItem);
		//RESOLVE_SIG(g_pGameConfig, "GetCSWeaponDataFromKey", addresses::GetCSWeaponDataFromKey);
		RESOLVE_SIG(g_pGameConfig, "CCSPlayerController_SwitchTeam", addresses::CCSPlayerController_SwitchTeam);
		RESOLVE_SIG(g_pGameConfig, "CBasePlayerController_SetPawn", addresses::CBasePlayerController_SetPawn);
		RESOLVE_SIG(g_pGameConfig, "CBaseModelEntity_SetModel", addresses::CBaseModelEntity_SetModel);
		RESOLVE_SIG(g_pGameConfig, "CGameEntitySystem_FindEntityByClassName", addresses::CGameEntitySystem_FindEntityByClassName);
		RESOLVE_SIG(g_pGameConfig, "CGameEntitySystem_FindEntityByName", addresses::CGameEntitySystem_FindEntityByName);
		RESOLVE_SIG(g_pGameConfig, "CreateEntityByName", addresses::CreateEntityByName);
		RESOLVE_SIG(g_pGameConfig, "DispatchSpawn", addresses::DispatchSpawn);
		RESOLVE_SIG(g_pGameConfig, "UTIL_Remove", addresses::UTIL_Remove);
		RESOLVE_SIG(g_pGameConfig, "CEntityInstance_AcceptInput", addresses::CEntityInstance_AcceptInput);
		RESOLVE_SIG(g_pGameConfig, "CGameRules_TerminateRound", addresses::CGameRules_TerminateRound);
		RESOLVE_SIG(g_pGameConfig, "CEntityIdentity_SetEntityName", addresses::CEntityIdentity_SetEntityName);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_EmitSoundParams", addresses::CBaseEntity_EmitSoundParams);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_SetParent", addresses::CBaseEntity_SetParent);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_EmitSoundFilter", addresses::CBaseEntity_EmitSoundFilter);
		RESOLVE_SIG(g_pGameConfig, "CBaseEntity_SetMoveType", addresses::CBaseEntity_SetMoveType);
	}

	void Terminate() {
		g_pGameConfig.reset();
		g_pCoreConfig.reset();
	}

	PlatModule_t FindModule(std::string_view name) {
		for (const auto& module : g_pCurrentAppSystem->m_Modules) {
			if (module.m_pModuleName == name) {
				return module.m_hModule;
			}
		}
		S2_LOGF(LS_ERROR, "Could not find module at \"{}\"\n", name);
		return {};
	}
	
	IAppSystem* FindInterface(std::string_view name) {
		for (const auto& system : g_pCurrentAppSystem->m_Systems) {
			if (system.m_pInterfaceName && system.m_pInterfaceName == name) {
				return system.m_pSystem;
			}
		}
		S2_LOGF(LS_ERROR, "Could not find interface at \"{}\"\n", name);
		return {};
	}

	void* QueryInterface(std::string_view module, std::string_view name) {
		for (const auto& system : g_pCurrentAppSystem->m_Systems) {
			if (system.m_pInterfaceName && system.m_pInterfaceName == name) {
				return system.m_pSystem;
			}

			if (system.m_pModuleName && system.m_pModuleName == module) {
				if (auto* interface = system.m_pSystem->QueryInterface(name.data())) {
					return interface;
				}
			}
		}

		S2_LOGF(LS_ERROR, "Could not query interface at \"{}\"\n", name);
		return {};
	}
}// namespace globals