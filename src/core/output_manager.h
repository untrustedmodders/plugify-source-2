#pragma once

#include "listener_manager.h"
#include <core/sdk/utils.h>

#include <entity2/entitysystem.h>
#include <plugify/dynohook.h>
#include <variant.h>

class CEntityIOOutput;

using OutputKey = std::pair<std::string, std::string>;

using EntityListenerCallback = ResultType (*)(int activator, int caller, float delay);

struct CallbackPair
{
	CListenerManager<EntityListenerCallback> pre;
	CListenerManager<EntityListenerCallback> post;
};

class CEntityOutputManager
{
public:
	CEntityOutputManager() = default;
	~CEntityOutputManager() = default;

	void HookEntityOutput(std::string szClassname, std::string szOutput, EntityListenerCallback callback, HookMode mode);
	void UnhookEntityOutput(std::string szClassname, std::string szOutput, EntityListenerCallback callback, HookMode mode);

	dyno::ReturnAction Hook_FireOutputInternal(dyno::IHook& hook);
	dyno::ReturnAction Hook_FireOutputInternal_Post(dyno::IHook& hook);

private:
	std::unordered_map<OutputKey, CallbackPair, utils::PairHash<std::string, std::string>> m_hookMap;
	std::vector<CallbackPair*> m_vecCallbackPairs;
};

extern CEntityOutputManager g_OutputManager;

struct EntityIOConnectionDesc_t
{
	string_t m_targetDesc;
	string_t m_targetInput;
	string_t m_valueOverride;
	CEntityHandle m_hTarget;
	EntityIOTargetType_t m_nTargetType;
	int32 m_nTimesToFire;
	float m_flDelay;
};

struct EntityIOConnection_t : EntityIOConnectionDesc_t
{
	bool m_bMarkedForRemoval;
	EntityIOConnection_t* m_pNext;
};

struct EntityIOOutputDesc_t
{
	const char* m_pName;
	uint32 m_nFlags;
	uint32 m_nOutputOffset;
};

class CEntityIOOutput
{
public:
	void* vtable;
	EntityIOConnection_t* m_pConnections;
	EntityIOOutputDesc_t* m_pDesc;
};