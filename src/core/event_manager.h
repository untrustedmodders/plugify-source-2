#pragma once

#include "listener_manager.h"
#include <igameevents.h>

#include <dynohook/dynohook.h>

struct EventInfo
{
	IGameEvent* pEvent{nullptr};
	bool bDontBroadcast{};
};

using EventListenerCallback = ResultType (*)(const std::string& name, EventInfo* pEvent, bool bDontBroadcast);

using HookCallback = CListenerManager<EventListenerCallback>;

struct EventHook
{
	std::string name;
	std::unique_ptr<HookCallback> preHook;
	std::unique_ptr<HookCallback> postHook;
	uint32_t refCount{};
	bool postCopy{};
};

enum class EventHookError : int
{
	Okay = 0,
	InvalidEvent,
	NotActive,
	InvalidCallback,
};

class CEventManager : public IGameEventListener2
{
public:
	CEventManager() = default;
	~CEventManager() override;

	EventHookError HookEvent(const std::string& name, EventListenerCallback callback, HookMode mode = HookMode::Post);
	EventHookError UnhookEvent(const std::string& name, EventListenerCallback callback, HookMode mode = HookMode::Post);

	EventInfo* CreateEvent(const std::string& name, bool force = false);
	void FireEvent(EventInfo* pInfo, bool bDontBroadcast);
	void FireEventToClient(EventInfo* pInfo, int entityIndex);
	void CancelCreatedEvent(EventInfo* pInfo);

	dyno::ReturnAction Hook_OnFireEvent(dyno::IHook& hook);
	dyno::ReturnAction Hook_OnFireEvent_Post(dyno::IHook& hook);

private:
	void FireGameEvent(IGameEvent* event) override;

private:
	std::unordered_map<std::string, EventHook> m_eventHooks;
	std::stack<EventInfo*> m_freeEvents;
	std::stack<EventHook*> m_eventStack;
	std::stack<IGameEvent*> m_eventCopies;
};

extern CEventManager g_EventManager;