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

#include "cbaseentity.h"
#include "cbasemodelentity.h"
#include "ccollisionproperty.h"

enum class gear_slot_t : uint32_t {
	GEAR_SLOT_INVALID = 0xffffffff,
	GEAR_SLOT_RIFLE = 0x0,
	GEAR_SLOT_PISTOL = 0x1,
	GEAR_SLOT_KNIFE = 0x2,
	GEAR_SLOT_GRENADES = 0x3,
	GEAR_SLOT_C4 = 0x4,
	GEAR_SLOT_RESERVED_SLOT6 = 0x5,
	GEAR_SLOT_RESERVED_SLOT7 = 0x6,
	GEAR_SLOT_RESERVED_SLOT8 = 0x7,
	GEAR_SLOT_RESERVED_SLOT9 = 0x8,
	GEAR_SLOT_RESERVED_SLOT10 = 0x9,
	GEAR_SLOT_RESERVED_SLOT11 = 0xa,
	GEAR_SLOT_BOOSTS = 0xb,
	GEAR_SLOT_UTILITY = 0xc,
	GEAR_SLOT_COUNT = 0xd,
	GEAR_SLOT_FIRST = 0x0,
	GEAR_SLOT_LAST = 0xc,
};

enum class CSWeaponType : uint32_t {
	WEAPONTYPE_KNIFE = 0x0,
	WEAPONTYPE_PISTOL = 0x1,
	WEAPONTYPE_SUBMACHINEGUN = 0x2,
	WEAPONTYPE_RIFLE = 0x3,
	WEAPONTYPE_SHOTGUN = 0x4,
	WEAPONTYPE_SNIPER_RIFLE = 0x5,
	WEAPONTYPE_MACHINEGUN = 0x6,
	WEAPONTYPE_C4 = 0x7,
	WEAPONTYPE_TASER = 0x8,
	WEAPONTYPE_GRENADE = 0x9,
	WEAPONTYPE_EQUIPMENT = 0xa,
	WEAPONTYPE_STACKABLEITEM = 0xb,
	WEAPONTYPE_FISTS = 0xc,
	WEAPONTYPE_BREACHCHARGE = 0xd,
	WEAPONTYPE_BUMPMINE = 0xe,
	WEAPONTYPE_TABLET = 0xf,
	WEAPONTYPE_MELEE = 0x10,
	WEAPONTYPE_SHIELD = 0x11,
	WEAPONTYPE_ZONE_REPULSOR = 0x12,
	WEAPONTYPE_UNKNOWN = 0x13,
};

enum class CSWeaponCategory : uint32_t {
	WEAPONCATEGORY_OTHER = 0x0,
	WEAPONCATEGORY_MELEE = 0x1,
	WEAPONCATEGORY_SECONDARY = 0x2,
	WEAPONCATEGORY_SMG = 0x3,
	WEAPONCATEGORY_RIFLE = 0x4,
	WEAPONCATEGORY_HEAVY = 0x5,
	WEAPONCATEGORY_COUNT = 0x6,
};

class CEconItemView {
public:
	DECLARE_SCHEMA_CLASS_INLINE(CEconItemView);

	SCHEMA_FIELD(uint16_t, m_iItemDefinitionIndex)
	SCHEMA_FIELD(bool, m_bInitialized)
};

class CAttributeContainer {
public:
	DECLARE_SCHEMA_CLASS_INLINE(CAttributeContainer);

	SCHEMA_FIELD(CEconItemView, m_Item)
};

class CBaseAnimGraph : public CBaseModelEntity {
public:
	DECLARE_SCHEMA_CLASS(CBaseAnimGraph);
};

class CBaseFlex : public CBaseAnimGraph {
public:
	DECLARE_SCHEMA_CLASS(CBaseFlex);
};

class CEconEntity : public CBaseFlex {
public:
	DECLARE_SCHEMA_CLASS(CEconEntity)

	SCHEMA_FIELD(CAttributeContainer, m_AttributeManager)
};

class CBasePlayerWeaponVData : public CEntitySubclassVDataBase {
public:
	DECLARE_SCHEMA_CLASS(CBasePlayerWeaponVData)
	SCHEMA_FIELD(int, m_iMaxClip1)
};

class CCSWeaponBaseVData : public CBasePlayerWeaponVData {
public:
	DECLARE_SCHEMA_CLASS(CCSWeaponBaseVData)

	SCHEMA_FIELD(CSWeaponType, m_WeaponType)
	SCHEMA_FIELD(CSWeaponCategory, m_WeaponCategory)
	SCHEMA_FIELD(gear_slot_t, m_GearSlot)
	SCHEMA_FIELD(int, m_nPrice)
	SCHEMA_FIELD(int, m_nKillAward);
	SCHEMA_FIELD(int, m_nPrimaryReserveAmmoMax);
	SCHEMA_FIELD(int, m_nSecondaryReserveAmmoMax);
	SCHEMA_FIELD(bool, m_bMeleeWeapon);
	SCHEMA_FIELD(bool, m_bHasBurstMode);
	SCHEMA_FIELD(bool, m_bIsRevolver);
	SCHEMA_FIELD(bool, m_bCannotShootUnderwater);
};

class CBasePlayerWeapon : public CEconEntity {
public:
	DECLARE_SCHEMA_CLASS(CBasePlayerWeapon)

	CCSWeaponBaseVData* GetWeaponVData() { return (CCSWeaponBaseVData*) GetVData(); }
};

class CCSWeaponBase : public CBasePlayerWeapon {
public:
	DECLARE_SCHEMA_CLASS(CCSWeaponBase)
};
