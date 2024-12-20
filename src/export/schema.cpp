#include <core/sdk/entity/cschemasystem.h>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/schema.h>
#include <core/sdk/utils.h>
#include <core/core_config.hpp>
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
extern "C" PLUGIN_API int32 GetSchemaOffset(const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto schemaKey = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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
extern "C" PLUGIN_API int32 GetSchemaChainOffset(const plg::string& className)
{
	return schema::FindChainOffset(className.c_str());
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
extern "C" PLUGIN_API bool IsSchemaFieldNetworked(const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto schemaKey = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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
extern "C" PLUGIN_API int GetSchemaClassSize(const plg::string& className)
{
	CSchemaSystemTypeScope2* pType = g_pSchemaSystem2->FindTypeScopeForModule(S2SDK_LIBRARY_PREFIX "server" S2SDK_LIBRARY_SUFFIX);
	SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className.c_str());
	if (!pClassInfo)
	{
		return -1;
	}

	return pClassInfo->m_nSize;
}

/**
 * @brief Peeks into an entity's object schema and retrieves the integer value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @return The integer value at the given memory location.
 */
extern "C" PLUGIN_API int64_t GetEntData(int entityHandle, int offset, int size)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return {};
	}

	switch (size) {
		case 1:
			return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 2:
			return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 4:
			return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 8:
			return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		default:
			return 0;
	}
}

/**
 * @brief Peeks into an entity's object data and sets the integer value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The integer value to set.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntData(int entityHandle, int offset, int64_t value, int size, bool changeState, int chainOffset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return;
	}

	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(pEntity) + chainOffset, offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (size) {
		case 1:
			*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<int8_t>(value);
		case 2:
			*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<int16_t>(value);
		case 4:
			*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<int32_t>(value);
		case 8:
			*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value;
		default:
			break;
	}
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
extern "C" PLUGIN_API double GetEntDataFloat(int entityHandle, int offset, int size)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return {};
	}

	switch (size) {
		case 4:
			return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 8:
			return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		default:
			return 0;
	}
}

/**
 * @brief Peeks into an entity's object data and sets the float value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The float value to set.
 * @param size Number of bytes to write (valid values are 1, 2, 4 or 8).
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataFloat(int entityHandle, int offset, double value, int size, bool changeState, int chainOffset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return;
	}

	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(pEntity) + chainOffset, offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (size) {
		case 4:
			*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<float>(value);
		case 8:
			*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value;
		default:
			break;
	}
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the string value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @return The string value at the given memory location.
 */
extern "C" PLUGIN_API plg::string GetEntDataString(int entityHandle, int offset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return {};
	}

	return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Get();
}

/**
 * @brief Peeks into an entity's object data and sets the string at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The string value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataString(int entityHandle, int offset, const plg::string& value, bool changeState, int chainOffset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return;
	}

	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(pEntity) + chainOffset, offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value.c_str();
}

//

/**
 * @brief Peeks into an entity's object schema and retrieves the vector value at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @return The vector value at the given memory location.
 */
extern "C" PLUGIN_API plg::vec3 GetEntDataVector(int entityHandle, int offset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return {};
	}

	return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(pEntity) + offset);
}

/**
 * @brief Peeks into an entity's object data and sets the vector at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataVector(int entityHandle, int offset, const plg::vec3& value, bool changeState, int chainOffset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return;
	}

	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(pEntity) + chainOffset, offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	*reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value;
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
extern "C" PLUGIN_API int GetEntDataEnt(int entityHandle, int offset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(pEntity) + offset)->ToInt();
}

/**
 * @brief Peeks into an entity's object data and sets the entity handle at the given offset.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param value The entity handle to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataEnt(int entityHandle, int offset, int value, bool changeState, int chainOffset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return;
	}

	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(pEntity) + chainOffset, offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(pEntity) + offset) = CEntityHandle((uint32)value);
}

/**
 * @brief Updates the networked state of a schema field for a given entity handle.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param offset The offset of the schema to use.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void ChangeEntityState(int entityHandle, int offset, int chainOffset)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot change state of '%d' with invalid entity handle: %d\n", offset, entityHandle);
		return;
	}

	if (chainOffset != 0)
	{
		schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
	}
	else
	{
		pEntity->NetworkStateChanged(offset);
	}
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
extern "C" PLUGIN_API int64_t GetEntData2(CEntityInstance* entity, int offset, int size)
{
	switch (size) {
		case 1:
			return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 2:
			return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 4:
			return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 8:
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
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntData2(CEntityInstance* entity, int offset, int64_t value, int size, bool changeState, int chainOffset)
{
	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(entity) + chainOffset, offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	switch (size) {
		case 1:
			*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int8_t>(value);
		case 2:
			*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int16_t>(value);
		case 4:
			*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(value);
		case 8:
			*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
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
extern "C" PLUGIN_API double GetEntDataFloat2(CEntityInstance* entity, int offset, int size)
{
	switch (size) {
		case 4:
			return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 8:
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
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataFloat2(CEntityInstance* entity, int offset, double value, int size, bool changeState, int chainOffset)
{
	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(entity) + chainOffset, offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	switch (size) {
		case 4:
			*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<float>(value);
		case 8:
			*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
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
extern "C" PLUGIN_API plg::string GetEntDataString2(CEntityInstance* entity, int offset)
{
	return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)->Get();
}

/**
 * @brief Peeks into an entity's object data and sets the string at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The string value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataString2(CEntityInstance* entity, int offset, const plg::string& value, bool changeState, int chainOffset)
{
	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(entity) + chainOffset, offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
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
extern "C" PLUGIN_API plg::vec3 GetEntDataVector2(CEntityInstance* entity, int offset)
{
	return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset);
}

/**
 * @brief Peeks into an entity's object data and sets the vector at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The vector value to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataVector2(CEntityInstance* entity, int offset, const plg::vec3& value, bool changeState, int chainOffset)
{
	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(entity) + chainOffset, offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
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
extern "C" PLUGIN_API int GetEntDataEnt2(CEntityInstance* entity, int offset)
{
	return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)->ToInt();
}

/**
 * @brief Peeks into an entity's object data and sets the entity handle at the given offset.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param value The entity handle to set.
 * @param changeState If true, change will be sent over the network.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void SetEntDataEnt2(CEntityInstance* entity, int offset, int value, bool changeState, int chainOffset)
{
	if (changeState)
	{
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged(reinterpret_cast<intptr_t>(entity) + chainOffset, offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset) = CEntityHandle((uint32)value);
}

/**
 * @brief Updates the networked state of a schema field for a given entity pointer.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param offset The offset of the schema to use.
 * @param chainOffset The offset of the chain entity in the class.
 */
extern "C" PLUGIN_API void ChangeEntityState2(CEntityInstance* entity, int offset, int chainOffset)
{
	if (chainOffset != 0)
	{
		schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
	}
	else
	{
		entity->NetworkStateChanged(offset);
	}
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
extern "C" PLUGIN_API int GetEntSchemaArraySize2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	auto [elementType, _] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			return static_cast<CSchemaType_FixedArray*>(type)->m_nElementCount;
		case schema::ElementType::Vector:
			return reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Count();
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
extern "C" PLUGIN_API int64_t GetEntSchema2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			switch (elementSize)
			{
				case 1:
					return reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case 2:
					return reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case 4:
					return reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case 8:
					return reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
			}
			return 0;
		case schema::ElementType::Vector:
			switch (elementSize)
			{
				case 1:
					return reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 2:
					return reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 4:
					return reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 8:
					return reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
			}
			return 0;
		default:
			switch (size)
			{
				case 1:
					return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 2:
					return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 4:
					return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 8:
					return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
			}
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
extern "C" PLUGIN_API void SetEntSchema2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int64_t value, bool changeState, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			switch (elementSize)
			{
				case 1:
					reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<int8_t>(value);
					break;
				case 2:
					reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<int16_t>(value);
					break;
				case 4:
					reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<int32_t>(value);
					break;
				case 8:
					reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
					break;
			}
			break;
		case schema::ElementType::Vector:
			switch (elementSize)
			{
				case 1:
					reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int8_t>(value);
					break;
				case 2:
					reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int16_t>(value);
					break;
				case 4:
					reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int32_t>(value);
					break;
				case 8:
					reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
					break;
			}
			break;
		default:
			switch (size)
			{
				case 1:
					*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int8_t>(value);
					break;
				case 2:
					*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int16_t>(value);
					break;
				case 4:
					*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(value);
					break;
				case 8:
					*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
					break;
			}
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
extern "C" PLUGIN_API double GetEntSchemaFloat2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			switch (elementSize)
			{
				case 4:
					return reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
				case 8:
					return reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
			}
			return 0;
		case schema::ElementType::Vector:
			switch (elementSize)
			{
				case 4:
					return reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 8:
					return reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
			}
			return 0;
		default:
			switch (size)
			{
				case 4:
					return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 8:
					return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset);
			}
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
extern "C" PLUGIN_API void SetEntSchemaFloat2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, double value, bool changeState, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			switch (elementSize)
			{
				case 4:
					reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = static_cast<float>(value);
					break;
				case 8:
					reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
					break;
			}
			break;
		case schema::ElementType::Vector:
			switch (elementSize)
			{
				case 4:
					reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<float>(value);
					break;
				case 8:
					reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
					break;
			}
			break;
		default:
			switch (size)
			{
				case 4:
					*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<float>(value);
					break;
				case 8:
					*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
					break;
			}
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
extern "C" PLUGIN_API plg::string GetEntSchemaString2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)[element].Get();
		case schema::ElementType::Vector:
			return reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element).Get();
		default:
			return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)->Get();
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
extern "C" PLUGIN_API void SetEntSchemaString2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::string& value, bool changeState, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value.c_str();
			break;
		case schema::ElementType::Vector:
			reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value.c_str();
			break;
		default:
			*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset) = value.c_str();
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
 * @return A string value at the given schema offset.
 */
extern "C" PLUGIN_API plg::vec3 GetEntSchemaVector2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			return reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset)[element];
		case schema::ElementType::Vector:
			return reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
		default:
			return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset);
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
 * @return An vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::vec3& value, bool changeState, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = value;
			break;
		case schema::ElementType::Vector:
			reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
			break;
		default:
			*reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
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
extern "C" PLUGIN_API int GetEntSchemaEnt2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return INVALID_EHANDLE_INDEX;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return INVALID_EHANDLE_INDEX;
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)[element].ToInt();
		case schema::ElementType::Vector:
			return reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element).ToInt();
		default:
			return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)->ToInt();
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
extern "C" PLUGIN_API void SetEntSchemaEnt2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, int value, bool changeState, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	auto [elementType, elementSize] = schema::GetElementType(type);
	switch (elementType)
	{
		case schema::ElementType::Array:
			reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)[element] = CEntityHandle((uint32)value);
			break;
		case schema::ElementType::Vector:
			reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = CEntityHandle((uint32)value);
			break;
		default:
			*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset) = CEntityHandle((uint32)value);
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
extern "C" PLUGIN_API void NetworkStateChanged2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}
	else
	{
		g_Logger.LogFormat(LS_WARNING, "Field '%s::%s' is not networked, but \"SetStateChanged\" was called on it.", className.c_str(), memberName.c_str());
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
extern "C" PLUGIN_API int GetEntSchemaArraySize(int entityHandle, const plg::string& className, const plg::string& memberName)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API int64_t GetEntSchema(int entityHandle, const plg::string& className, const plg::string& memberName, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API void SetEntSchema(int entityHandle, const plg::string& className, const plg::string& memberName, int64_t value, bool changeState, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API double GetEntSchemaFloat(int entityHandle, const plg::string& className, const plg::string& memberName, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API void SetEntSchemaFloat(int entityHandle, const plg::string& className, const plg::string& memberName, double value, bool changeState, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API plg::string GetEntSchemaString(int entityHandle, const plg::string& className, const plg::string& memberName, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API void SetEntSchemaString(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::string& value, bool changeState, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API plg::vec3 GetEntSchemaVector(int entityHandle, const plg::string& className, const plg::string& memberName, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	return GetEntSchemaVector2(pEntity, className, memberName, element);
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
 * @return An vector value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaVector(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::vec3& value, bool changeState, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	SetEntSchemaVector2(pEntity, className, memberName, value, changeState, element);
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
extern "C" PLUGIN_API int GetEntSchemaEnt(int entityHandle, const plg::string& className, const plg::string& memberName, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API void SetEntSchemaEnt(int entityHandle, const plg::string& className, const plg::string& memberName, int value, bool changeState, int element)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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
extern "C" PLUGIN_API void NetworkStateChanged(int entityHandle, const plg::string& className, const plg::string& memberName)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot change state of '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
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

/**
 * @brief Retrieves a value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @return A value at the given schema offset.
 */
extern "C" PLUGIN_API plg::any GetEntSchemaAny2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (type->m_eTypeCategory)
	{
		case SCHEMA_TYPE_BUILTIN:
		{
			auto* pTypeBuiltin = static_cast<CSchemaType_Builtin*>(type);
			switch (pTypeBuiltin->m_eBuiltinType)
			{
				case SCHEMA_BUILTIN_TYPE_CHAR:
					return *reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_INT8:
					return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_UINT8:
					return *reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_INT16:
					return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_UINT16:
					return *reinterpret_cast<uint16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_INT32:
					return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_UINT32:
					return *reinterpret_cast<uint32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_INT64:
					return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_UINT64:
					return *reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_FLOAT32:
					return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_FLOAT64:
					return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_BOOL:
					return *reinterpret_cast<bool*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_COUNT:
					return *reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case SCHEMA_BUILTIN_TYPE_VOID:
					return plg::none{};
				case SCHEMA_BUILTIN_TYPE_INVALID:
					return {};
			}
			break;
		}
		case SCHEMA_TYPE_POINTER:
		{
			return *reinterpret_cast<void**>(reinterpret_cast<intptr_t>(entity) + offset);
		}
		case SCHEMA_TYPE_BITFIELD:
		{
			g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_BITFIELD not supported!\n");
			return {};
		}
		case SCHEMA_TYPE_FIXED_ARRAY:
		{
			auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
			auto* pElementType = pTypeFixedArray->m_pElementType;
			switch (pElementType->m_eTypeCategory)
			{
				case SCHEMA_TYPE_BUILTIN:
				{
					auto* pElementTypeBuiltin = static_cast<CSchemaType_Builtin*>(pElementType);
					switch (pElementTypeBuiltin->m_eBuiltinType)
					{
						case SCHEMA_BUILTIN_TYPE_CHAR:
							return plg::vector<char>(reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_INT8:
							return plg::vector<int8_t>(reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_UINT8:
							return plg::vector<uint8_t>(reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_INT16:
							return plg::vector<int16_t>(reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_UINT16:
							return plg::vector<uint16_t>(reinterpret_cast<uint16_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<uint16_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_INT32:
							return plg::vector<int32_t>(reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_UINT32:
							return plg::vector<uint32_t>(reinterpret_cast<uint32_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<uint32_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_INT64:
							return plg::vector<int64_t>(reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_UINT64:
							return plg::vector<uint64_t>(reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_FLOAT32:
							return plg::vector<float>(reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_FLOAT64:
							return plg::vector<double>(reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_BOOL:
							return plg::vector<bool>(reinterpret_cast<bool*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<bool*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_COUNT:
							return plg::vector<uint64_t>(reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case SCHEMA_BUILTIN_TYPE_VOID:
							return plg::none{};
						case SCHEMA_BUILTIN_TYPE_INVALID:
							return {};
					}
					break;
				}
				case SCHEMA_TYPE_POINTER:
					return plg::vector<void*>(reinterpret_cast<void**>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<void**>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
				case SCHEMA_TYPE_BITFIELD:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_BITFIELD> not supported!\n");
					return {};
				case SCHEMA_TYPE_FIXED_ARRAY:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_FIXED_ARRAY> not supported!\n");
					return {};
				case SCHEMA_TYPE_ATOMIC:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_ATOMIC> not supported!\n");
					return {};
				case SCHEMA_TYPE_DECLARED_CLASS:
				{
					auto* pTypeDeclaredClass = static_cast<CSchemaType_DeclaredClass*>(pElementType);
					std::string_view typeClassName(pTypeDeclaredClass->m_pClassInfo->m_pszName);
					if (typeClassName.find("CUtlString") != std::string::npos) {
						plg::vector<plg::string> ret;
						ret.reserve(static_cast<size_t>(pTypeFixedArray->m_nElementCount));
						for (int i = 0; i < pTypeFixedArray->m_nElementCount; ++i) {
							ret.emplace_back(reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset + (i * pTypeFixedArray->m_nElementSize))->Get());
						}
						return ret;
					} else if (typeClassName.find("CUtlSymbolLarge") != std::string::npos) {
						plg::vector<plg::string> ret;
						ret.reserve(static_cast<size_t>(pTypeFixedArray->m_nElementCount));
						for (int i = 0; i < pTypeFixedArray->m_nElementCount; ++i) {
							ret.emplace_back(reinterpret_cast<CUtlSymbolLarge*>(reinterpret_cast<intptr_t>(entity) + offset + (i * pTypeFixedArray->m_nElementSize))->String());
						}
						return ret;
					} else if (typeClassName.find("CHandle") != std::string::npos) {
						plg::vector<int32_t> ret;
						ret.reserve(static_cast<size_t>(pTypeFixedArray->m_nElementCount));
						for (int i = 0; i < pTypeFixedArray->m_nElementCount; ++i) {
							ret.emplace_back(reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset + (i * pTypeFixedArray->m_nElementSize))->ToInt());
						}
						return ret;
					} else {
						g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_DECLARED_CLASS(%s)> not supported!\n", typeClassName);
					}
					return {};
				}
				case SCHEMA_TYPE_DECLARED_ENUM:
				{
					auto* pElementTypeDeclaredEnum = static_cast<CSchemaType_DeclaredEnum*>(pElementType);
					switch (pElementTypeDeclaredEnum->m_pEnumInfo->m_nSize)
					{
						case 1:
							return plg::vector<int8_t>(reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case 2:
							return plg::vector<int16_t>(reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case 4:
							return plg::vector<int32_t>(reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						case 8:
							return plg::vector<int64_t>(reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset), reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset + (pTypeFixedArray->m_nElementCount * pTypeFixedArray->m_nElementSize)));
						default:
							g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_DECLARED_ENUM> size not supported!\n");
							return 0;
					}
					break;
				}
				default:
					break;
			}
			break;
		}
		case SCHEMA_TYPE_ATOMIC:
		{
			g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_ATOMIC not supported!\n");
			return {};
		}
		case SCHEMA_TYPE_DECLARED_CLASS:
		{
			auto* pTypeDeclaredClass = static_cast<CSchemaType_DeclaredClass*>(type);
			std::string_view typeClassName(pTypeDeclaredClass->m_pClassInfo->m_pszName);
			if (typeClassName.find("UtlVector") != std::string::npos) {
				if (typeClassName.find("CHandle") != std::string::npos) {
					plg::vector<int32_t> ret;
					auto vec = reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset);
					ret.reserve(static_cast<size_t>(vec->Count()));
					for (int i = 0; i < vec->Count(); ++i) {
						ret.emplace_back(vec->Element(i).ToInt());
					}
					return ret;
				} else if (typeClassName.find("CUtlString") != std::string::npos) {
					plg::vector<plg::string> ret;
					auto vec = reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset);
					ret.reserve(static_cast<size_t>(vec->Count()));
					for (int i = 0; i < vec->Count(); ++i) {
						ret.emplace_back(vec->Element(i).Get());
					}
					return ret;
				} else if (typeClassName.find("CUtlSymbolLarge") != std::string::npos) {
					plg::vector<plg::string> ret;
					auto vec = reinterpret_cast<CUtlVector<CUtlSymbolLarge>*>(reinterpret_cast<intptr_t>(entity) + offset);
					ret.reserve(static_cast<size_t>(vec->Count()));
					for (int i = 0; i < vec->Count(); ++i) {
						ret.emplace_back(vec->Element(i).String());
					}
					// TODO: Add support of all types
					return ret;
				} else {
					g_Logger.LogFormat(LS_WARNING, "UtlVector<SCHEMA_TYPE_DECLARED_CLASS>(%s) not supported!\n", typeClassName);
				}
			} else if (typeClassName.find("CUtlString") != std::string::npos) {
				return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)->Get();
			} else if (typeClassName.find("CUtlSymbolLarge")) {
				return reinterpret_cast<CUtlSymbolLarge*>(reinterpret_cast<intptr_t>(entity) + offset)->String();
			} else if (typeClassName.find("CHandle") != std::string::npos) {
				return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)->ToInt();
			} else {
				g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_DECLARED_CLASS(%s) not supported!\n", typeClassName);
			}
			return {};
		}
		case SCHEMA_TYPE_DECLARED_ENUM:
		{
			auto* pTypeDeclaredEnum = static_cast<CSchemaType_DeclaredEnum*>(type);
			switch (pTypeDeclaredEnum->m_pEnumInfo->m_nSize)
			{
				case 1:
					return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 2:
					return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 4:
					return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				case 8:
					return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
				default:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_DECLARED_ENUM size not supported!\n");
					return 0;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_INVALID not supported!\n");
	return {};
}

/**
 * @brief Sets a value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entity Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The value to set.
 * @param changeState If true, change will be sent over the network.
 * @return A value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaAny2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::any& value, bool changeState)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, size, type]  = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(entity) + chainOffset), offset);
		}
		else
		{
			entity->NetworkStateChanged(offset);
		}
	}

	switch (type->m_eTypeCategory)
	{
		case SCHEMA_TYPE_BUILTIN:
		{
			auto* pTypeBuiltin = static_cast<CSchemaType_Builtin*>(type);
			switch (pTypeBuiltin->m_eBuiltinType)
			{
				case SCHEMA_BUILTIN_TYPE_CHAR:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<char>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_INT8:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int8_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_UINT8:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<uint8_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_INT16:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int16_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_UINT16:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<uint16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<uint16_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_INT32:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_UINT32:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<uint32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<uint32_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_INT64:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int64_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_UINT64:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<uint64_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_FLOAT32:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<float>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_FLOAT64:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<double>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_BOOL:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<bool*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<bool>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_COUNT:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<uint64_t>(v);
						}
					}, value);
					return;
				case SCHEMA_BUILTIN_TYPE_VOID:
					return;
				case SCHEMA_BUILTIN_TYPE_INVALID:
					return;
			}
			break;
		}
		case SCHEMA_TYPE_POINTER:
		{
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) {
					*reinterpret_cast<intptr_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<intptr_t>(v);
				} else if constexpr (std::is_pointer_v<T>) {
					*reinterpret_cast<void**>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<void*>(v);
				}
			}, value);
			return;
		}
		case SCHEMA_TYPE_BITFIELD:
		{
			g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_BITFIELD not supported!\n");
			return;
		}
		case SCHEMA_TYPE_FIXED_ARRAY:
		{
			auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
			auto* pElementType = pTypeFixedArray->m_pElementType;
			switch (pElementType->m_eTypeCategory)
			{
				case SCHEMA_TYPE_BUILTIN:
				{
					auto* pElementTypeBuiltin = static_cast<CSchemaType_Builtin*>(pElementType);
					switch (pElementTypeBuiltin->m_eBuiltinType)
					{
						case SCHEMA_BUILTIN_TYPE_CHAR:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<char*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<char>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_INT8:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int8_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_UINT8:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<uint8_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_INT16:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int16_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_UINT16:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<uint16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<uint16_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_INT32:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int32_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_UINT32:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<uint32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<uint32_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_INT64:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int64_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_UINT64:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<uint64_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_FLOAT32:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<float>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_FLOAT64:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<double>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_BOOL:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<bool*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<bool>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_COUNT:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<uint64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<uint64_t>(v[i]);
									}
								}
							}, value);
							return;
						case SCHEMA_BUILTIN_TYPE_VOID:
							return;
						case SCHEMA_BUILTIN_TYPE_INVALID:
							return;
					}
					break;
				}
				case SCHEMA_TYPE_POINTER:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						 if constexpr (std::is_same_v<T, plg::vector<void*>> || std::is_same_v<T, plg::vector<int64_t>> || std::is_same_v<T, plg::vector<uint64_t>>) {
							for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
							{
								reinterpret_cast<void**>(reinterpret_cast<intptr_t>(entity) + offset)[i] = reinterpret_cast<void*>(v[i]);
							}
						}
					 }, value);
					return;
				case SCHEMA_TYPE_BITFIELD:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_BITFIELD> not supported!\n");
					return;
				case SCHEMA_TYPE_FIXED_ARRAY:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_ATOMIC> not supported!\n");
					return;
				case SCHEMA_TYPE_ATOMIC:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_ATOMIC> not supported!\n");
					return;
				case SCHEMA_TYPE_DECLARED_CLASS:
				{
					auto* pTypeDeclaredClass = static_cast<CSchemaType_DeclaredClass*>(pElementType);
					std::string_view typeClassName(pTypeDeclaredClass->m_pClassInfo->m_pszName);
					if (typeClassName.find("CUtlString") != std::string::npos) {
						plg::visit([&](const auto& v) {
							using T = std::decay_t<decltype(v)>;
							if constexpr (std::is_same_v<T, plg::vector<plg::string>>) {
								for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
								{
									reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = v[i].c_str();
								}
							}
						}, value);
					} else if (typeClassName.find("CUtlSymbolLarge") != std::string::npos) {
						plg::visit([&](const auto& v) {
							using T = std::decay_t<decltype(v)>;
							if constexpr (std::is_same_v<T, plg::vector<plg::string>>) {
								for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
								{
									reinterpret_cast<CUtlSymbolLarge*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = v[i].c_str();
								}
							}
						}, value);
					} else if (typeClassName.find("CHandle") != std::string::npos) {
						plg::visit([&](const auto& v) {
							using T = std::decay_t<decltype(v)>;
							if constexpr (std::is_same_v<T, plg::vector<int32_t>> || std::is_same_v<T, plg::vector<int64_t>> || std::is_same_v<T, plg::vector<uint32_t>> || std::is_same_v<T, plg::vector<uint64_t>>) {
								for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
								{
									reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = CEntityHandle((uint32)v[i]);
								}
							}
						}, value);
					} else {
						g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_DECLARED_CLASS(%s)> not supported!\n", typeClassName);
					}
					return;
				}
				case SCHEMA_TYPE_DECLARED_ENUM:
				{
					auto* pElementTypeDeclaredEnum = static_cast<CSchemaType_DeclaredEnum*>(pElementType);
					switch (pElementTypeDeclaredEnum->m_pEnumInfo->m_nSize)
					{
						case 1:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int8_t>(v[i]);
									}
								}
							}, value);
							return;
						case 2:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int16_t>(v[i]);
									}
								}
							}, value);
							return;

						case 4:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int32_t>(v[i]);
									}
								}
							}, value);
							return;

						case 8:
							plg::visit([&](const auto& v) {
								using T = std::decay_t<decltype(v)>;
								if constexpr (plg::is_vector_v<T> && plg::has_arithmetic_value_type_v<T>) {
									for (size_t i = 0; i < std::min<size_t>(v.size(), static_cast<size_t>(pTypeFixedArray->m_nElementSize)); ++i)
									{
										reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset)[i] = static_cast<int64_t>(v[i]);
									}
								}
							}, value);
							return;
						default:
							g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_FIXED_ARRAY<SCHEMA_TYPE_DECLARED_ENUM> size not supported!\n");
							return;
					}
					break;
				}
				default:
					break;
			}
			break;
		}
		case SCHEMA_TYPE_ATOMIC:
		{
			g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_ATOMIC not supported!\n");
			return;
		}
		case SCHEMA_TYPE_DECLARED_CLASS:
		{
			auto* pTypeDeclaredClass = static_cast<CSchemaType_DeclaredClass*>(type);
			std::string_view typeClassName(pTypeDeclaredClass->m_pClassInfo->m_pszName);
			if (typeClassName.starts_with("CUtlString")) {
				plg::visit([&](const auto& v) {
					using T = std::decay_t<decltype(v)>;
					if constexpr (std::is_same_v<T, plg::string>) {
						*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset) = v.c_str();
					}
				 }, value);
			} else if (typeClassName.starts_with("CUtlSymbolLarge")) {
				plg::visit([&](const auto& v) {
					using T = std::decay_t<decltype(v)>;
					if constexpr (std::is_same_v<T, plg::string>) {
						*reinterpret_cast<CUtlSymbolLarge*>(reinterpret_cast<intptr_t>(entity) + offset) = v.c_str();
					}
				}, value);
			} else if (typeClassName.starts_with("CHandle")) {
				plg::visit([&](const auto& v) {
					using T = std::decay_t<decltype(v)>;
					if constexpr (std::is_arithmetic_v<T>) {
						*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset) = CEntityHandle((uint32)v);
					}
				}, value);
			} else {
				g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_DECLARED_CLASS(%s) not supported!\n", typeClassName);
			}
			return;
		}
		case SCHEMA_TYPE_DECLARED_ENUM:
		{
			auto* pTypeDeclaredEnum = static_cast<CSchemaType_DeclaredEnum*>(type);
			switch (pTypeDeclaredEnum->m_pEnumInfo->m_nSize)
			{
				case 1:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(v);
						}
					}, value);
					return;
				case 2:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(v);
						}
					}, value);
					return;
				case 4:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(v);
						}
					}, value);
					return;
				case 8:
					plg::visit([&](const auto& v) {
						using T = std::decay_t<decltype(v)>;
						if constexpr (std::is_arithmetic_v<T>) {
							*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(v);
						}
					}, value);
					return;
				default:
					g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_DECLARED_ENUM size not supported!\n");
					return;;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	g_Logger.LogFormat(LS_WARNING, "SCHEMA_TYPE_INVALID not supported!\n");
}

/**
 * @brief Retrieves a value from an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @return A value at the given schema offset.
 */
extern "C" PLUGIN_API plg::any GetEntSchemaAny(int entityHandle, const plg::string& className, const plg::string& memberName)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot change state of '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	return GetEntSchemaAny2(pEntity, className, memberName);
}

/**
 * @brief Sets a value in an entity's schema.
 *
 * This function is considered safer and more robust over GetEntData, because it performs strict offset checking and typing rules.
 *
* @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the schema member.
 * @param value The value to set.
 * @param changeState If true, change will be sent over the network.
 * @return A value at the given schema offset.
 */
extern "C" PLUGIN_API void SetEntSchemaAny(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::any& value, bool changeState)
{
	CEntityInstance* pEntity = g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot change state of '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	return SetEntSchemaAny2(pEntity, className, memberName, value, changeState);
}

PLUGIFY_WARN_POP()

