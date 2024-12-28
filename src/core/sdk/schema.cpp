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

#include "schema.h"

#include "entity/cschemasystem.h"
#include <tier1/utlmap.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

extern CSchemaSystem* g_pSchemaSystem2;
extern CGlobalVars* gpGlobals;

using SchemaKeyValueMap_t = CUtlMap<uint32_t, SchemaKey>;
using SchemaTableMap_t = CUtlMap<uint32_t, SchemaKeyValueMap_t*>;

namespace {
	bool IsFieldNetworked(SchemaClassFieldData_t& field) {
		for (int i = 0; i < field.m_nStaticMetadataCount; ++i) {
			static auto networkEnabled = hash_32_fnv1a_const("MNetworkEnable");
			if (networkEnabled == hash_32_fnv1a_const(field.m_pStaticMetadata[i].m_pszName))
				return true;
		}

		return false;
	}

	bool InitSchemaFieldsForClass(SchemaTableMap_t* tableMap, const char* className, uint32_t classKey) {
		CSchemaSystemTypeScope2* pType = g_pSchemaSystem2->FindTypeScopeForModule(S2SDK_LIBRARY_PREFIX "server" S2SDK_LIBRARY_SUFFIX);

		if (!pType)
			return false;

		SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className);

		if (!pClassInfo) {
			SchemaKeyValueMap_t* map = new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
			tableMap->Insert(classKey, map);

			g_Logger.LogFormat(LS_ERROR, "InitSchemaFieldsForClass(): '%s' was not found!\n", className);
			return false;
		}

		int fieldsSize = static_cast<int>(pClassInfo->m_nFieldCount);
		SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;

		SchemaKeyValueMap_t* keyValueMap = new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
		keyValueMap->EnsureCapacity(fieldsSize);
		tableMap->Insert(classKey, keyValueMap);

		int nSize = 0;
		uint8 nAlignment = 0;

		for (int i = 0; i < fieldsSize; ++i) {
			SchemaClassFieldData_t& field = pFields[i];

			field.m_pType->GetSizeAndAlignment(nSize, nAlignment);

			g_Logger.LogFormat(LS_DEBUG, "%s::%s found at -> 0x%X - %llx\n", className, field.m_pszName, field.m_nSingleInheritanceOffset, &field);

			keyValueMap->Insert(hash_32_fnv1a_const(field.m_pszName), {field.m_nSingleInheritanceOffset, IsFieldNetworked(field), nSize, field.m_pType});
		}

		return true;
	}
}// namespace

namespace schema {
	int32_t FindChainOffset(const char* className) {
		CSchemaSystemTypeScope2* pType = g_pSchemaSystem2->FindTypeScopeForModule(S2SDK_LIBRARY_PREFIX "server" S2SDK_LIBRARY_SUFFIX);

		if (!pType)
			return false;

		SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className);

		do {
			SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;
			uint16 fieldsSize = pClassInfo->m_nFieldCount;
			for (int i = 0; i < fieldsSize; ++i) {
				SchemaClassFieldData_t& field = pFields[i];

				std::string_view name(field.m_pszName);
				if (name == "__m_pChainEntity") {
					return field.m_nSingleInheritanceOffset;
				}
			}
		} while ((pClassInfo = pClassInfo->GetParent()) != nullptr);

		return 0;
	}

	SchemaKey GetOffset(const char* className, const char* memberName) {
		return GetOffset(className, hash_32_fnv1a_const(className), memberName, hash_32_fnv1a_const(memberName));
	}

	SchemaKey GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey) {
		static SchemaTableMap_t schemaTableMap(0, 0, DefLessFunc(uint32_t));
		int16_t tableMapIndex = schemaTableMap.Find(classKey);
		if (!schemaTableMap.IsValidIndex(tableMapIndex)) {
			if (InitSchemaFieldsForClass(&schemaTableMap, className, classKey))
				return GetOffset(className, classKey, memberName, memberKey);

			return {0, false};
		}

		SchemaKeyValueMap_t* tableMap = schemaTableMap[tableMapIndex];
		int16_t memberIndex = tableMap->Find(memberKey);
		if (!tableMap->IsValidIndex(memberIndex)) {
			g_Logger.LogFormat(LS_WARNING, "schema::GetOffset(): '%s' was not found in '%s'!\n", memberName, className);
			return {0, false};
		}

		return tableMap->Element(memberIndex);
	}

	void NetworkStateChanged(int64 chainEntity, uint32 nLocalOffset, int nArrayIndex) {
		CNetworkVarChainer* chainEnt = reinterpret_cast<CNetworkVarChainer*>(chainEntity);
		CEntityInstance* pEntity = chainEnt->GetObject();
		if (pEntity && !(pEntity->m_pEntity->m_flags & EF_IS_CONSTRUCTION_IN_PROGRESS)) {
			pEntity->NetworkStateChanged(nLocalOffset, nArrayIndex, chainEnt->m_PathIndex.m_Value);
		}
	}

	ElementType GetElementType(CSchemaType* type) {
		switch (type->m_eTypeCategory) {
			case SCHEMA_TYPE_FIXED_ARRAY: {
				return Array;
			}
			case SCHEMA_TYPE_ATOMIC: {
				if (type->m_eAtomicCategory == SCHEMA_ATOMIC_COLLECTION_OF_T)
					return Collection;
				else
					return Invalid;
			}
			default:
				return Single;
		}
	}

	std::pair<ElementType, int> IsIntType(CSchemaType* type) {
		switch (type->m_eTypeCategory) {
			case SCHEMA_TYPE_FIXED_ARRAY: {
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				switch (pElementType->m_eTypeCategory) {
					case SCHEMA_TYPE_BUILTIN: {
						switch (static_cast<CSchemaType_Builtin*>(pElementType)->m_eBuiltinType) {
							case SCHEMA_BUILTIN_TYPE_BOOL:
								return {Array, sizeof(bool)};
							case SCHEMA_BUILTIN_TYPE_CHAR:
								return {Array, sizeof(char)};
							case SCHEMA_BUILTIN_TYPE_INT8:
								return {Array, sizeof(int8)};
							case SCHEMA_BUILTIN_TYPE_UINT8:
								return {Array, sizeof(uint8)};
							case SCHEMA_BUILTIN_TYPE_INT16:
								return {Array, sizeof(int16)};
							case SCHEMA_BUILTIN_TYPE_UINT16:
								return {Array, sizeof(uint16)};
							case SCHEMA_BUILTIN_TYPE_INT32:
								return {Array, sizeof(int32)};
							case SCHEMA_BUILTIN_TYPE_UINT32:
								return {Array, sizeof(uint32)};
							case SCHEMA_BUILTIN_TYPE_INT64:
								return {Array, sizeof(int64)};
							case SCHEMA_BUILTIN_TYPE_UINT64:
								return {Array, sizeof(uint64_t)};
							case SCHEMA_BUILTIN_TYPE_COUNT:
								return {Array, sizeof(size_t)};
							default:
								return {Invalid, -1};
						}
					}
					case SCHEMA_TYPE_DECLARED_CLASS: {
						int nSize = static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_nSize;
						if (nSize <= sizeof(double)) {
							return {Array, nSize};
						}
						return {Invalid, -1};
					}
					case SCHEMA_TYPE_DECLARED_ENUM:
						return {Array, static_cast<CSchemaType_DeclaredEnum*>(pElementType)->m_pEnumInfo->m_nSize};
					case SCHEMA_TYPE_POINTER:
						return {Array, sizeof(void*)};
					default:
						break;
				}
				return {Invalid, -1};
			}
			case SCHEMA_TYPE_DECLARED_CLASS: {
				int nSize = static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_nSize;
				if (nSize <= sizeof(void*)) {
					return {Single, nSize};
				}
				return {Class, -1};
			}
			case SCHEMA_TYPE_ATOMIC: {
				if (type->m_eAtomicCategory == SCHEMA_ATOMIC_COLLECTION_OF_T) {
					switch (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize) {
						case sizeof(int8):
							return {Collection, sizeof(int8)};
						case sizeof(int16):
							return {Collection, sizeof(int16)};
						case sizeof(int32):
							return {Collection, sizeof(int32)};
						case sizeof(int64):
							return {Collection, sizeof(int64)};
						default:
							break;
					}
				}
				return {Invalid, -1};
			}
			case SCHEMA_TYPE_BUILTIN: {
				switch (static_cast<CSchemaType_Builtin*>(type)->m_eBuiltinType) {
					case SCHEMA_BUILTIN_TYPE_BOOL:
						return {Single, sizeof(bool)};
					case SCHEMA_BUILTIN_TYPE_CHAR:
						return {Single, sizeof(char)};
					case SCHEMA_BUILTIN_TYPE_INT8:
						return {Single, sizeof(int8)};
					case SCHEMA_BUILTIN_TYPE_UINT8:
						return {Single, sizeof(uint8)};
					case SCHEMA_BUILTIN_TYPE_INT16:
						return {Single, sizeof(int16)};
					case SCHEMA_BUILTIN_TYPE_UINT16:
						return {Single, sizeof(uint16)};
					case SCHEMA_BUILTIN_TYPE_INT32:
						return {Single, sizeof(int32)};
					case SCHEMA_BUILTIN_TYPE_UINT32:
						return {Single, sizeof(uint32)};
					case SCHEMA_BUILTIN_TYPE_INT64:
						return {Single, sizeof(int64)};
					case SCHEMA_BUILTIN_TYPE_UINT64:
						return {Single, sizeof(uint64_t)};
					case SCHEMA_BUILTIN_TYPE_COUNT:
						return {Single, sizeof(size_t)};
					default:
						return {Invalid, -1};
				}
			}
			case SCHEMA_TYPE_DECLARED_ENUM: {
				return {Single, static_cast<CSchemaType_DeclaredEnum*>(type)->m_pEnumInfo->m_nSize};
			}
			case SCHEMA_TYPE_POINTER: {
				return {Single, sizeof(void*)};
			}
			default: {
				return {Invalid, -1};
			}
		}
	}

	std::pair<ElementType, int> IsFloatType(CSchemaType* type) {
		switch (type->m_eTypeCategory) {
			case SCHEMA_TYPE_FIXED_ARRAY: {
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				switch (pElementType->m_eTypeCategory) {
					case SCHEMA_TYPE_BUILTIN: {
						auto* pElementTypeBuiltin = static_cast<CSchemaType_Builtin*>(pElementType);
						switch (pElementTypeBuiltin->m_eBuiltinType) {
							case SCHEMA_BUILTIN_TYPE_FLOAT32:
								return {Array, sizeof(float32)};
							case SCHEMA_BUILTIN_TYPE_FLOAT64:
								return {Array, sizeof(float64)};
							default:
								return {Invalid, -1};
						}
					}
					case SCHEMA_TYPE_DECLARED_CLASS: {
						int nSize = static_cast<CSchemaType_DeclaredClass*>(pElementType)->m_pClassInfo->m_nSize;
						if (nSize <= sizeof(double)) {
							return {Array, nSize};
						}
						return {Invalid, -1};
					}
					default:
						break;
				}
				return {Invalid, -1};
			}
			case SCHEMA_TYPE_DECLARED_CLASS: {
				int nSize = static_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo->m_nSize;
				if (nSize <= sizeof(double)) {
					return {Single, nSize};
				}
				return {Invalid, -1};
			}
			case SCHEMA_TYPE_ATOMIC: {
				if (type->m_eAtomicCategory == SCHEMA_ATOMIC_COLLECTION_OF_T) {
					switch (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize) {
						case sizeof(float32):
							return {Collection, sizeof(float32)};
						case sizeof(float64):
							return {Collection, sizeof(float64)};
						default:
							break;
					}
				}
				return {Invalid, -1};
			}
			case SCHEMA_TYPE_BUILTIN: {
				switch (static_cast<CSchemaType_Builtin*>(type)->m_eBuiltinType) {
					case SCHEMA_BUILTIN_TYPE_FLOAT32:
						return {Single, sizeof(float32)};
					case SCHEMA_BUILTIN_TYPE_FLOAT64:
						return {Single, sizeof(float64)};
					default:
						return {Invalid, -1};
				}
			}
			default:
				return {Invalid, -1};
		}
	}

	ElementType IsPlainType(CSchemaType* type, size_t size) {
		switch (type->m_eTypeCategory) {
			case SCHEMA_TYPE_FIXED_ARRAY: {
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_ATOMIC && pElementType->m_eAtomicCategory == SCHEMA_ATOMIC_PLAIN) {
					if (static_cast<CSchemaType_Atomic*>(pElementType)->m_nSize == size)
						return Array;
				}
				return Invalid;
			}
			case SCHEMA_TYPE_ATOMIC: {
				switch (type->m_eAtomicCategory) {
					case SCHEMA_ATOMIC_PLAIN:
						if (static_cast<CSchemaType_Atomic*>(type)->m_nSize == size)
							return Single;
						break;
					case SCHEMA_ATOMIC_COLLECTION_OF_T:
						if (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize == size)
							return Collection;
						break;
					default:
						return Invalid;
				}
			}
			default:
				return Invalid;
		}
	}

	ElementType IsAtomicType(CSchemaType* type, size_t size) {
		switch (type->m_eTypeCategory) {
			case SCHEMA_TYPE_FIXED_ARRAY: {
				auto* pElementType = static_cast<CSchemaType_FixedArray*>(type)->m_pElementType;
				if (pElementType->m_eTypeCategory == SCHEMA_TYPE_ATOMIC && pElementType->m_eAtomicCategory == SCHEMA_ATOMIC_T) {
					if (static_cast<CSchemaType_Atomic_T*>(pElementType)->m_nSize == size)
						return Array;
				}
				return Invalid;
			}
			case SCHEMA_TYPE_ATOMIC: {
				switch (type->m_eAtomicCategory) {
					case SCHEMA_ATOMIC_T:
						if (static_cast<CSchemaType_Atomic_T*>(type)->m_nSize == size)
							return Single;
						break;
					case SCHEMA_ATOMIC_COLLECTION_OF_T:
						if (static_cast<CSchemaType_Atomic_CollectionOfT*>(type)->m_nElementSize == size)
							return Collection;
						break;
					default:
						return Invalid;
				}
			}
			default:
				return Invalid;
		}
	}
}// namespace schema
