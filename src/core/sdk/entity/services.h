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

#include "cinbuttonstate.h"
#include "globaltypes.h"
#include <platform.h>

class CBaseEntity;

struct CSPerRoundStats_t {
public:
	DECLARE_SCHEMA_CLASS_INLINE(CSPerRoundStats_t)

	SCHEMA_FIELD(int, m_iKills)
	SCHEMA_FIELD(int, m_iDeaths)
	SCHEMA_FIELD(int, m_iAssists)
	SCHEMA_FIELD(int, m_iDamage)
};

struct CSMatchStats_t : public CSPerRoundStats_t {
public:
	DECLARE_SCHEMA_CLASS_INLINE(CSMatchStats_t)

	SCHEMA_FIELD(int32_t, m_iEntryWins);
};

class CCSPlayerController_ActionTrackingServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerController_ActionTrackingServices)

	SCHEMA_FIELD(CSMatchStats_t, m_matchStats)
};

class CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayerPawnComponent);

	SCHEMA_FIELD(CBasePlayerPawn*, __m_pChainEntity)

	CBasePlayerPawn* GetPawn() { return __m_pChainEntity; }
};

class CPlayer_MovementServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_MovementServices);

	SCHEMA_FIELD(CInButtonState, m_nButtons)
	SCHEMA_FIELD(uint64_t, m_nQueuedButtonDownMask)
	SCHEMA_FIELD(uint64_t, m_nQueuedButtonChangeMask)
	SCHEMA_FIELD(uint64_t, m_nButtonDoublePressed)

	// m_pButtonPressedCmdNumber[64]
	SCHEMA_FIELD_POINTER(uint32_t, m_pButtonPressedCmdNumber)
	SCHEMA_FIELD(uint32_t, m_nLastCommandNumberProcessed)
	SCHEMA_FIELD(uint64_t, m_nToggleButtonDownMask)
	SCHEMA_FIELD(float, m_flMaxspeed)
};

class CPlayer_MovementServices_Humanoid : public CPlayer_MovementServices {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_MovementServices_Humanoid);

	SCHEMA_FIELD(float, m_flFallVelocity)
	SCHEMA_FIELD(float, m_bInCrouch)
	SCHEMA_FIELD(uint32_t, m_nCrouchState)
	SCHEMA_FIELD(bool, m_bInDuckJump)
	SCHEMA_FIELD(float, m_flSurfaceFriction)
};

class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_MovementServices);

	SCHEMA_FIELD(float, m_flMaxFallVelocity)
	SCHEMA_FIELD(float, m_flJumpVel)
	SCHEMA_FIELD(float, m_flStamina)
	SCHEMA_FIELD(float, m_flDuckSpeed)
	SCHEMA_FIELD(bool, m_bDuckOverride)
};

class CPlayer_UseServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_UseServices);
};

class CPlayer_ViewModelServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_ViewModelServices);
};

class CPlayer_CameraServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_CameraServices);

	SCHEMA_FIELD_POINTER(CHandle<CBaseEntity>, m_hViewEntity);
	SCHEMA_FIELD(float, m_flOldPlayerViewOffsetZ);
};

class CCSPlayerBase_CameraServices : public CPlayer_CameraServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerBase_CameraServices);
};

class CBasePlayerWeapon;

class CPlayer_WeaponServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_WeaponServices);

private:
	virtual void unk_00() = 0;
#if S2SDK_PLATFORM_LINUX
	virtual void unk_01() = 0;
#endif
public:
	virtual ~CPlayer_WeaponServices() = 0;

private:
	virtual void unk_03() = 0;
	virtual void unk_04() = 0;
	virtual void unk_05() = 0;
	virtual void unk_06() = 0;
	virtual void unk_07() = 0;
	virtual void unk_08() = 0;
	virtual void unk_09() = 0;
	virtual void unk_10() = 0;
	virtual void unk_11() = 0;
	virtual void unk_12() = 0;
	virtual void unk_13() = 0;
	virtual void unk_14() = 0;
	virtual void unk_15() = 0;
	virtual void unk_16() = 0;
	virtual void unk_17() = 0;
	virtual void unk_18() = 0;
	virtual void unk_19() = 0;
	virtual void unk_20() = 0;
	virtual void unk_21() = 0;

public:
	virtual bool CanUse(CBasePlayerWeapon* pWeapon) = 0;
	virtual void Drop(CBasePlayerWeapon* pWeapon, Vector* pVecTarget = nullptr, Vector* pVelocity = nullptr) = 0;
	virtual int Bump(CBasePlayerWeapon* pWeapon) = 0;				// May return 2 if CSGameRules()->IsPlayingGunGameDeathmatch, meaning that pWeapon will be deleted
	virtual bool Switch(CBasePlayerWeapon* pWeapon, int a3 = 0) = 0;// If a3 is equal to 3 some code will be executed
private:
	virtual void unk_25() = 0;
	virtual void unk_26() = 0;
	virtual void unk_27() = 0;
	virtual void unk_28() = 0;
	virtual void unk_29() = 0;

public:
	SCHEMA_FIELD_POINTER(CUtlVector<CHandle<CBasePlayerWeapon>>, m_hMyWeapons)
	SCHEMA_FIELD(CHandle<CBasePlayerWeapon>, m_hActiveWeapon)
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_WeaponServices);

	SCHEMA_FIELD(GameTime_t, m_flNextAttack)
	SCHEMA_FIELD(bool, m_bIsLookingAtWeapon)
	SCHEMA_FIELD(bool, m_bIsHoldingLookAtWeapon)

	SCHEMA_FIELD(CHandle<CBasePlayerWeapon>, m_hSavedWeapon)
	SCHEMA_FIELD(int32_t, m_nTimeToMelee)
	SCHEMA_FIELD(int32_t, m_nTimeToSecondary)
	SCHEMA_FIELD(int32_t, m_nTimeToPrimary)
	SCHEMA_FIELD(int32_t, m_nTimeToSniperRifle)
	SCHEMA_FIELD(bool, m_bIsBeingGivenItem)
	SCHEMA_FIELD(bool, m_bIsPickingUpItemWithUse)
	SCHEMA_FIELD(bool, m_bPickedUpWeapon)

	void RemoveItem(CBasePlayerWeapon* pWeapon) {
		addresses::CCSPlayer_WeaponServices_RemoveItem(this, pWeapon);
	}
};

class CCSPlayerController_InGameMoneyServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerController_InGameMoneyServices);

	SCHEMA_FIELD(int, m_iAccount)
};

class CCSPlayer_ItemServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_ItemServices);

	virtual ~CCSPlayer_ItemServices() = 0;

private:
	virtual void unk_01() = 0;
	virtual void unk_02() = 0;
	virtual void unk_03() = 0;
	virtual void unk_04() = 0;
	virtual void unk_05() = 0;
	virtual void unk_06() = 0;
	virtual void unk_07() = 0;
	virtual void unk_08() = 0;
	virtual void unk_09() = 0;
	virtual void unk_10() = 0;
	virtual void unk_11() = 0;
	virtual void unk_12() = 0;
	virtual void unk_13() = 0;
	virtual void unk_14() = 0;
	virtual CBaseEntity* _GiveNamedItem(const char* pchName) = 0;

public:
	virtual bool GiveNamedItemBool(const char* pchName) = 0;
	virtual CBaseEntity* GiveNamedItem(const char* pchName) = 0;
	// Recommended to use CCSPlayer_WeaponServices::DropWeapon instead (parameter is ignored here)
	virtual void DropActiveWeapon(CBasePlayerWeapon* pWeapon) = 0;
	virtual void StripPlayerWeapons(bool removeSuit) = 0;
};

class CCSPlayer_DamageReactServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_DamageReactServices);
};

class CBaseViewModel;

class CCSPlayer_ViewModelServices : public CPlayer_ViewModelServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_ViewModelServices);

	SCHEMA_FIELD_POINTER(CHandle<CBaseViewModel>, m_hViewModel);

	CBaseViewModel* GetViewModel(int iIndex = 0);
	void SetViewModel(int iIndex, CBaseViewModel* pViewModel);
};

// We need an exactly sized class to be able to iterate the vector, our schema system implementation can't do this
class WeaponPurchaseCount_t {
private:
	virtual void unk01() {};
	uint64_t unk1 = 0; // 0x8
	uint64_t unk2 = 0; // 0x10
	uint64_t unk3 = 0; // 0x18
	uint64_t unk4 = 0; // 0x20
	uint64_t unk5 = -1;// 0x28
public:
	uint16_t m_nItemDefIndex;// 0x30
	uint16_t m_nCount;		 // 0x32
private:
	uint32_t unk6 = 0;
};

struct WeaponPurchaseTracker_t {
public:
	DECLARE_SCHEMA_CLASS_INLINE(WeaponPurchaseTracker_t)

	SCHEMA_FIELD_POINTER(CUtlVector<WeaponPurchaseCount_t>, m_weaponPurchases)
};

class CCSPlayer_ActionTrackingServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_ActionTrackingServices)

	SCHEMA_FIELD(WeaponPurchaseTracker_t, m_weaponPurchasesThisRound)
};

class CPlayer_ObserverServices {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_ObserverServices)

	SCHEMA_FIELD(ObserverMode_t, m_iObserverMode)
	SCHEMA_FIELD(CHandle<CBaseEntity>, m_hObserverTarget)
	SCHEMA_FIELD(ObserverMode_t, m_iObserverLastMode)
	SCHEMA_FIELD(bool, m_bForcedObserverMode)
};