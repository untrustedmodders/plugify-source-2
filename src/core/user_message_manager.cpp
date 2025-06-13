#include "user_message_manager.hpp"
#include "user_message.hpp"

bool UserMessageManager::HookUserMessage(int16_t messageId, UserMessageCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);
	
	if (messageId == 0) {
		return m_globalCallbacks[static_cast<size_t>(mode)].Register(callback);
	}

	auto it = m_hooksMap.find(messageId);
	if (it == m_hooksMap.end()) {
		auto& commandInfo = m_hooksMap.emplace(messageId, UserMessageHook{}).first->second;
		return commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	} else {
		auto& commandInfo = std::get<UserMessageHook>(*it);
		return commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	}
}

bool UserMessageManager::UnhookUserMessage(int16_t messageId, UserMessageCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);
	
	if (messageId == 0) {
		return m_globalCallbacks[static_cast<size_t>(mode)].Unregister(callback);
	}

	auto it = m_hooksMap.find(messageId);
	if (it == m_hooksMap.end()) {
		return false;
	}

	auto& commandInfo = std::get<UserMessageHook>(*it);
	return commandInfo.callbacks[static_cast<size_t>(mode)].Unregister(callback);
}

ResultType UserMessageManager::ExecuteMessageCallbacks(INetworkMessageInternal* msgSerializable, const CNetMessage* msgData, uint64_t* clients, HookMode mode) {
	UserMessage message(msgSerializable, msgData, *clients);

	int16_t messageID = message.GetMessageID();
	
	//S2_LOGF(LS_DEBUG, "[CUserMessageManager::ExecuteMessageCallbacks][{}] Pushing user message `{}` pointer: %p\n", mode == HookMode::Pre ? "Pre" : "Post",  messageID, pEvent);

	ResultType result = ResultType::Continue;
	
	const auto& globalCallback = m_globalCallbacks[static_cast<size_t>(mode)];

	for (size_t i = 0; i < globalCallback.GetCount(); ++i) {
		auto thisResult = globalCallback.Notify(i, &message);
		if (thisResult >= ResultType::Stop) {
			if (mode == HookMode::Pre) {
				*clients = message.GetRecipientMask();
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
	if (it != m_hooksMap.end()) {
		const auto& messageHook = std::get<UserMessageHook>(*it);
		const auto& callback = messageHook.callbacks[static_cast<size_t>(mode)];

		for (size_t i = 0; i < callback.GetCount(); ++i) {
			auto thisResult = callback.Notify(i, &message);
			if (thisResult >= ResultType::Handled) {
				if (mode == HookMode::Pre) {
					*clients = message.GetRecipientMask();
				}
				return thisResult;
			} else if (thisResult > result) {
				result = thisResult;
			}
		}
	}

	if (mode == HookMode::Pre) {
		*clients = message.GetRecipientMask();
	}
	return result;
}

UserMessageManager g_UserMessageManager;