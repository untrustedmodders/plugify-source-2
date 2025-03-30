#pragma once

#include "listener_manager.hpp"

#include <networksystem/netmessage.h>

#include <plugify/polyhook.hpp>

class UserMessage;
using UserMessageCallback = ResultType (*)(UserMessage* message);

struct UserMessageHook {
	std::array<ListenerManager<UserMessageCallback>, 2> callbacks;
};

class UserMessageManager {
public:
	UserMessageManager() = default;
	~UserMessageManager() = default;

	bool HookUserMessage(uint16_t messageId, UserMessageCallback callback, HookMode mode);
	bool UnhookUserMessage(uint16_t messageId, UserMessageCallback callback, HookMode mode);

	poly::ReturnAction Hook_PostEvent(poly::Params& params, int count, poly::Return& ret, HookMode node);
	ResultType ExecuteMessageCallbacks(INetworkMessageInternal* pEvent, CNetMessage* pData, int nClientCount, uint64_t* clients, HookMode mode);

private:
	std::unordered_map<uint16_t, UserMessageHook> m_hooksMap;
	std::array<ListenerManager<UserMessageCallback>, 2> m_globalCallbacks;
	std::mutex m_registerCmdLock;
};

extern UserMessageManager g_UserMessageManager;