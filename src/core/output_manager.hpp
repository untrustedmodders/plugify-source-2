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
	std::array<ListenerManager<EntityListenerCallback>, 2> callbacks;
};

class EntityOutputManager {
public:
	EntityOutputManager() = default;
	~EntityOutputManager() = default;

	bool HookEntityOutput(plg::string classname, plg::string output, EntityListenerCallback callback, HookMode mode);
	bool UnhookEntityOutput(plg::string classname, plg::string output, EntityListenerCallback callback, HookMode mode);

	poly::ReturnAction Hook_FireOutputInternal(poly::Params& params, int count, poly::Return& ret);
	poly::ReturnAction Hook_FireOutputInternal_Post(poly::Params& params, int count, poly::Return& ret);

private:
	std::unordered_map<OutputKey, CallbackPair, utils::pair_hash<plg::string, plg::string>> m_hookMap;
	std::vector<CallbackPair*> m_vecCallbackPairs;
	std::mutex m_registerHookLock;
};

extern EntityOutputManager g_OutputManager;
