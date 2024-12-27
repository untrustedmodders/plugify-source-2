#pragma once

#include "listener_manager.hpp"
#include <plugin_export.h>

#define DEFINE_MANAGER_CREATOR(name, ret, ...) \
	using Fn##name = ret (*)(__VA_ARGS__);     \
	CListenerManager<Fn##name>& Get##name##ListenerManager();

DEFINE_MANAGER_CREATOR(OnClientConnect, bool, int, const plg::string&, const plg::string&)
DEFINE_MANAGER_CREATOR(OnClientConnect_Post, void, int)
DEFINE_MANAGER_CREATOR(OnClientConnected, void, int)
DEFINE_MANAGER_CREATOR(OnClientPutInServer, void, int)
DEFINE_MANAGER_CREATOR(OnClientDisconnect, void, int, int)
DEFINE_MANAGER_CREATOR(OnClientDisconnect_Post, void, int, int)
DEFINE_MANAGER_CREATOR(OnClientActive, void, int, bool)
DEFINE_MANAGER_CREATOR(OnClientFullyConnect, void, int)
DEFINE_MANAGER_CREATOR(OnClientSettingsChanged, void, int)
DEFINE_MANAGER_CREATOR(OnClientAuthorized, void, int, uint64)

DEFINE_MANAGER_CREATOR(OnLevelInit, void, const plg::string&, const plg::string&)
DEFINE_MANAGER_CREATOR(OnLevelShutdown, void)

DEFINE_MANAGER_CREATOR(OnEntitySpawned, void, CEntityInstance*)
DEFINE_MANAGER_CREATOR(OnEntityCreated, void, CEntityInstance*)
DEFINE_MANAGER_CREATOR(OnEntityDeleted, void, CEntityInstance*)
DEFINE_MANAGER_CREATOR(OnEntityParentChanged, void, CEntityInstance*, CEntityInstance*)

DEFINE_MANAGER_CREATOR(OnServerStartup, void)
DEFINE_MANAGER_CREATOR(OnServerActivate, void)
DEFINE_MANAGER_CREATOR(OnGameFrame, void, bool, bool, bool)
//DEFINE_MANAGER_CREATOR(OnServerHibernationUpdate, void, bool)
//DEFINE_MANAGER_CREATOR(OnGameServerSteamAPIActivated, void)
//DEFINE_MANAGER_CREATOR(OnGameServerSteamAPIDeactivated, void)
//DEFINE_MANAGER_CREATOR(OnHostNameChanged, void, const char*)
//DEFINE_MANAGER_CREATOR(OnPreFatalShutdown, void)
DEFINE_MANAGER_CREATOR(OnUpdateWhenNotInGame, void, float)
DEFINE_MANAGER_CREATOR(OnPreWorldUpdate, void, bool)

/*DEFINE_MANAGER_CREATOR(OnNetworkidValidated)
DEFINE_MANAGER_CREATOR(OnEdictAllocated)
DEFINE_MANAGER_CREATOR(OnEdictFreed)
DEFINE_MANAGER_CREATOR(OnQueryCvarValueFinished)
DEFINE_MANAGER_CREATOR(OnEntityPreSpawned)
DEFINE_MANAGER_CREATOR(OnNetworkedEntityPreSpawned)
DEFINE_MANAGER_CREATOR(OnNetworkedEntityCreated)
DEFINE_MANAGER_CREATOR(OnNetworkedEntitySpawned)
DEFINE_MANAGER_CREATOR(OnNetworkedEntityDeleted)
DEFINE_MANAGER_CREATOR(OnDataLoaded)
DEFINE_MANAGER_CREATOR(OnCombinerPreCache)
DEFINE_MANAGER_CREATOR(OnDataUnloaded)
DEFINE_MANAGER_CREATOR(OnPlayerRunCommand)
DEFINE_MANAGER_CREATOR(OnPlayerPostRunCommand)
DEFINE_MANAGER_CREATOR(OnButtonStateChanged)*/
