#include "eventmanager.h"
#include "fwd.h"

CEventManager::~CEventManager() {
	while (!m_FreeEvents.empty()) {
		delete m_FreeEvents.top();
		m_FreeEvents.pop();
	}

	if (!m_EventHooks.empty()) {
		g_gameEventManager->RemoveListener(this);
	}
}

EventHookError CEventManager::HookEvent(const char* name, FnEventListenerCallback callback, EventHookMode mode) {
	if (!g_gameEventManager->FindListener(this, name)) {
		if (!g_gameEventManager->AddListener(this, name, true)) {
			return EventHookError::InvalidEvent;
		}
	}

	auto it = m_EventHooks.find(name);
	if (it == m_EventHooks.end()) {
		EventHook pHook{name};

		if (mode == EventHookMode::Pre) {
			pHook.preHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
			pHook.preHook->Register(callback);
		} else {
			pHook.postHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
			pHook.postHook->Register(callback);
			pHook.postCopy = (mode == EventHookMode::Post);
		}

		pHook.refCount++;

		m_EventHooks.emplace(name, std::move(pHook));

		return EventHookError::Okay;
	}

	auto& pHook = std::get<EventHook>(*it);
	if (mode == EventHookMode::Pre) {
		if (pHook.preHook == nullptr) {
			pHook.preHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
		}

		pHook.preHook->Register(callback);
	} else {
		if (pHook.postHook == nullptr) {
			pHook.postHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
		}

		if (!pHook.postCopy) {
			pHook.postCopy = (mode == EventHookMode::Post);
		}

		pHook.postHook->Register(callback);
	}

	pHook.refCount++;

	return EventHookError::Okay;
}

EventHookError CEventManager::UnhookEvent(const char* name, FnEventListenerCallback callback, EventHookMode mode) {
	auto it = m_EventHooks.find(name);
	if (it == m_EventHooks.end()) {
		return EventHookError::NotActive;
	}

	HookCallback* pCallbackHook;
	auto& pHook = std::get<EventHook>(*it);
	if (mode == EventHookMode::Pre) {
		pCallbackHook = &pHook.preHook;
	} else {
		pCallbackHook = &pHook.postHook;
	}

	if (*pCallbackHook == nullptr || !(*pCallbackHook)->Unregister(callback)) {
		return EventHookError::InvalidCallback;
	}

	if (--pHook.refCount == 0) {
		m_EventHooks.erase(it);
	}

	return EventHookError::Okay;
}

void CEventManager::FireGameEvent(IGameEvent* event) {
}

void CEventManager::FireEvent(EventInfo* pInfo, bool bDontBroadcast) {
	g_gameEventManager->FireEvent(pInfo->pEvent, bDontBroadcast);

	m_FreeEvents.push(pInfo);
}

EventInfo* CEventManager::CreateEvent(const char* name, bool force) {
	EventInfo* pInfo;
	IGameEvent* pEvent = g_gameEventManager->CreateEvent(name, force);

	if (pEvent) {
		if (m_FreeEvents.empty()) {
			pInfo = new EventInfo{};
		} else {
			pInfo = m_FreeEvents.top();
			m_FreeEvents.pop();
		}

		pInfo->pEvent = pEvent;
		pInfo->bDontBroadcast = false;

		return pInfo;
	}

	return nullptr;
}

void CEventManager::FireEventToClient(EventInfo* pInfo, IClient* pClient) {
	auto pGameClient = (IGameEventListener2 *)((intptr_t)pClient - sizeof(void *));

	pGameClient->FireGameEvent(pInfo->pEvent);
}

void CEventManager::CancelCreatedEvent(EventInfo* pInfo) {
	g_gameEventManager->FreeEvent(pInfo->pEvent);

	m_FreeEvents.push(pInfo);
}

dyno::ReturnAction CEventManager::Hook_OnFireEvent(dyno::IHook& hook) {
	auto pEvent = dyno::GetArgument<IGameEvent*>(hook, 1);
	auto bDontBroadcast = dyno::GetArgument<bool>(hook, 2);

	if (!pEvent)
		return dyno::ReturnAction::Ignored;

	bool broadcast = bDontBroadcast;
	const char* name = pEvent->GetName();

	auto it = m_EventHooks.find(name);
	if (it != m_EventHooks.end()) {
		auto& pHook = std::get<EventHook>(*it);
		pHook.refCount++;
		m_EventStack.push(&pHook);

		ResultType res;

		if (pHook.preHook != nullptr) {
			EventInfo info{pEvent, bDontBroadcast};

			auto& mgr = *pHook.preHook.get();
			for (int i = 0; i < mgr.GetCount(); ++i) {
				res = Max(mgr(i, name, &info, bDontBroadcast), res);
			}

			broadcast = info.bDontBroadcast;
		}

		if (pHook.postCopy) {
			m_EventCopies.push(g_gameEventManager->DuplicateEvent(pEvent));
		}

		if (res >= ResultType::Handled) {
			g_gameEventManager->FreeEvent(pEvent);
			return dyno::ReturnAction::Supercede;
		}
	} else {
		m_EventStack.push(nullptr);
	}

	if (broadcast != bDontBroadcast) {
		dyno::SetArgument<bool>(hook, 2, broadcast);
		return dyno::ReturnAction::Handled;
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction CEventManager::Hook_OnFireEvent_Post(dyno::IHook& hook) {
	auto pEvent = dyno::GetArgument<IGameEvent*>(hook, 1);
	auto bDontBroadcast = dyno::GetArgument<bool>(hook, 2);

	if (!pEvent)
		return dyno::ReturnAction::Ignored;

	EventHook* pHook = m_EventStack.top();

	if (pHook != nullptr) {
		if (pHook->postHook != nullptr) {
			if (pHook->postCopy) {
				EventInfo info{m_EventCopies.top(), bDontBroadcast};

				pHook->postHook->Notify(pHook->name.c_str(), &info, bDontBroadcast);

				g_gameEventManager->FreeEvent(info.pEvent);

				m_EventCopies.pop();
			} else {
				pHook->postHook->Notify(pHook->name.c_str(), nullptr, bDontBroadcast);
			}
		}

		if (--pHook->refCount == 0) {
			assert(pHook->postHook == nullptr);
			assert(pHook->preHook == nullptr);
			m_EventHooks.erase(pHook->name);
			delete pHook;
		}
	}

	m_EventStack.pop();

	return dyno::ReturnAction::Ignored;
}

CEventManager g_EventManager;