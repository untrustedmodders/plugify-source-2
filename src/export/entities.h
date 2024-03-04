#pragma once

#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbasemodelentity.h>

extern "C" PLUGIN_API CBaseEntity* GetEntityFromIndex(int entityIndex)
{
	return g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex));
}

extern "C" PLUGIN_API int GetUserIdFromIndex(int entityIndex)
{
	// CPlayerSlot is 1 less than index
	return g_pEngineServer2->GetPlayerUserId(CPlayerSlot(entityIndex - 1)).Get();
}

extern "C" PLUGIN_API void GetEntityClassname(std::string& output, CBaseEntity* entity)
{
	output = entity->GetClassname();
}

extern "C" PLUGIN_API void* GetEntityPointerFromHandle(CEntityHandle* handle)
{
	if (!handle->IsValid())
	{
		return nullptr;
	}

	return g_pEntitySystem->GetBaseEntity(*handle);
}

extern "C" PLUGIN_API void* GetEntityPointerFromRef(uint32_t ref)
{
	if (ref == INVALID_EHANDLE_INDEX)
	{
		return nullptr;
	}

	CBaseHandle hndl(ref);

	return g_pEntitySystem->GetBaseEntity(hndl);
}

extern "C" PLUGIN_API uint32_t GetRefFromEntityPointer(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
	{
		return INVALID_EHANDLE_INDEX;
	}

	auto hndl = pEntity->GetRefEHandle();

	if (hndl == INVALID_EHANDLE_INDEX)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return hndl.ToInt();
}

extern "C" PLUGIN_API bool IsRefValidEntity(uint32_t ref)
{
	if (ref == INVALID_EHANDLE_INDEX)
	{
		return false;
	}

	CBaseHandle hndl(ref);

	if (!hndl.IsValid())
	{
		return false;
	}

	return g_pEntitySystem->GetBaseEntity(hndl) != nullptr;
}

extern "C" PLUGIN_API CEntityIdentity* GetFirstActiveEntity()
{
	return g_pEntitySystem->m_EntityList.m_pFirstActiveEntity;
}

extern "C" PLUGIN_API void* GetConcreteEntityListPointer()
{
	return &g_pEntitySystem->m_EntityList;
}

extern "C" PLUGIN_API void HookEntityOutput(const std::string& szClassname, const std::string& szOutput, EntityListenerCallback callback, bool post)
{
	g_OutputManager.HookEntityOutput(szClassname, szOutput, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API void UnhookEntityOutput(const std::string& szClassname, const std::string& szOutput, EntityListenerCallback callback, bool post)
{
	g_OutputManager.UnhookEntityOutput(szClassname, szOutput, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API CBaseEntity* FindEntityByClassname(CEntityInstance* pStartEntity, const std::string& szName)
{
	return addresses::CGameEntitySystem_FindEntityByClassName(g_pEntitySystem, pStartEntity, szName.c_str());
}

extern "C" PLUGIN_API CBaseEntity* FindEntityByName(CEntityInstance* pStartEntity, const std::string& szName, CEntityInstance* pSearchingEntity, CEntityInstance* pActivator, CEntityInstance* pCaller, IEntityFindFilter* pFilter)
{
	return addresses::CGameEntitySystem_FindEntityByName(g_pEntitySystem, pStartEntity, szName.c_str(), pSearchingEntity, pActivator, pCaller, pFilter);
}

extern "C" PLUGIN_API CBaseEntity* CreateEntityByName(const std::string& className)
{
	return addresses::CreateEntityByName(className.c_str(), -1);
}

//DispatchSpawn

///

extern "C" PLUGIN_API int GetEntityMoveType(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_MoveType();
}
extern "C" PLUGIN_API void SetEntityMoveType(int entityIndex, int moveType)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_MoveType = static_cast<MoveType_t>(moveType);
}

extern "C" PLUGIN_API float GetEntityGravity(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0.0f;
	}

	return ent->m_flGravityScale();
}
extern "C" PLUGIN_API void SetEntityGravity(int entityIndex, float gravity)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_flGravityScale = gravity;
}

extern "C" PLUGIN_API int GetEntityFlags(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_fFlags();
}
extern "C" PLUGIN_API void SetEntityFlags(int entityIndex, int flags)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_fFlags = flags;
}

extern "C" PLUGIN_API int GetEntityRenderColor(int entityIndex)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_clrRender().GetRawColor();
}
extern "C" PLUGIN_API void SetEntityRenderColor(int entityIndex, int color)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	Color clr;
	clr.SetRawColor(color);
	ent->m_clrRender = clr;
}

extern "C" PLUGIN_API int8_t GetEntityRenderMode(int entityIndex)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_nRenderMode();
}
extern "C" PLUGIN_API void SetEntityRenderMode(int entityIndex, int8_t renderMode)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_nRenderMode = renderMode;
}

extern "C" PLUGIN_API int GetEntityHealth(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iHealth();
}
extern "C" PLUGIN_API void SetEntityHealth(int entityIndex, int health)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_iHealth = health;
}

extern "C" PLUGIN_API int GetTeamEntity(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iTeamNum();
}
extern "C" PLUGIN_API void SetTeamEntity(int entityIndex, int team)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_iTeamNum = team;
}

extern "C" PLUGIN_API CEntityInstance* GetEntityOwner(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return nullptr;
	}

	return ent->m_CBodyComponent->m_pSceneNode->m_pOwner();

}
extern "C" PLUGIN_API void SetEntityOwner(int entityIndex, CEntityInstance* owner)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}
	ent->m_CBodyComponent->m_pSceneNode->m_pOwner = owner;
}

extern "C" PLUGIN_API void GetEntityAbsOrigin(Vector& output, int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
}
extern "C" PLUGIN_API void SetEntityAbsOrigin(int entityIndex, const Vector& origin)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = origin;
}

extern "C" PLUGIN_API void GetEntityAngRotation(QAngle& output, int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_angRotation();

}
extern "C" PLUGIN_API void SetEntityAngRotation(int entityIndex, const QAngle& angle)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_angRotation = angle;
}	

extern "C" PLUGIN_API void GetEntityAbsVelocity(Vector& output, int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->m_vecAbsVelocity();

}
extern "C" PLUGIN_API void SetEntityAbsVelocity(int entityIndex, const Vector& velocity)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_vecAbsVelocity = velocity;
}	

///


extern "C" PLUGIN_API float GetEntityWaterLevel(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0.0f;
	}
	
	return ent->m_flWaterLevel();

}
extern "C" PLUGIN_API CBaseEntity2* GetEntityGroundEntity(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return nullptr;
	}
	
	return ent->m_hGroundEntity();

}
extern "C" PLUGIN_API int GetEntityEffects(int entityIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_fEffects();
}