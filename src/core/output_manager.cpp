#include "output_manager.hpp"

void CEntityOutputManager::HookEntityOutput(plg::string szClassname, plg::string szOutput, EntityListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerHookLock);
	OutputKey outputKey{std::move(szClassname), std::move(szOutput)};
	CallbackPair* pCallbackPair;

	auto it = m_hookMap.find(outputKey);
	if (it == m_hookMap.end()) {
		pCallbackPair = &m_hookMap.emplace(std::move(outputKey), CallbackPair{}).first->second;
	} else {
		pCallbackPair = &std::get<CallbackPair>(*it);
	}

	auto& listener = mode == HookMode::Pre ? pCallbackPair->pre : pCallbackPair->post;
	listener.Register(callback);
}

void CEntityOutputManager::UnhookEntityOutput(plg::string szClassname, plg::string szOutput, EntityListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerHookLock);
	OutputKey outputKey{std::move(szClassname), std::move(szOutput)};

	auto it = m_hookMap.find(outputKey);
	if (it != m_hookMap.end()) {
		auto& callbackPair = std::get<CallbackPair>(*it);

		auto& listener = mode == HookMode::Pre ? callbackPair.pre : callbackPair.post;
		listener.Unregister(callback);

		if (callbackPair.pre.Empty() && callbackPair.post.Empty()) {
			m_hookMap.erase(it);
		}
	}
}

poly::ReturnAction CEntityOutputManager::Hook_FireOutputInternal(poly::Params& params, int count, poly::Return& ret) {
	// CEntityIOOutput* const pThis, CEntityInstance* pActivator, CEntityInstance* pCaller, const CVariant* const value, float flDelay
	auto pThis = poly::GetArgument<CEntityIOOutput* const>(params, 0);
	auto pActivator = poly::GetArgument<CEntityInstance*>(params, 1);
	auto pCaller = poly::GetArgument<CEntityInstance*>(params, 2);
	//auto value = poly::GetArgument<const CVariant* const>(params, 3);
	auto flDelay = poly::GetArgument<float>(params, 4);

	if (pCaller) {
		g_Logger.LogFormat(LS_DEBUG, "[EntityOutputManager][FireOutputHook] - %s, %s\n", pThis->m_pDesc->m_pName, pCaller->GetClassname());

		std::array<OutputKey, 4> vecSearchKeys{
				OutputKey{"*", pThis->m_pDesc->m_pName},
				OutputKey{"*", "*"},
				OutputKey{pCaller->GetClassname(), pThis->m_pDesc->m_pName},
				OutputKey{pCaller->GetClassname(), "*"}};

		m_vecCallbackPairs.clear();

		for (const auto& searchKey: vecSearchKeys) {
			auto it = m_hookMap.find(searchKey);
			if (it != m_hookMap.end()) {
				m_vecCallbackPairs.emplace_back(&std::get<CallbackPair>(*it));
			}
		}
	} else {
		g_Logger.LogFormat(LS_DEBUG, "[EntityOutputManager][FireOutputHook] - %s, unknown caller\n", pThis->m_pDesc->m_pName);
	}

	ResultType result = ResultType::Continue;

	int activator = pActivator != nullptr ? pActivator->GetEntityIndex().Get() : -1;
	int caller = pCaller != nullptr ? pCaller->GetEntityIndex().Get() : -1;

	for (const auto& pCallbackPair: m_vecCallbackPairs) {
		for (size_t i = 0; i < pCallbackPair->pre.GetCount(); ++i) {
			auto thisResult = pCallbackPair->pre.Notify(i, activator, caller, flDelay);
			if (thisResult >= ResultType::Stop) {
				break;
			}

			if (thisResult > result) {
				result = thisResult;
			}
		}
	}

	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction CEntityOutputManager::Hook_FireOutputInternal_Post(poly::Params& params, int count, poly::Return& ret) {
	//auto pThis = poly::GetArgument<CEntityIOOutput* const>(params, 0);
	auto pActivator = poly::GetArgument<CEntityInstance*>(params, 1);
	auto pCaller = poly::GetArgument<CEntityInstance*>(params, 2);
	//auto value = poly::GetArgument<const CVariant* const>(params, 3);
	auto flDelay = poly::GetArgument<float>(params, 4);

	int activator = pActivator != nullptr ? pActivator->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX;
	int caller = pCaller != nullptr ? pCaller->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX;

	for (const auto& pCallbackPair: m_vecCallbackPairs) {
		pCallbackPair->post.Notify(activator, caller, flDelay);
	}

	return poly::ReturnAction::Ignored;
}

CEntityOutputManager g_OutputManager;