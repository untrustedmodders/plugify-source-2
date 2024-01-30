#pragma once

#include <concepts>

namespace cs2sdk {
	template <class = void>
	class CListenerManager;

	template<class Ret, class... Args>
	class CListenerManager<Ret(*)(Args...)> {
	public:
		using Func = Ret(*)(Args...);

		template<typename Callable>
		void Register(Callable&& callable) requires std::invocable<Callable, Args...> {
			// Is the callable already in the vector?
			if(!IsRegistered(callable)) {
				m_vecCallables.AddToTail(callable);
			} else {
				Error("Callback already registered.");
			}
		}

		template<typename Callable>
		void Unregister(Callable&& callable) requires std::invocable<Callable, Args...> {
			int index = Find(callable);
			if (index == -1) {
				Error("Callback not registered.");
			} else {
				m_vecCallables.Remove(index);
			}
		}

		template <typename Callable>
		bool IsRegistered(Callable&& callable) {
			return Find(callable) != -1;
		}

		template <typename Callable>
		int Find(Callable&& callable) {
			for (int i = 0; i < m_vecCallables.Count(); ++i) {
				if (callable == m_vecCallables[i]) {
					return i;
				}
			}
			return -1;
		}

		void Notify(Args... args) const {
			for (int i = 0; i < m_vecCallables.Count(); ++i) {
				m_vecCallables[i](std::forward<Args>(args)...);
			}
		}

		Ret operator()(size_t index, Args... args) const {
			return m_vecCallables[index](std::forward<Args>(args)...);
		}

		void Clear() {
			if (GetCount()) {
				m_vecCallables.RemoveAll();
			}
		}

		int GetCount() {
			return m_vecCallables.Count();
		}

	private:
		CUtlVector<Func> m_vecCallables;
	};
}

#define DEFINE_MANAGER_ACCESSOR(name, ret, ...) \
    typedef ret (*Fn##name)(__VA_ARGS__); \
    auto& Get##name##ListenerManager() { \
		static cs2sdk::CListenerManager<Fn##name> s_##name; \
		return s_##name; \
	} \
	extern "C" \
	PLUGIN_API void name##_Register(Fn##name func) { \
		Get##name##ListenerManager().Register(func); \
	} \
	extern "C" \
	PLUGIN_API void name##_Unregister(Fn##name func) { \
		Get##name##ListenerManager().Unregister(func); \
	}

DEFINE_MANAGER_ACCESSOR(OnClientActive, void, CPlayerSlot, bool, const char*, uint64)
DEFINE_MANAGER_ACCESSOR(OnClientConnect, bool, CPlayerSlot, const char*, uint64, const char*, bool, const char*)
DEFINE_MANAGER_ACCESSOR(OnClientConnected, void, CPlayerSlot, const char*, uint64, const char*, const char*, bool)
DEFINE_MANAGER_ACCESSOR(OnClientFullyConnect, void, CPlayerSlot)
DEFINE_MANAGER_ACCESSOR(OnClientDisconnect, void, CPlayerSlot, int, const char*, uint64, const char*)
DEFINE_MANAGER_ACCESSOR(OnClientPutInServer, void, CPlayerSlot, char const*, int, uint64)
DEFINE_MANAGER_ACCESSOR(OnClientSettingsChanged, void, CPlayerSlot)
DEFINE_MANAGER_ACCESSOR(OnLevelInit, void, const char*, const char*)
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