#include <core/output_manager.hpp>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbasemodelentity.h>
#include <core/sdk/utils.h>
#include <entitykeyvalues.h>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Converts an entity index into an entity pointer.
 *
 * This function retrieves a pointer to the entity corresponding to the given
 * entity index. It is essential for accessing entity properties and methods
 * using the entity index.
 *
 * @param entityIndex The index of the entity to convert.
 * @return A pointer to the entity instance, or nullptr if the entity does not exist.
 */
extern "C" PLUGIN_API void* EntIndexToEntPointer(int entityIndex) {
	return g_pGameEntitySystem->GetEntityInstance(CEntityIndex(entityIndex));
}

/**
 * @brief Retrieves the entity index from an entity pointer.
 *
 * This function returns the index associated with a given entity pointer,
 * allowing for the identification of the entity's position in the entity list.
 *
 * @param entity A pointer to the entity whose index is to be retrieved.
 * @return The index of the entity, or -1 if the entity is nullptr.
 */
extern "C" PLUGIN_API int EntPointerToEntIndex(CEntityInstance* entity) {
	if (!g_pGameEntitySystem->IsEntityPtr(entity)) {
		return -1;
	}

	return entity->GetRefEHandle().GetEntryIndex();
}

/**
 * @brief Converts an entity pointer into an entity handle.
 *
 * This function creates and returns an entity handle from a given entity pointer.
 * If the entity pointer is null, it returns an invalid entity handle index.
 *
 * @param entity A pointer to the entity to convert.
 * @return The entity handle as an integer, or INVALID_EHANDLE_INDEX if the entity is nullptr.
 */
extern "C" PLUGIN_API int EntPointerToEntHandle(CEntityInstance* entity) {
	if (!g_pGameEntitySystem->IsEntityPtr(entity)) {
		return INVALID_EHANDLE_INDEX;
	}

	return entity->GetRefEHandle().ToInt();
}

/**
 * @brief Retrieves the entity pointer from an entity handle.
 *
 * This function retrieves the entity pointer corresponding to the given entity handle.
 * If the handle is invalid, it returns nullptr.
 *
 * @param entityHandle The entity handle to convert.
 * @return A pointer to the entity instance, or nullptr if the handle is invalid.
 */
extern "C" PLUGIN_API void* EntHandleToEntPointer(int entityHandle) {
	CEntityHandle handle((uint32) entityHandle);
	if (!handle.IsValid()) {
		return nullptr;
	}

	return g_pGameEntitySystem->GetEntityInstance(handle);
}

/**
 * @brief Converts an entity index into an entity handle.
 *
 * This function converts a given entity index into its corresponding entity handle.
 * If the entity index is invalid, it returns -1.
 *
 * @param entityIndex The index of the entity to convert.
 * @return The entity handle as an integer, or -1 if the entity index is invalid.
 */
extern "C" PLUGIN_API int EntIndexToEntHandle(int entityIndex) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!pEntity) {
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->GetRefEHandle().ToInt();
}

/**
 * @brief Retrieves the entity index from an entity handle.
 *
 * This function retrieves the entity index corresponding to the provided entity handle.
 * If the handle is invalid or does not correspond to an entity, it returns -1.
 *
 * @param entityHandle The entity handle from which to retrieve the index.
 * @return The index of the entity, or -1 if the handle is invalid.
 */
extern "C" PLUGIN_API int EntHandleToEntIndex(int entityHandle) {
	CEntityHandle handle((uint32) entityHandle);
	if (!handle.IsValid()) {
		return -1;
	}

	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(handle));
	if (!pEntity) {
		return -1;
	}

	return pEntity->entindex();
}

/**
 * @brief Checks if the provided entity handle is valid.
 *
 * This function checks whether the entity handle points to a valid entity in the
 * entity system. It returns true if valid, and false otherwise.
 *
 * @param entityHandle The entity handle to check.
 * @return True if the entity handle is valid, false otherwise.
 */
extern "C" PLUGIN_API bool IsValidEntHandle(int entityHandle) {
	CEntityHandle handle((uint32) entityHandle);
	if (!handle.IsValid()) {
		return false;
	}

	return g_pGameEntitySystem->GetEntityInstance(handle) != nullptr;
}

/**
 * @brief Checks if the provided entity pointer is valid.
 *
 * This function checks whether the entity pointer points to a valid entity in the
 * entity system. It returns true if valid, and false otherwise.
 *
 * @param entity The entity pointer to check.
 * @return True if the entity pointer is valid, false otherwise.
 */
extern "C" PLUGIN_API bool IsValidEntPointer(CEntityInstance* entity) {
	return g_pGameEntitySystem->IsEntityPtr(entity);
}

/**
 * @brief Retrieves the pointer to the first active entity.
 *
 * This function returns a pointer to the first active entity in the entity system,
 * which can be useful for iterating through active entities.
 *
 * @return A pointer to the first active entity.
 */
extern "C" PLUGIN_API void* GetFirstActiveEntity() {
	return g_pGameEntitySystem->m_EntityList.m_pFirstActiveEntity;
}

/**
 * @brief Retrieves a pointer to the concrete entity list.
 *
 * This function returns a pointer to the concrete entity list, allowing for direct
 * access to the list of entities managed by the entity system.
 *
 * @return A pointer to the entity list structure.
 */
extern "C" PLUGIN_API void* GetConcreteEntityListPointer() {
	return &g_pGameEntitySystem->m_EntityList;
}

/**
 * @brief Adds an entity output hook on a specified entity class name.
 *
 * This function hooks a specified output for a given entity class name. The provided
 * callback will be triggered when the output is fired.
 *
 * @param szClassname The class name of the entity to hook the output for.
 * @param szOutput The output event name to hook.
 * @param callback The callback function to invoke when the output is fired.
 * @param post Indicates whether the hook should be a post-hook (true) or pre-hook (false).
 */
extern "C" PLUGIN_API void HookEntityOutput(const plg::string& szClassname, const plg::string& szOutput, EntityListenerCallback callback, bool post) {
	g_OutputManager.HookEntityOutput(szClassname, szOutput, callback, static_cast<HookMode>(post));
}

/**
 * @brief Removes an entity output hook.
 *
 * This function unhooks a previously set output for a given entity class name.
 *
 * @param szClassname The class name of the entity from which to unhook the output.
 * @param szOutput The output event name to unhook.
 * @param callback The callback function that was previously hooked.
 * @param post Indicates whether the hook was a post-hook (true) or pre-hook (false).
 */
extern "C" PLUGIN_API void UnhookEntityOutput(const plg::string& szClassname, const plg::string& szOutput, EntityListenerCallback callback, bool post) {
	g_OutputManager.UnhookEntityOutput(szClassname, szOutput, callback, static_cast<HookMode>(post));
}

////////////////////////
/**
 * @brief Searches for an entity by classname.
 *
 * This function searches for the first entity that matches the specified class name,
 * starting from a given entity. If no entity is found, it returns an invalid handle index.
 *
 * @param startEntity The entity handle from which to start the search.
 * @param classname The class name of the entity to search for.
 * @return The entity handle of the found entity, or INVALID_EHANDLE_INDEX if no entity is found.
 */
extern "C" PLUGIN_API int FindEntityByClassname(int startEntity, const plg::string& classname) {
	CBaseEntity* pStartStart = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) startEntity)));
	/*if (!pStartStart)
	{
		return INVALID_EHANDLE_INDEX;
	}*/

	CBaseEntity* pEntity = static_cast<CBaseEntity*>(addresses::CGameEntitySystem_FindEntityByClassName(g_pGameEntitySystem, pStartStart, classname.c_str()));
	if (!pEntity) {
		//g_Logger.LogFormat(LS_WARNING, "Cannot execute 'FindEntityByClassname' with invalid entity classname: %s\n", classname.c_str());
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->GetRefEHandle().ToInt();
}

/**
 * @brief Searches for an entity by name.
 *
 * This function searches for the first entity that matches the specified name,
 * starting from a given entity. If no entity is found, it returns an invalid handle index.
 *
 * @param startEntity The entity handle from which to start the search.
 * @param name The name of the entity to search for.
 * @return The entity handle of the found entity, or INVALID_EHANDLE_INDEX if no entity is found.
 */
extern "C" PLUGIN_API int FindEntityByName(int startEntity, const plg::string& name) {
	CBaseEntity* pStartStart = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) startEntity)));
	/*if (!pStartStart)
	{
		return INVALID_EHANDLE_INDEX;
	}*/

	CBaseEntity* pEntity = static_cast<CBaseEntity*>(addresses::CGameEntitySystem_FindEntityByName(g_pGameEntitySystem, pStartStart, name.c_str(), nullptr, nullptr, nullptr, nullptr));
	if (!pEntity) {
		//g_Logger.LogFormat(LS_WARNING, "Cannot execute 'FindEntityByName' with invalid entity name: %s\n", name.c_str());
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->GetRefEHandle().ToInt();
}

/**
 * @brief Creates an entity by string name but does not spawn it.
 *
 * This function creates an entity using the specified class name. The entity
 * is not spawned until DispatchSpawn is called. If ForceEdictIndex is not -1,
 * it will use the edict by that index. If the index is invalid or there is
 * already an edict using that index, it will error out.
 *
 * @param className The class name of the entity to create.
 * @return The entity handle of the created entity, or INVALID_EHANDLE_INDEX if the entity could not be created.
 */
extern "C" PLUGIN_API int CreateEntityByName(const plg::string& className) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(addresses::CreateEntityByName(className.c_str(), -1));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'CreateEntityByName' with invalid entity classname: %s\n", className.c_str());
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->GetRefEHandle().ToInt();
}

/**
 * @brief Spawns an entity into the game.
 *
 * This function spawns the specified entity into the game world.
 * It requires a valid entity handle to function correctly.
 *
 * @param entityHandle The handle of the entity to spawn.
 */
extern "C" PLUGIN_API void DispatchSpawn(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'DispatchSpawn' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->DispatchSpawn();
}

/**
 * @brief Spawns an entity into the game with key-value properties.
 *
 * This function spawns the specified entity into the game world.
 * It requires a valid entity handle to function correctly.
 *
 * @param entityHandle The handle of the entity to spawn.
 * @param keys A vector of keys representing the property names to set on the entity.
 * @param values A vector of values corresponding to the keys, representing the property values to set on the entity.
 */
extern "C" PLUGIN_API void DispatchSpawn2(int entityHandle, const plg::vector<plg::string>& keys, const plg::vector<plg::any>& values) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'DispatchSpawn2' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	if (keys.size() != values.size()) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'DispatchSpawn2': Mismatch between keys and values sizes. Keys size: %d, Values size: %d.", static_cast<int>(keys.size()), static_cast<int>(values.size()));
		return;
	}

	CEntityKeyValues* pESKeyValues = new CEntityKeyValues(g_pGameEntitySystem->GetEntityKeyValuesAllocator(), EKV_ALLOCATOR_EXTERNAL);

	g_pGameEntitySystem->AddRefKeyValues(pESKeyValues);

	for (size_t i = 0; i < values.size(); ++i) {
		auto key = EntityKeyId_t::Make(keys[i].c_str());
		plg::visit([&](const auto& v) {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, plg::string>) {
				pESKeyValues->SetString(key, v.c_str());
			} else if constexpr (std::is_pointer_v<T>) {
				pESKeyValues->SetPtr(key, v);
			} else if constexpr (std::is_same_v<T, int32_t>) {
				pESKeyValues->SetInt(key, v);
			} else if constexpr (std::is_same_v<T, uint32_t>) {
				pESKeyValues->SetUint(key, v);
			} else if constexpr (std::is_same_v<T, int64_t>) {
				pESKeyValues->SetInt64(key, v);
			} else if constexpr (std::is_same_v<T, uint64_t>) {
				pESKeyValues->SetUint64(key, v);
			} else if constexpr (std::is_same_v<T, bool>) {
				pESKeyValues->SetBool(key, v);
			} else if constexpr (std::is_same_v<T, float>) {
				pESKeyValues->SetFloat(key, v);
			} else if constexpr (std::is_same_v<T, double>) {
				pESKeyValues->SetDouble(key, v);
			} else if constexpr (std::is_same_v<T, plg::vec2>) {
				pESKeyValues->SetVector2D(key, *reinterpret_cast<const Vector2D*>(&v));
			} else if constexpr (std::is_same_v<T, plg::vec3>) {
				pESKeyValues->SetVector(key, *reinterpret_cast<const Vector*>(&v));
			} else if constexpr (std::is_same_v<T, plg::vec4>) {
				pESKeyValues->SetVector4D(key, *reinterpret_cast<const Vector4D*>(&v));
			} else if constexpr (std::is_same_v<T, plg::mat4x4>) {
				pESKeyValues->SetMatrix3x4(key, *reinterpret_cast<const matrix3x4_t*>(&v));
			} else if constexpr (std::is_arithmetic_v<T>) {
				pESKeyValues->SetInt(key, static_cast<int>(v));
			}
		},
				   values[i]);
	}

	pEntity->DispatchSpawn(pESKeyValues);

	g_pGameEntitySystem->ReleaseKeyValues(pESKeyValues);
}

/**
 * @brief Marks an entity for deletion.
 *
 * This function marks the specified entity for deletion from the game world.
 * It requires a valid entity handle to function correctly.
 *
 * @param entityHandle The handle of the entity to be deleted.
 */
extern "C" PLUGIN_API void RemoveEntity(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'RemoveEntity' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->Remove();
}

///

/**
 * @brief Retrieves the class name of an entity.
 *
 * This function gets the class name of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose class name is to be retrieved.
 * @return A string where the class name will be stored.
 */
extern "C" PLUGIN_API plg::string GetEntityClassname(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityClassname' on invalid entity handle: %d\n", entityHandle);
		return {};
	}

	return pEntity->GetClassname();
}

/**
 * @brief Retrieves the name of an entity.
 *
 * This function gets the name of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose name is to be retrieved.
 * @retrun A string where the entity name will be stored.
 */
extern "C" PLUGIN_API plg::string GetEntityName(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityName' on invalid entity handle: %d\n", entityHandle);
		return {};
	}

	return pEntity->GetName();
}

/**
 * @brief Sets the name of an entity.
 *
 * This function updates the name of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose name is to be set.
 * @param name The new name to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityName(int entityHandle, const plg::string& name) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityName' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->SetName(name.c_str());
}

/**
 * @brief Retrieves the movement type of an entity.
 *
 * This function returns the movement type of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose movement type is to be retrieved.
 * @return The movement type of the entity, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API MoveType_t GetEntityMoveType(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityMoveType' on invalid entity handle: %d\n", entityHandle);
		return MOVETYPE_NONE;
	}

	return pEntity->m_MoveType();
}

/**
 * @brief Sets the movement type of an entity.
 *
 * This function updates the movement type of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose movement type is to be set.
 * @param moveType The new movement type to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityMoveType(int entityHandle, MoveType_t moveType) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityMoveType' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->SetMoveType(moveType);
}

/**
 * @brief Retrieves the gravity scale of an entity.
 *
 * This function returns the gravity scale of the specified entity.
 * If the entity is invalid, it returns 0.0f.
 *
 * @param entityHandle The handle of the entity whose gravity scale is to be retrieved.
 * @return The gravity scale of the entity, or 0.0f if the entity is invalid.
 */
extern "C" PLUGIN_API float GetEntityGravity(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityGravity' on invalid entity handle: %d\n", entityHandle);
		return 0.0f;
	}

	return pEntity->m_flGravityScale();
}

/**
 * @brief Sets the gravity scale of an entity.
 *
 * This function updates the gravity scale of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose gravity scale is to be set.
 * @param gravity The new gravity scale to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityGravity(int entityHandle, float gravity) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityGravity' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_flGravityScale = gravity;
}

/**
 * @brief Retrieves the flags of an entity.
 *
 * This function returns the flags of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose flags are to be retrieved.
 * @return The flags of the entity, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityFlags(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityFlags' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return static_cast<int>(pEntity->m_fFlags());
}

/**
 * @brief Sets the flags of an entity.
 *
 * This function updates the flags of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose flags are to be set.
 * @param flags The new flags to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityFlags(int entityHandle, int flags) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityFlags' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_fFlags = static_cast<uint32>(flags);
}

/**
 * @brief Retrieves the render color of an entity.
 *
 * This function gets the render color of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose render color is to be retrieved.
 * @return The raw color value of the entity's render color, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityRenderColor(int entityHandle) {
	CBaseModelEntity* pEntity = static_cast<CBaseModelEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityRenderColor' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return pEntity->m_clrRender().GetRawColor();
}

/**
 * @brief Sets the render color of an entity.
 *
 * This function updates the render color of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose render color is to be set.
 * @param color The new raw color value to set for the entity's render color.
 */
extern "C" PLUGIN_API void SetEntityRenderColor(int entityHandle, int color) {
	CBaseModelEntity* pEntity = static_cast<CBaseModelEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityRenderColor' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_clrRender = *reinterpret_cast<Color*>(&color);
}

/**
 * @brief Retrieves the render mode of an entity.
 *
 * This function gets the render mode of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose render mode is to be retrieved.
 * @return The render mode of the entity, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API int8_t GetEntityRenderMode(int entityHandle) {
	CBaseModelEntity* pEntity = static_cast<CBaseModelEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityRenderMode' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return pEntity->m_nRenderMode();
}

/**
 * @brief Sets the render mode of an entity.
 *
 * This function updates the render mode of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose render mode is to be set.
 * @param renderMode The new render mode to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityRenderMode(int entityHandle, int8_t renderMode) {
	CBaseModelEntity* pEntity = static_cast<CBaseModelEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityRenderMode' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_nRenderMode = renderMode;
}

/**
 * @brief Retrieves the health of an entity.
 *
 * This function returns the current health of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose health is to be retrieved.
 * @return The health of the entity, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityHealth(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityHealth' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return pEntity->m_iHealth();
}

/**
 * @brief Sets the health of an entity.
 *
 * This function updates the health of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose health is to be set.
 * @param health The new health value to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityHealth(int entityHandle, int health) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityHealth' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_iHealth = health;
}

/**
 * @brief Retrieves the team number of an entity.
 *
 * This function returns the team number of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose team number is to be retrieved.
 * @return The team number of the entity, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API int GetTeamEntity(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetTeamEntity' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return pEntity->m_iTeamNum();
}

/**
 * @brief Sets the team number of an entity.
 *
 * This function updates the team number of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose team number is to be set.
 * @param team The new team number to set for the entity.
 */
extern "C" PLUGIN_API void SetTeamEntity(int entityHandle, int team) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetTeamEntity' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_iTeamNum = team;
}

/**
 * @brief Retrieves the owner of an entity.
 *
 * This function returns the entity handle of the owner for the specified entity.
 * If the entity is invalid, it returns -1.
 *
 * @param entityHandle The handle of the entity whose owner is to be retrieved.
 * @return The handle of the owner entity, or INVALID_EHANDLE_INDEX if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityOwner(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityOwner' on invalid entity handle: %d\n", entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->m_CBodyComponent->m_pSceneNode->m_pOwner->GetRefEHandle().ToInt();
}

/**
 * @brief Sets the owner of an entity.
 *
 * This function updates the owner of the specified entity.
 * If either the entity or the new owner is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose owner is to be set.
 * @param ownerHandle The handle of the new owner entity.
 */
extern "C" PLUGIN_API void SetEntityOwner(int entityHandle, int ownerHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityOwner' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	CBaseEntity* pOwner = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) ownerHandle)));
	if (!pOwner) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityOwner' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_CBodyComponent->m_pSceneNode->m_pOwner = pOwner;
}

/**
 * @brief Retrieves the parent of an entity.
 *
 * This function returns the entity handle of the parent for the specified entity.
 * If the entity is invalid, it returns -1.
 *
 * @param entityHandle The handle of the entity whose parent is to be retrieved.
 * @return The handle of the parent entity, or INVALID_EHANDLE_INDEX if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityParent(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityParent' on invalid entity handle: %d\n", entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->m_CBodyComponent->m_pSceneNode->m_pParent->m_pOwner->GetRefEHandle().ToInt();
}

/**
 * @brief Sets the parent of an entity.
 *
 * This function updates the parent of the specified entity.
 * If either the entity or the new parent is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose parent is to be set.
 * @param parentHandle The handle of the new parent entity.
 */
extern "C" PLUGIN_API void SetEntityParent(int entityHandle, int parentHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityParent' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	CBaseEntity* pNewParent = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) parentHandle)));
	if (!pNewParent) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityParent' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->SetParent(pNewParent);
}

/**
 * @brief Retrieves the absolute origin of an entity.
 *
 * This function gets the absolute position of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose absolute origin is to be retrieved.
 * @return A vector where the absolute origin will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetEntityAbsOrigin(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityAbsOrigin' on invalid entity handle: %d\n", entityHandle);
		return {};
	}

	const Vector& vec = pEntity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Sets the absolute origin of an entity.
 *
 * This function updates the absolute position of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose absolute origin is to be set.
 * @param origin The new absolute origin to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityAbsOrigin(int entityHandle, const Vector& origin) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityAbsOrigin' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = origin;
}

/**
 * @brief Retrieves the angular rotation of an entity.
 *
 * This function gets the angular rotation of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose angular rotation is to be retrieved.
 * @return A QAngle where the angular rotation will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetEntityAngRotation(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityAngRotation' on invalid entity handle: %d\n", entityHandle);
		return {};
	}

	const QAngle& ang = pEntity->m_CBodyComponent->m_pSceneNode->m_angRotation();
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Sets the angular rotation of an entity.
 *
 * This function updates the angular rotation of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose angular rotation is to be set.
 * @param angle The new angular rotation to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityAngRotation(int entityHandle, const QAngle& angle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityAngRotation' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_CBodyComponent->m_pSceneNode->m_angRotation = angle;
}

/**
 * @brief Retrieves the absolute velocity of an entity.
 *
 * This function gets the absolute velocity of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose absolute velocity is to be retrieved.
 * @return A vector where the absolute velocity will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetEntityAbsVelocity(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityAbsVelocity' on invalid entity handle: %d\n", entityHandle);
		return {};
	}

	const Vector& vec = pEntity->m_vecAbsVelocity();
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Sets the absolute velocity of an entity.
 *
 * This function updates the absolute velocity of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose absolute velocity is to be set.
 * @param velocity The new absolute velocity to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityAbsVelocity(int entityHandle, const Vector& velocity) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityAbsVelocity' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->m_vecAbsVelocity = velocity;
}

/**
 * @brief Retrieves the model name of an entity.
 *
 * This function gets the model name of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose model name is to be retrieved.
 * @return A string where the model name will be stored.
 */
extern "C" PLUGIN_API plg::string GetEntityModel(int entityHandle) {
	CBaseModelEntity* pEntity = static_cast<CBaseModelEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityModel' on invalid entity handle: %d\n", entityHandle);
		return {};
	}

	return pEntity->GetModelName();
}

/**
 * @brief Sets the model name of an entity.
 *
 * This function updates the model name of the specified entity.
 * If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity whose model name is to be set.
 * @param model The new model name to set for the entity.
 */
extern "C" PLUGIN_API void SetEntityModel(int entityHandle, const plg::string& model) {
	CBaseModelEntity* pEntity = static_cast<CBaseModelEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetEntityModel' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->SetModel(model.c_str());
}

/**
 * @brief Retrieves the water level of an entity.
 *
 * This function returns the water level of the specified entity.
 * If the entity is invalid, it returns 0.0f.
 *
 * @param entityHandle The handle of the entity whose water level is to be retrieved.
 * @return The water level of the entity, or 0.0f if the entity is invalid.
 */
extern "C" PLUGIN_API float GetEntityWaterLevel(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityWaterLevel' on invalid entity handle: %d\n", entityHandle);
		return 0.0f;
	}

	return pEntity->m_flWaterLevel();
}

/**
 * @brief Retrieves the ground entity of an entity.
 *
 * This function returns the handle of the ground entity for the specified entity.
 * If the entity is invalid, it returns -1.
 *
 * @param entityHandle The handle of the entity whose ground entity is to be retrieved.
 * @return The handle of the ground entity, or INVALID_EHANDLE_INDEX if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityGroundEntity(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityGroundEntity' on invalid entity handle: %d\n", entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	return pEntity->m_hGroundEntity->GetRefEHandle().ToInt();
}

/**
 * @brief Retrieves the effects of an entity.
 *
 * This function returns the effect flags of the specified entity.
 * If the entity is invalid, it returns 0.
 *
 * @param entityHandle The handle of the entity whose effects are to be retrieved.
 * @return The effect flags of the entity, or 0 if the entity is invalid.
 */
extern "C" PLUGIN_API int GetEntityEffects(int entityHandle) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetEntityEffects' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return pEntity->m_fEffects();
}

/**
 * @brief Teleports an entity to a specified location and orientation.
 *
 * This function teleports the specified entity to the given absolute position,
 * with an optional new orientation and velocity. If the entity is invalid, the function does nothing.
 *
 * @param entityHandle The handle of the entity to teleport.
 * @param origin A pointer to a Vector representing the new absolute position. Can be nullptr.
 * @param angles A pointer to a QAngle representing the new orientation. Can be nullptr.
 * @param velocity A pointer to a Vector representing the new velocity. Can be nullptr.
 */
extern "C" PLUGIN_API void TeleportEntity(int entityHandle, const Vector* origin, const QAngle* angles, const Vector* velocity) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'TeleportEntity' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	pEntity->Teleport(origin, angles, velocity);
}

/**
 * @brief Invokes a named input method on a specified entity.
 *
 * This function triggers an input action on an entity, allowing dynamic interaction
 * between game objects or entities within the system. The input can be initiated
 * by another entity (activator) or by a specific caller entity.
 *
 * @param entityHandle The handle of the target entity that will receive the input.
 * @param inputName    The name of the input action to invoke.
 * @param activatorHandle The handle of the entity that initiated the sequence of actions.
 * @param callerHandle The handle of the entity sending this event. Use -1 to specify
 * @param value        The value associated with the input action.
 * @param type         The type or classification of the value.
* @param outputId      An identifier for tracking the output of this operation.
 */
extern "C" PLUGIN_API void AcceptInput(int entityHandle, const plg::string& inputName, int activatorHandle, int callerHandle, const plg::any& value, FieldType type, int outputId) {
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle)));
	if (!pEntity) {
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'AcceptInput' on invalid entity handle: %d\n", entityHandle);
		return;
	}

	variant_t variant;

	switch (type) {
		case FieldType::Float32:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<float>(v);
			}, value);
			break;
		case FieldType::Float64:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<double>(v);
			}, value);
			break;
		case FieldType::Int32:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<int>(v);
			}, value);
			break;
		case FieldType::UInt32:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<uint>(v);
			}, value);
			break;
		case FieldType::Int64:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<int64>(v);
			}, value);
			break;
		case FieldType::UInt64:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<uint64>(v);
			}, value);
			break;
		case FieldType::Boolean:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<bool>(v);
			}, value);
			break;
		case FieldType::Character:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = static_cast<char>(v);
			}, value);
			break;
		case FieldType::String:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::string>) variant = castable_string_t(v.c_str());
			}, value);
			break;
		case FieldType::CString:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::string>) variant = v.c_str();
			}, value);
			break;
		case FieldType::HScript:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_pointer_v<T>) variant = reinterpret_cast<HSCRIPT>(v);
			}, value);
			break;
		case FieldType::EHandle:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) variant = CEntityHandle((uint32) v);
			}, value);
			break;
		case FieldType::Resource:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_pointer_v<T>) variant = reinterpret_cast<ResourceHandle_t>(v);
			}, value);
			break;
		case FieldType::Vector3d:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec3> || std::is_same_v<T, plg::vec4>) variant = *reinterpret_cast<const Vector*>(&v);
			}, value);
			break;
		case FieldType::Vector2d:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec2> || std::is_same_v<T, plg::vec3> || std::is_same_v<T, plg::vec4>) variant = *reinterpret_cast<const Vector2D*>(&v);
			}, value);
			break;
		case FieldType::Vector4d:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec4>) variant = *reinterpret_cast<const Vector4D*>(&v);
			}, value);
			break;
		case FieldType::Color32:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, int>) variant = *reinterpret_cast<const Color*>(&v);
			}, value);
			break;
		case FieldType::QAngle:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec3> || std::is_same_v<T, plg::vec4>) variant = *reinterpret_cast<const QAngle*>(&v);
			}, value);
			break;
		case FieldType::Quaternion:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec4>) variant = *reinterpret_cast<const Quaternion*>(&v);
			}, value);
			break;
		default:
			plg::visit([&variant](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::string>) {
					variant = v.c_str();
				} else if constexpr (std::is_pointer_v<T>) {
					variant = reinterpret_cast<int64>(v);
				} else if constexpr (std::is_same_v<T, long>) {
					variant = static_cast<int64>(v);
				} else if constexpr (std::is_same_v<T, unsigned long>) {
					variant = static_cast<uint64>(v);
				} else if constexpr (std::is_arithmetic_v<T>) {
					variant = v;
				} else if constexpr (std::is_same_v<T, plg::vec2>) {
					variant = *reinterpret_cast<const Vector2D*>(&v);
				} else if constexpr (std::is_same_v<T, plg::vec3>) {
					variant = *reinterpret_cast<const Vector*>(&v);
				} else if constexpr (std::is_same_v<T, plg::vec4>) {
					variant = *reinterpret_cast<const Vector4D*>(&v);
				}
			}, value);
			break;
	}

	CEntityInstance* pActivator = activatorHandle != INVALID_EHANDLE_INDEX ? g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) activatorHandle)) : nullptr;
	CEntityInstance* pCaller = callerHandle != INVALID_EHANDLE_INDEX ? g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) callerHandle)) : nullptr;

	pEntity->AcceptInput(inputName.c_str(), variant, pActivator, pCaller, outputId);
}

PLUGIFY_WARN_POP()
