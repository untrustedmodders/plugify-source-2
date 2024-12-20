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
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	switch (info[0].size)
	{
		case 24:
			return reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Count();
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	switch (info[0].size)
	{
		case 1:
			return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 2:
			return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 4:
			return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 8:
			return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 24:
			switch (info[1].size)
			{
				case 1:
					return reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				case 2:
					return reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				case 4:
					return reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				case 8:
					return reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				default:
					return 0;
			}
		default:
			return 0;
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (info[0].size)
	{
		case 1:
			*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<int8_t>(value);
		case 2:
			*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<int16_t>(value);
		case 4:
			*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<int32_t>(value);
		case 8:
			*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value;
		case 24:
			switch (info[1].size)
			{
				case 1:
					reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = static_cast<int8_t>(value);
				case 2:
					reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = static_cast<int16_t>(value);
				case 4:
					reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = static_cast<int32_t>(value);
				case 8:
					reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = value;
				default:
					break;
			}
		default:
			break;
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	switch (info[0].size)
	{
		case 4:
			return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 8:
			return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 24:
			switch (info[1].size)
			{
				case 4:
					return reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				case 8:
					return reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				default:
					return 0;
			}
		default:
			return 0;
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (info[0].size)
	{
		case 4:
			*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(pEntity) + offset) = static_cast<float>(value);
		case 8:
			*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value;
		case 24:
			switch (info[1].size)
			{
				case 4:
					reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = static_cast<float>(value);
				case 8:
					reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = value;
				default:
					break;
			}
		default:
			break;
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	switch (info[0].size)
	{
		case 8:
			return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Get();
		case 24:
			switch (info[1].size)
			{
				case 8:
					return reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element).Get();
				default:
					return {};
			}
		default:
			return {};
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (info[0].size)
	{
		case 8:
			*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value.c_str();
		case 24:
			switch (info[1].size)
			{
				case 8:
					reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = value.c_str();
				default:
					break;
			}
		default:
			break;
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	switch (info[0].size)
	{
		case 12:
			return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(pEntity) + offset);
		case 24:
			switch (info[1].size)
			{
				case 12:
					return reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element);
				default:
					return {};
			}
		default:
			return {};
	}
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (info[0].size)
	{
		case 12:
			*reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(pEntity) + offset) = value;
		case 24:
			switch (info[1].size)
			{
				case 12:
					reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = value;
				default:
					break;
			}
		default:
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return INVALID_EHANDLE_INDEX;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return INVALID_EHANDLE_INDEX;
	}

	switch (info[0].size)
	{
		case 4:
			return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(pEntity) + offset)->ToInt();
		case 24:
			switch (info[1].size)
			{
				case 4:
					reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element).ToInt();
				default:
					return INVALID_EHANDLE_INDEX;
			}
		default:
			return INVALID_EHANDLE_INDEX;
	}
}

/**
 * @brief Sets a entity handle in an entity's schema.
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

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	if (changeState && networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
		}
	}

	switch (info[0].size)
	{
		case 4:
			*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(pEntity) + offset) = CEntityHandle((uint32)value);
		case 24:
			switch (info[1].size)
			{
				case 4:
					reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(pEntity) + offset)->Element(element) = CEntityHandle((uint32)value);
				default:
					break;
			}
		default:
			break;
	}
}

/**
 * @brief Updates the networked state of a schema field for a given entity handle.
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
extern "C" PLUGIN_API void NetworkStateChanged(int entityHandle, const plg::string& className, const plg::string& memberName)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot change state of '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}

	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (networked)
	{
		int chainOffset = schema::FindChainOffset(className.c_str());
		if (chainOffset != 0)
		{
			schema::NetworkStateChanged((reinterpret_cast<intptr_t>(pEntity) + chainOffset), offset);
		}
		else
		{
			pEntity->NetworkStateChanged(offset);
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (info[0].size)
	{
		case 24:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (info[0].size)
	{
		case 1:
			return *reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 2:
			return *reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 4:
			return *reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 8:
			return *reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 24:
			switch (info[1].size)
			{
				case 1:
					return reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 2:
					return reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 4:
					return reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 8:
					return reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				default:
					return 0;
			}
		default:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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

	switch (info[0].size)
	{
		case 1:
			*reinterpret_cast<int8_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int8_t>(value);
		case 2:
			*reinterpret_cast<int16_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int16_t>(value);
		case 4:
			*reinterpret_cast<int32_t*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<int32_t>(value);
		case 8:
			*reinterpret_cast<int64_t*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
		case 24:
			switch (info[1].size)
			{
				case 1:
					reinterpret_cast<CUtlVector<int8_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int8_t>(value);
				case 2:
					reinterpret_cast<CUtlVector<int16_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int16_t>(value);
				case 4:
					reinterpret_cast<CUtlVector<int32_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<int32_t>(value);
				case 8:
					reinterpret_cast<CUtlVector<int64_t>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
				default:
					break;
			}
		default:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (info[0].size)
	{
		case 4:
			return *reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 8:
			return *reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 24:
			switch (info[1].size)
			{
				case 4:
					return reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				case 8:
					return reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				default:
					return 0;
			}
		default:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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

	switch (info[0].size)
	{
		case 4:
			*reinterpret_cast<float*>(reinterpret_cast<intptr_t>(entity) + offset) = static_cast<float>(value);
		case 8:
			*reinterpret_cast<double*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
		case 24:
			switch (info[1].size)
			{
				case 4:
					reinterpret_cast<CUtlVector<float>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = static_cast<float>(value);
				case 8:
					reinterpret_cast<CUtlVector<double>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
				default:
					break;
			}
		default:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (info[0].size)
	{
		case 8:
			return reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset)->Get();
		case 24:
			switch (info[1].size)
			{
				case 8:
					return reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element).Get();
				default:
					return {};
			}
		default:
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
extern "C" PLUGIN_API void SetEntSchemaString2(CEntityInstance* entity, const plg::string& className, const plg::string& memberName, const plg::string& value, bool changeState, int element)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && schema::CS2BadList.find(memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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

	switch (info[0].size)
	{
		case 8:
			*reinterpret_cast<CUtlString*>(reinterpret_cast<intptr_t>(entity) + offset) = value.c_str();
		case 24:
			switch (info[1].size)
			{
				case 8:
					reinterpret_cast<CUtlVector<CUtlString>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value.c_str();
				default:
					break;
			}
		default:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (info[0].size)
	{
		case 12:
			return *reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset);
		case 24:
			switch (info[1].size)
			{
				case 12:
					return reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element);
				default:
					return {};
			}
		default:
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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

	switch (info[0].size)
	{
		case 12:
			*reinterpret_cast<plg::vec3*>(reinterpret_cast<intptr_t>(entity) + offset) = value;
		case 24:
			switch (info[1].size)
			{
				case 12:
					reinterpret_cast<CUtlVector<plg::vec3>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = value;
				default:
					break;
			}
		default:
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
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
	if (offset == 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot find offset for '%s::%s' with entity pointer: %p\n", className.c_str(), memberName.c_str(), entity);
		return {};
	}

	switch (info[0].size)
	{
		case 4:
			return reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset)->ToInt();
		case 24:
			switch (info[1].size)
			{
				case 4:
					reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element).ToInt();
				default:
					return INVALID_EHANDLE_INDEX;
			}
		default:
			return INVALID_EHANDLE_INDEX;
	}
}

/**
 * @brief Sets a entity handle in an entity's schema.
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

	const auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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

	switch (info[0].size)
	{
		case 4:
			*reinterpret_cast<CEntityHandle*>(reinterpret_cast<intptr_t>(entity) + offset) = CEntityHandle((uint32)value);
		case 24:
			switch (info[1].size)
			{
				case 4:
					reinterpret_cast<CUtlVector<CEntityHandle>*>(reinterpret_cast<intptr_t>(entity) + offset)->Element(element) = CEntityHandle((uint32)value);
				default:
					break;
			}
		default:
			break;
	}
}

//
//
//
//
//
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

	auto [offset, networked, info] = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);
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

PLUGIFY_WARN_POP()
