#include <core/core_config.h>
#include <core/sdk/entity/cschemasystem.h>
#include <core/sdk/schema.h>
#include <tier0/utlstring.h>
#include <plugin_export.h>

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
	CSchemaSystemTypeScope2* pType = g_pSchemaSystem2->FindTypeScopeForModule(CS2SDK_LIBRARY_PREFIX "server" CS2SDK_LIBRARY_SUFFIX);
	SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className.c_str());
	if (!pClassInfo)
		return -1;

	return pClassInfo->m_nSize;
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API bool GetSchemaBoolByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<bool>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int8_t GetSchemaInt8ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int8_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int16_t GetSchemaInt16ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int16_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int32_t GetSchemaInt32ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int32_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API int64_t GetSchemaInt64ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int64_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint8_t GetSchemaUInt8ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint8_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint16_t GetSchemaUInt16ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint16_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint32_t GetSchemaUInt32ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint32_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API uint64_t GetSchemaUInt64ByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint64_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API float GetSchemaFloatByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<float>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API double GetSchemaDoubleByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<double>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API void* GetSchemaPointerByName(void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<void*>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API void GetSchemaStringByName(plg::string& output, void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	auto str = reinterpret_cast<std::add_pointer_t<CUtlString>>((uintptr_t)(instancePointer) + m_key.offset);
	std::construct_at(&output, str != nullptr ? str->Get() : "");
}

/**
 * @brief Get a value from a schema member by its name.
 *
 * This function retrieves the value of the specified member from the given instance.
 *
 * @param instancePointer Pointer to the instance from which the value is to be retrieved.
 * @param className The name of the class.
 * @param memberName The name of the member whose value is to be retrieved.
 * @return The value of the member.
 */
extern "C" PLUGIN_API void GetSchemaVectorByName(Vector& output, void* instancePointer, const plg::string& className, const plg::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	output = *reinterpret_cast<std::add_pointer_t<Vector>>((uintptr_t)(instancePointer) + m_key.offset);
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueBoolByName(void* instancePointer, const plg::string& className, const plg::string& memberName, bool value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<bool>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueInt8ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, int8_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int8_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueInt16ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, int16_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int16_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueInt32ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, int32_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int32_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueInt64ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, int64_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int64_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt8ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, uint8_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint8_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt16ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, uint16_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint16_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt32ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, uint32_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint32_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueUInt64ByName(void* instancePointer, const plg::string& className, const plg::string& memberName, uint64_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint64_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueFloatByName(void* instancePointer, const plg::string& className, const plg::string& memberName, float value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<float>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueDoubleByName(void* instancePointer, const plg::string& className, const plg::string& memberName, double value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<double>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueStringByName(void* instancePointer, const plg::string& className, const plg::string& memberName, const plg::string& value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<CUtlString*>((uintptr_t)(instancePointer) + m_key.offset) = value.c_str();
}

/**
 * @brief Set a value for a schema member by its name.
 *
 * This function sets the specified member's value for a given instance of a class.
 * If the "FollowCS2ServerGuidelines" option is enabled and the member is in the bad list,
 * a warning is logged and the value is not set.
 *
 * @param instancePointer Pointer to the instance of the class where the value is to be set.
 * @param className The name of the class that contains the member.
 * @param memberName The name of the member to be set.
 * @param value The value to assign to the member.
 */
extern "C" PLUGIN_API void SetSchemaValueVectorByName(void* instancePointer, const plg::string& className, const plg::string& memberName, const Vector& value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<Vector*>((uintptr_t)(instancePointer) + m_key.offset) = value;
}
