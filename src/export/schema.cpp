#include <core/core_config.hpp>
#include <core/sdk/entity/cschemasystem.h>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/schema.h>
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

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return m_key.offset;
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

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return m_key.networked;
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
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API bool GetSchemaBoolByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<bool>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int8_t GetSchemaInt8ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int8_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int16_t GetSchemaInt16ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int16_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int32_t GetSchemaInt32ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int32_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int64_t GetSchemaInt64ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int64_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint8_t GetSchemaUInt8ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint8_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint16_t GetSchemaUInt16ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint16_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint32_t GetSchemaUInt32ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint32_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint64_t GetSchemaUInt64ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint64_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API float GetSchemaFloatByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<float>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API double GetSchemaDoubleByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<double>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API void* GetSchemaPointerByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<void*>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API plg::string GetSchemaStringByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	auto str = reinterpret_cast<std::add_pointer_t<CUtlString>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
	return str != nullptr ? str->Get() : "";
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param entityHandle The handle of the entity from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @param extraOffset The value to add to the offset.
 * @return The value of the member.
 */
extern "C" PLUGIN_API plg::vec3 GetSchemaVectorByName(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot get '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return {};
	}
	
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	const Vector& vec = *reinterpret_cast<std::add_pointer_t<Vector>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset);
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueBoolByName(int entityHandle, const plg::string& className, const plg::string& memberName, bool value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<bool>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueInt8ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int8_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int8_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueInt16ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int16_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int16_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueInt32ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int32_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int32_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueInt64ByName(int entityHandle, const plg::string& className, const plg::string& memberName, int64_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int64_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt8ByName(int entityHandle, const plg::string& className, const plg::string& memberName, uint8_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint8_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt16ByName(int entityHandle, const plg::string& className, const plg::string& memberName, uint16_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint16_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt32ByName(int entityHandle, const plg::string& className, const plg::string& memberName, uint32_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint32_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt64ByName(int entityHandle, const plg::string& className, const plg::string& memberName, uint64_t value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint64_t>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueFloatByName(int entityHandle, const plg::string& className, const plg::string& memberName, float value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<float>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueDoubleByName(int entityHandle, const plg::string& className, const plg::string& memberName, double value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<double>>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueStringByName(int entityHandle, const plg::string& className, const plg::string& memberName, const plg::string& value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<CUtlString*>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value.c_str();
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 * @param extraOffset The value to add to the offset.
 */
extern "C" PLUGIN_API void SetSchemaValueVectorByName(int entityHandle, const plg::string& className, const plg::string& memberName, const Vector& value, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(pEntity) + m_key.offset + extraOffset) = value;
}

/**
 * @brief Updates the networked state of a schema field for a given entity instance.
 *
 * This function ensures that changes to a schema field are appropriately networked,
 * while adhering to specific guidelines if configured. It validates whether the field
 * is networked, computes necessary offsets, and invokes the required methods to mark
 * the state as changed.
 *
 * @param entityHandle The handle of the entity where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param extraOffset The value to add to the offset.
 *
 * @note If the "FollowCS2ServerGuidelines" configuration option is enabled, certain fields
 *       may be restricted from being marked as changed. An appropriate warning is logged in such cases.
 *
 * @warning Logs a warning if the field is not networked or if it is in the restricted list
 *          when "FollowCS2ServerGuidelines" is enabled.
 *
 * @details
 * - Computes a hash key for the class and member names to locate the field.
 * - Verifies whether the field is networked before proceeding.
 * - Calculates chain offsets to determine the correct memory location.
 * - Calls appropriate methods to mark the state as changed and update network-related
 *   fields for the entity instance.
 */
extern "C" PLUGIN_API void SetSchemaStateChanged(int entityHandle, const plg::string& className, const plg::string& memberName, int extraOffset)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with invalid entity handle: %d\n", className.c_str(), memberName.c_str(), entityHandle);
		return;
	}
	
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	if (!m_key.networked)
	{
		g_Logger.LogFormat(LS_WARNING, "Field '%s::%s' is not networked, but \"SetStateChanged\" was called on it.", className.c_str(), memberName.c_str());
		return;
	}

	int chainOffset = schema::FindChainOffset(className.c_str());

	if (chainOffset != 0)
	{
		schema::NetworkStateChanged(((int64_t)(pEntity) + m_key.offset), m_key.offset + extraOffset, 0xFFFFFFFF);
		return;
	}

	pEntity->NetworkStateChanged(m_key.offset + extraOffset);
}

PLUGIFY_WARN_POP()
