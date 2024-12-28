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

#include <core/sdk/virtual.h>

class CSchemaType;
class CSchemaSystemTypeScope;
class ISaveRestoreOps;

class CSchemaSystemTypeScope2 {
public:
	SchemaClassInfoData_t* FindDeclaredClass(const char* pClass) {
#if S2SDK_PLATFORM_WINDOWS
		SchemaClassInfoData_t* rv = nullptr;
		CALL_VIRTUAL(void, 2, this, &rv, pClass);
		return rv;
#else
		return CALL_VIRTUAL(SchemaClassInfoData_t*, 2, this, pClass);
#endif
	}
};

class CSchemaSystem {
public:
	CSchemaSystemTypeScope2* FindTypeScopeForModule(const char* module) {
		return CALL_VIRTUAL(CSchemaSystemTypeScope2*, 13, this, module, nullptr);
	}
};
