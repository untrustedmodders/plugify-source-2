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

		eventHook.refCount++;

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

	eventHook.refCount++;

	return EventHookError::Okay;
}

EventHookError EventManager::UnhookEvent(const plg::string& name, EventListenerCallback callback, HookMode mode) {
	std::unique_lock<std::mutex> lock(m_registerEventLock);

	auto it = m_eventHooks.find(name);
	if (it == m_eventHooks.end()) {
		return EventHookError::NotActive;
	}

	std::unique_ptr<HookCallback>* pCallbackHook;
	auto& eventHook = std::get<EventHook>(*it);
	if (mode == HookMode::Pre) {
		pCallbackHook = &eventHook.preHook;
	} else {
		pCallbackHook = &eventHook.postHook;
	}

	if (*pCallbackHook == nullptr || !(*pCallbackHook)->Unregister(callback)) {
		return EventHookError::InvalidCallback;
	}

	if (--eventHook.refCount == 0) {
		m_eventHooks.erase(it);
	}

	return EventHookError::Okay;
}

void EventManager::FireGameEvent(IGameEvent* event) {
}

void EventManager::FireEvent(EventInfo* pInfo, bool bDontBroadcast) {
	g_pGameEventManager->FireEvent(pInfo->pEvent, bDontBroadcast);

	m_freeEvents.push(pInfo);
}

EventInfo* EventManager::CreateEvent(const plg::string& name, bool force) {
	EventInfo* pInfo;
	IGameEvent* pEvent = g_pGameEventManager->CreateEvent(name.c_str(), force);

	if (pEvent) {
		if (m_freeEvents.empty()) {
			pInfo = new EventInfo{};
		} else {
			pInfo = m_freeEvents.top();
			m_freeEvents.pop();
		}

		pInfo->pEvent = pEvent;
		pInfo->bDontBroadcast = false;

		return pInfo;
	}

	return nullptr;
}

void EventManager::FireEventToClient(EventInfo* pInfo, CPlayerSlot slot) {
	IGameEventListener2* pListener = addresses::GetLegacyGameEventListener(slot);

	pListener->FireGameEvent(pInfo->pEvent);
}

void EventManager::CancelCreatedEvent(EventInfo* pInfo) {
	g_pGameEventManager->FreeEvent(pInfo->pEvent);

	m_freeEvents.push(pInfo);
}

poly::ReturnAction EventManager::Hook_OnFireEvent(poly::Params& params, int count, poly::Return& ret) {
	auto pEvent = poly::GetArgument<IGameEvent*>(params, 1);
	auto bDontBroadcast = poly::GetArgument<bool>(params, 2);

	if (!pEvent)
		return poly::ReturnAction::Ignored;

	bool bLocalDontBroadcast = bDontBroadcast;
	plg::string name(pEvent->GetName());

	auto it = m_eventHooks.find(name);
	if (it != m_eventHooks.end()) {
		auto& eventHook = std::get<EventHook>(*it);
		eventHook.refCount++;
		m_eventStack.push(&eventHook);

		if (eventHook.preHook != nullptr) {
			//S2_LOGF(LS_DEBUG, "Pushing event `%s` pointer: %p, dont broadcast: %d, post: %d\n", pEvent->GetName(), pEvent, bDontBroadcast, false);

			EventInfo eventInfo{pEvent, bDontBroadcast};

			for (size_t i = 0; i < eventHook.preHook->GetCount(); ++i) {
				auto result = eventHook.preHook->Notify(i, name, &eventInfo, bDontBroadcast);
				bLocalDontBroadcast = eventInfo.bDontBroadcast;

				if (result >= ResultType::Handled) {
					// m_EventCopies.push(g_gameEventManager->DuplicateEvent(pEvent));
					g_pGameEventManager->FreeEvent(pEvent);
					return poly::ReturnAction::Supercede;
				}
			}
		}

		if (eventHook.postCopy) {
			m_eventCopies.push(g_pGameEventManager->DuplicateEvent(pEvent));
		}
	} else {
		m_eventStack.push(nullptr);
	}

	if (bLocalDontBroadcast != bDontBroadcast) {
		poly::SetArgument<bool>(params, 2, bLocalDontBroadcast);
		return poly::ReturnAction::Handled;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction EventManager::Hook_OnFireEvent_Post(poly::Params& params, int count, poly::Return& ret) {
	auto pEvent = poly::GetArgument<IGameEvent*>(params, 1);
	auto bDontBroadcast = poly::GetArgument<bool>(params, 2);

	if (!pEvent)
		return poly::ReturnAction::Ignored;

	EventHook* pHook = m_eventStack.top();

	if (pHook != nullptr) {
		if (pHook->postHook != nullptr) {
			if (pHook->postCopy) {
				auto pEventCopy = m_eventCopies.top();
				//S2_LOGF(LS_DEBUG, "Pushing event `%s` pointer: %p, dont broadcast: %d, post: %d\n", pEventCopy->GetName(), pEventCopy, bDontBroadcast, true);
				EventInfo eventInfo{pEventCopy, bDontBroadcast};

				pHook->postHook->Notify(pHook->name, &eventInfo, bDontBroadcast);

				g_pGameEventManager->FreeEvent(eventInfo.pEvent);

				m_eventCopies.pop();
			} else {
				pHook->postHook->Notify(pHook->name, nullptr, bDontBroadcast);
			}
		}

		if (--pHook->refCount == 0) {
			AssertFatal(pHook->postHook == nullptr);
			AssertFatal(pHook->preHook == nullptr);
			m_eventHooks.erase(pHook->name);
			delete pHook;
		}
	}

	m_eventStack.pop();

	return poly::ReturnAction::Ignored;
}

EventManager g_EventManager;