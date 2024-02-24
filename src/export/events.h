#pragma once

#include <core/eventmanager.h>

extern "C" PLUGIN_API EventHookError EventManager_HookEvent(const std::string& name, FnEventListenerCallback pCallback, EventHookMode mode)
{
	return g_EventManager.HookEvent(name, pCallback, mode);
}

extern "C" PLUGIN_API EventHookError EventManager_UnhookEvent(const std::string& name, FnEventListenerCallback pCallback, EventHookMode mode)
{
	return g_EventManager.UnhookEvent(name, pCallback, mode);
}

extern "C" PLUGIN_API EventInfo* EventManager_CreateEvent(const std::string& name, bool force)
{
	return g_EventManager.CreateEvent(name, force);
}

extern "C" PLUGIN_API void EventManager_FireEvent(EventInfo* pInfo, bool bDontBroadcast)
{
	g_EventManager.FireEvent(pInfo, bDontBroadcast);
}

extern "C" PLUGIN_API void EventManager_FireEventToClient(EventInfo* pInfo, IClient* pClient)
{
	g_EventManager.FireEventToClient(pInfo, pClient);
}

extern "C" PLUGIN_API void EventManager_CancelCreatedEvent(EventInfo* pInfo)
{
	g_EventManager.CancelCreatedEvent(pInfo);
}

extern "C" PLUGIN_API bool Event_GetBool(EventInfo* pInfo, const std::string& key)
{
	return pInfo->pEvent->GetBool(key.c_str());
}

extern "C" PLUGIN_API float Event_GetFloat(EventInfo* pInfo, const std::string& key)
{
	return pInfo->pEvent->GetFloat(key.c_str());
}

extern "C" PLUGIN_API int Event_GetInt(EventInfo* pInfo, const std::string& key)
{
	return pInfo->pEvent->GetInt(key.c_str());
}

extern "C" PLUGIN_API uint64 Event_GetUInt64(EventInfo* pInfo, const std::string& key)
{
	return pInfo->pEvent->GetUint64(key.c_str());
}

extern "C" PLUGIN_API void Event_GetString(std::string& output, EventInfo* pInfo, const std::string& key)
{
	output = pInfo->pEvent->GetString(key.c_str());
}

extern "C" PLUGIN_API void* Event_GetPtr(EventInfo* pInfo, const std::string& key)
{
	return pInfo->pEvent->GetPtr(key.c_str());
}

extern "C" PLUGIN_API void Event_GetName(std::string& output, EventInfo* pInfo)
{
	output = pInfo->pEvent->GetName();
}

extern "C" PLUGIN_API void Event_SetBool(EventInfo* pInfo, const std::string& key, bool value)
{
	pInfo->pEvent->SetBool(key.c_str(), value);
}

extern "C" PLUGIN_API void Event_SetFloat(EventInfo* pInfo, const std::string& key, float value)
{
	pInfo->pEvent->SetFloat(key.c_str(), value);
}

extern "C" PLUGIN_API void Event_SetInt(EventInfo* pInfo, const std::string& key, int value)
{
	pInfo->pEvent->SetInt(key.c_str(), value);
}

extern "C" PLUGIN_API void Event_SetUInt64(EventInfo* pInfo, const std::string& key, uint64 value)
{
	pInfo->pEvent->SetUint64(key.c_str(), value);
}

extern "C" PLUGIN_API void Event_SetString(EventInfo* pInfo, const std::string& key, const std::string& value)
{
	pInfo->pEvent->SetString(key.c_str(), value.c_str());
}

extern "C" PLUGIN_API void Event_SetPtr(EventInfo* pInfo, const std::string& key, void* value)
{
	pInfo->pEvent->SetPtr(key.c_str(), value);
}

extern "C" PLUGIN_API void Event_SetBroadcast(EventInfo* pInfo, bool dontBroadcast)
{
	pInfo->bDontBroadcast = dontBroadcast;
}
