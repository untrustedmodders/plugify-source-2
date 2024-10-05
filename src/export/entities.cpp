#include <core/output_manager.h>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbasemodelentity.h>
#include <plugin_export.h>

extern "C" PLUGIN_API void* EntIndexToEntPointer(int entityIndex)
{
	return static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
}

extern "C" PLUGIN_API int EntPointerToEntIndex(CBaseEntity* entity)
{
	return entity->entindex();
}

extern "C" PLUGIN_API int EntPointerToEntHandle(CBaseEntity* entity)
{
	if (entity == nullptr)
	{
		return INVALID_EHANDLE_INDEX;
	}

	auto hndl = entity->GetRefEHandle();

	if (!hndl.IsValid())
	{
		return INVALID_EHANDLE_INDEX;
	}

	return hndl.ToInt();
}

extern "C" PLUGIN_API void* EntHandleToEntPointer(int entityHandle)
{
	CEntityHandle handle((uint32)entityHandle);

	if (!handle.IsValid())
	{
		return nullptr;
	}

	return g_pEntitySystem->GetEntityInstance(handle);
}

extern "C" PLUGIN_API int EntIndexToEntHandle(int entityIndex)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	auto handle = ent->GetRefEHandle();

	if (!handle.IsValid())
	{
		return INVALID_EHANDLE_INDEX;
	}

	return handle.ToInt();
}

extern "C" PLUGIN_API int EntHandleToEntIndex(int entityHandle)
{
	CEntityHandle handle((uint32)entityHandle);

	if (!handle.IsValid())
	{
		return -1;
	}

	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(handle));
	if (!ent)
	{
		return -1;
	}

	return ent->entindex();
}

extern "C" PLUGIN_API bool IsValidEntHandle(int entityHandle)
{
	CEntityHandle handle((uint32)entityHandle);

	if (!handle.IsValid())
	{
		return false;
	}

	return g_pEntitySystem->GetEntityInstance(handle) != nullptr;
}

extern "C" PLUGIN_API void* GetFirstActiveEntity()
{
	return g_pEntitySystem->m_EntityList.m_pFirstActiveEntity;
}

extern "C" PLUGIN_API void* GetConcreteEntityListPointer()
{
	return &g_pEntitySystem->m_EntityList;
}

extern "C" PLUGIN_API void HookEntityOutput(const plg::string& szClassname, const plg::string& szOutput, EntityListenerCallback callback, bool post)
{
	g_OutputManager.HookEntityOutput(szClassname, szOutput, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API void UnhookEntityOutput(const plg::string& szClassname, const plg::string& szOutput, EntityListenerCallback callback, bool post)
{
	g_OutputManager.UnhookEntityOutput(szClassname, szOutput, callback, static_cast<HookMode>(post));
}

////////////////////////


extern "C" PLUGIN_API int FindEntityByClassname(int startEntity, const plg::string& szName)
{
	CBaseEntity* start = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle(startEntity)));
	/*if (!start)
	{
		return INVALID_EHANDLE_INDEX;
	}*/

	CBaseEntity* ent = static_cast<CBaseEntity*>(addresses::CGameEntitySystem_FindEntityByClassName(g_pEntitySystem, start, szName.c_str()));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return ent->GetRefEHandle().ToInt();
}

extern "C" PLUGIN_API int FindEntityByName(int startEntity, const plg::string& szName)
{
	CBaseEntity* start = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle(startEntity)));
	/*if (!start)
	{
		return INVALID_EHANDLE_INDEX;
	}*/

	CBaseEntity* ent = static_cast<CBaseEntity*>(addresses::CGameEntitySystem_FindEntityByName(g_pEntitySystem, start, szName.c_str(), nullptr, nullptr, nullptr, nullptr));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return ent->GetRefEHandle().ToInt();
}

extern "C" PLUGIN_API int CreateEntityByName(const plg::string& className)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(addresses::CreateEntityByName(className.c_str(), -1));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return ent->GetRefEHandle().ToInt();
}

extern "C" PLUGIN_API void DispatchSpawn(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->DispatchSpawn();
}

extern "C" PLUGIN_API void RemoveEntity(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->Remove();
}

///

extern "C" PLUGIN_API void GetEntityClassname(plg::string& output, int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	std::construct_at(&output, ent->GetClassname());
}

extern "C" PLUGIN_API void GetEntityName(plg::string& output, int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	std::construct_at(&output, ent->GetName());
}

extern "C" PLUGIN_API void SetEntityName(int entityHandle, const plg::string& name)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->SetName(name.c_str());
}

extern "C" PLUGIN_API int GetEntityMoveType(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_MoveType();
}
extern "C" PLUGIN_API void SetEntityMoveType(int entityHandle, int moveType)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->SetMoveType(static_cast<MoveType_t>(moveType));
}

extern "C" PLUGIN_API float GetEntityGravity(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0.0f;
	}

	return ent->m_flGravityScale();
}
extern "C" PLUGIN_API void SetEntityGravity(int entityHandle, float gravity)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_flGravityScale = gravity;
}

extern "C" PLUGIN_API int GetEntityFlags(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_fFlags();
}
extern "C" PLUGIN_API void SetEntityFlags(int entityHandle, int flags)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_fFlags = flags;
}

extern "C" PLUGIN_API int GetEntityRenderColor(int entityHandle)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_clrRender().GetRawColor();
}
extern "C" PLUGIN_API void SetEntityRenderColor(int entityHandle, int color)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	Color clr;
	clr.SetRawColor(color);
	ent->m_clrRender = clr;
}

extern "C" PLUGIN_API int8_t GetEntityRenderMode(int entityHandle)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_nRenderMode();
}
extern "C" PLUGIN_API void SetEntityRenderMode(int entityHandle, int8_t renderMode)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_nRenderMode = renderMode;
}

extern "C" PLUGIN_API int GetEntityHealth(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iHealth();
}
extern "C" PLUGIN_API void SetEntityHealth(int entityHandle, int health)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_iHealth = health;
}

extern "C" PLUGIN_API int GetTeamEntity(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iTeamNum();
}
extern "C" PLUGIN_API void SetTeamEntity(int entityHandle, int team)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_iTeamNum = team;
}

extern "C" PLUGIN_API int GetEntityOwner(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return ent->m_CBodyComponent->m_pSceneNode->m_pOwner()->GetRefEHandle().ToInt();

}
extern "C" PLUGIN_API void SetEntityOwner(int entityHandle, int ownerHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	CBaseEntity* owner = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)ownerHandle)));
	if (!owner)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_pOwner = owner;
}

extern "C" PLUGIN_API int GetEntityParent(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return ent->m_CBodyComponent->m_pSceneNode->m_pParent()->m_pOwner->GetRefEHandle().ToInt();

}
extern "C" PLUGIN_API void SetEntityParent(int entityHandle, int parentHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	CBaseEntity* owner = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)parentHandle)));
	if (!owner)
	{
		return;
	}

	ent->SetParent(owner);
}

extern "C" PLUGIN_API void GetEntityAbsOrigin(Vector& output, int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
}
extern "C" PLUGIN_API void SetEntityAbsOrigin(int entityHandle, const Vector& origin)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = origin;
}

extern "C" PLUGIN_API void GetEntityAngRotation(QAngle& output, int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	std::construct_at(&output, ent->m_CBodyComponent->m_pSceneNode->m_angRotation());

}
extern "C" PLUGIN_API void SetEntityAngRotation(int entityHandle, const QAngle& angle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_CBodyComponent->m_pSceneNode->m_angRotation = angle;
}

extern "C" PLUGIN_API void GetEntityAbsVelocity(Vector& output, int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	output = ent->m_vecAbsVelocity();

}
extern "C" PLUGIN_API void SetEntityAbsVelocity(int entityHandle, const Vector& velocity)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->m_vecAbsVelocity = velocity;
}

extern "C" PLUGIN_API void GetEntityModel(plg::string& output, int entityHandle)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	std::construct_at(&output, ent->GetModelName());

}
extern "C" PLUGIN_API void SetEntityModel(int entityHandle, const plg::string& model)
{
	CBaseModelEntity* ent = static_cast<CBaseModelEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->SetModel(model.c_str());
}

///

extern "C" PLUGIN_API float GetEntityWaterLevel(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0.0f;
	}

	return ent->m_flWaterLevel();

}
extern "C" PLUGIN_API int GetEntityGroundEntity(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return ent->m_hGroundEntity()->GetRefEHandle().ToInt();

}
extern "C" PLUGIN_API int GetEntityEffects(int entityHandle)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_fEffects();
}

extern "C" PLUGIN_API void TeleportEntity(int entityHandle, const Vector* origin, const QAngle* angles, const Vector* velocity)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!ent)
	{
		return;
	}

	ent->Teleport(origin, angles, velocity);
}