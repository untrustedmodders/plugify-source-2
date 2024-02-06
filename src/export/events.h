#pragma once

#include <core/eventmanager.h>

extern "C"
PLUGIN_API EventHookError EventManager_HookEvent(const char* name, FnEventListenerCallback pCallback, EventHookMode mode) {
	return g_EventManager.HookEvent(name, pCallback, mode);
}

extern "C"
PLUGIN_API EventHookError EventManager_UnhookEvent(const char* name, FnEventListenerCallback pCallback, EventHookMode mode) {
	return g_EventManager.UnhookEvent(name, pCallback, mode);
}

extern "C"
PLUGIN_API EventInfo* EventManager_CreateEvent(const char* name, bool force) {
	return g_EventManager.CreateEvent(name, force);
}

extern "C"
PLUGIN_API void EventManager_FireEvent(EventInfo* pInfo, bool bDontBroadcast) {
	g_EventManager.FireEvent(pInfo, bDontBroadcast);
}

extern "C"
PLUGIN_API void EventManager_FireEventToClient(EventInfo* pInfo, IClient* pClient) {
	g_EventManager.FireEventToClient(pInfo, pClient);
}

extern "C"
PLUGIN_API void EventManager_CancelCreatedEvent(EventInfo* pInfo) {
	g_EventManager.CancelCreatedEvent(pInfo);
}

extern "C"
PLUGIN_API bool Event_GetBool(EventInfo* pInfo, const char* key) {
	return pInfo->pEvent->GetBool(key);
}

extern "C"
PLUGIN_API float Event_GetFloat(EventInfo* pInfo, const char* key) {
	return pInfo->pEvent->GetFloat(key);
}

extern "C"
PLUGIN_API int Event_GetInt(EventInfo* pInfo, const char* key) {
	return pInfo->pEvent->GetInt(key);
}

extern "C"
PLUGIN_API uint64 Event_GetUInt64(EventInfo* pInfo, const char* key) {
	return pInfo->pEvent->GetUint64(key);
}

extern "C"
PLUGIN_API const char* Event_GetString(EventInfo* pInfo, const char* key) {
	return pInfo->pEvent->GetString(key);
}

extern "C"
PLUGIN_API void* Event_GetPtr(EventInfo* pInfo, const char* key) {
	return pInfo->pEvent->GetPtr(key);
}

extern "C"
PLUGIN_API const char* Event_GetName(EventInfo* pInfo) {
	return pInfo->pEvent->GetName();
}

extern "C"
PLUGIN_API void Event_SetBool(EventInfo* pInfo, const char* key, bool value) {
	pInfo->pEvent->SetBool(key, value);
}

extern "C"
PLUGIN_API void Event_SetFloat(EventInfo* pInfo, const char* key, float value) {
	pInfo->pEvent->SetFloat(key, value);
}

extern "C"
PLUGIN_API void Event_SetInt(EventInfo* pInfo, const char* key, int value) {
	pInfo->pEvent->SetInt(key, value);
}

extern "C"
PLUGIN_API void Event_SetUInt64(EventInfo* pInfo, const char* key, uint64 value) {
	pInfo->pEvent->SetUint64(key, value);
}

extern "C"
PLUGIN_API void Event_SetString(EventInfo* pInfo, const char* key, const char* value) {
	pInfo->pEvent->SetString(key, value);
}

extern "C"
PLUGIN_API void Event_SetPtr(EventInfo* pInfo, const char* key, void* value) {
	pInfo->pEvent->SetPtr(key, value);
}

extern "C"
PLUGIN_API void Event_SetBroadcast(EventInfo* pInfo, bool dontBroadcast) {
	pInfo->bDontBroadcast = dontBroadcast;
}
