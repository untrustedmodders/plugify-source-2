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

struct CNetworkVarChainer : CSmartPtr<CEntityInstance> {
	struct ChainUpdatePropagationLL_t {
		ChainUpdatePropagationLL_t* pNext{};
		CUtlDelegate<void(const CNetworkVarChainer&)> updateDelegate;
	};
	uint8 unk[24];
	ChangeAccessorFieldPathIndex_t m_PathIndex;
	ChainUpdatePropagationLL_t* m_pPropagationList;
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

	int32_t FindChainOffset(std::string_view className);
	SchemaKey GetOffset(std::string_view className, std::string_view memberName);
	void NetworkStateChanged(intptr_t chainEntity, uint localOffset, int arrayIndex = 0xFFFFFFFF);

	ElementType GetElementType(CSchemaType* type);
	std::pair<ElementType, int> IsIntType(CSchemaType* type);
	std::pair<ElementType, int> IsFloatType(CSchemaType* type);
	ElementType IsPlainType(CSchemaType* type, size_t size);
	ElementType IsAtomicType(CSchemaType* type, size_t size);

}// namespace schema

class CBaseEntity;

#define SCHEMA_FIELD_OFFSET(type, varName, extraOffset)                                                                       \
	PLUGIFY_NO_UNIQUE_ADDRESS class varName##_prop {                                                                          \
		const type* Ptr() const {                                                                                             \
			static const auto schemaKey = schema::GetOffset(ThisClassName, #varName);                                         \
			static const size_t offset = offsetof(ThisClass, varName);                                                        \
			auto* pThisClass = reinterpret_cast<const ThisClass*>(reinterpret_cast<const byte*>(this) - offset);              \
                                                                                                                              \
			return reinterpret_cast<const type*>(reinterpret_cast<intptr_t>(pThisClass) + schemaKey.offset + extraOffset);    \
		}                                                                                                                     \
		void Set(type val) {                                                                                                  \
			static const auto schemaKey = schema::GetOffset(ThisClassName, #varName);                                         \
			static const size_t offset = offsetof(ThisClass, varName);                                                        \
			auto* pThisClass = reinterpret_cast<ThisClass*>(reinterpret_cast<byte*>(this) - offset);                          \
                                                                                                                              \
			if (schemaKey.networked) {                                                                                        \
				static const auto chainOffset = schema::FindChainOffset(ThisClassName);                                       \
				if (chainOffset != 0) {                                                                                       \
					S2_LOGF(LS_DEBUG, "Found chain offset {} for {}::{}\n", chainOffset, ThisClassName, #varName);            \
					schema::NetworkStateChanged(                                                                              \
							reinterpret_cast<intptr_t>(pThisClass) + chainOffset, schemaKey.offset + extraOffset);            \
				} else {                                                                                                      \
					/* WIP: Works fine for most props, but inlined classes in the middle of a class will                      \
						need to have their this pointer corrected by the offset .*/                                           \
					S2_LOGF(LS_DEBUG, "Attempting to call SetStateChanged on on {}::{}\n", ThisClassName, #varName);          \
					if constexpr (!IsStruct)                                                                                  \
						reinterpret_cast<CEntityInstance*>(pThisClass)->NetworkStateChanged(schemaKey.offset + extraOffset);  \
					else if constexpr (IsPlatformPosix()) /* This is currently broken on windows */                           \
						CALL_VIRTUAL(void, 1, pThisClass, schemaKey.offset + extraOffset, 0xFFFFFFFF, 0xFFFF);                \
				}                                                                                                             \
			}                                                                                                                 \
			*reinterpret_cast<type*>(reinterpret_cast<intptr_t>(pThisClass) + schemaKey.offset + extraOffset) = val;          \
		}                                                                                                                     \
	public:                                                                                                                   \
		operator bool() const { return Ptr(); }                                                                               \
		operator const type&() const { return *Ptr(); }                                                                       \
		operator type&() { return *const_cast<type*>(Ptr()); }                                                                \
		const type* operator->() const { return Ptr(); }                                                                      \
		type* operator->() { return const_cast<type*>(Ptr()); }                                                               \
		const type& operator*() const { return *Ptr(); }                                                                      \
		type& operator*() { return *const_cast<type*>(Ptr()); }                                                               \
		varName##_prop& operator=(type val) {                                                                                 \
			Set(val);                                                                                                         \
			return *this;                                                                                                     \
		}                                                                                                                     \
	} varName;

#define SCHEMA_FIELD_POINTER_OFFSET(type, varName, extraOffset)                                                               \
	PLUGIFY_NO_UNIQUE_ADDRESS class varName##_prop {                                                                          \
		const type* Ptr() const {                                                                                             \
			static const auto schemaKey = schema::GetOffset(ThisClassName, #varName);                                         \
			static const size_t offset = offsetof(ThisClass, varName);                                                        \
			auto* pThisClass = reinterpret_cast<const ThisClass*>(reinterpret_cast<const byte*>(this) - offset);              \
                                                                                                                              \
			return reinterpret_cast<const type*>(reinterpret_cast<intptr_t>(pThisClass) + schemaKey.offset + extraOffset);    \
		}                                                                                                                     \
	public:                                                                                                                   \
		operator bool() const { return Ptr(); }                                                                               \
		operator const type*() const { return Ptr(); }                                                                        \
		operator type*() { return const_cast<type*>(Ptr()); }                                                                 \
		const type* operator->() const { return Ptr(); }                                                                      \
		type* operator->() { return const_cast<type*>(Ptr()); }                                                               \
	} varName;

// Use this when you want the member's value itself
#define SCHEMA_FIELD(type, varName) \
	SCHEMA_FIELD_OFFSET(type, varName, 0)

// Use this when you want a pointer to a member
#define SCHEMA_FIELD_POINTER(type, varName) \
	SCHEMA_FIELD_POINTER_OFFSET(type, varName, 0)

#define DECLARE_SCHEMA_CLASS_BASE(className, isStruct)            \
	typedef className ThisClass;                                  \
	static constexpr std::string_view ThisClassName = #className; \
	static constexpr bool IsStruct = isStruct;

#define DECLARE_SCHEMA_CLASS(className) DECLARE_SCHEMA_CLASS_BASE(className, false)

// Use this for classes that can be wholly included within other classes (like CCollisionProperty within CBaseModelEntity)
#define DECLARE_SCHEMA_CLASS_INLINE(className) \
	DECLARE_SCHEMA_CLASS_BASE(className, true)
