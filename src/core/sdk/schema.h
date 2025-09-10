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

#include "virtual.h"
#include <entityinstance.h>
#include <smartptr.h>
#include <tier0/dbg.h>
#include <utldelegate.h>
#undef schema

struct SchemaKey {
	int32 offset{};
	bool networked{};
	int32 size{};
	CSchemaType* type{};
};

namespace schema {
	static std::unordered_set<plg::string> CS2BadList = {
			"m_bIsValveDS",
			"m_bIsQuestEligible",
			"m_iItemDefinitionIndex",// in unmanaged this cannot be set.
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
			"m_flValue", // MNetworkAlias "m_iRawValue32"
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

	int32_t FindChainOffset(const char* className, uint32_t classNameHash);
	SchemaKey GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey);
	void NetworkStateChanged(intptr_t chainEntity, uint localOffset, int arrayIndex = 0xFFFFFFFF);

	ElementType GetElementType(CSchemaType* type);
	std::pair<ElementType, int> IsIntType(CSchemaType* type);
	std::pair<ElementType, int> IsFloatType(CSchemaType* type);
	ElementType IsPlainType(CSchemaType* type, size_t size);
	ElementType IsAtomicType(CSchemaType* type, size_t size);

}// namespace schema

void EntityNetworkStateChanged(uintptr_t pEntity, uint32_t nOffset);
void ChainNetworkStateChanged(uintptr_t pNetworkVarChainer, uint32_t nOffset);
void NetworkVarStateChanged(uintptr_t pNetworkVar, uint32_t nOffset, uint32_t nNetworkStateChangedOffset);

void SafeNetworkStateChanged(intptr_t pEntity, int offset, int chainOffset);

class CBaseEntity;

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
#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)                                                                     \
	PLUGIFY_NO_UNIQUE_ADDRESS class varName##_prop                                                                           \
	{                                                                                                                        \
	public:                                                                                                                  \
		std::add_lvalue_reference_t<type> Get()                                                                              \
		{                                                                                                                    \
			static const auto m_key = schema::GetOffset(m_className, m_classNameHash, #varName, m_varNameHash);              \
			static const auto m_offset = offsetof(ThisClass, varName);														 \
																															 \
			uintptr_t pThisClass = ((uintptr_t)this - m_offset);                                                             \
                                                                                                                             \
			return *reinterpret_cast<std::add_pointer_t<type>>(pThisClass + m_key.offset + extra_offset);                    \
		}                                                                                                                    \
		void Set(type& val)                                                                                                  \
		{                                                                                                                    \
			static const auto m_key = schema::GetOffset(m_className, m_classNameHash, #varName, m_varNameHash);              \
			static const auto m_chain = schema::FindChainOffset(m_className, m_classNameHash);                               \
			static const auto m_offset = offsetof(ThisClass, varName);														 \
																															 \
			uintptr_t pThisClass = ((uintptr_t)this - m_offset);                                                             \
                                                                                                                             \
			NetworkStateChanged();                                                                                           \
			*reinterpret_cast<std::add_pointer_t<type>>(pThisClass + m_key.offset + extra_offset) = val;                     \
		}                                                                                                                    \
		void NetworkStateChanged()                                                                                           \
		{                                                                                                                    \
			static const auto m_key = schema::GetOffset(m_className, m_classNameHash, #varName, m_varNameHash);				 \
			static const auto m_chain = schema::FindChainOffset(m_className, m_classNameHash);								 \
			static const auto m_offset = offsetof(ThisClass, varName);														 \
																															 \
			uintptr_t pThisClass = ((uintptr_t)this - m_offset);                                                             \
                                                                                                                             \
			if (m_chain != 0 && m_key.networked)                                                                             \
			{                                                                                                                \
				::ChainNetworkStateChanged(pThisClass + m_chain, m_key.offset + extra_offset);								 \
			}                                                                                                                \
			else if (m_key.networked)                                                                                        \
			{                                                                                                                \
				if (!m_networkStateChangedOffset)                                                                            \
					::EntityNetworkStateChanged(pThisClass, m_key.offset + extra_offset);									 \
				else                                                                                                         \
					::NetworkVarStateChanged(pThisClass, m_key.offset + extra_offset, m_networkStateChangedOffset);			 \
			}                                                                                                                \
		}                                                                                                                    \
		operator std::add_lvalue_reference_t<type>()                                                                         \
		{                                                                                                                    \
			return Get();                                                                                                    \
		}                                                                                                                    \
		std::add_lvalue_reference_t<type> operator()()                                                                       \
		{                                                                                                                    \
			return Get();                                                                                                    \
		}                                                                                                                    \
		std::add_lvalue_reference_t<type> operator->()                                                                       \
		{                                                                                                                    \
			return Get();                                                                                                    \
		}                                                                                                                    \
		void operator()(type val)                                                                                            \
		{                                                                                                                    \
			Set(val);                                                                                                        \
		}                                                                                                                    \
		std::add_lvalue_reference_t<type> operator=(type val)                                                                \
		{                                                                                                                    \
			Set(val);                                                                                                        \
			return Get();                                                                                                    \
		}                                                                                                                    \
		std::add_lvalue_reference_t<type> operator=(varName##_prop& val)                                                     \
		{                                                                                                                    \
			Set(val());                                                                                                      \
			return Get();                                                                                                    \
		}                                                                                                                    \
	private:                                                                                                                 \
		/*Prevent accidentally copying this wrapper class instead of the underlying field*/                                  \
		varName##_prop(const varName##_prop&) = delete;                                                                      \
		static constexpr auto m_varNameHash = hash_32_fnv1a_const(#varName);                                                 \
	} varName;

#define SCHEMA_FIELD_POINTER_OFFSET(type, varName, extra_offset)                                                             \
	PLUGIFY_NO_UNIQUE_ADDRESS class varName##_prop                                                                           \
	{                                                                                                                        \
	public:                                                                                                                  \
		type* Get()                                                                                                          \
		{                                                                                                                    \
			static const auto m_key = schema::GetOffset(m_className, m_classNameHash, #varName, m_varNameHash);				 \
			static const auto m_offset = offsetof(ThisClass, varName);														 \
																															 \
			uintptr_t pThisClass = ((uintptr_t)this - m_offset);                                                             \
                                                                                                                             \
			return reinterpret_cast<std::add_pointer_t<type>>(pThisClass + m_key.offset + extra_offset);                     \
		}                                                                                                                    \
		void NetworkStateChanged() /*Call this after editing the field*/                                                     \
		{                                                                                                                    \
			static const auto m_key = schema::GetOffset(m_className, m_classNameHash, #varName, m_varNameHash);				 \
			static const auto m_chain = schema::FindChainOffset(m_className, m_classNameHash);								 \
			static const auto m_offset = offsetof(ThisClass, varName);														 \
																															 \
			uintptr_t pThisClass = ((uintptr_t)this - m_offset);                                                             \
                                                                                                                             \
			if (m_chain != 0 && m_key.networked)                                                                             \
			{                                                                                                                \
				::ChainNetworkStateChanged(pThisClass + m_chain, m_key.offset + extra_offset);								 \
			}                                                                                                                \
			else if (m_key.networked)                                                                                        \
			{                                                                                                                \
				if (!m_networkStateChangedOffset)                                                                            \
					::EntityNetworkStateChanged(pThisClass, m_key.offset + extra_offset);									 \
				else                                                                                                         \
					::NetworkVarStateChanged(pThisClass, m_key.offset + extra_offset, m_networkStateChangedOffset);			 \
			}                                                                                                                \
		}                                                                                                                    \
		operator type*()                                                                                                     \
		{                                                                                                                    \
			return Get();                                                                                                    \
		}                                                                                                                    \
		type* operator()()                                                                                                   \
		{                                                                                                                    \
			return Get();                                                                                                    \
		}                                                                                                                    \
		type* operator->()                                                                                                   \
		{                                                                                                                    \
			return Get();                                                                                                    \
		}                                                                                                                    \
	private:                                                                                                                 \
		/*Prevent accidentally copying this wrapper class instead of the underlying field*/                                  \
		varName##_prop(const varName##_prop&) = delete;                                                                      \
		static constexpr auto m_varNameHash = hash_32_fnv1a_const(#varName);                                                 \
	} varName;

// Use this when you want the member's value itself
#define SCHEMA_FIELD(type, varName) \
	SCHEMA_FIELD_OFFSET(type, varName, 0)

// Use this when you want a pointer to a member
#define SCHEMA_FIELD_POINTER(type, varName) \
	SCHEMA_FIELD_POINTER_OFFSET(type, varName, 0)

// If the class needs a specific offset for its NetworkStateChanged (like CEconItemView), use this and provide the offset
#define DECLARE_SCHEMA_CLASS_BASE(ClassName, offset)								\
	private:																		\
		typedef ClassName ThisClass;												\
		static constexpr const char* m_className = #ClassName;						\
		static constexpr uint32_t m_classNameHash = hash_32_fnv1a_const(#ClassName);\
		static constexpr int m_networkStateChangedOffset = offset;					\
	public:

#define DECLARE_SCHEMA_CLASS(className) DECLARE_SCHEMA_CLASS_BASE(className, 0)

// Use this for non-entity classes such as CCollisionProperty or CGlowProperty
// The only difference is that their NetworkStateChanged function is index 1 on their vtable rather than being CEntityInstance::NetworkStateChanged
// Though some classes like CGameRules will instead use their CNetworkVarChainer as a link back to the parent entity
#define DECLARE_SCHEMA_CLASS_INLINE(className) DECLARE_SCHEMA_CLASS_BASE(className, 1)