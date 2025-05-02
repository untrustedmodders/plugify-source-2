#include "listeners.hpp"

#define DEFINE_MANAGER_ACCESSOR(name, ret, ...)                   \
	using Fn##name = ret (*)(__VA_ARGS__);                        \
	ListenerManager<Fn##name>& Get##name##ListenerManager() {    \
		static ListenerManager<Fn##name> s_##name;               \
		return s_##name;                                          \
	}                                                             \
	extern "C" PLUGIN_API void name##_Register(Fn##name func) {   \
		Get##name##ListenerManager().Register(func);              \
	}                                                             \
	extern "C" PLUGIN_API void name##_Unregister(Fn##name func) { \
		Get##name##ListenerManager().Unregister(func);            \
	}

DEFINE_MANAGER_ACCESSOR(OnClientConnect, bool, int, const plg::string&, const plg::string&)
DEFINE_MANAGER_ACCESSOR(OnClientConnect_Post, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientConnected, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientPutInServer, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientDisconnect, void, int, int)
DEFINE_MANAGER_ACCESSOR(OnClientDisconnect_Post, void, int, int)
DEFINE_MANAGER_ACCESSOR(OnClientActive, void, int, bool)
DEFINE_MANAGER_ACCESSOR(OnClientFullyConnect, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientSettingsChanged, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientAuthenticated, void, int, uint64)

DEFINE_MANAGER_ACCESSOR(OnLevelInit, void, const plg::string&, const plg::string&)
DEFINE_MANAGER_ACCESSOR(OnLevelShutdown, void)

DEFINE_MANAGER_ACCESSOR(OnEntitySpawned, void, int)
DEFINE_MANAGER_ACCESSOR(OnEntityCreated, void, int)
DEFINE_MANAGER_ACCESSOR(OnEntityDeleted, void, int)
DEFINE_MANAGER_ACCESSOR(OnEntityParentChanged, void, int, int)

DEFINE_MANAGER_ACCESSOR(OnServerStartup, void)
DEFINE_MANAGER_ACCESSOR(OnServerActivate, void)
DEFINE_MANAGER_ACCESSOR(OnServerSpawn, void)
DEFINE_MANAGER_ACCESSOR(OnChangeLevel, void)
DEFINE_MANAGER_ACCESSOR(OnGameFrame, void, bool, bool, bool)
//DEFINE_MANAGER_ACCESSOR(OnServerHibernationUpdate, void, bool)
//DEFINE_MANAGER_ACCESSOR(OnGameServerSteamAPIActivated, void)
//DEFINE_MANAGER_ACCESSOR(OnGameServerSteamAPIDeactivated, void)
//DEFINE_MANAGER_ACCESSOR(OnHostNameChanged, void, const char*)
//DEFINE_MANAGER_ACCESSOR(OnPreFatalShutdown, void)
DEFINE_MANAGER_ACCESSOR(OnUpdateWhenNotInGame, void, float)
DEFINE_MANAGER_ACCESSOR(OnPreWorldUpdate, void, bool)