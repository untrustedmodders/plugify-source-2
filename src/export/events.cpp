#include <core/event_manager.hpp>
#include <entity2/entitysystem.h>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Creates a hook for when a game event is fired.
 * @param name The name of the event to hook.
 * @param pCallback The callback function to call when the event is fired.
 * @param mode Whether the hook was in post mode (after processing) or pre mode (before processing).
 * @return An integer indicating the result of the hook operation.
 */
extern "C" PLUGIN_API int HookEvent(const plg::string& name, EventListenerCallback pCallback, HookMode mode) {
	return static_cast<int>(g_EventManager.HookEvent(name, pCallback, mode));
}

/**
 * @brief Removes a hook for when a game event is fired.
 * @param name The name of the event to unhook.
 * @param pCallback The callback function to remove.
 * @param mode Whether the hook was in post mode (after processing) or pre mode (before processing).
 * @return An integer indicating the result of the unhook operation.
 */
extern "C" PLUGIN_API int UnhookEvent(const plg::string& name, EventListenerCallback pCallback, HookMode mode) {
	return static_cast<int>(g_EventManager.UnhookEvent(name, pCallback, mode));
}

/**
 * @brief Creates a game event to be fired later.
 * @param name The name of the event to create.
 * @param force A boolean indicating whether to force the creation of the event.
 * @return A pointer to the created EventInfo structure.
 */
extern "C" PLUGIN_API EventInfo* CreateEvent(const plg::string& name, bool force) {
	return g_EventManager.CreateEvent(name, force);
}

/**
 * @brief Fires a game event.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param bDontBroadcast A boolean indicating whether to broadcast the event.
 */
extern "C" PLUGIN_API void FireEvent(EventInfo* info, bool bDontBroadcast) {
	g_EventManager.FireEvent(info, bDontBroadcast);
}

/**
 * @brief Fires a game event to a specific client.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param playerSlot The index of the player's slot to fire the event to.
 */
extern "C" PLUGIN_API void FireEventToClient(EventInfo* info, int playerSlot) {
	g_EventManager.FireEventToClient(info, playerSlot);
}

/**
 * @brief Cancels a previously created game event that has not been fired.
 * @param info A pointer to the EventInfo structure of the event to cancel.
 */
extern "C" PLUGIN_API void CancelCreatedEvent(EventInfo* info) {
	g_EventManager.CancelCreatedEvent(info);
}

/**
 * @brief Retrieves the boolean value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the boolean value.
 * @return The boolean value associated with the key.
 */
extern "C" PLUGIN_API bool GetEventBool(EventInfo* info, const plg::string& key) {
	return info->event->GetBool(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the float value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the float value.
 * @return The float value associated with the key.
 */
extern "C" PLUGIN_API float GetEventFloat(EventInfo* info, const plg::string& key) {
	return info->event->GetFloat(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the integer value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the integer value.
 * @return The integer value associated with the key.
 */
extern "C" PLUGIN_API int GetEventInt(EventInfo* info, const plg::string& key) {
	return info->event->GetInt(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the long integer value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the long integer value.
 * @return The long integer value associated with the key.
 */
extern "C" PLUGIN_API uint64_t GetEventUInt64(EventInfo* info, const plg::string& key) {
	return info->event->GetUint64(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the string value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the string value.
 * @return A string where the result will be stored.
 */
extern "C" PLUGIN_API plg::string GetEventString(EventInfo* info, const plg::string& key) {
	return info->event->GetString(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the pointer value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the pointer value.
 * @return The pointer value associated with the key.
 */
extern "C" PLUGIN_API void* GetEventPtr(EventInfo* info, const plg::string& key) {
	return info->event->GetPtr(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the player controller address of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the player controller address.
 * @return A pointer to the player controller associated with the key.
 */
extern "C" PLUGIN_API CEntityInstance* GetEventPlayerController(EventInfo* info, const plg::string& key) {
	return info->event->GetPlayerController(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the player index of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the player index.
 * @return The player index associated with the key.
 */
extern "C" PLUGIN_API int GetEventPlayerIndex(EventInfo* info, const plg::string& key) {
	return info->event->GetPlayerSlot(GameEventKeySymbol_t::Make(key.c_str())).Get();
}

/**
 * @brief Retrieves the player pawn address of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the player pawn address.
 * @return A pointer to the player pawn associated with the key.
 */
extern "C" PLUGIN_API CEntityInstance* GetEventPlayerPawn(EventInfo* info, const plg::string& key) {
	return info->event->GetPlayerPawn(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the entity address of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the entity address.
 * @return A pointer to the entity associated with the key.
 */
extern "C" PLUGIN_API CEntityInstance* GetEventEntity(EventInfo* info, const plg::string& key) {
	return info->event->GetEntity(GameEventKeySymbol_t::Make(key.c_str()));
}

/**
 * @brief Retrieves the entity index of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the entity index.
 * @return The entity index associated with the key.
 */
extern "C" PLUGIN_API int GetEventEntityIndex(EventInfo* info, const plg::string& key) {
	return info->event->GetEntityIndex(GameEventKeySymbol_t::Make(key.c_str())).Get();
}

/**
 * @brief Retrieves the entity handle of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to retrieve the entity handle.
 * @return The entity handle associated with the key.
 */
extern "C" PLUGIN_API int GetEventEntityHandle(EventInfo* info, const plg::string& key) {
	return info->event->GetEHandle(GameEventKeySymbol_t::Make(key.c_str())).ToInt();
}

/**
 * @brief Retrieves the name of a game event.
 * @param info A pointer to the EventInfo structure containing event data.
 * @return A string where the result will be stored.
 */
extern "C" PLUGIN_API plg::string GetEventName(EventInfo* info) {
	return info->event->GetName();
}

/**
 * @brief Sets the boolean value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the boolean value.
 * @param value The boolean value to set.
 */
extern "C" PLUGIN_API void SetEventBool(EventInfo* info, const plg::string& key, bool value) {
	info->event->SetBool(GameEventKeySymbol_t::Make(key.c_str()), value);
}

/**
 * @brief Sets the floating point value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the float value.
 * @param value The float value to set.
 */
extern "C" PLUGIN_API void SetEventFloat(EventInfo* info, const plg::string& key, float value) {
	info->event->SetFloat(GameEventKeySymbol_t::Make(key.c_str()), value);
}

/**
 * @brief Sets the integer value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the integer value.
 * @param value The integer value to set.
 */
extern "C" PLUGIN_API void SetEventInt(EventInfo* info, const plg::string& key, int value) {
	info->event->SetInt(GameEventKeySymbol_t::Make(key.c_str()), value);
}

/**
 * @brief Sets the long integer value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the long integer value.
 * @param value The long integer value to set.
 */
extern "C" PLUGIN_API void SetEventUInt64(EventInfo* info, const plg::string& key, uint64_t value) {
	info->event->SetUint64(GameEventKeySymbol_t::Make(key.c_str()), value);
}

/**
 * @brief Sets the string value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the string value.
 * @param value The string value to set.
 */
extern "C" PLUGIN_API void SetEventString(EventInfo* info, const plg::string& key, const plg::string& value) {
	info->event->SetString(GameEventKeySymbol_t::Make(key.c_str()), value.c_str());
}

/**
 * @brief Sets the pointer value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the pointer value.
 * @param value The pointer value to set.
 */
extern "C" PLUGIN_API void SetEventPtr(EventInfo* info, const plg::string& key, void* value) {
	info->event->SetPtr(GameEventKeySymbol_t::Make(key.c_str()), value);
}

/**
 * @brief Sets the player controller address of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the player controller address.
 * @param value A handle to the player controller to set.
 */
extern "C" PLUGIN_API void SetEventPlayerController(EventInfo* info, const plg::string& key, int value) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) value));
	if (!pEntity) {
		return;
	}

	info->event->SetPlayer(GameEventKeySymbol_t::Make(key.c_str()), pEntity);
}

/**
 * @brief Sets the player index value of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the player index value.
 * @param value The player index value to set.
 */
extern "C" PLUGIN_API void SetEventPlayerIndex(EventInfo* info, const plg::string& key, int value) {
	info->event->SetPlayer(GameEventKeySymbol_t::Make(key.c_str()), CPlayerSlot(value));
}

/**
 * @brief Sets the entity address of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the entity address.
 * @param value A handle to the entity to set.
 */
extern "C" PLUGIN_API void SetEventEntity(EventInfo* info, const plg::string& key, int value) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) value));
	if (!pEntity) {
		return;
	}

	info->event->SetEntity(GameEventKeySymbol_t::Make(key.c_str()), pEntity);
}

/**
 * @brief Sets the entity index of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the entity index.
 * @param value The entity index value to set.
 */
extern "C" PLUGIN_API void SetEventEntityIndex(EventInfo* info, const plg::string& key, int value) {
	info->event->SetEntity(GameEventKeySymbol_t::Make(key.c_str()), CEntityIndex(value));
}

/**
 * @brief Sets the entity handle of a game event's key.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param key The key for which to set the entity handle.
 * @param value The entity handle value to set.
 */
extern "C" PLUGIN_API void SetEventEntityHandle(EventInfo* info, const plg::string& key, int value) {
	info->event->SetEntity(GameEventKeySymbol_t::Make(key.c_str()), CEntityHandle((uint32) value).Get());
}

/**
 * @brief Sets whether an event's broadcasting will be disabled or not.
 * @param info A pointer to the EventInfo structure containing event data.
 * @param dontBroadcast A boolean indicating whether to disable broadcasting.
 */
extern "C" PLUGIN_API void SetEventBroadcast(EventInfo* info, bool dontBroadcast) {
	info->dontBroadcast = dontBroadcast;
}

/**
 * @brief Load game event descriptions from a file (e.g., "resource/gameevents.res").
 * @param path The path to the file containing event descriptions.
 * @param searchAll A boolean indicating whether to search all paths for the file.
 * @return An integer indicating the result of the loading operation.
 */
extern "C" PLUGIN_API int LoadEventsFromFile(const plg::string& path, bool searchAll) {
	return g_pGameEventManager->LoadEventsFromFile(path.c_str(), searchAll);
}

PLUGIFY_WARN_POP()
