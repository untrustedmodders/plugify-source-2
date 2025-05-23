#include "output_manager.hpp"

bool EntityOutputManager::HookEntityOutput(plg::string classname, plg::string output, EntityListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerHookLock);
	OutputKey outputKey{std::move(classname), std::move(output)};

	auto it = m_hookMap.find(outputKey);
	if (it == m_hookMap.end()) {
		auto& callbackPair = m_hookMap.emplace(std::move(outputKey), CallbackPair{}).first->second;
		return callbackPair.callbacks[static_cast<size_t>(mode)].Register(callback);
	} else {
		auto& callbackPair = std::get<CallbackPair>(*it);
		return callbackPair.callbacks[static_cast<size_t>(mode)].Register(callback);
	}
}

bool EntityOutputManager::UnhookEntityOutput(plg::string classname, plg::string output, EntityListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerHookLock);
	OutputKey outputKey{std::move(classname), std::move(output)};

	auto it = m_hookMap.find(outputKey);
	if (it != m_hookMap.end()) {
		auto& callbackPair = std::get<CallbackPair>(*it);
		auto status = callbackPair.callbacks[static_cast<size_t>(mode)].Unregister(callback);
		if (callbackPair.callbacks[0].Empty() && callbackPair.callbacks[1].Empty()) {
			m_hookMap.erase(it);
		}
		return status;
	}

	return false;
}

ResultType EntityOutputManager::FireOutputInternal(CEntityIOOutput* pThis, CEntityInstance* pActivator, CEntityInstance* pCaller, float flDelay) {
	if (pCaller) {
		//S2_LOGF(LS_DEBUG, "[EntityOutputManager][FireOutputHook] - {}, {}\n", pThis->m_pDesc->m_pName, pCaller->GetClassname());

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
		//S2_LOGF(LS_DEBUG, "[EntityOutputManager][FireOutputHook] - {}, unknown caller\n", pThis->m_pDesc->m_pName);
	}

	ResultType result = ResultType::Continue;

	int activator = pActivator != nullptr ? pActivator->GetEntityIndex().Get() : -1;
	int caller = pCaller != nullptr ? pCaller->GetEntityIndex().Get() : -1;

	for (const auto& pCallbackPair: m_vecCallbackPairs) {
		auto& cb = pCallbackPair->callbacks[0];
		for (size_t i = 0; i < cb.GetCount(); ++i) {
			auto thisResult = cb.Notify(i, activator, caller, flDelay);
			if (thisResult >= ResultType::Stop) {
				break;
			}

			if (thisResult > result) {
				result = thisResult;
			}
		}
	}

	return result;
}

ResultType EntityOutputManager::FireOutputInternal_Post(CEntityIOOutput* pThis, CEntityInstance* pActivator, CEntityInstance* pCaller, float flDelay) {
	int activator = pActivator != nullptr ? pActivator->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX;
	int caller = pCaller != nullptr ? pCaller->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX;

	for (const auto& pCallbackPair: m_vecCallbackPairs) {
		auto& cb = pCallbackPair->callbacks[1];
		cb.Notify(activator, caller, flDelay);
	}

	return ResultType::Continue;
}

EntityOutputManager g_OutputManager;