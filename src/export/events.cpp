#include <core/event_manager.h>
#include <plugin_export.h>

extern "C" PLUGIN_API int HookEvent(const plg::string& name, EventListenerCallback pCallback, bool post)
{
	return static_cast<int>(g_EventManager.HookEvent(name, pCallback, static_cast<HookMode>(post)));
}

extern "C" PLUGIN_API int UnhookEvent(const plg::string& name, EventListenerCallback pCallback, bool post)
{
	return static_cast<int>(g_EventManager.UnhookEvent(name, pCallback, static_cast<HookMode>(post)));
}

extern "C" PLUGIN_API EventInfo* CreateEvent(const plg::string& name, bool force)
{
	return g_EventManager.CreateEvent(name, force);
}

extern "C" PLUGIN_API void FireEvent(EventInfo* pInfo, bool bDontBroadcast)
{
	g_EventManager.FireEvent(pInfo, bDontBroadcast);
}

extern "C" PLUGIN_API void FireEventToClient(EventInfo* pInfo, int clientIndex)
{
	g_EventManager.FireEventToClient(pInfo, CPlayerSlot(clientIndex - 1));
}

extern "C" PLUGIN_API void CancelCreatedEvent(EventInfo* pInfo)
{
	g_EventManager.CancelCreatedEvent(pInfo);
}

extern "C" PLUGIN_API bool GetEventBool(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetBool(key.c_str());
}

extern "C" PLUGIN_API float GetEventFloat(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetFloat(key.c_str());
}

extern "C" PLUGIN_API int GetEventInt(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetInt(key.c_str());
}

extern "C" PLUGIN_API uint64_t GetEventUInt64(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetUint64(key.c_str());
}

extern "C" PLUGIN_API void GetEventString(plg::string& output, EventInfo* pInfo, const plg::string& key)
{
	std::construct_at(&output, pInfo->pEvent->GetString(key.c_str()));
}

extern "C" PLUGIN_API void* GetEventPtr(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetPtr(key.c_str());
}

extern "C" PLUGIN_API CEntityInstance* GetEventPlayerController(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetPlayerController(key.c_str());
}

extern "C" PLUGIN_API int GetEventPlayerIndex(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetPlayerSlot(key.c_str()).Get();
}

extern "C" PLUGIN_API CEntityInstance* GetEventPlayerPawn(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetPlayerPawn(key.c_str());
}

extern "C" PLUGIN_API CEntityInstance* GetEventEntity(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetEntity(key.c_str());
}

extern "C" PLUGIN_API int GetEventEntityIndex(EventInfo* pInfo, const plg::string& key)
{
	return pInfo->pEvent->GetEntityIndex(key.c_str()).Get();
}

extern "C" PLUGIN_API void GetEventName(plg::string& output, EventInfo* pInfo)
{
	std::construct_at(&output, pInfo->pEvent->GetName());
}

extern "C" PLUGIN_API void SetEventBool(EventInfo* pInfo, const plg::string& key, bool value)
{
	pInfo->pEvent->SetBool(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventFloat(EventInfo* pInfo, const plg::string& key, float value)
{
	pInfo->pEvent->SetFloat(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventInt(EventInfo* pInfo, const plg::string& key, int value)
{
	pInfo->pEvent->SetInt(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventUInt64(EventInfo* pInfo, const plg::string& key, uint64_t value)
{
	pInfo->pEvent->SetUint64(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventString(EventInfo* pInfo, const plg::string& key, const plg::string& value)
{
	pInfo->pEvent->SetString(key.c_str(), value.c_str());
}

extern "C" PLUGIN_API void SetEventPtr(EventInfo* pInfo, const plg::string& key, void* value)
{
	pInfo->pEvent->SetPtr(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventPlayerController(EventInfo* pInfo, const plg::string& key, CEntityInstance* value)
{
	pInfo->pEvent->SetPlayer(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventPlayerIndex(EventInfo* pInfo, const plg::string& key, int value)
{
	pInfo->pEvent->SetPlayer(key.c_str(), CPlayerSlot(value));
}

extern "C" PLUGIN_API void SetEventEntity(EventInfo* pInfo, const plg::string& key, CEntityInstance* value)
{
	pInfo->pEvent->SetEntity(key.c_str(), value);
}

extern "C" PLUGIN_API void SetEventEntityIndex(EventInfo* pInfo, const plg::string& key, int value)
{
	pInfo->pEvent->SetEntity(key.c_str(), CEntityIndex(value));
}

extern "C" PLUGIN_API void SetEventBroadcast(EventInfo* pInfo, bool dontBroadcast)
{
	pInfo->bDontBroadcast = dontBroadcast;
}

extern "C" PLUGIN_API int LoadEventsFromFile(const plg::string& path, bool searchAll)
{
	return g_gameEventManager->LoadEventsFromFile(path.c_str(), searchAll);
}
