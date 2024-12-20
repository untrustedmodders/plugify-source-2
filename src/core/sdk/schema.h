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

	enum ElementType : int {
		Invalid,
		Single,
		Array,
		Collection,
		Class
	};

	int32_t FindChainOffset(const char* className);
	SchemaKey GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey);
	void NetworkStateChanged(int64 chainEntity, uint32 nLocalOffset, int nArrayIndex = 0xFFFFFFFF);

	inline schema::ElementType GetElementType(CSchemaType* type)
	{
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				return Array;
			}
			case SCHEMA_TYPE_ATOMIC:
			{
				if (type->m_eAtomicCategory == SCHEMA_ATOMIC_COLLECTION_OF_T)
					return Collection;
				else
					return Invalid;
			}
		}
		return Single;
	}

	inline std::pair<ElementType, int> IsIntType(CSchemaType* type)
	{
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				switch (pElementType->m_eTypeCategory)
				{
					case SCHEMA_TYPE_BUILTIN:
					{
						switch (static_cast<CSchemaType_Builtin*>(pElementType)->m_eBuiltinType)
						{
							case SCHEMA_BUILTIN_TYPE_BOOL:
								return { Array, sizeof(bool) };
							case SCHEMA_BUILTIN_TYPE_CHAR:
								return { Array, sizeof(char) };
							case SCHEMA_BUILTIN_TYPE_INT8:
								return { Array, sizeof(int8) };
							case SCHEMA_BUILTIN_TYPE_UINT8:
								return { Array, sizeof(uint8) };
							case SCHEMA_BUILTIN_TYPE_INT16:
								return { Array, sizeof(int16) };
							case SCHEMA_BUILTIN_TYPE_UINT16:
								return { Array, sizeof(uint16) };
							case SCHEMA_BUILTIN_TYPE_INT32:
								return { Array, sizeof(int32) };
							case SCHEMA_BUILTIN_TYPE_UINT32:
								return { Array, sizeof(uint32) };
							case SCHEMA_BUILTIN_TYPE_INT64:
								return { Array, sizeof(int64) };
							case SCHEMA_BUILTIN_TYPE_UINT64:
								return { Array, sizeof(uint64_t) };
							case SCHEMA_BUILTIN_TYPE_COUNT:
								return { Array, sizeof(size_t) };
							default:
								return { Invalid, -1 };
						}
					}
					case SCHEMA_TYPE_DECLARED_CLASS:
					{
						int nSize = static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_nSize;
						if (nSize <= sizeof(double))
						{
							return { Array, nSize };
						}
						return { Invalid, -1 };
					}
					case SCHEMA_TYPE_DECLARED_ENUM:
						return { Array, static_cast<CSchemaType_DeclaredEnum*>(pElementType)->m_pEnumInfo->m_nSize };
					case SCHEMA_TYPE_POINTER:
						return { Array, sizeof(void*) };
					default:
						break;
				}
				return { Invalid, -1 };
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				int nSize = static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_nSize;
				if (nSize <= sizeof(void*))
				{
					return { Single, nSize };
				}
				return { Class, -1 };
			}
			case SCHEMA_TYPE_ATOMIC:
			{
				if (type->m_eAtomicCategory == SCHEMA_ATOMIC_COLLECTION_OF_T)
				{
					switch (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize)
					{
						case sizeof(int8):
							return { Collection, sizeof(int8) };
						case sizeof(int16):
							return { Collection, sizeof(int16) };
						case sizeof(int32):
							return { Collection, sizeof(int32) };
						case sizeof(int64):
							return { Collection, sizeof(int64) };
						default:
							break;
					}
				}
				return { Invalid, -1 };
			}
			case SCHEMA_TYPE_BUILTIN:
			{
				switch (static_cast<CSchemaType_Builtin*>(type)->m_eBuiltinType) {
					case SCHEMA_BUILTIN_TYPE_BOOL:
						return { Single, sizeof(bool) };
					case SCHEMA_BUILTIN_TYPE_CHAR:
						return { Single, sizeof(char) };
					case SCHEMA_BUILTIN_TYPE_INT8:
						return { Single, sizeof(int8) };
					case SCHEMA_BUILTIN_TYPE_UINT8:
						return { Single, sizeof(uint8) };
					case SCHEMA_BUILTIN_TYPE_INT16:
						return { Single, sizeof(int16) };
					case SCHEMA_BUILTIN_TYPE_UINT16:
						return { Single, sizeof(uint16) };
					case SCHEMA_BUILTIN_TYPE_INT32:
						return { Single, sizeof(int32) };
					case SCHEMA_BUILTIN_TYPE_UINT32:
						return { Single, sizeof(uint32) };
					case SCHEMA_BUILTIN_TYPE_INT64:
						return { Single, sizeof(int64) };
					case SCHEMA_BUILTIN_TYPE_UINT64:
						return { Single, sizeof(uint64_t) };
					case SCHEMA_BUILTIN_TYPE_COUNT:
						return { Single, sizeof(size_t) };
					default:
						return { Invalid, -1 };
				}
			}
			case SCHEMA_TYPE_DECLARED_ENUM:
			{
				return { Single, static_cast<CSchemaType_DeclaredEnum*>(type)->m_pEnumInfo->m_nSize };
			}
			case SCHEMA_TYPE_POINTER:
			{
				return { Single, sizeof(void*) };
			}
		}

		return { Invalid, -1 };
	}

	inline std::pair<ElementType, int> IsFloatType(CSchemaType* type)
	{
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				switch (pElementType->m_eTypeCategory)
				{
					case SCHEMA_TYPE_BUILTIN:
					{
						auto* pElementTypeBuiltin = static_cast<CSchemaType_Builtin*>(pElementType);
						switch (pElementTypeBuiltin->m_eBuiltinType)
						{
							case SCHEMA_BUILTIN_TYPE_FLOAT32:
								return { Array, sizeof(float32) };
							case SCHEMA_BUILTIN_TYPE_FLOAT64:
								return { Array, sizeof(float64) };
							default:
								return { Invalid, -1 };
						}
					}
					case SCHEMA_TYPE_DECLARED_CLASS:
					{
						int nSize = static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_nSize;
						if (nSize <= sizeof(double))
						{
							return { Array, nSize };
						}
						return { Invalid, -1 };
					}
					default:
						break;
				}
				return { Invalid, -1 };
			}
			case SCHEMA_TYPE_DECLARED_CLASS:
			{
				int nSize = static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_nSize;
				if (nSize <= sizeof(double))
				{
					return { Single, nSize };
				}
				return { Invalid, -1 };
			}
			case SCHEMA_TYPE_ATOMIC:
			{
				if (type->m_eAtomicCategory == SCHEMA_ATOMIC_COLLECTION_OF_T)
				{
					switch (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize)
					{
						case sizeof(float32):
							return { Collection, sizeof(float32) };
						case sizeof(float64):
							return { Collection, sizeof(float64) };
						default:
							break;
					}
				}
				return { Invalid, -1 };
			}
			case SCHEMA_TYPE_BUILTIN:
			{
				switch (static_cast<CSchemaType_Builtin*>(type)->m_eBuiltinType) {
					case SCHEMA_BUILTIN_TYPE_FLOAT32:
						return { Single, sizeof(float32) };
					case SCHEMA_BUILTIN_TYPE_FLOAT64:
						return { Single, sizeof(float64) };
					default:
						return { Invalid, -1 };
				}
			}
		}

		return { Invalid, -1 };
	}

	inline ElementType IsPlainType(CSchemaType* type, size_t size)
	{
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_ATOMIC && pElementType->m_eAtomicCategory == SCHEMA_ATOMIC_PLAIN) {
					if (static_cast<CSchemaType_Atomic*>(pElementType)->m_nSize == size)
						return Array;
				}
				return Invalid;
			}
			case SCHEMA_TYPE_ATOMIC:
			{
				switch (type->m_eAtomicCategory)
				{
					case SCHEMA_ATOMIC_PLAIN:
						if (static_cast<CSchemaType_Atomic*>(type)->m_nSize == size)
							return Single;
						break;
					case SCHEMA_ATOMIC_COLLECTION_OF_T:
						if (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize == size)
							return Collection;
						break;
				}
				return Invalid;
			}
		}

		return Invalid;
	}

	inline ElementType IsAtomicType(CSchemaType* type, size_t size)
	{
		switch (type->m_eTypeCategory)
		{
			case SCHEMA_TYPE_FIXED_ARRAY:
			{
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_ATOMIC && pElementType->m_eAtomicCategory == SCHEMA_ATOMIC_T) {
					if (static_cast<CSchemaType_Atomic_T*>(pElementType)->m_nSize == size)
						return Array;
				}
				return Invalid;
			}
			case SCHEMA_TYPE_ATOMIC:
			{
				switch (type->m_eAtomicCategory)
				{
					case SCHEMA_ATOMIC_T:
						if (static_cast<CSchemaType_Atomic_T*>(type)->m_nSize == size)
							return Single;
						break;
					case SCHEMA_ATOMIC_COLLECTION_OF_T:
						if (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize == size)
							return Collection;
						break;
				}
				return Invalid;
			}
		}

		return Invalid;
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