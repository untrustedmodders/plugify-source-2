#pragma once

#include "listenermanager.h"

#define DEFINE_MANAGER_ACCESSOR(name, ret, ...)                 \
	typedef ret (*Fn##name)(__VA_ARGS__);                       \
	auto& Get##name##ListenerManager()                          \
	{                                                           \
		static CListenerManager<Fn##name> s_##name;             \
		return s_##name;                                        \
	}                                                           \
	extern "C" PLUGIN_API void name##_Register(Fn##name func)   \
	{                                                           \
		Get##name##ListenerManager().Register(func);            \
	}                                                           \
	extern "C" PLUGIN_API void name##_Unregister(Fn##name func) \
	{                                                           \
		Get##name##ListenerManager().Unregister(func);          \
	}

DEFINE_MANAGER_ACCESSOR(OnClientActive, void, int, bool, const std::string&, uint64)
DEFINE_MANAGER_ACCESSOR(OnClientConnect, bool, int, const std::string&, uint64, const std::string&, bool, const std::string&)
DEFINE_MANAGER_ACCESSOR(OnClientConnected, void, int, const std::string&, uint64, const std::string&, const std::string&, bool)
DEFINE_MANAGER_ACCESSOR(OnClientFullyConnect, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientDisconnect, void, int, int, const std::string&, uint64, const std::string&)
DEFINE_MANAGER_ACCESSOR(OnClientPutInServer, void, int, char const*, int, uint64)
DEFINE_MANAGER_ACCESSOR(OnClientSettingsChanged, void, int)
DEFINE_MANAGER_ACCESSOR(OnLevelInit, void, const std::string&, const std::string&)
DEFINE_MANAGER_ACCESSOR(OnLevelShutdown, void)
/*DEFINE_MANAGER_ACCESSOR(OnNetworkidValidated)
DEFINE_MANAGER_ACCESSOR(OnEdictAllocated)
DEFINE_MANAGER_ACCESSOR(OnEdictFreed)
DEFINE_MANAGER_ACCESSOR(OnQueryCvarValueFinished)
DEFINE_MANAGER_ACCESSOR(OnServerActivate)
DEFINE_MANAGER_ACCESSOR(OnGameFrame)
DEFINE_MANAGER_ACCESSOR(OnEntityPreSpawned)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntityPreSpawned)
DEFINE_MANAGER_ACCESSOR(OnEntityCreated)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntityCreated)
DEFINE_MANAGER_ACCESSOR(OnEntitySpawned)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntitySpawned)
DEFINE_MANAGER_ACCESSOR(OnEntityDeleted)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntityDeleted)
DEFINE_MANAGER_ACCESSOR(OnDataLoaded)
DEFINE_MANAGER_ACCESSOR(OnCombinerPreCache)
DEFINE_MANAGER_ACCESSOR(OnDataUnloaded)
DEFINE_MANAGER_ACCESSOR(OnPlayerRunCommand)
DEFINE_MANAGER_ACCESSOR(OnPlayerPostRunCommand)
DEFINE_MANAGER_ACCESSOR(OnButtonStateChanged)*/
