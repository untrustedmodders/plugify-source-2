#include "user_message_manager.hpp"
#include "user_message.hpp"

bool CUserMessageManager::HookUserMessage(int messageId, UserMessageCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);
	
	if (messageId == 0) {
		m_globalCallbacks[static_cast<size_t>(mode)].Register(callback);
		return true;
	}

	auto it = m_hooksMap.find(messageId);
	if (it == m_hooksMap.end()) {
		auto& commandInfo = m_hooksMap.emplace(messageId, UserMessageHook{}).first->second;
		commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	} else {
		auto& commandInfo = std::get<UserMessageHook>(*it);
		commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	}
	return true;
}

bool CUserMessageManager::UnhookUserMessage(int messageId, UserMessageCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);
	
	if (messageId == 0) {
		m_globalCallbacks[static_cast<size_t>(mode)].Unregister(callback);
		return true;
	}

	auto it = m_hooksMap.find(messageId);
	if (it == m_hooksMap.end()) {
		return false;
	}

	auto& commandInfo = std::get<UserMessageHook>(*it);
	commandInfo.callbacks[static_cast<size_t>(mode)].Unregister(callback);
	return true;
}

ResultType CUserMessageManager::ExecuteMessageCallbacks(INetworkMessageInternal* pEvent, CNetMessage* pData, int nClientCount, uint64* clients, HookMode mode) {
	UserMessage message(pEvent, pData, nClientCount, clients);

	int messageID = message.GetMessageID();
	
	//S2_LOGF(LS_DEBUG, "[CUserMessageManager::ExecuteMessageCallbacks][%s] Pushing user message `%d` pointer: %p\n", mode == HookMode::Pre ? "Pre" : "Post",  messageID, pEvent);

	ResultType result = ResultType::Continue;
	
	const auto& globalCallback = m_globalCallbacks[static_cast<size_t>(mode)];
	
	for (size_t i = 0; i < globalCallback.GetCount(); ++i) {
		auto thisResult = globalCallback.Notify(i, &message);
		if (thisResult >= ResultType::Stop) {
			if (mode == HookMode::Pre) {
				return ResultType::Stop;
			}
			
			result = thisResult;
			break;
		}

		if (thisResult >= ResultType::Handled) {
			result = thisResult;
		}
	}
	
	auto it = m_hooksMap.find(messageID);
	if (it == m_hooksMap.end()) {
		return result;
	}

	const auto& messageHook = std::get<UserMessageHook>(*it);
	const auto& callback = messageHook.callbacks[static_cast<size_t>(mode)];

	for (size_t i = 0; i < callback.GetCount(); ++i) {
		auto thisResult = callback.Notify(i, &message);
		if (thisResult >= ResultType::Handled) {
			return thisResult;
		} else if (thisResult > result) {
			result = thisResult;
		}
	}

	return result;
}

poly::ReturnAction CUserMessageManager::Hook_PostEvent(poly::Params& params, int count, poly::Return& ret, HookMode mode) {
	auto nClientCount = poly::GetArgument<int>(params, 3);
	auto clients = poly::GetArgument<uint64*>(params, 4);
	auto pEvent = poly::GetArgument<INetworkMessageInternal*>(params, 5);
	auto pData = poly::GetArgument<CNetMessage*>(params, 6);

	//S2_LOGF(LS_DEBUG, "[CUserMessageManager::Hook_PostEvent]\n");

	auto result = ExecuteMessageCallbacks(pEvent, pData, nClientCount, clients, mode);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

CUserMessageManager g_UserMessageManager;