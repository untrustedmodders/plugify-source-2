#pragma once

#include "listener_manager.hpp"
#include <core/sdk/utils.h>
#include <core/sdk/entity/entityio.h>

#include <entity2/entitysystem.h>
#include <plugify/polyhook.hpp>
#include <variant.h>

using OutputKey = std::pair<plg::string, plg::string>;
using EntityListenerCallback = ResultType (*)(int activatorHandle, int callerHandle, float delay);

struct CallbackPair {
	CListenerManager<EntityListenerCallback> pre;
	CListenerManager<EntityListenerCallback> post;
};

class CEntityOutputManager {
public:
	CEntityOutputManager() = default;
	~CEntityOutputManager() = default;

	void HookEntityOutput(plg::string szClassname, plg::string szOutput, EntityListenerCallback callback, HookMode mode);
	void UnhookEntityOutput(plg::string szClassname, plg::string szOutput, EntityListenerCallback callback, HookMode mode);

	poly::ReturnAction Hook_FireOutputInternal(poly::Params& params, int count, poly::Return& ret);
	poly::ReturnAction Hook_FireOutputInternal_Post(poly::Params& params, int count, poly::Return& ret);

private:
	std::unordered_map<OutputKey, CallbackPair, utils::PairHash<plg::string, plg::string>> m_hookMap;
	std::vector<CallbackPair*> m_vecCallbackPairs;
	std::mutex m_registerHookLock;
};

extern CEntityOutputManager g_OutputManager;
