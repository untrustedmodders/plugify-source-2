#pragma once

#include <engine/IEngineTrace.h>
#include <engine/IEngineSound.h>
#include <edict.h>
#include <eiface.h>
#include <filesystem.h>
#include <worldsize.h>

extern "C" PLUGIN_API void GetGameDirectory(std::string& result)
{
	result = Plat_GetGameDirectory();
}

extern "C" PLUGIN_API void GetMapName(std::string& result)
{
	if (gpGlobals == nullptr)
		return;

	result = gpGlobals->mapname.ToCStr();
}

extern "C" PLUGIN_API bool IsMapValid(const std::string& mapname)
{
	return g_pEngineServer2->IsMapValid(mapname.c_str()) != 0;
}

extern "C" PLUGIN_API float GetGameTime()
{
	return gpGlobals->curtime;
}

extern "C" PLUGIN_API int GetGameTickCount()
{
	return gpGlobals->tickcount;
}

extern "C" PLUGIN_API float GetGameFrameTime()
{
	return gpGlobals->frametime;
}

extern "C" PLUGIN_API double GetEngineTime()
{
	return Plat_FloatTime();
}

extern "C" PLUGIN_API int GetMaxClients()
{
	auto globalVars = gpGlobals;
	if (globalVars == nullptr) {
		g_Logger.Error("Global Variables not initialized yet.");
		return -1;
	}

	return globalVars->maxClients;
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
	return TimerSystem::GetTickedInterval();
}

extern "C" PLUGIN_API double GetTickedTime()
{
	return TimerSystem::GetTickedTime();
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

//extern "C" PLUGIN_API void EmitSound(int client, int entitySource, int channel, const std::string& sound, float volume, float attenuation, int flags, int pitch, int origin, int direction)
//{
//   auto recipients = new CustomRecipientFilter();
//   recipients->AddPlayer(client);
//
//   g_pEngineSound->EmitSound(static_cast<IRecipientFilter&>(*recipients),
//                             entitySource,channel, sound.c_str(), -1, sound, volume,
//                             attenuation, 0, flags, pitch, origin, direction);
//}