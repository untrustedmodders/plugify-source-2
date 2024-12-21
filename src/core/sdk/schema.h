/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2024 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

#if S2SDK_PLATFORM_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4005)
#endif

#include <type_traits>

#if S2SDK_PLATFORM_WINDOWS
#pragma warning(pop)
#endif

#include "virtual.h"
#include <const.h>
#include <entityinstance.h>
#include <smartptr.h>
#include <tier0/dbg.h>
#include <utldelegate.h>
#undef schema

struct SchemaKey
{
	int32 offset;
	bool networked;
	int size;
	CSchemaType* type;
};

struct CNetworkVarChainer : public CSmartPtr<CEntityInstance>
{
	struct ChainUpdatePropagationLL_t
	{
		ChainUpdatePropagationLL_t *pNext;
		CUtlDelegate<void(const CNetworkVarChainer &)> updateDelegate;
	};
	uint8 unk[24];
	ChangeAccessorFieldPathIndex_t m_PathIndex;
	ChainUpdatePropagationLL_t* m_pPropagationList;
};

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline constexpr uint32_t hash_32_fnv1a_const(const char* const str, const uint32_t value = val_32_const) noexcept
{
	return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

inline constexpr uint64_t hash_64_fnv1a_const(const char* const str, const uint64_t value = val_64_const) noexcept
{
	return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}

namespace schema
{
	static std::unordered_set<plg::string> CS2BadList = {
		"m_bIsValveDS",
		"m_bIsQuestEligible",
		"m_iItemDefinitionIndex", // in unmanaged this cannot be set.
		"m_iEntityLevel",
		"m_iItemIDHigh",
		"m_iItemIDLow",
		"m_iAccountID",
		"m_iEntityQuality",

		"m_bInitialized",
		"m_szCustomName",
		"m_iAttributeDefinitionIndex",
		"m_iRawValue32",
		"m_iRawInitialValue32",
		"m_flValue",		// MNetworkAlias "m_iRawValue32"
		"m_flInitialValue", // MNetworkAlias "m_iRawInitialValue32"
		"m_bSetBonus",
		"m_nRefundableCurrency",

		"m_OriginalOwnerXuidLow",
		"m_OriginalOwnerXuidHigh",

		"m_nFallbackPaintKit",
		"m_nFallbackSeed",
		"m_flFallbackWear",
		"m_nFallbackStatTrak",

		"m_iCompetitiveWins",
		"m_iCompetitiveRanking",
		"m_iCompetitiveRankType",
		"m_iCompetitiveRankingPredicted_Win",
		"m_iCompetitiveRankingPredicted_Loss",
		"m_iCompetitiveRankingPredicted_Tie",

		"m_nActiveCoinRank",
		"m_nMusicID",
	};

	enum class ElementType {
		Invalid,
		Single,
		Array,
		Vector
	};

	int32_t FindChainOffset(const char* className);
	SchemaKey GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey);
	void NetworkStateChanged(int64 chainEntity, uint32 nLocalOffset, int nArrayIndex = 0xFFFFFFFF);

	inline std::pair<ElementType, int> IsIntType(CSchemaType* type) {
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
				auto* pElementType = pTypeFixedArray->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_BUILTIN) {
					auto* pElementTypeBuiltin = static_cast<CSchemaType_Builtin*>(pElementType);
					switch (pElementTypeBuiltin->m_eBuiltinType)
					{
						case SCHEMA_BUILTIN_TYPE_BOOL:
							return { ElementType::Array, sizeof(bool) };
						case SCHEMA_BUILTIN_TYPE_CHAR:
							return { ElementType::Array, sizeof(char) };
						case SCHEMA_BUILTIN_TYPE_INT8:
							return { ElementType::Array, sizeof(int8) };
						case SCHEMA_BUILTIN_TYPE_UINT8:
							return { ElementType::Array, sizeof(uint8) };
						case SCHEMA_BUILTIN_TYPE_INT16:
							return { ElementType::Array, sizeof(int16) };
						case SCHEMA_BUILTIN_TYPE_UINT16:
							return { ElementType::Array, sizeof(uint16) };
						case SCHEMA_BUILTIN_TYPE_INT32:
							return { ElementType::Array, sizeof(int32) };
						case SCHEMA_BUILTIN_TYPE_UINT32:
							return { ElementType::Array, sizeof(uint32) };
						case SCHEMA_BUILTIN_TYPE_INT64:
							return { ElementType::Array, sizeof(int64) };
						case SCHEMA_BUILTIN_TYPE_UINT64:
							return { ElementType::Array, sizeof(uint64_t) };
						case SCHEMA_BUILTIN_TYPE_COUNT:
							return { ElementType::Array, sizeof(size_t) };
						default:
							return { ElementType::Invalid, -1 };
					}
				}
				return { ElementType::Invalid, -1 };
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_pszName);
				if (typeClassName.find("UtlVector") != std::string::npos) {
					if (typeClassName.find('*') != std::string::npos) {
						return { ElementType::Vector, sizeof(uint64) };
					} else if (typeClassName.find("int8") != std::string::npos) {
						return { ElementType::Vector, sizeof(int8) };
					} else if (typeClassName.find("int16") != std::string::npos) {
						return { ElementType::Vector, sizeof(int16) };
					} else if (typeClassName.find("int32") != std::string::npos) {
						return { ElementType::Vector, sizeof(int32) };
					} else if (typeClassName.find("int64") != std::string::npos) {
						return { ElementType::Vector, sizeof(int64) };
					} else if (typeClassName.find("uint8") != std::string::npos) {
						return { ElementType::Vector, sizeof(uint8) };
					} else if (typeClassName.find("uint16") != std::string::npos) {
						return { ElementType::Vector, sizeof(uint16) };
					} else if (typeClassName.find("uint32") != std::string::npos) {
						return { ElementType::Vector, sizeof(uint32) };
					} else if (typeClassName.find("uint64") != std::string::npos) {
						return { ElementType::Vector, sizeof(uint64) };
					}
				}
				return { ElementType::Invalid, -1 };
			}
			case SCHEMA_TYPE_BUILTIN:
			{
				auto* pTypeBuiltin = static_cast<CSchemaType_Builtin*>(type);
				switch (pTypeBuiltin->m_eBuiltinType) {
					case SCHEMA_BUILTIN_TYPE_BOOL:
						return { ElementType::Single, sizeof(bool) };
					case SCHEMA_BUILTIN_TYPE_CHAR:
						return { ElementType::Single, sizeof(char) };
					case SCHEMA_BUILTIN_TYPE_INT8:
						return { ElementType::Single, sizeof(int8) };
					case SCHEMA_BUILTIN_TYPE_UINT8:
						return { ElementType::Single, sizeof(uint8) };
					case SCHEMA_BUILTIN_TYPE_INT16:
						return { ElementType::Single, sizeof(int16) };
					case SCHEMA_BUILTIN_TYPE_UINT16:
						return { ElementType::Single, sizeof(uint16) };
					case SCHEMA_BUILTIN_TYPE_INT32:
						return { ElementType::Single, sizeof(int32) };
					case SCHEMA_BUILTIN_TYPE_UINT32:
						return { ElementType::Single, sizeof(uint32) };
					case SCHEMA_BUILTIN_TYPE_INT64:
						return { ElementType::Single, sizeof(int64) };
					case SCHEMA_BUILTIN_TYPE_UINT64:
						return { ElementType::Single, sizeof(uint64_t) };
					case SCHEMA_BUILTIN_TYPE_COUNT:
						return { ElementType::Single, sizeof(size_t) };
					default:
						return { ElementType::Invalid, -1 };
				}
			}
			case SCHEMA_TYPE_POINTER:
			{
				return { ElementType::Single, sizeof(void*) };
			}
		}

		return { ElementType::Invalid, -1 };
	}

	inline std::pair<ElementType, int> IsFloatType(CSchemaType* type) {
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
				auto* pElementType = pTypeFixedArray->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_BUILTIN) {
					auto* pElementTypeBuiltin = static_cast<CSchemaType_Builtin*>(pElementType);
					switch (pElementTypeBuiltin->m_eBuiltinType)
					{
						case SCHEMA_BUILTIN_TYPE_FLOAT32:
							return { ElementType::Array, sizeof(float32) };
						case SCHEMA_BUILTIN_TYPE_FLOAT64:
							return { ElementType::Array, sizeof(float64) };
						default:
							return { ElementType::Invalid, -1 };
					}
				}
				return { ElementType::Invalid, -1 };
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_pszName);
				if (typeClassName.find("UtlVector") != std::string::npos) {
					if (typeClassName.find("float32") != std::string::npos) {
						return { ElementType::Vector, sizeof(float32) };
					} else if (typeClassName.find("float64") != std::string::npos) {
						return { ElementType::Vector, sizeof(float64) };
					}
				}
				return { ElementType::Invalid, -1 };
			}
			case SCHEMA_TYPE_BUILTIN:
			{
				auto* pTypeBuiltin = static_cast<CSchemaType_Builtin*>(type);
				switch (pTypeBuiltin->m_eBuiltinType) {
					case SCHEMA_BUILTIN_TYPE_FLOAT32:
						return { ElementType::Single, sizeof(float32) };
					case SCHEMA_BUILTIN_TYPE_FLOAT64:
						return { ElementType::Single, sizeof(float64) };
					default:
						return { ElementType::Invalid, -1 };
				}
			}
		}

		return { ElementType::Invalid, -1 };
	}

	inline ElementType IsStringType(CSchemaType* type) {
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
				auto* pElementType = pTypeFixedArray->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_DECLARED_CLASS) {
					std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_pszName);
					if (typeClassName.find("CUtlString") != std::string::npos) {
						return ElementType::Array;
					}
				}
				return ElementType::Invalid;
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_pszName);
				if (typeClassName.find("UtlVector") != std::string::npos) {
					if (typeClassName.find("CUtlString") != std::string::npos) {
						return ElementType::Vector;
					}
				} else if (typeClassName.find("CUtlString") != std::string::npos) {
					return ElementType::Single;
				}
				return ElementType::Invalid;
			}
		}

		return ElementType::Invalid;
	}

	inline ElementType IsVectorType(CSchemaType* type) {
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
				auto* pElementType = pTypeFixedArray->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_DECLARED_CLASS) {
					std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_pszName);
					if (typeClassName.find("Vector") != std::string::npos) {
						return ElementType::Array;
					}
				}
				return ElementType::Invalid;
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_pszName);
				if (typeClassName.find("UtlVector") != std::string::npos) {
					if (typeClassName.find("Vector") != std::string::npos) {
						return ElementType::Vector;
					}
				} else if (typeClassName.find("Vector") != std::string::npos) {
					return ElementType::Single;
				}
				return ElementType::Invalid;
			}
		}

		return ElementType::Invalid;
	}

	inline ElementType IsHandleType(CSchemaType* type) {
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pTypeFixedArray = static_cast<CSchemaType_FixedArray*>(type);
				auto* pElementType = pTypeFixedArray->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_DECLARED_CLASS) {
					std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_pszName);
					if (typeClassName.find("CHandle") != std::string::npos) {
						return ElementType::Array;
					}
				}
				return ElementType::Invalid;
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				std::string_view typeClassName(static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_pszName);
				if (typeClassName.find("UtlVector") != std::string::npos) {
					if (typeClassName.find("CHandle") != std::string::npos) {
						return ElementType::Vector;
					}
				} else if (typeClassName.find("CHandle") != std::string::npos) {
					return ElementType::Single;
				}
				return ElementType::Invalid;
			}
		}

		return ElementType::Invalid;
	}

} // namespace schema

class CBaseEntity;

#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)                                                                             \
	class varName##_prop                                                                                                             \
	{                                                                                                                                \
	public:                                                                                                                          \
		std::add_lvalue_reference_t<type> Get()                                                                                      \
		{                                                                                                                            \
			static constexpr auto classKey = hash_32_fnv1a_const(ThisClassName);                                                     \
			static constexpr auto memberName = hash_32_fnv1a_const(#varName);                                                        \
                                                                                                                                     \
			static const auto schemaKey = schema::GetOffset(ThisClassName, classKey, #varName, memberName);                          \
                                                                                                                                     \
			static const size_t offset = offsetof(ThisClass, varName);                                                               \
			ThisClass* pThisClass = (ThisClass*)((byte*)this - offset);                                                              \
                                                                                                                                     \
			return *reinterpret_cast<std::add_pointer_t<type>>(                                                                      \
				(uintptr_t)(pThisClass) + schemaKey.offset + extra_offset);                                                          \
		}                                                                                                                            \
		void Set(type val)                                                                                                           \
		{                                                                                                                            \
			static constexpr auto classKey = hash_32_fnv1a_const(ThisClassName);                                                     \
			static constexpr auto memberName = hash_32_fnv1a_const(#varName);                                                        \
                                                                                                                                     \
			static const auto schemaKey = schema::GetOffset(ThisClassName, classKey, #varName, memberName);                          \
                                                                                                                                     \
			static const size_t offset = offsetof(ThisClass, varName);                                                               \
			ThisClass* pThisClass = (ThisClass*)((byte*)this - offset);                                                              \
                                                                                                                                     \
			if (schemaKey.networked)                                                                                                 \
			{                                                                                                                        \
				static const auto chainOffset = schema::FindChainOffset(ThisClassName);                                              \
				if (chainOffset != 0)                                                                                                \
				{                                                                                                                    \
					DevMsg("Found chain offset %d for %s::%s\n", chainOffset, ThisClassName, #varName);                              \
					schema::NetworkStateChanged((uintptr_t)(pThisClass) + chainOffset, schemaKey.offset + extra_offset, 0xFFFFFFFF); \
				}                                                                                                                    \
				else                                                                                                                 \
				{                                                                                                                    \
					/* WIP: Works fine for most props, but inlined classes in the middle of a class will                             \
						need to have their this pointer corrected by the offset .*/                                                  \
					DevMsg("Attempting to call SetStateChanged on on %s::%s\n", ThisClassName, #varName);                            \
					if (!IsStruct)                                                                                                   \
						((CEntityInstance*)pThisClass)->NetworkStateChanged(schemaKey.offset + extra_offset, -1, -1);                \
					else if (IsPlatformPosix()) /* This is currently broken on windows */                                            \
						CALL_VIRTUAL(void, 1, pThisClass, schemaKey.offset + extra_offset, 0xFFFFFFFF, 0xFFFF);                      \
				}                                                                                                                    \
			}                                                                                                                        \
			*reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)(pThisClass) + schemaKey.offset + extra_offset) = val;            \
		}                                                                                                                            \
		operator std::add_lvalue_reference_t<type>() { return Get(); }                                                               \
		std::add_lvalue_reference_t<type> operator()() { return Get(); }                                                             \
		std::add_lvalue_reference_t<type> operator->() { return Get(); }                                                             \
		varName##_prop& operator()(type val)                                                                                         \
		{                                                                                                                            \
			Set(val);                                                                                                                \
			return *this;                                                                                                            \
		}                                                                                                                            \
		varName##_prop& operator=(type val)                                                                                          \
		{                                                                                                                            \
			Set(val);                                                                                                                \
			return *this;                                                                                                            \
		}                                                                                                                            \
	} varName;

#define SCHEMA_FIELD_POINTER_OFFSET(type, varName, extra_offset)                                            \
	class varName##_prop                                                                                    \
	{                                                                                                       \
	public:                                                                                                 \
		type* Get()                                                                                         \
		{                                                                                                   \
			static constexpr auto classKey = hash_32_fnv1a_const(ThisClassName);                            \
			static constexpr auto memberName = hash_32_fnv1a_const(#varName);                               \
                                                                                                            \
			static const auto schemaKey = schema::GetOffset(ThisClassName, classKey, #varName, memberName); \
                                                                                                            \
			static const size_t offset = offsetof(ThisClass, varName);                                      \
			ThisClass* pThisClass = (ThisClass*)((byte*)this - offset);                                     \
                                                                                                            \
			return reinterpret_cast<std::add_pointer_t<type>>(                                              \
				(uintptr_t)(pThisClass) + schemaKey.offset + extra_offset);                                 \
		}                                                                                                   \
		operator type*() { return Get(); }                                                                  \
		type* operator()() { return Get(); }                                                                \
		type* operator->() { return Get(); }                                                                \
	} varName;

// Use this when you want the member's value itself
#define SCHEMA_FIELD(type, varName) \
	SCHEMA_FIELD_OFFSET(type, varName, 0)

// Use this when you want a pointer to a member
#define SCHEMA_FIELD_POINTER(type, varName) \
	SCHEMA_FIELD_POINTER_OFFSET(type, varName, 0)

#define DECLARE_SCHEMA_CLASS_BASE(className, isStruct)       \
	typedef className ThisClass;                             \
	static constexpr const char* ThisClassName = #className; \
	static constexpr bool IsStruct = isStruct;

#define DECLARE_SCHEMA_CLASS(className) DECLARE_SCHEMA_CLASS_BASE(className, false)

// Use this for classes that can be wholly included within other classes (like CCollisionProperty within CBaseModelEntity)
#define DECLARE_SCHEMA_CLASS_INLINE(className) \
	DECLARE_SCHEMA_CLASS_BASE(className, true)