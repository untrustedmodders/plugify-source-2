#pragma once

// TODO: Add Vector and QAngle

#include <core/core_config.h>
#include <core/sdk/cschemasystem.h>
#include <utils/schema.h>
#include <tier0/utlstring.h>

extern "C" PLUGIN_API int32 GetSchemaOffset(const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return m_key.offset;
}

extern "C" PLUGIN_API bool IsSchemaFieldNetworked(const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return m_key.networked;
}

extern "C" PLUGIN_API int GetSchemaClassSize(const std::string& className)
{
	CSchemaSystemTypeScope* pType = g_pSchemaSystem2->FindTypeScopeForModule(BINARY_MODULE_PREFIX "server" BINARY_MODULE_SUFFIX);
	SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className.c_str());
	if (!pClassInfo)
		return -1;

	return pClassInfo->GetSize();
}

extern "C" PLUGIN_API bool GetSchemaBoolByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<bool>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API int8_t GetSchemaInt8ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int8_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API int16_t GetSchemaInt16ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int16_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API int32_t GetSchemaInt32ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int32_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API int64_t GetSchemaInt64ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<int64_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API uint8_t GetSchemaUInt8ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint8_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API uint16_t GetSchemaUInt16ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint16_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API uint32_t GetSchemaUInt32ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint32_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API uint64_t GetSchemaUInt64ByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<uint64_t>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API float GetSchemaFloatByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<float>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API double GetSchemaDoubleByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<double>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API void* GetSchemaPointerByName(void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	return *reinterpret_cast<std::add_pointer_t<void*>>((uintptr_t)(instancePointer) + m_key.offset);
}

extern "C" PLUGIN_API void GetSchemaStringByName(std::string& output, void* instancePointer, const std::string& className, const std::string& memberName)
{
	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	auto str = reinterpret_cast<std::add_pointer_t<CUtlString>>((uintptr_t)(instancePointer) + m_key.offset);
	output = str != nullptr ? str->Get() : "";
}

extern "C" PLUGIN_API void SetSchemaValueBoolByName(void* instancePointer, const std::string& className, const std::string& memberName, bool value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<bool>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueInt8ByName(void* instancePointer, const std::string& className, const std::string& memberName, int8_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int8_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueInt16ByName(void* instancePointer, const std::string& className, const std::string& memberName, int16_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int16_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueInt32ByName(void* instancePointer, const std::string& className, const std::string& memberName, int32_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int32_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueInt64ByName(void* instancePointer, const std::string& className, const std::string& memberName, int64_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<int64_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueUInt8ByName(void* instancePointer, const std::string& className, const std::string& memberName, uint8_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint8_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueUInt16ByName(void* instancePointer, const std::string& className, const std::string& memberName, uint16_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint16_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueUInt32ByName(void* instancePointer, const std::string& className, const std::string& memberName, uint32_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint32_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueUInt64ByName(void* instancePointer, const std::string& className, const std::string& memberName, uint64_t value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<uint64_t>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueFloatByName(void* instancePointer, const std::string& className, const std::string& memberName, float value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<float>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueDoubleByName(void* instancePointer, const std::string& className, const std::string& memberName, double value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<std::add_pointer_t<double>>((uintptr_t)(instancePointer) + m_key.offset) = value;
}

extern "C" PLUGIN_API void SetSchemaValueStringByName(void* instancePointer, const std::string& className, const std::string& memberName, const std::string& value)
{
	if (g_pCoreConfig->FollowCS2ServerGuidelines && std::find(schema::CS2BadList.begin(), schema::CS2BadList.end(), memberName) != schema::CS2BadList.end())
	{
		g_Logger.ErrorFormat("Cannot set '%s::%s' with \"FollowCS2ServerGuidelines\" option enabled.\n", className.c_str(), memberName.c_str());
		return;
	}

	auto classKey = hash_32_fnv1a_const(className.c_str());
	auto memberKey = hash_32_fnv1a_const(memberName.c_str());

	const auto m_key = schema::GetOffset(className.c_str(), classKey, memberName.c_str(), memberKey);

	*reinterpret_cast<CUtlString*>((uintptr_t)(instancePointer) + m_key.offset) = value.c_str();
}
