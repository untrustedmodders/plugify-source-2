#pragma once

#include "listenermanager.h"
#include <igameevents.h>

#include <dynohook/dynohook.h>

class IClient;

struct EventInfo {
	IGameEvent* pEvent{ nullptr };
	bool bDontBroadcast{};
};

typedef ResultType (*FnEventListenerCallback)(const char* name, EventInfo* pEvent, bool bDontBroadcast);

using HookCallback = std::unique_ptr<CListenerManager<FnEventListenerCallback>>;

struct EventHook {
	std::string name;
	HookCallback preHook;
	HookCallback postHook;
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

	EventHookError HookEvent(const char* name, FnEventListenerCallback callback, EventHookMode mode = EventHookMode::Post);
	EventHookError UnhookEvent(const char* name, FnEventListenerCallback callback, EventHookMode mode = EventHookMode::Post);

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