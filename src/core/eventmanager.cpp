#include "eventmanager.h"
#include "globals.h"
#include "fwd.h"

using namespace cs2sdk;

CEventManager::~CEventManager() {
	while (!m_FreeEvents.empty()) {
		delete m_FreeEvents.top();
		m_FreeEvents.pop();
	}
}

EventHookError CEventManager::HookEvent(const char* name, FnEventListenerCallback pCallback, EventHookMode mode) {
	if (!g_gameEventManager->FindListener(this, name)) {
		if (!g_gameEventManager->AddListener(this, name, true)) {
			return EventHookError::InvalidEvent;
		}
	}

	auto it = m_EventHooks.find(name);
	if (it == m_EventHooks.end()) {
		EventHook pHook{name};

		if (mode == EventHookMode::Pre) {
			pHook.pPreHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
			pHook.pPreHook->Register(pCallback);
		} else {
			pHook.pPostHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
			pHook.pPostHook->Register(pCallback);
			pHook.postCopy = (mode == EventHookMode::Post);
		}

		pHook.refCount++;

		m_EventHooks.emplace(name, std::move(pHook));

		return EventHookError::Okay;
	}

	auto& pHook = std::get<EventHook>(*it);
	if (mode == EventHookMode::Pre) {
		if (!pHook.pPreHook) {
			pHook.pPreHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
		}

		pHook.pPreHook->Register(pCallback);
	} else {
		if (!pHook.pPostHook) {
			pHook.pPostHook = std::make_unique<CListenerManager<FnEventListenerCallback>>();
		}

		if (!pHook.postCopy) {
			pHook.postCopy = (mode == EventHookMode::Post);
		}

		pHook.pPostHook->Register(pCallback);
	}

	pHook.refCount++;

	return EventHookError::Okay;
}

EventHookError CEventManager::UnhookEvent(const char* name, FnEventListenerCallback pCallback, EventHookMode mode) {
	auto it = m_EventHooks.find(name);
	if (it == m_EventHooks.end()) {
		return EventHookError::NotActive;
	}

	Hook* pCallbackHook;
	auto& pHook = std::get<EventHook>(*it);
	if (mode == EventHookMode::Pre) {
		pCallbackHook = &pHook.pPreHook;
	} else {
		pCallbackHook = &pHook.pPostHook;
	}

	if (*pCallbackHook == nullptr || !(*pCallbackHook)->Unregister(pCallback)) {
		return EventHookError::InvalidCallback;
	}

	if (--pHook.refCount == 0) {
		m_EventHooks.erase(name);
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

void CEventManager::FireEventToClient(EventInfo *pInfo, IClient *pClient) {
	auto pGameClient = (IGameEventListener2 *)((intptr_t)pClient - sizeof(void *));

	pGameClient->FireGameEvent(pInfo->pEvent);
}

void CEventManager::CancelCreatedEvent(EventInfo *pInfo) {
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

		if (pHook.pPreHook) {
			EventInfo info{pEvent, bDontBroadcast};

			auto& mgr = *pHook.pPreHook.get();
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
		if (pHook->pPostHook) {
			if (pHook->postCopy) {
				EventInfo info{m_EventCopies.top(), bDontBroadcast};

				pHook->pPostHook->Notify(pHook->name.c_str(), &info, bDontBroadcast);

				g_gameEventManager->FreeEvent(info.pEvent);
				m_EventCopies.pop();
			} else {
				pHook->pPostHook->Notify(pHook->name.c_str(), nullptr, bDontBroadcast);
			}
		}

		if (--pHook->refCount == 0) {
			assert(pHook->pPostHook == nullptr);
			assert(pHook->pPreHook == nullptr);
			m_EventHooks.erase(pHook->name);
			delete pHook;
		}
	}

	m_EventStack.pop();

	return dyno::ReturnAction::Ignored;
}

namespace cs2sdk {
	CEventManager g_EventManager;
}