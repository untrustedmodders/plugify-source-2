#pragma once

#include <dynohook/dynohook.h>
#include "gameevents.h"

class IClient;

namespace cs2sdk {
	enum class EventHookMode : bool {
		Pre,
		Post,
	};

	enum class EventHookError : byte {
		Okay = 0,
	};

	class CEventManager {
	public:
		/*EventHookError HookEvent(const char* name, FnEventListenerCallback* pCallback, EventHookMode mode = EventHookMode::Post);
		EventHookError UnhookEvent(const char* name, FnEventListenerCallback* pCallback, EventHookMode mode = EventHookMode::Post);

		IGameEvent* CreateEvent(const char* name, bool force = false);
		void FireEvent(IGameEvent* event);
		void FireEventToClient(IGameEvent* event, IClient* pClient);
		void CancelCreatedEvent(IGameEvent* event);*/

		dyno::ReturnAction Hook_OnFireEvent(dyno::CallbackType type, dyno::IHook& hook);

	private:
		//NameHashSet<EventHook *> m_EventHooks;
	};
	
	extern CEventManager g_EventManager;
}