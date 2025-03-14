#pragma once

#include "listener_manager.hpp"

#include <networksystem/netmessage.h>

#include <plugify/polyhook.hpp>

class UserMessage;
using UserMessageCallback = ResultType (*)(UserMessage* message);

struct UserMessageHook {
	std::array<CListenerManager<UserMessageCallback>, 2> callbacks;
};

class CUserMessageManager {
public:
	CUserMessageManager() = default;
	~CUserMessageManager() = default;

	bool HookUserMessage(int messageId, UserMessageCallback callback, HookMode mode);
	bool UnhookUserMessage(int messageId, UserMessageCallback callback, HookMode mode);

	poly::ReturnAction Hook_PostEvent(poly::Params& params, int count, poly::Return& ret, HookMode node);
	ResultType ExecuteMessageCallbacks(INetworkMessageInternal* pEvent, CNetMessage* pData, int nClientCount, uint64* clients, HookMode mode);

private:
	std::unordered_map<int, UserMessageHook> m_hooksMap;
	std::array<CListenerManager<UserMessageCallback>, 2> m_globalCallbacks;
	std::mutex m_registerCmdLock;
};

extern CUserMessageManager g_UserMessageManager;