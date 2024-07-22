#include "output_manager.h"

void CEntityOutputManager::HookEntityOutput(std::string szClassname, std::string szOutput, EntityListenerCallback callback, HookMode mode)
{
	OutputKey outputKey{std::move(szClassname), std::move(szOutput)};
	CallbackPair* pCallbackPair;

	auto it = m_hookMap.find(outputKey);
	if (it == m_hookMap.end())
	{
		pCallbackPair = &m_hookMap.emplace(std::move(outputKey), CallbackPair{}).first->second;
	}
	else
	{
		pCallbackPair = &std::get<CallbackPair>(*it);
	}

	auto& listener = mode == HookMode::Pre ? pCallbackPair->pre : pCallbackPair->post;
	listener.Register(callback);
}

void CEntityOutputManager::UnhookEntityOutput(std::string szClassname, std::string szOutput, EntityListenerCallback callback, HookMode mode)
{
	OutputKey outputKey{std::move(szClassname), std::move(szOutput)};

	auto it = m_hookMap.find(outputKey);
	if (it != m_hookMap.end())
	{
		auto& callbackPair = std::get<CallbackPair>(*it);

		auto& listener = mode == HookMode::Pre ? callbackPair.pre : callbackPair.post;
		listener.Unregister(callback);

		if (callbackPair.pre.Empty() && callbackPair.post.Empty())
		{
			m_hookMap.erase(it);
		}
	}
}

dyno::ReturnAction CEntityOutputManager::Hook_FireOutputInternal(dyno::IHook& hook)
{
	// CEntityIOOutput* const pThis, CEntityInstance* pActivator, CEntityInstance* pCaller, const CVariant* const value, float flDelay
	auto pThis = dyno::GetArgument<CEntityIOOutput* const>(hook, 0);
	auto pActivator = dyno::GetArgument<CEntityInstance*>(hook, 1);
	auto pCaller = dyno::GetArgument<CEntityInstance*>(hook, 2);
	//auto value = dyno::GetArgument<const CVariant* const>(hook, 3);
	auto flDelay = dyno::GetArgument<float>(hook, 4);

	if (pCaller)
	{
		g_Logger.LogFormat(LS_DEBUG, "[EntityOutputManager][FireOutputHook] - %s, %s\n", pThis->m_pDesc->m_pName, pCaller->GetClassname());

		std::array<OutputKey, 4> vecSearchKeys{
			OutputKey{"*", pThis->m_pDesc->m_pName},
			OutputKey{"*", "*"},
			OutputKey{pCaller->GetClassname(), pThis->m_pDesc->m_pName},
			OutputKey{pCaller->GetClassname(), "*"}};

		m_vecCallbackPairs.clear();

		for (const auto& searchKey : vecSearchKeys)
		{
			auto it = m_hookMap.find(searchKey);
			if (it != m_hookMap.end())
			{
				m_vecCallbackPairs.emplace_back(&std::get<CallbackPair>(*it));
			}
		}
	}
	else
	{
		g_Logger.LogFormat(LS_DEBUG, "[EntityOutputManager][FireOutputHook] - %s, unknown caller\n", pThis->m_pDesc->m_pName);
	}

	ResultType result = ResultType::Continue;

	int activator = pActivator != nullptr ? pActivator->GetEntityIndex().Get() : -1;
	int caller = pCaller != nullptr ? pCaller->GetEntityIndex().Get() : -1;

	for (auto pCallbackPair : m_vecCallbackPairs)
	{
		for (size_t i = 0; i < pCallbackPair->pre.GetCount(); ++i)
		{
			auto thisResult = pCallbackPair->pre.Notify(i, activator, caller, flDelay);
			if (thisResult >= ResultType::Stop)
			{
				break;
			}

			if (thisResult > result)
			{
				result = thisResult;
			}
		}
	}

	if (result >= ResultType::Handled)
	{
		return dyno::ReturnAction::Supercede;
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction CEntityOutputManager::Hook_FireOutputInternal_Post(dyno::IHook& hook)
{
	//auto pThis = dyno::GetArgument<CEntityIOOutput* const>(hook, 0);
	auto pActivator = dyno::GetArgument<CEntityInstance*>(hook, 1);
	auto pCaller = dyno::GetArgument<CEntityInstance*>(hook, 2);
	//auto value = dyno::GetArgument<const CVariant* const>(hook, 3);
	auto flDelay = dyno::GetArgument<float>(hook, 4);

	int activator = pActivator != nullptr ? pActivator->GetEntityIndex().Get() : -1;
	int caller = pCaller != nullptr ? pCaller->GetEntityIndex().Get() : -1;

	for (auto pCallbackPair : m_vecCallbackPairs)
	{
		pCallbackPair->post.Notify(activator, caller, flDelay);
	}

	return dyno::ReturnAction::Ignored;
}

CEntityOutputManager g_OutputManager;