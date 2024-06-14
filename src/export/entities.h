#pragma once

#include <core/sdk/entity/CBaseEntity.h>
#include <core/sdk/entity/cbasemodelentity.h>

extern "C" PLUGIN_API CBaseEntity* GetEntityFromIndex(int entityIndex)
{
	return static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
}

extern "C" PLUGIN_API int GetIndexFromEntity(CBaseEntity* entity)
{
	return entity->entindex();
}

extern "C" PLUGIN_API int GetEntityIndexFromRef(uint32_t ref)
{
	if (ref == INVALID_EHANDLE_INDEX)
	{
		return -1;
	}

	CBaseHandle hndl(ref);

	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(hndl));
	if (!ent)
	{
		return -1;
	}

	return ent->entindex();
}

extern "C" PLUGIN_API uint32_t GetRefFromEntityIndex(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	auto hndl = ent->GetRefEHandle();

	if (hndl == INVALID_EHANDLE_INDEX)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return hndl.ToInt();
}

extern "C" PLUGIN_API void* GetEntityPointerFromRef(uint32_t ref)
{
	if (ref == INVALID_EHANDLE_INDEX)
	{
		return nullptr;
	}

	CBaseHandle hndl(ref);

	return g_pEntitySystem->GetEntityInstance(hndl);
}

extern "C" PLUGIN_API uint32_t GetRefFromEntityPointer(CBaseEntity* entity)
{
	if (entity == nullptr)
	{
		return INVALID_EHANDLE_INDEX;
	}

	auto hndl = entity->GetRefEHandle();

	if (hndl == INVALID_EHANDLE_INDEX)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return hndl.ToInt();
}

extern "C" PLUGIN_API void* GetEntityPointerFromHandle(CEntityHandle* handle)
{
	if (!handle->IsValid())
	{
		return nullptr;
	}

	return g_pEntitySystem->GetEntityInstance(*handle);
}

extern "C" PLUGIN_API int GetEntityIndexFromHandle(CEntityHandle* handle)
{
	if (!handle->IsValid())
	{
		return -1;
	}

	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(*handle));
	if (!ent)
	{
		return -1;
	}

	return ent->entindex();
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

	return g_pEntitySystem->GetEntityInstance(hndl) != nullptr;
}

extern "C" PLUGIN_API void* GetFirstActiveEntity()
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

extern "C" PLUGIN_API int FindEntityByClassname(int startEntity, const std::string& szName)
{
	CBaseEntity* start = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(startEntity)));
	if (!start)
	{
		return -1;
	}

	CBaseEntity* ent = static_cast<CBaseEntity*>(addresses::CGameEntitySystem_FindEntityByClassName(g_pEntitySystem, start, szName.c_str()));
	if (!ent)
	{
		return -1;
	}

	return ent->entindex();
}

extern "C" PLUGIN_API int FindEntityByName(int startEntity, const std::string& szName)
{
	CBaseEntity* start = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(startEntity)));
	if (!start)
	{
		return -1;
	}

	CBaseEntity* ent = static_cast<CBaseEntity*>(addresses::CGameEntitySystem_FindEntityByName(g_pEntitySystem, start, szName.c_str(), nullptr, nullptr, nullptr, nullptr));
	if (!ent)
	{
		return -1;
	}

	return ent->entindex();
}

extern "C" PLUGIN_API int CreateEntityByName(const std::string& className)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(addresses::CreateEntityByName(className.c_str(), -1));
	if (!ent)
	{
		return -1;
	}

	return ent->entindex();
}

extern "C" PLUGIN_API void DispatchSpawn(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->DispatchSpawn();
}

///

extern "C" PLUGIN_API void GetEntityClassname(std::string& output, int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->GetClassname();
}

extern "C" PLUGIN_API void GetEntityName(std::string& output, int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	// TODO:
}

extern "C" PLUGIN_API void SetEntityName(int entityIndex, const std::string& name)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	// TODO:
}

extern "C" PLUGIN_API int GetEntityMoveType(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_MoveType();
}
extern "C" PLUGIN_API void SetEntityMoveType(int entityIndex, int moveType)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_MoveType = static_cast<MoveType_t>(moveType);
}

extern "C" PLUGIN_API float GetEntityGravity(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0.0f;
	}

	return ent->m_flGravityScale();
}
extern "C" PLUGIN_API void SetEntityGravity(int entityIndex, float gravity)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_flGravityScale = gravity;
}

extern "C" PLUGIN_API int GetEntityFlags(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_fFlags();
}
extern "C" PLUGIN_API void SetEntityFlags(int entityIndex, int flags)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_fFlags = flags;
}

extern "C" PLUGIN_API int GetEntityRenderColor(int entityIndex)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_clrRender().GetRawColor();
}
extern "C" PLUGIN_API void SetEntityRenderColor(int entityIndex, int color)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
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
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_nRenderMode();
}
extern "C" PLUGIN_API void SetEntityRenderMode(int entityIndex, int8_t renderMode)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_nRenderMode = renderMode;
}

extern "C" PLUGIN_API int GetEntityHealth(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iHealth();
}
extern "C" PLUGIN_API void SetEntityHealth(int entityIndex, int health)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_iHealth = health;
}

extern "C" PLUGIN_API int GetTeamEntity(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iTeamNum();
}
extern "C" PLUGIN_API void SetTeamEntity(int entityIndex, int team)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_iTeamNum = team;
}

extern "C" PLUGIN_API int GetEntityOwner(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return -1;
	}

	return ent->m_CBodyComponent->m_pSceneNode->m_pOwner()->GetEntityIndex().Get();

}
extern "C" PLUGIN_API void SetEntityOwner(int entityIndex, int ownerIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	CBaseEntity* owner = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(ownerIndex)));
	if (!owner)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_pOwner = owner;
}

extern "C" PLUGIN_API void GetEntityAbsOrigin(Vector& output, int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
}
extern "C" PLUGIN_API void SetEntityAbsOrigin(int entityIndex, const Vector& origin)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = origin;
}

extern "C" PLUGIN_API void GetEntityAngRotation(QAngle& output, int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_angRotation();

}
extern "C" PLUGIN_API void SetEntityAngRotation(int entityIndex, const QAngle& angle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_angRotation = angle;
}

extern "C" PLUGIN_API void GetEntityAbsVelocity(Vector& output, int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	output = ent->m_vecAbsVelocity();

}
extern "C" PLUGIN_API void SetEntityAbsVelocity(int entityIndex, const Vector& velocity)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->m_vecAbsVelocity = velocity;
}

///

extern "C" PLUGIN_API float GetEntityWaterLevel(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0.0f;
	}

	return ent->m_flWaterLevel();

}
extern "C" PLUGIN_API int GetEntityGroundEntity(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return -1;
	}

	return ent->m_hGroundEntity()->entindex();

}
extern "C" PLUGIN_API int GetEntityEffects(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_fEffects();
}

extern "C" PLUGIN_API void TeleportEntity(int entityIndex, const Vector* origin, const QAngle* angles, const Vector* velocity)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	ent->Teleport(origin, angles, velocity);
}