#include "event_manager.hpp"

EventManager::~EventManager() {
	while (!m_freeEvents.empty()) {
		delete m_freeEvents.top();
		m_freeEvents.pop();
	}

	if (!m_eventHooks.empty()) {
		g_pGameEventManager->RemoveListener(this);
	}
}

EventHookError EventManager::HookEvent(const plg::string& name, EventListenerCallback callback, HookMode mode) {
	std::unique_lock<std::mutex> lock(m_registerEventLock);

	if (!g_pGameEventManager->FindListener(this, name.c_str())) {
		if (!g_pGameEventManager->AddListener(this, name.c_str(), true)) {
			return EventHookError::InvalidEvent;
		}
	}

	auto it = m_eventHooks.find(name);
	if (it == m_eventHooks.end()) {
		EventHook eventHook{name};

		if (mode == HookMode::Pre) {
			eventHook.preHook = std::make_unique<HookCallback>();
			eventHook.preHook->Register(callback);
		} else {
			eventHook.postHook = std::make_unique<HookCallback>();
			eventHook.postHook->Register(callback);
			eventHook.postCopy = (mode == HookMode::Post);
		}

		++eventHook.refCount;

		m_eventHooks.emplace(name, std::move(eventHook));

		return EventHookError::Okay;
	}

	auto& eventHook = std::get<EventHook>(*it);
	if (mode == HookMode::Pre) {
		if (eventHook.preHook == nullptr) {
			eventHook.preHook = std::make_unique<HookCallback>();
		}

		eventHook.preHook->Register(callback);
	} else {
		if (eventHook.postHook == nullptr) {
			eventHook.postHook = std::make_unique<HookCallback>();
		}

		if (!eventHook.postCopy) {
			eventHook.postCopy = (mode == HookMode::Post);
		}

		eventHook.postHook->Register(callback);
	}

	++eventHook.refCount;

	return EventHookError::Okay;
}

EventHookError EventManager::UnhookEvent(const plg::string& name, EventListenerCallback callback, HookMode mode) {
	std::unique_lock<std::mutex> lock(m_registerEventLock);

	auto it = m_eventHooks.find(name);
	if (it == m_eventHooks.end()) {
		return EventHookError::NotActive;
	}

	std::unique_ptr<HookCallback>* callbackHook;
	auto& eventHook = std::get<EventHook>(*it);
	if (mode == HookMode::Pre) {
		callbackHook = &eventHook.preHook;
	} else {
		callbackHook = &eventHook.postHook;
	}

	if (*callbackHook == nullptr || !(*callbackHook)->Unregister(callback)) {
		return EventHookError::InvalidCallback;
	}

	if (--eventHook.refCount == 0) {
		m_eventHooks.erase(it);
	}

	return EventHookError::Okay;
}

void EventManager::FireGameEvent(IGameEvent* event) {
}

void EventManager::FireEvent(EventInfo* info, bool dontBroadcast) {
	g_pGameEventManager->FireEvent(info->event, dontBroadcast);

	m_freeEvents.push(info);
}

EventInfo* EventManager::CreateEvent(const plg::string& name, bool force) {
	EventInfo* info;
	IGameEvent* event = g_pGameEventManager->CreateEvent(name.c_str(), force);

	if (event) {
		if (m_freeEvents.empty()) {
			info = new EventInfo{};
		} else {
			info = m_freeEvents.top();
			m_freeEvents.pop();
		}

		info->event = event;
		info->dontBroadcast = false;

		return info;
	}

	return nullptr;
}

void EventManager::FireEventToClient(EventInfo* info, CPlayerSlot slot) {
	IGameEventListener2* listener = addresses::GetLegacyGameEventListener(slot);

	listener->FireGameEvent(info->event);
}

void EventManager::CancelCreatedEvent(EventInfo* info) {
	g_pGameEventManager->FreeEvent(info->event);

	m_freeEvents.push(info);
}

ResultType EventManager::OnFireEvent(IGameEvent* event, const bool dontBroadcast) {
	if (!event)
		return ResultType::Continue;

	plg::string name(event->GetName());
	bool localDontBroadcast = dontBroadcast;

	auto it = m_eventHooks.find(name);
	if (it != m_eventHooks.end()) {
		auto& eventHook = std::get<EventHook>(*it);
		++eventHook.refCount;
		m_eventStack.push(&eventHook);

		if (eventHook.preHook != nullptr) {
			//S2_LOGF(LS_DEBUG, "Pushing event `{}` pointer: {}, dont broadcast: {}, post: {}\n", pEvent->GetName(), pEvent, bDontBroadcast, false);

			EventInfo eventInfo{event, dontBroadcast};

			for (size_t i = 0; i < eventHook.preHook->GetCount(); ++i) {
				auto result = eventHook.preHook->Notify(i, name, &eventInfo, dontBroadcast);
				localDontBroadcast = eventInfo.dontBroadcast;

				if (result >= ResultType::Handled) {
					// m_EventCopies.push(g_gameEventManager->DuplicateEvent(pEvent));
					g_pGameEventManager->FreeEvent(event);
					return ResultType::Handled;
				}
			}
		}

		if (eventHook.postCopy) {
			m_eventCopies.push(g_pGameEventManager->DuplicateEvent(event));
		}
	} else {
		m_eventStack.push(nullptr);
	}

	if (localDontBroadcast != dontBroadcast) {
		return ResultType::Changed;
	}

	return ResultType::Continue;
}

ResultType EventManager::OnFireEvent_Post(IGameEvent* event, bool dontBroadcast) {
	if (!event)
		return ResultType::Continue;

	EventHook* hook = m_eventStack.top();

	if (hook != nullptr) {
		if (hook->postHook != nullptr) {
			if (hook->postCopy) {
				auto eventCopy = m_eventCopies.top();
				//S2_LOGF(LS_DEBUG, "Pushing event `{}` pointer: {}, dont broadcast: {}, post: {}\n", eventCopy->GetName(), eventCopy, bDontBroadcast, true);
				EventInfo eventInfo{eventCopy, dontBroadcast};

				hook->postHook->Notify(hook->name, &eventInfo, dontBroadcast);

				g_pGameEventManager->FreeEvent(eventInfo.event);

				m_eventCopies.pop();
			} else {
				hook->postHook->Notify(hook->name, nullptr, dontBroadcast);
			}
		}

		if (--hook->refCount == 0) {
			AssertFatal(hook->postHook == nullptr);
			AssertFatal(hook->preHook == nullptr);
			m_eventHooks.erase(hook->name);
			delete hook;
		}
	}

	m_eventStack.pop();

	return ResultType::Continue;
}

EventManager g_EventManager;