#pragma once

#include "listener_manager.hpp"
#include <igameevents.h>

struct EventInfo {
	IGameEvent* event{};
	bool dontBroadcast{};
};

using EventListenerCallback = ResultType (*)(const plg::string& name, EventInfo* pEvent, bool bDontBroadcast);

using HookCallback = ListenerManager<EventListenerCallback>;

struct EventHook {
	plg::string name;
	std::unique_ptr<HookCallback> preHook;
	std::unique_ptr<HookCallback> postHook;
	uint32_t refCount{};
	bool postCopy{};
};

enum class EventHookError : int {
	Okay = 0,
	InvalidEvent,
	NotActive,
	InvalidCallback,
};

class EventManager : public IGameEventListener2 {
public:
	EventManager() = default;
	~EventManager() override;

	EventHookError HookEvent(const plg::string& name, EventListenerCallback callback, HookMode mode = HookMode::Post);
	EventHookError UnhookEvent(const plg::string& name, EventListenerCallback callback, HookMode mode = HookMode::Post);

	EventInfo* CreateEvent(const plg::string& name, bool force = false);
	void FireEvent(EventInfo* info, bool dontBroadcast);
	void FireEventToClient(EventInfo* info, CPlayerSlot slot);
	void CancelCreatedEvent(EventInfo* info);

	ResultType OnFireEvent(IGameEvent* event, bool dontBroadcast);
	ResultType OnFireEvent_Post(IGameEvent* event, bool dontBroadcast);

private:
	void FireGameEvent(IGameEvent* event) override;

private:
	std::unordered_map<plg::string, EventHook> m_eventHooks;
	std::stack<EventInfo*> m_freeEvents;
	std::stack<EventHook*> m_eventStack;
	std::stack<IGameEvent*> m_eventCopies;
	std::mutex m_registerEventLock;
};

extern EventManager g_EventManager;