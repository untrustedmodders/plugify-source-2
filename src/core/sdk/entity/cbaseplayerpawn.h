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

class CPlayer_MovementServices;
class CPlayer_ObserverServices;
class CCSPlayer_WeaponServices;
class CCSPlayer_ItemServices;

#include "cbasemodelentity.h"
#include "ccsweaponbase.h"

class CBasePlayerPawn : public CBaseModelEntity
{
public:
	DECLARE_SCHEMA_CLASS(CBasePlayerPawn);

	SCHEMA_FIELD(CPlayer_MovementServices*, m_pMovementServices)
	SCHEMA_FIELD(CCSPlayer_WeaponServices*, m_pWeaponServices)
	SCHEMA_FIELD(CCSPlayer_ItemServices*, m_pItemServices)
	SCHEMA_FIELD(CPlayer_ObserverServices*, m_pObserverServices)
	SCHEMA_FIELD(CHandle<CBasePlayerController>, m_hController)

	// Drops any map-spawned weapons the pawn is holding
	// NOTE: Currently very broken with map items (entities parented to weapons?) due to a game bug..? Needs further investigation/work
	void DropMapWeapons()
	{
		if (!m_pWeaponServices())
			return;

		CUtlVector<CHandle<CBasePlayerWeapon>>* weapons = m_pWeaponServices()->m_hMyWeapons();

		FOR_EACH_VEC(*weapons, i)
		{
			CBasePlayerWeapon* pWeapon = (*weapons)[i].Get();

			if (!pWeapon)
				continue;

			// If this is a map-spawned weapon (items), drop it
			if (V_strcmp(pWeapon->m_sUniqueHammerID().Get(), "") && pWeapon->GetWeaponVData()->m_GearSlot() != GEAR_SLOT_KNIFE)
				m_pWeaponServices()->DropWeapon(pWeapon);
		}
	}

	void CommitSuicide(bool bExplode, bool bForce)
	{
		static int offset = g_pGameConfig->GetOffset("CommitSuicide");
		CALL_VIRTUAL(void, offset, this, bExplode, bForce);
	}

	bool IsBot()
	{
		return !!(this->m_fFlags() & FL_PAWN_FAKECLIENT);
	}
};
