#pragma once

extern "C" PLUGIN_API CBaseEntity* GetEntityFromIndex(int entityIndex)
{
	return g_pEntitySystem->GetBaseEntity(CEntityIndex(entityIndex));
}

extern "C" PLUGIN_API int GetUserIdFromIndex(int entityIndex)
{
	// CPlayerSlot is 1 less than index
	return g_pEngineServer2->GetPlayerUserId(CPlayerSlot(entityIndex - 1)).Get();
}

extern "C" PLUGIN_API void GetDesignerName(std::string& output, CBaseEntity* entity)
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

extern "C" PLUGIN_API uint64_t GetPlayerAuthorizedSteamID(int iSlot)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(iSlot);
	if (pPlayer == nullptr || !pPlayer->m_bAuthorized)
	{
		return -1;
	}

	auto pSteamId = pPlayer->GetSteamId();
	if (pSteamId == nullptr)
	{
		return -1;
	}

	return pSteamId->ConvertToUint64();
}

extern "C" PLUGIN_API void GetPlayerIpAddress(std::string& output, int iSlot)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(iSlot);
	if (pPlayer == nullptr)
	{
		return;
	}

	output = pPlayer->GetIpAddress();
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