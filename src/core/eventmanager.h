#pragma once

#include "listenermanager.h"
#include <igameevents.h>
#include <dynohook/dynohook.h>

class IClient;

namespace cs2sdk {
	struct EventInfo {
		IGameEvent* pEvent{ nullptr };
		bool bDontBroadcast{};
	};

	enum class ResultType : uint8_t {
		Continue = 0,
		Changed = 1,
		Handled = 3,
		Stop = 4,
	};

	typedef ResultType (*FnEventListenerCallback)(const char* name, EventInfo* pEvent, bool bDontBroadcast);

	using Hook = std::unique_ptr<CListenerManager<FnEventListenerCallback>>;

	struct EventHook {
		std::string name;
		Hook pPreHook;
		Hook pPostHook;
		uint32_t refCount{};
		bool postCopy{};
	};

	enum class EventHookMode : bool {
		Pre,
		Post,
	};

	enum class EventHookError : uint8_t {
		Okay = 0,
		InvalidEvent,
		NotActive,
		InvalidCallback,
	};

	class CEventManager : public IGameEventListener2 {
	public:
		CEventManager() = default;
		~CEventManager() override;

		EventHookError HookEvent(const char* name, FnEventListenerCallback pCallback, EventHookMode mode = EventHookMode::Post);
		EventHookError UnhookEvent(const char* name, FnEventListenerCallback pCallback, EventHookMode mode = EventHookMode::Post);

		EventInfo* CreateEvent(const char* name, bool force = false);
		void FireEvent(EventInfo* pInfo, bool bDontBroadcast);
		void FireEventToClient(EventInfo* pInfo, IClient* pClient);
		void CancelCreatedEvent(EventInfo* pInfo);

		dyno::ReturnAction Hook_OnFireEvent(dyno::IHook& hook);
		dyno::ReturnAction Hook_OnFireEvent_Post(dyno::IHook& hook);

	private:
		void FireGameEvent(IGameEvent* event) override;

	private:
		std::unordered_map<std::string, EventHook> m_EventHooks;
		std::stack<EventInfo *> m_FreeEvents;
		std::stack<EventHook *> m_EventStack;
		std::stack<IGameEvent *> m_EventCopies;
	};
	
	extern CEventManager g_EventManager;
}