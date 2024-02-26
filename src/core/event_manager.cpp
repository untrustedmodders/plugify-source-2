#include "event_manager.h"

CEventManager::~CEventManager()
{
	while (!m_FreeEvents.empty())
	{
		delete m_FreeEvents.top();
		m_FreeEvents.pop();
	}

	if (!m_EventHooks.empty())
	{
		g_gameEventManager->RemoveListener(this);
	}
}

EventHookError CEventManager::HookEvent(const std::string& name, EventListenerCallback callback, HookMode mode)
{
	if (!g_gameEventManager->FindListener(this, name.c_str()))
	{
		if (!g_gameEventManager->AddListener(this, name.c_str(), true))
		{
			return EventHookError::InvalidEvent;
		}
	}

	auto it = m_EventHooks.find(name);
	if (it == m_EventHooks.end())
	{
		EventHook eventHook{name};

		if (mode == HookMode::Pre)
		{
			eventHook.preHook = std::make_unique<HookCallback>();
			eventHook.preHook->Register(callback);
		}
		else
		{
			eventHook.postHook = std::make_unique<HookCallback>();
			eventHook.postHook->Register(callback);
			eventHook.postCopy = (mode == HookMode::Post);
		}

		eventHook.refCount++;

		m_EventHooks.emplace(name, std::move(eventHook));

		return EventHookError::Okay;
	}

	auto& eventHook = std::get<EventHook>(*it);
	if (mode == HookMode::Pre)
	{
		if (eventHook.preHook == nullptr)
		{
			eventHook.preHook = std::make_unique<HookCallback>();
		}

		eventHook.preHook->Register(callback);
	}
	else
	{
		if (eventHook.postHook == nullptr)
		{
			eventHook.postHook = std::make_unique<HookCallback>();
		}

		if (!eventHook.postCopy)
		{
			eventHook.postCopy = (mode == HookMode::Post);
		}

		eventHook.postHook->Register(callback);
	}

	eventHook.refCount++;

	return EventHookError::Okay;
}

EventHookError CEventManager::UnhookEvent(const std::string& name, EventListenerCallback callback, HookMode mode)
{
	auto it = m_EventHooks.find(name);
	if (it == m_EventHooks.end())
	{
		return EventHookError::NotActive;
	}

	std::unique_ptr<HookCallback>* pCallbackHook;
	auto& eventHook = std::get<EventHook>(*it);
	if (mode == HookMode::Pre)
	{
		pCallbackHook = &eventHook.preHook;
	}
	else
	{
		pCallbackHook = &eventHook.postHook;
	}

	if (*pCallbackHook == nullptr || !(*pCallbackHook)->Unregister(callback))
	{
		return EventHookError::InvalidCallback;
	}

	if (--eventHook.refCount == 0)
	{
		m_EventHooks.erase(it);
	}

	return EventHookError::Okay;
}

void CEventManager::FireGameEvent(IGameEvent* event)
{
}

void CEventManager::FireEvent(EventInfo* pInfo, bool bDontBroadcast)
{
	g_gameEventManager->FireEvent(pInfo->pEvent, bDontBroadcast);

	m_FreeEvents.push(pInfo);
}

EventInfo* CEventManager::CreateEvent(const std::string& name, bool force)
{
	EventInfo* pInfo;
	IGameEvent* pEvent = g_gameEventManager->CreateEvent(name.c_str(), force);

	if (pEvent)
	{
		if (m_FreeEvents.empty())
		{
			pInfo = new EventInfo{};
		}
		else
		{
			pInfo = m_FreeEvents.top();
			m_FreeEvents.pop();
		}

		pInfo->pEvent = pEvent;
		pInfo->bDontBroadcast = false;

		return pInfo;
	}

	return nullptr;
}

void CEventManager::FireEventToClient(EventInfo* pInfo, int entityIndex)
{
	IGameEventListener2* pListener = addresses::GetLegacyGameEventListener(CPlayerSlot(entityIndex - 1));

	pListener->FireGameEvent(pInfo->pEvent);
}

void CEventManager::CancelCreatedEvent(EventInfo* pInfo)
{
	g_gameEventManager->FreeEvent(pInfo->pEvent);

	m_FreeEvents.push(pInfo);
}

dyno::ReturnAction CEventManager::Hook_OnFireEvent(dyno::IHook& hook)
{
	auto pEvent = dyno::GetArgument<IGameEvent*>(hook, 1);
	auto bDontBroadcast = dyno::GetArgument<bool>(hook, 2);

	if (!pEvent)
		return dyno::ReturnAction::Ignored;

	bool bLocalDontBroadcast = bDontBroadcast;
	std::string name(pEvent->GetName());

	auto it = m_EventHooks.find(name);
	if (it != m_EventHooks.end())
	{
		auto& eventHook = std::get<EventHook>(*it);
		eventHook.refCount++;
		m_EventStack.push(&eventHook);

		if (eventHook.preHook != nullptr)
		{
			g_Logger.MessageFormat("Pushing event `%s` pointer: %p, dont broadcast: %d, post: %d", pEvent->GetName(), (void*)pEvent, bDontBroadcast, false);

			EventInfo eventInfo{pEvent, bDontBroadcast};

			for (size_t i = 0; i < eventHook.preHook->GetCount(); ++i)
			{
				auto result = eventHook.preHook->Notify(i, name, &eventInfo, bDontBroadcast);
				bLocalDontBroadcast = eventInfo.bDontBroadcast;

				if (result >= ResultType::Handled)
				{
					// m_EventCopies.push(g_gameEventManager->DuplicateEvent(pEvent));
					g_gameEventManager->FreeEvent(pEvent);
					return dyno::ReturnAction::Supercede;
				}
			}
		}

		if (eventHook.postCopy)
		{
			m_EventCopies.push(g_gameEventManager->DuplicateEvent(pEvent));
		}
	}
	else
	{
		m_EventStack.push(nullptr);
	}

	if (bLocalDontBroadcast != bDontBroadcast)
	{
		dyno::SetArgument<bool>(hook, 2, bLocalDontBroadcast);
		return dyno::ReturnAction::Handled;
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction CEventManager::Hook_OnFireEvent_Post(dyno::IHook& hook)
{
	auto pEvent = dyno::GetArgument<IGameEvent*>(hook, 1);
	auto bDontBroadcast = dyno::GetArgument<bool>(hook, 2);

	if (!pEvent)
		return dyno::ReturnAction::Ignored;

	EventHook* pHook = m_EventStack.top();

	if (pHook != nullptr)
	{
		if (pHook->postHook != nullptr)
		{
			if (pHook->postCopy)
			{
				auto pEventCopy = m_EventCopies.top();
				g_Logger.MessageFormat("Pushing event `%s` pointer: %p, dont broadcast: %d, post: %d", pEventCopy->GetName(), (void*)pEventCopy, bDontBroadcast, true);
				EventInfo eventInfo{pEventCopy, bDontBroadcast};

				pHook->postHook->Notify(pHook->name, &eventInfo, bDontBroadcast);

				g_gameEventManager->FreeEvent(eventInfo.pEvent);

				m_EventCopies.pop();
			}
			else
			{
				pHook->postHook->Notify(pHook->name, nullptr, bDontBroadcast);
			}
		}

		if (--pHook->refCount == 0)
		{
			AssertFatal(pHook->postHook == nullptr);
			AssertFatal(pHook->preHook == nullptr);
			m_EventHooks.erase(pHook->name);
			delete pHook;
		}
	}

	m_EventStack.pop();

	return dyno::ReturnAction::Ignored;
}

CEventManager g_EventManager;