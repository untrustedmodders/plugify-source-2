#pragma once

#include "listener_manager.hpp"

#include <networksystem/netmessage.h>

class UserMessage;
using UserMessageCallback = ResultType (*)(UserMessage* message);

struct UserMessageHook {
	std::array<ListenerManager<UserMessageCallback>, 2> callbacks;
};

class UserMessageManager {
public:
	UserMessageManager() = default;
	~UserMessageManager() = default;

	bool HookUserMessage(int16_t messageId, UserMessageCallback callback, HookMode mode);
	bool UnhookUserMessage(int16_t messageId, UserMessageCallback callback, HookMode mode);

	ResultType ExecuteMessageCallbacks(INetworkMessageInternal* msgSerializable, CNetMessage* msgData, int clientCount, uint64_t* clients, HookMode mode);

private:
	std::unordered_map<int16_t, UserMessageHook> m_hooksMap;
	std::array<ListenerManager<UserMessageCallback>, 2> m_globalCallbacks;
	std::mutex m_registerCmdLock;
};

extern UserMessageManager g_UserMessageManager;