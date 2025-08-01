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

#include <core/game_config.hpp>
#include <core/sdk/schema.h>
#include <ehandle.h>

#include "ccollisionproperty.h"
#include "ctakedamageinfo.h"
#include "globaltypes.h"
#include "services.h"

extern std::unique_ptr<GameConfig> g_pGameConfig;

class CGameSceneNode {
public:
	DECLARE_SCHEMA_CLASS(CGameSceneNode)

	SCHEMA_FIELD(CEntityInstance*, m_pOwner)
	SCHEMA_FIELD_POINTER(CGameSceneNode, m_pParent)
	SCHEMA_FIELD_POINTER(CGameSceneNode, m_pChild)
	SCHEMA_FIELD(CNetworkOriginCellCoordQuantizedVector, m_vecOrigin)
	SCHEMA_FIELD(QAngle, m_angRotation)
	SCHEMA_FIELD(float, m_flScale)
	SCHEMA_FIELD(float, m_flAbsScale)
	SCHEMA_FIELD(Vector, m_vecAbsOrigin)
	SCHEMA_FIELD(QAngle, m_angAbsRotation)
	SCHEMA_FIELD(Vector, m_vRenderOrigin)

	matrix3x4_t EntityToWorldTransform() {
		matrix3x4_t mat;

		// issues with this and im tired so hardcoded it
		// AngleMatrix(this->m_angAbsRotation(), this->m_vecAbsOrigin(), mat);

		QAngle angles = this->m_angAbsRotation;
		float sr, sp, sy, cr, cp, cy;
		SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
		SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
		SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);
		mat[0][0] = cp * cy;
		mat[1][0] = cp * sy;
		mat[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		mat[0][1] = sp * srcy - crsy;
		mat[1][1] = sp * srsy + crcy;
		mat[2][1] = sr * cp;

		mat[0][2] = (sp * crcy + srsy);
		mat[1][2] = (sp * crsy - srcy);
		mat[2][2] = cr * cp;

		Vector pos = this->m_vecAbsOrigin;
		mat[0][3] = pos.x;
		mat[1][3] = pos.y;
		mat[2][3] = pos.z;

		return mat;
	}
};

class CBodyComponent {
public:
	DECLARE_SCHEMA_CLASS(CBodyComponent)

	SCHEMA_FIELD_POINTER(CGameSceneNode, m_pSceneNode)
};

class CModelState {
public:
	DECLARE_SCHEMA_CLASS(CModelState)

	SCHEMA_FIELD(CUtlSymbolLarge, m_ModelName)
};

class CSkeletonInstance : public CGameSceneNode {
public:
	DECLARE_SCHEMA_CLASS(CSkeletonInstance)

	SCHEMA_FIELD(CModelState, m_modelState)
};

class CEntitySubclassVDataBase {
public:
	DECLARE_SCHEMA_CLASS(CEntitySubclassVDataBase)
};

class CBaseEntity : public CEntityInstance {
public:
	DECLARE_SCHEMA_CLASS(CBaseEntity)

	SCHEMA_FIELD_POINTER(CBodyComponent, m_CBodyComponent)
	SCHEMA_FIELD(CBitVec<64>, m_isSteadyState)
	SCHEMA_FIELD(float, m_lastNetworkChange)
	SCHEMA_FIELD_POINTER(CNetworkTransmitComponent, m_NetworkTransmitComponent)
	SCHEMA_FIELD(int, m_iHealth)
	SCHEMA_FIELD(int, m_iMaxHealth)
	SCHEMA_FIELD(int, m_iTeamNum)
	SCHEMA_FIELD(bool, m_bLagCompensate)
	SCHEMA_FIELD(Vector, m_vecAbsVelocity)
	SCHEMA_FIELD(Vector, m_vecBaseVelocity)
	SCHEMA_FIELD_POINTER(CCollisionProperty, m_pCollision)
	SCHEMA_FIELD(MoveCollide_t, m_MoveCollide)
	SCHEMA_FIELD(MoveType_t, m_MoveType)
	SCHEMA_FIELD(MoveType_t, m_nActualMoveType)
	SCHEMA_FIELD(CHandle<CBaseEntity>, m_hEffectEntity)
	SCHEMA_FIELD(uint32, m_spawnflags)
	SCHEMA_FIELD(uint32, m_fFlags)
	SCHEMA_FIELD(LifeState_t, m_lifeState)
	SCHEMA_FIELD(float, m_flDamageAccumulator)
	SCHEMA_FIELD(bool, m_bTakesDamage)
	SCHEMA_FIELD(TakeDamageFlags_t, m_nTakeDamageFlags)
	SCHEMA_FIELD_POINTER(CUtlStringToken, m_nSubclassID)
	SCHEMA_FIELD(float, m_flFriction)
	SCHEMA_FIELD(float, m_flGravityScale)
	SCHEMA_FIELD(float, m_flTimeScale)
	SCHEMA_FIELD(float, m_flSpeed)
	SCHEMA_FIELD(CUtlString, m_sUniqueHammerID);
	SCHEMA_FIELD(CUtlSymbolLarge, m_target);
	SCHEMA_FIELD(CUtlSymbolLarge, m_iGlobalname);
	SCHEMA_FIELD(CHandle<CBaseEntity>, m_hGroundEntity)
	SCHEMA_FIELD(float, m_flWaterLevel)
	SCHEMA_FIELD(int, m_fEffects)
	SCHEMA_FIELD(bool, m_bIsHLTV)

	int entindex() { return m_pEntity->m_EHandle.GetEntryIndex(); }

	// TODO: Validate
	Vector GetAbsOrigin() { return m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin; }
	QAngle GetAbsRotation() { return m_CBodyComponent->m_pSceneNode->m_angAbsRotation; }
	void SetAbsOrigin(const Vector& vecOrigin) { m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = vecOrigin; }
	void SetAbsRotation(const QAngle& angAbsRotation) { m_CBodyComponent->m_pSceneNode->m_angAbsRotation = angAbsRotation; }

	void SetAbsVelocity(const Vector& vecVelocity) { m_vecAbsVelocity = vecVelocity; }
	void SetBaseVelocity(const Vector& vecVelocity) { m_vecBaseVelocity = vecVelocity; }

	void SetName(const char* pName) {
		addresses::CEntityIdentity_SetEntityName(m_pEntity, pName);
	}

	const char* GetName() {
		return m_iGlobalname->String();
	}

	/*void TakeDamage(CTakeDamageInfo &info) {
		Detour_CBaseEntity_TakeDamageOld(this, &info);
	}*/

	void SetCollisionGroup(StandardCollisionGroups_t nCollisionGroup) {
		if (!m_pCollision)
			return;

		m_pCollision->m_collisionAttribute->m_nCollisionGroup = COLLISION_GROUP_DEBRIS;
		m_pCollision->m_CollisionGroup = COLLISION_GROUP_DEBRIS;
		CollisionRulesChanged();
	}

	bool IsPawn() {
		static int offset = g_pGameConfig->GetOffset("IsEntityPawn");
		return CALL_VIRTUAL(bool, offset, this);
	}

	bool IsController() {
		static int offset = g_pGameConfig->GetOffset("IsEntityController");
		return CALL_VIRTUAL(bool, offset, this);
	}

	bool IsAlive() {
		return this->m_lifeState == LIFE_ALIVE;
	}

	void CollisionRulesChanged() {
		static int offset = g_pGameConfig->GetOffset("CollisionRulesChanged");
		CALL_VIRTUAL(void, offset, this);
	}

	void StartTouch(CBaseEntity* pOther) {
		static int offset = g_pGameConfig->GetOffset("StartTouch");
		CALL_VIRTUAL(bool, offset, this, pOther);
	}
	void Touch(CBaseEntity* pOther) {
		static int offset = g_pGameConfig->GetOffset("Touch");
		CALL_VIRTUAL(bool, offset, this, pOther);
	}
	void EndTouch(CBaseEntity* pOther) {
		static int offset = g_pGameConfig->GetOffset("EndTouch");
		CALL_VIRTUAL(bool, offset, this, pOther);
	}

	void Teleport(const Vector* newPosition, const QAngle* newAngles, const Vector* newVelocity) {
		static int offset = g_pGameConfig->GetOffset("Teleport");
		CALL_VIRTUAL(bool, offset, this, newPosition, newAngles, newVelocity);
	}

	CHandle<CBaseEntity> GetHandle() { return m_pEntity->m_EHandle; }

	// A double pointer to entity VData is available 4 bytes past m_nSubclassID, if applicable
	CEntitySubclassVDataBase* GetVData() {
		CUtlStringToken* subclassID = m_nSubclassID;
		return *reinterpret_cast<CEntitySubclassVDataBase**>(reinterpret_cast<uint8*>(subclassID) + 4);
	}

	void DispatchSpawn(CEntityKeyValues* pEntityKeyValues = nullptr) {
		addresses::DispatchSpawn(this, pEntityKeyValues);
	}

	// Emit a sound event
	void EmitSound(const char* pszSound, int nPitch = 100, float flVolume = 1.0, float flDelay = 0.0) {
		addresses::CBaseEntity_EmitSoundParams(this, pszSound, nPitch, flVolume, flDelay);
	}

	SndOpEventGuid_t EmitSoundFilter(IRecipientFilter& filter, const char* pszSound, float flVolume = 1.0, float flPitch = 1.0) {
		EmitSound_t params;
		params.m_pSoundName = pszSound;
		params.m_flVolume = flVolume;
		params.m_nPitch = static_cast<int>(flPitch);

		return addresses::CBaseEntity_EmitSoundFilter(filter, entindex(), params);
	}

	/*void DispatchParticle(const char* pszParticleName, IRecipientFilter* pFilter, ParticleAttachment_t nAttachType = PATTACH_POINT_FOLLOW, char iAttachmentPoint = 0, CUtlSymbolLarge iAttachmentName = "") {
		addresses::DispatchParticleEffect(pszParticleName, nAttachType, this, iAttachmentPoint, iAttachmentName, false, 0, pFilter, 0);
	}*/

	// This was needed so we can parent to nameless entities using pointers
	void SetParent(CBaseEntity* pNewParent) {
		addresses::CBaseEntity_SetParent(this, pNewParent, 0u, nullptr);
	}

	void Remove() {
		addresses::UTIL_Remove(this);
	}

	void AcceptInput(const char* pInputName, const variant_t& value = variant_t(), CEntityInstance* pActivator = nullptr, CEntityInstance* pCaller = nullptr, int outputId = 0) {
		addresses::CEntityInstance_AcceptInput(this, pInputName, pActivator, pCaller, const_cast<variant_t*>(&value), outputId);
	}

	void SetMoveType(MoveType_t nMoveType) {
		addresses::CBaseEntity_SetMoveType(this, nMoveType, m_MoveCollide);
	}

	const char* GetName() const { return m_pEntity->m_name.String(); }
};

class SpawnPoint : public CBaseEntity {
public:
	DECLARE_SCHEMA_CLASS(SpawnPoint);

	SCHEMA_FIELD(bool, m_bEnabled);
};
