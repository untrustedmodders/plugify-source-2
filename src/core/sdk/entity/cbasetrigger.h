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

#include "cbasemodelentity.h"

class CBaseToggle : public CBaseModelEntity {
public:
	DECLARE_SCHEMA_CLASS(CBaseToggle);

	SCHEMA_FIELD(float, m_flWait);
};

class CBaseTrigger : public CBaseToggle {
public:
	DECLARE_SCHEMA_CLASS(CBaseTrigger)

	SCHEMA_FIELD(CUtlSymbolLarge, m_iFilterName)
	SCHEMA_FIELD(CEntityHandle, m_hFilter)
	SCHEMA_FIELD_POINTER(CUtlVector<CHandle<CBaseEntity>>, m_hTouchingEntities)
	SCHEMA_FIELD(bool, m_bClientSidePredicted)

	bool PassesTriggerFilters(CBaseEntity* pOther) {
		static int offset = g_pGameConfig->GetOffset("PassesTriggerFilters");
		return CALL_VIRTUAL(bool, offset, this, pOther);
	}

	//bool IsStartZone() { return !V_stricmp(this->GetClassname(), "trigger_multiple") && this->m_pEntity->NameMatches("timer_startzone"); }
	//bool IsEndZone() { return !V_stricmp(this->GetClassname(), "trigger_multiple") && this->m_pEntity->NameMatches("timer_endzone"); }
};
