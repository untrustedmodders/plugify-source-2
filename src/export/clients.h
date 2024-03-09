#pragma once

#include <core/sdk/entity/cbaseentity.h>

extern "C" PLUGIN_API uint64_t GetSteamAccountId(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
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

extern "C" PLUGIN_API void GetClientIp(std::string& output, int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return;
	}

	output = pPlayer->GetIpAddress();
}

extern "C" PLUGIN_API void GetClientName(std::string& output, int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return;
	}

	output = pPlayer->GetName();
}

extern "C" PLUGIN_API float GetClientTime(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return -1.0f;
	}

	return pPlayer->GetTimeConnected();
}

extern "C" PLUGIN_API float GetClientLatency(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return 0.0f;
	}

	return pPlayer->GetLatency();
}

extern "C" PLUGIN_API int GetClientUserId(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return 0;
	}

	return pPlayer->GetUserId();
}

extern "C" PLUGIN_API bool IsClientAuthorized(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsAuthorized();
}

extern "C" PLUGIN_API bool IsClientConnected(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsConnected();
}

extern "C" PLUGIN_API bool IsClientInGame(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsInGame();
}

extern "C" PLUGIN_API bool IsFakeClient(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsFakeClient();
}

extern "C" PLUGIN_API bool IsPlayerAlive(int clientIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(clientIndex + 1)));
	if (!ent)
	{
		return false;
	}

	return ent->IsAlive();
}

/////

extern "C" PLUGIN_API int GetClientTeam(int clientIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(clientIndex + 1)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iTeamNum();
}

extern "C" PLUGIN_API int GetClientHealth(int clientIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(clientIndex + 1)));
	if (!ent)
	{
		return 0;
	}

	return ent->m_iHealth();
}

extern "C" PLUGIN_API void GetClientAbsOrigin(Vector& output, int clientIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(clientIndex + 1)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
}



extern "C" PLUGIN_API void GetClientAbsAngles(QAngle& output, int clientIndex)
{
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(clientIndex + 1)));
	if (!ent)
	{
		return;
	}

	output = ent->m_CBodyComponent->m_pSceneNode->m_angRotation();
}

//ChangeTeam
//SwitchTeam
//CommitSuicide
//KickClient