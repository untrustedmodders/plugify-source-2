#include <core/core_config.hpp>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/schema.h>
#include <core/sdk/utils.h>
#include <schemasystem/schemasystem.h>
#include <plugify/cpp_plugin.hpp>
#include <plugin_export.h>
#include <tier0/utlstring.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Get the offset of a member in a given schema class.
 *
 * This function retrieves the offset of a member in the specified class.
 *
 * @param className The name of the class.
 * @param memberName The name of the member whose offset is to be retrieved.
 * @return The offset of the member in the class.
 */
extern "C" PLUGIN_API int32 GetSchemaOffset(const plg::string& className, const plg::string& memberName) {
	const auto schemaKey = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	return schemaKey.offset;
}

/**
 * @brief Get the offset of a chain in a given schema class.
 *
 * This function retrieves the offset of a chain in the specified class.
 *
 * @param className The name of the class.
 * @return The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API int32 GetSchemaChainOffset(const plg::string& className) {
	return schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
}

/**
 * @brief Check if a schema field is networked.
 *
 * This function checks whether the specified member of the given class is networked.
 *
 * @param className The name of the class.
 * @param memberName The name of the member to check.
 * @return True if the member is networked, false otherwise.
 */
extern "C" PLUGIN_API bool IsSchemaFieldNetworked(const plg::string& className, const plg::string& memberName) {
	const auto schemaKey = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	return schemaKey.networked;
}

/**
 * @brief Get the size of a schema class.
 *
 * This function retrieves the size of the specified class in bytes.
 *
 * @param className The name of the class.
 * @return The size of the class in bytes, or -1 if the class is not found.
 */
extern "C" PLUGIN_API int GetSchemaClassSize(const plg::string& className) {
	CSchemaSystemTypeScope* pType = g_pSchemaSystem->FindTypeScopeForModule(S2SDK_LIBRARY_PREFIX "server" S2SDK_LIBRARY_SUFFIX);
	SchemaMetaInfoHandle_t<CSchemaClassInfo> pClassInfo = pType->FindDeclaredClass(className.c_str());
	if (!pClassInfo) {
		return -1;
	}

	return pClassInfo->m_nSize;
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the integer value at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @return The integer value at the given memory location.
 */
extern "C" PLUGIN_API int64_t GetEntData2(CEntityInstance* entity, int offset, int size) {
	switch (size) {
		case sizeof(int8_t):
			return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case sizeof(int16_t):
			return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case sizeof(int32_t):
			return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case sizeof(int64_t):
			return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		default:
			return 0;
	}
}

/**
 * @brief Peeks into an entity's object data and sets the integer value at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The integer value to set.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntData2(CEntityInstance* entity, int offset, int64_t value, int size, bool changeState, int chainOffset) {
	if (changeState) {
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (size) {
		case sizeof(int8_t):
			*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int8_t>(value);
			break;
		case sizeof(int16_t):
			*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int16_t>(value);
			break;
		case sizeof(int32_t):
			*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(value);
			break;
		case sizeof(int64_t):
			*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
			break;
		default:
			break;
	}
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the float value at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @return The float value at the given memory location.
 */
extern "C" PLUGIN_API double GetEntDataFloat2(CEntityInstance* entity, int offset, int size) {
	switch (size) {
		case sizeof(int32_t):
			return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset);
		case sizeof(int64_t):
			return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset);
		default:
			return 0;
	}
}

/**
 * @brief Peeks into an entity's object data and sets the float value at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The float value to set.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataFloat2(CEntityInstance* entity, int offset, double value, int size, bool changeState, int chainOffset) {
	if (changeState) {
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (size) {
		case sizeof(int32_t):
			*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<float>(value);
			break;
		case sizeof(int64_t):
			*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
			break;
		default:
			break;
	}
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the string value at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @return The string value at the given memory location.
 */
extern "C" PLUGIN_API plg::string GetEntDataString2(CEntityInstance* entity, int offset) {
	return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)->Get();
}

/**
 * @brief Peeks into an entity's object data and sets the string at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The string value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataString2(CEntityInstance* entity, int offset, const plg::string& value, bool changeState, int chainOffset) {
	if (changeState) {
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset) = value.c_str();
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the vector value at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @return The vector value at the given memory location.
 */
extern "C" PLUGIN_API plg::vec3 GetEntDataVector2(CEntityInstance* entity, int offset) {
	return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset);
}

/**
 * @brief Peeks into an entity's object data and sets the vector at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataVector2(CEntityInstance* entity, int offset, const plg::vec3& value, bool changeState, int chainOffset) {
	if (changeState) {
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	*reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
}

//

/**
 * @brief Peeks into an entity's object data and retrieves the entity handle at the given offset.
 *
 * @note This will only work on offsets that are stored as "entity handles" (which usually looks like m_h* in properties)
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @return The entity handle at the given memory location.
 */
extern "C" PLUGIN_API int GetEntDataEnt2(CEntityInstance* entity, int offset) {
	return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)->ToInt();
}

/**
 * @brief Peeks into an entity's object data and sets the entity handle at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The entity handle to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataEnt2(CEntityInstance* entity, int offset, int value, bool changeState, int chainOffset) {
	if (changeState) {
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset) = CEntityHandle((uint32) value);
}

/**
 * @brief Updates the networked state of a schema field for a given entity pointer.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void ChangeEntityState2(CEntityInstance* entity, int offset, int chainOffset) {
	SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
}

//


/**
 * @brief Peeks into an entity's object schema and retrieves the integer value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @return The integer value at the given memory location.
 */
extern "C" PLUGIN_API int64_t GetEntData(int entityHandle, int offset, int size) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot get '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return {};
	}

	return GetEntData2(pEntity, offset, size);
}

/**
 * @brief Peeks into an entity's object data and sets the integer value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The integer value to set.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntData(int entityHandle, int offset, int64_t value, int size, bool changeState, int chainOffset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return;
	}

	SetEntData2(pEntity, offset, value, size, changeState, chainOffset);
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the float value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @return The float value at the given memory location.
 */
extern "C" PLUGIN_API double GetEntDataFloat(int entityHandle, int offset, int size) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot get '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return {};
	}

	return GetEntDataFloat2(pEntity, offset, size);
}

/**
 * @brief Peeks into an entity's object data and sets the float value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The float value to set.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataFloat(int entityHandle, int offset, double value, int size, bool changeState, int chainOffset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return;
	}

	SetEntDataFloat2(pEntity, offset, value, size, changeState, chainOffset);
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the string value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @return The string value at the given memory location.
 */
extern "C" PLUGIN_API plg::string GetEntDataString(int entityHandle, int offset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot get '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return {};
	}

	return GetEntDataString2(pEntity, offset);
}

/**
 * @brief Peeks into an entity's object data and sets the string at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The string value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataString(int entityHandle, int offset, const plg::string& value, bool changeState, int chainOffset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return;
	}

	SetEntDataString2(pEntity, offset, value, changeState, chainOffset);
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the vector value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @return The vector value at the given memory location.
 */
extern "C" PLUGIN_API plg::vec3 GetEntDataVector(int entityHandle, int offset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot get '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return {};
	}

	return GetEntDataVector2(pEntity, offset);
}

/**
 * @brief Peeks into an entity's object data and sets the vector at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataVector(int entityHandle, int offset, const plg::vec3& value, bool changeState, int chainOffset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return;
	}

	SetEntDataVector2(pEntity, offset, value, changeState, chainOffset);
}

//

/**
 * @brief Peeks into an entity's object data and retrieves the entity handle at the given offset.
 *
 * @note This will only work on offsets that are stored as "entity handles" (which usually looks like m_h* in properties)
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @return The entity handle at the given memory location.
 */
extern "C" PLUGIN_API int GetEntDataEnt(int entityHandle, int offset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	return GetEntDataEnt2(pEntity, offset);
}

/**
 * @brief Peeks into an entity's object data and sets the entity handle at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The entity handle to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void SetEntDataEnt(int entityHandle, int offset, int value, bool changeState, int chainOffset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return;
	}

	SetEntDataEnt2(pEntity, offset, value, changeState, chainOffset);
}

/**
 * @brief Updates the networked state of a schema field for a given entity handle.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param chainOffset The offset of the chain entity in the class (-1 for non-entity classes).
 */
extern "C" PLUGIN_API void ChangeEntityState(int entityHandle, int offset, int chainOffset) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot change state of '{}' with invalid entity handle: {}\n", offset, entityHandle);
		return;
	}

	return ChangeEntityState2(pEntity, offset, chainOffset);
}

//

/**
 * @brief Retrieves the count of values that an entity schema's array can store.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @return Size of array (in elements) or 0 if schema is not an array.
 */
extern "C" PLUGIN_API int GetEntSchemaArraySize2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	auto elementType = schema::GetElementType(type);
	switch (elementType) {
		case schema::ElementType::Array:
			return static_cast<CSchemaType_FixedArray*>(type)->m_nElementCount;
		case schema::ElementType::Collection:
			return reinterpret_cast<CUtlVector<intptr_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Count();
		default:
			return 0;
	}
}

//

/**
 * @brief Retrieves an integer value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An integer value at the given schema offset.
 */
extern "C" PLUGIN_API int64_t GetEntSchema2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	const auto [elementType, elementSize] = schema::IsIntType(type);
	switch (elementType) {
		case schema::ElementType::Array:
			switch (elementSize) {
				case sizeof(int8_t):
					return reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case sizeof(int16_t):
					return reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case sizeof(int32_t):
					return reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case sizeof(int64_t):
					return reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				default:
					return 0;
			}
			break;
		case schema::ElementType::Collection:
			switch (elementSize) {
				case sizeof(int8_t):
					return reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case sizeof(int16_t):
					return reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case sizeof(int32_t):
					return reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case sizeof(int64_t):
					return reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				default:
					return 0;
			}
			break;
		case schema::ElementType::Single:
			switch (size) {
				case sizeof(int8_t):
					return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case sizeof(int16_t):
					return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case sizeof(int32_t):
					return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case sizeof(int64_t):
					return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				default:
					return 0;
			}
			break;
		case schema::ElementType::Class:
			return reinterpret_cast<intptr_t>(entity) + offset;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a integer, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return 0;
	}
}

/**
 * @brief Sets an integer value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The integer value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An integer value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchema2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int64_t value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	const auto [elementType, elementSize] = schema::IsIntType(type);
	switch (elementType) {
		case schema::ElementType::Array:
			switch (elementSize) {
				case sizeof(int8_t):
					reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<int8_t>(value);
					break;
				case sizeof(int16_t):
					reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<int16_t>(value);
					break;
				case sizeof(int32_t):
					reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<int32_t>(value);
					break;
				case sizeof(int64_t):
					reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
					break;
				default:
					break;
			}
			break;
		case schema::ElementType::Collection:
			switch (elementSize) {
				case sizeof(int8_t):
					reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int8_t>(value);
					break;
				case sizeof(int16_t):
					reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int16_t>(value);
					break;
				case sizeof(int32_t):
					reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int32_t>(value);
					break;
				case sizeof(int64_t):
					reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
					break;
				default:
					break;
			}
			break;
		case schema::ElementType::Single:
			switch (size) {
				case sizeof(int8_t):
					*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int8_t>(value);
					break;
				case sizeof(int16_t):
					*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int16_t>(value);
					break;
				case sizeof(int32_t):
					*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(value);
					break;
				case sizeof(int64_t):
					*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
					break;
				default:
					break;
			}
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a integer, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Retrieves a float value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A float value at the given schema offset.
 */
extern "C" PLUGIN_API double GetEntSchemaFloat2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	const auto [elementType, elementSize] = schema::IsFloatType(type);
	switch (elementType) {
		case schema::ElementType::Array:
			switch (elementSize) {
				case sizeof(float):
					return reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case sizeof(double):
					return reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				default:
					return 0;
			}
			break;
		case schema::ElementType::Collection:
			switch (elementSize) {
				case sizeof(float):
					return reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case sizeof(double):
					return reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				default:
					return 0;
			}
			break;
		case schema::ElementType::Single:
			switch (size) {
				case sizeof(float):
					return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset);
				case sizeof(double):
					return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset);
				default:
					return 0;
			}
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a float, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return 0;
	}
}

/**
 * @brief Sets a float value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The float value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An float value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaFloat2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, double value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	const auto [elementType, elementSize] = schema::IsFloatType(type);
	switch (elementType) {
		case schema::ElementType::Array:
			switch (elementSize) {
				case sizeof(float):
					reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<float>(value);
					break;
				case sizeof(double):
					reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
					break;
				default:
					break;
			}
			break;
		case schema::ElementType::Collection:
			switch (elementSize) {
				case sizeof(float):
					reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<float>(value);
					break;
				case sizeof(double):
					reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
					break;
				default:
					break;
			}
			break;
		case schema::ElementType::Single:
			switch (size) {
				case sizeof(float):
					*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<float>(value);
					break;
				case sizeof(double):
					*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
					break;
				default:
					break;
			}
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a float, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Retrieves a string value from an entity's schema.
*
* This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API plg::string GetEntSchemaString2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	switch (schema::IsPlainType(type, sizeof(CUtlString))) {
		case schema::ElementType::Array:
			return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)[element].Get();
		case schema::ElementType::Collection:
			return reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element).Get();
		case schema::ElementType::Single:
			return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)->Get();
		default:
			const auto [elementType, elementSize] = schema::IsIntType(type);
			switch (elementType) {
				case schema::ElementType::Array:
					if (elementSize == sizeof(char)) {
						return plg::string(reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset), size - 1);
					}
					break;
				case schema::ElementType::Collection:
					if (elementSize == sizeof(char)) {
						return reinterpret_cast<CUtlVector<char>*>(reinterpret_cast<intptr_t>(entity) + offset)->Base();
					}
					break;
				case schema::ElementType::Single:
					if (size == sizeof(char)) {
						return plg::string(1, *reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset));
					}
					break;
				default:
					break;
			}
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a string, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return {};
	}
}

/**
 * @brief Sets a string value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The string value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An string value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaString2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::string& value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (schema::IsPlainType(type, sizeof(CUtlString))) {
		case schema::ElementType::Array:
			reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value.c_str();
			break;
		case schema::ElementType::Collection:
			reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value.c_str();
			break;
		case schema::ElementType::Single:
			*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset) = value.c_str();
			break;
		default:
			const auto [elementType, elementSize] = schema::IsIntType(type);
			switch (elementType) {
				case schema::ElementType::Array:
					if (elementSize == sizeof(char)) {
						value.copy(reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset), size - 1);
						return;
					}
					break;
				case schema::ElementType::Collection:
					if (elementSize == sizeof(char)) {
						reinterpret_cast<CUtlVector<char>*>(reinterpret_cast<intptr_t>(entity) + offset)->CopyArray(value.data(), static_cast<CUtlVector<char>::IndexType_t>(value.size()));
						return;
					}
					break;
				case schema::ElementType::Single:
					if (size == sizeof(char)) {
						*reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset) = value.empty() ? 0 : *value.begin();
						return;
					}
					break;
				default:
					break;
			}
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a string, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Retrieves a vector value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec3 GetEntSchemaVector3D2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	switch (schema::IsPlainType(type, sizeof(Vector))) {
		case schema::ElementType::Array:
			return reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
		case schema::ElementType::Collection:
			return reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
		case schema::ElementType::Single:
			return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset);
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a vector, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return {};
	}
}

/**
 * @brief Sets a vector value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector3D2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::vec3& value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (schema::IsPlainType(type, sizeof(Vector))) {
		case schema::ElementType::Array:
			reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
			break;
		case schema::ElementType::Collection:
			reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
			break;
		case schema::ElementType::Single:
			*reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a vector, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Retrieves a vector value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec2 GetEntSchemaVector2D2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	switch (schema::IsPlainType(type, sizeof(Vector2D))) {
		case schema::ElementType::Array:
			return reinterpret_cast<plg::vec2*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
		case schema::ElementType::Collection:
			return reinterpret_cast<CUtlVector<plg::vec2>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
		case schema::ElementType::Single:
			return *reinterpret_cast<plg::vec2*>(reinterpret_cast<intptr_t>(entity) + offset);
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a vector, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return {};
	}
}

//

/**
 * @brief Sets a vector value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector2D2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::vec2& value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (schema::IsPlainType(type, sizeof(Vector2D))) {
		case schema::ElementType::Array:
			reinterpret_cast<plg::vec2*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
			break;
		case schema::ElementType::Collection:
			reinterpret_cast<CUtlVector<plg::vec2>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
			break;
		case schema::ElementType::Single:
			*reinterpret_cast<plg::vec2*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a vector, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Retrieves a vector value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec4 GetEntSchemaVector4D2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return {};
	}

	switch (schema::IsPlainType(type, sizeof(Vector4D))) {
		case schema::ElementType::Array:
			return reinterpret_cast<plg::vec4*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
		case schema::ElementType::Collection:
			return reinterpret_cast<CUtlVector<plg::vec4>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
		case schema::ElementType::Single:
			return *reinterpret_cast<plg::vec4*>(reinterpret_cast<intptr_t>(entity) + offset);
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a vector, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return {};
	}
}

/**
 * @brief Sets a vector value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector4D2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::vec4& value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (schema::IsPlainType(type, sizeof(Vector4D))) {
		case schema::ElementType::Array:
			reinterpret_cast<plg::vec4*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
			break;
		case schema::ElementType::Collection:
			reinterpret_cast<CUtlVector<plg::vec4>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
			break;
		case schema::ElementType::Single:
			*reinterpret_cast<plg::vec4*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a vector, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Retrieves an entity handle from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @note This will only work on offsets that are stored as "entity handles" (which usually looks like m_h* in properties)
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API int GetEntSchemaEnt2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return INVALID_EHANDLE_INDEX;
	}

	switch (schema::IsAtomicType(type, sizeof(CEntityHandle))) {
		case schema::ElementType::Array:
			return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)[element].ToInt();
		case schema::ElementType::Collection:
			return reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element).ToInt();
		case schema::ElementType::Single:
			return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)->ToInt();
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a entity handle, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			return INVALID_EHANDLE_INDEX;
	}
}

/**
 * @brief Sets an entity handle in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @note This will only work on offsets that are stored as "entity handles" (which usually looks like m_h* in properties)
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The entity handle to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An entity handle at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaEnt2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int value, bool changeState, int element) {
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.contains(memberName)) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with \"FollowCS2ServerGuidelines\" option enabled.\n", className, memberName);
		return;
	}

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (offset == 0) {
		S2_LOGF(LS_WARNING, "Cannot find offset for '{}::{}' with entity pointer: {}\n", className, memberName, (void*) entity);
		return;
	}

	if (changeState) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	}

	switch (schema::IsAtomicType(type, sizeof(CEntityHandle))) {
		case schema::ElementType::Array:
			reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = CEntityHandle((uint32) value);
			break;
		case schema::ElementType::Collection:
			reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = CEntityHandle((uint32) value);
			break;
		case schema::ElementType::Single:
			*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset) = CEntityHandle((uint32) value);
			break;
		default:
			S2_LOGF(LS_WARNING, "Schema field '{}::{}' is not a entity handle, but '{}'\n", className, memberName, type->m_sTypeName.Get());
			break;
	}
}

//

/**
 * @brief Updates the networked state of a schema field for a given entity pointer.
 *
 * This function ensures that changes to a schema field are appropriately networked,
 * while adhering to specific guidelines if configured. It validates whether the field
 * is networked, computes necessary offsets, and invokes the required methods to mark
 * the state as changed.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 */
extern "C" PLUGIN_API void NetworkStateChanged2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName) {
	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()), memberName.c_str(), hash_32_fnv1a_const(memberName.c_str()));
	if (networked) {
		const auto chainOffset = schema::FindChainOffset(className.c_str(), hash_32_fnv1a_const(className.c_str()));
		SafeNetworkStateChanged(reinterpret_cast<intptr_t>(entity), offset, chainOffset);
	} else {
		S2_LOGF(LS_WARNING, "Field '{}::{}' is not networked, but \"SetStateChanged\" was called on it.", className, memberName);
	}
}

//
//
//
//
//
//


/**
 * @brief Retrieves the count of values that an entity schema's array can store.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @return Size of array (in elements) or 0 if schema is not an array.
 */
extern "C" PLUGIN_API int GetEntSchemaArraySize(int entityHandle, const plg::string& className, const plg::string& memberName) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchemaArraySize2(pEntity, className, memberName);
}

//

/**
 * @brief Retrieves an integer value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An integer value at the given schema offset.
 */
extern "C" PLUGIN_API int64_t GetEntSchema(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchema2(pEntity, className, memberName, element);
}

/**
 * @brief Sets an integer value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The integer value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An integer value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchema(int entityHandle, const plg::string& className, const plg::string& memberName, int64_t value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchema2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Retrieves a float value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A float value at the given schema offset.
 */
extern "C" PLUGIN_API double GetEntSchemaFloat(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchemaFloat2(pEntity, className, memberName, element);
}

/**
 * @brief Sets a float value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The float value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An float value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaFloat(int entityHandle, const plg::string& className, const plg::string& memberName, double value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchemaFloat2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Retrieves a string value from an entity's schema.
*
* This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API plg::string GetEntSchemaString(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchemaString2(pEntity, className, memberName, element);
}

/**
 * @brief Sets a string value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The string value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An string value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaString(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::string& value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchemaString2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Retrieves a vector value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec3 GetEntSchemaVector3D(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchemaVector3D2(pEntity, className, memberName, element);
}

/**
 * @brief Sets a vector value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector3D(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::vec3& value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchemaVector3D2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Retrieves a vector value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec2 GetEntSchemaVector2D(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchemaVector2D2(pEntity, className, memberName, element);
}

/**
 * @brief Sets a vector value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector2D(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::vec2& value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchemaVector2D2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Retrieves a vector value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec4 GetEntSchemaVector4D(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return {};
	}

	return GetEntSchemaVector4D2(pEntity, className, memberName, element);
}

/**
 * @brief Sets a vector value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector4D(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::vec4& value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchemaVector4D2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Retrieves an entity handle from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @note This will only work on offsets that are stored as "entity handles" (which usually looks like m_h* in properties)
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param element Element # (starting from 0) if schema is an array.
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API int GetEntSchemaEnt(int entityHandle, const plg::string& className, const plg::string& memberName, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	return GetEntSchemaEnt2(pEntity, className, memberName, element);
}

/**
 * @brief Sets an entity handle in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @note This will only work on offsets that are stored as "entity handles" (which usually looks like m_h* in properties)
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The entity handle to set.
 * @param changeState If true, change will be sent over the network.
 * @param element Element # (starting from 0) if schema is an array.
 * @return An entity handle at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaEnt(int entityHandle, const plg::string& className, const plg::string& memberName, int value, bool changeState, int element) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot set '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	SetEntSchemaEnt2(pEntity, className, memberName, value, changeState, element);
}

//

/**
 * @brief Updates the networked state of a schema field for a given entity handle.
 *
 * This function ensures that changes to a schema field are appropriately networked,
 * while adhering to specific guidelines if configured. It validates whether the field
 * is networked, computes necessary offsets, and invokes the required methods to mark
 * the state as changed.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 */
extern "C" PLUGIN_API void NetworkStateChanged(int entityHandle, const plg::string& className, const plg::string& memberName) {
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) entityHandle));
	if (!pEntity) {
		S2_LOGF(LS_WARNING, "Cannot change state of '{}::{}' with invalid entity handle: {}\n", className, memberName, entityHandle);
		return;
	}

	NetworkStateChanged2(pEntity, className, memberName);
}

//
//
//
//
//
//

PLUGIFY_WARN_POP()
