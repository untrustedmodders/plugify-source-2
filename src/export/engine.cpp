#pragma once

#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/utils.h>
#include <core/server_manager.h>
#include <core/timer_system.h>
#include <entity2/entitysystem.h>
#include <engine/IEngineSound.h>
#include <plugin_export.h>

extern "C" PLUGIN_API void GetGameDirectory(std::string& result)
{
	std::construct_at(&result, Plat_GetGameDirectory());
}

extern "C" PLUGIN_API void GetMapName(std::string& result)
{
	if (gpGlobals == nullptr)
		return;

	std::construct_at(&result, gpGlobals->mapname.ToCStr());
}

extern "C" PLUGIN_API bool IsMapValid(const std::string& mapname)
{
	return g_pEngineServer2->IsMapValid(mapname.c_str()) != 0;
}

extern "C" PLUGIN_API float GetGameTime()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Warning("Global Variables not initialized yet.\n");
		return 0.0f;
	}

	return gpGlobals->curtime;
}

extern "C" PLUGIN_API int GetGameTickCount()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Warning("Global Variables not initialized yet.\n");
		return 0;
	}

	return gpGlobals->tickcount;
}

extern "C" PLUGIN_API float GetGameFrameTime()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Warning("Global Variables not initialized yet.\n");
		return 0.0f;
	}

	return gpGlobals->frametime;
}

extern "C" PLUGIN_API double GetEngineTime()
{
	return Plat_FloatTime();
}

extern "C" PLUGIN_API int GetMaxClients()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Warning("Global Variables not initialized yet.\n");
		return -1;
	}

	return gpGlobals->maxClients;
}

extern "C" PLUGIN_API int PrecacheModel(const std::string& model)
{
	return g_pEngineServer2->PrecacheGeneric(model.c_str());
}

extern "C" PLUGIN_API void IsModelPrecache(const std::string& model)
{
	g_pEngineServer2->IsGenericPrecached(model.c_str());
}

extern "C" PLUGIN_API bool PrecacheSound(const std::string& sound, bool preload)
{
	return g_pEngineSound->PrecacheSound(sound.c_str(), preload);
}

extern "C" PLUGIN_API bool IsSoundPrecached(const std::string& sound)
{
	return g_pEngineSound->IsSoundPrecached(sound.c_str());
}

extern "C" PLUGIN_API int PrecacheDecal(const std::string& decal, bool preload)
{
	return g_pEngineServer2->PrecacheDecal(decal.c_str(), preload);
}

extern "C" PLUGIN_API bool IsDecalPrecached(const std::string& decal)
{
	return g_pEngineServer2->IsDecalPrecached(decal.c_str());
}

extern "C" PLUGIN_API float GetTickInterval()
{
	return CTimerSystem::GetTickedInterval();
}

extern "C" PLUGIN_API double GetTickedTime()
{
	return CTimerSystem::GetTickedTime();
}

extern "C" PLUGIN_API void* GetEconItemSystem()
{
	return g_pSource2Server->GetEconItemSystem();
}

extern "C" PLUGIN_API bool IsServerPaused()
{
	return g_pSource2Server->IsPaused();
}

extern "C" PLUGIN_API void QueueTaskForNextFrame(TaskCallback callback)
{
	g_ServerManager.AddTaskForNextFrame(callback);
}

extern "C" PLUGIN_API void QueueTaskForNextWorldUpdate(TaskCallback callback)
{
	g_ServerManager.AddTaskForNextWorldUpdate(callback);
}

extern "C" PLUGIN_API float GetSoundDuration(const std::string& name)
{
	return g_pEngineSound->GetSoundDuration(name.c_str());
}

extern "C" PLUGIN_API void EmitSound(int entityIndex, const std::string& sound, int pitch, float volume, float delay)
{
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(entityIndex)));
	if (!ent)
	{
		return;
	}

	addresses::CBaseEntity_EmitSoundParams(ent, sound.c_str(), pitch, volume, delay);
}

extern "C" PLUGIN_API void EmitSoundToClient(int clientIndex, int channel, const std::string& sound, float volume, int soundLevel, int flags, int pitch, const Vector& origin, float soundTime)
{
	utils::PlaySoundToClient(CPlayerSlot(clientIndex - 1), channel, sound.c_str(), volume, static_cast<soundlevel_t>(soundLevel), flags, pitch, origin, soundTime);
}



