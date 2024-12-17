#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/utils.h>
#include <core/server_manager.hpp>
#include <core/timer_system.hpp>
#include <engine/IEngineSound.h>
#include <entity2/entitysystem.h>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Returns the path of the game's directory.
 *
 * @param result A reference to a string where the game directory path will be stored.
 */
extern "C" PLUGIN_API void GetGameDirectory(plg::string& result)
{
	std::construct_at(&result, Plat_GetGameDirectory());
}

/**
 * @brief Returns the current map name.
 *
 * @param result A reference to a string where the current map name will be stored.
 */
extern "C" PLUGIN_API void GetCurrentMap(plg::string& result)
{
	if (gpGlobals == nullptr)
		return;

	std::construct_at(&result, gpGlobals->mapname.ToCStr());
}

/**
 * @brief Returns whether a specified map is valid or not.
 *
 * @param mapname The name of the map to check for validity.
 * @return True if the map is valid, false otherwise.
 */
extern "C" PLUGIN_API bool IsMapValid(const plg::string& mapname)
{
	return g_pEngineServer2->IsMapValid(mapname.c_str()) != 0;
}

/**
 * @brief Returns the game time based on the game tick.
 *
 * @return The current game time.
 */
extern "C" PLUGIN_API float GetGameTime()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Global Variables not initialized yet.\n");
		return 0.0f;
	}

	return gpGlobals->curtime;
}

/**
 * @brief Returns the game's internal tick count.
 *
 * @return The current tick count of the game.
 */
extern "C" PLUGIN_API int GetGameTickCount()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Global Variables not initialized yet.\n");
		return 0;
	}

	return gpGlobals->tickcount;
}

/**
 * @brief Returns the time the game took processing the last frame.
 *
 * @return The frame time of the last processed frame.
 */
extern "C" PLUGIN_API float GetGameFrameTime()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Global Variables not initialized yet.\n");
		return 0.0f;
	}

	return gpGlobals->frametime;
}

/**
 * @brief Returns a high-precision time value for profiling the engine.
 *
 * @return A high-precision time value.
 */
extern "C" PLUGIN_API double GetEngineTime()
{
	return Plat_FloatTime();
}

/**
 * @brief Returns the maximum number of clients that can connect to the server.
 *
 * @return The maximum client count, or -1 if global variables are not initialized.
 */
extern "C" PLUGIN_API int GetMaxClients()
{
	if (gpGlobals == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Global Variables not initialized yet.\n");
		return -1;
	}

	return gpGlobals->maxClients;
}

/**
 * @brief Precaches a given generic file.
 *
 * @param model The name of the model to be precached.
 * @return An integer identifier for the generic file.
 */
extern "C" PLUGIN_API int PrecacheGeneric(const plg::string& model)
{
	return g_pEngineServer2->PrecacheGeneric(model.c_str());
}

/**
 * @brief Checks if a specified generic file is precached.
 *
 * @param model The name of the generic file to check.
 */
extern "C" PLUGIN_API bool IsGenericPrecache(const plg::string& model)
{
	return g_pEngineServer2->IsGenericPrecached(model.c_str());
}

/**
 * @brief Precaches a specified model.
 *
 * @param model The name of the model to be precached.
 * @return An integer identifier for the model.
 */
extern "C" PLUGIN_API int PrecacheModel(const plg::string& model)
{
	return g_pEngineServer2->PrecacheGeneric(model.c_str());
}

/**
 * @brief Checks if a specified model is precached.
 *
 * @param model The name of the model to check.
 */
extern "C" PLUGIN_API bool IsModelPrecache(const plg::string& model)
{
	return g_pEngineServer2->IsGenericPrecached(model.c_str());
}

/**
 * @brief Precaches a specified sound.
 *
 * @param sound The name of the sound to be precached.
 * @param preload A boolean indicating if the sound should be preloaded.
 * @return True if the sound is successfully precached, false otherwise.
 */
extern "C" PLUGIN_API bool PrecacheSound(const plg::string& sound, bool preload)
{
	return g_pEngineSound->PrecacheSound(sound.c_str(), preload);
}

/**
 * @brief Checks if a specified sound is precached.
 *
 * @param sound The name of the sound to check.
 * @return True if the sound is precached, false otherwise.
 */
extern "C" PLUGIN_API bool IsSoundPrecached(const plg::string& sound)
{
	return g_pEngineSound->IsSoundPrecached(sound.c_str());
}

/**
 * @brief Precaches a specified decal.
 *
 * @param decal The name of the decal to be precached.
 * @param preload A boolean indicating if the decal should be preloaded.
 * @return An integer identifier for the decal.
 */
extern "C" PLUGIN_API int PrecacheDecal(const plg::string& decal, bool preload)
{
	return g_pEngineServer2->PrecacheDecal(decal.c_str(), preload);
}

/**
 * @brief Checks if a specified decal is precached.
 *
 * @param decal The name of the decal to check.
 * @return True if the decal is precached, false otherwise.
 */
extern "C" PLUGIN_API bool IsDecalPrecached(const plg::string& decal)
{
	return g_pEngineServer2->IsDecalPrecached(decal.c_str());
}

/**
 * @brief Returns a pointer to the Economy Item System.
 *
 * @return A pointer to the Econ Item System.
 */
extern "C" PLUGIN_API void* GetEconItemSystem()
{
	return g_pSource2Server->GetEconItemSystem();
}

/**
 * @brief Checks if the server is currently paused.
 *
 * @return True if the server is paused, false otherwise.
 */
extern "C" PLUGIN_API bool IsServerPaused()
{
	return g_pSource2Server->IsPaused();
}

/**
 * @brief Queues a task to be executed on the next frame.
 *
 * @param callback A callback function to be executed on the next frame.
 * @param userData An array intended to hold user-related data, allowing for elements of any type.
 */
extern "C" PLUGIN_API void QueueTaskForNextFrame(TaskCallback callback, const plg::vector<plg::any>& userData)
{
	g_ServerManager.AddTaskForNextFrame(callback, userData);
}

/**
 * @brief Queues a task to be executed on the next world update.
 *
 * @param callback A callback function to be executed on the next world update.
 * @param userData An array intended to hold user-related data, allowing for elements of any type.
 */
extern "C" PLUGIN_API void QueueTaskForNextWorldUpdate(TaskCallback callback, const plg::vector<plg::any>& userData)
{
	g_ServerManager.AddTaskForNextWorldUpdate(callback, userData);
}

/**
 * @brief Returns the duration of a specified sound.
 *
 * @param name The name of the sound to check.
 * @return The duration of the sound in seconds.
 */
extern "C" PLUGIN_API float GetSoundDuration(const plg::string& name)
{
	return g_pEngineSound->GetSoundDuration(name.c_str());
}

/**
 * @brief Emits a sound from a specified entity.
 *
 * @param entityHandle The handle of the entity that will emit the sound.
 * @param sound The name of the sound to emit.
 * @param pitch The pitch of the sound.
 * @param volume The volume of the sound.
 * @param delay The delay before the sound is played.
 */
extern "C" PLUGIN_API void EmitSound(int entityHandle, const plg::string& sound, int pitch, float volume, float delay)
{
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pEntity)
	{
		return;
	}

	addresses::CBaseEntity_EmitSoundParams(pEntity, sound.c_str(), pitch, volume, delay);
}

/**
 * @brief Emits a sound to a specific client.
 *
 * @param clientIndex The index of the client to whom the sound will be emitted.
 * @param channel The channel through which the sound will be played.
 * @param sound The name of the sound to emit.
 * @param volume The volume of the sound.
 * @param soundLevel The level of the sound.
 * @param flags Additional flags for sound playback.
 * @param pitch The pitch of the sound.
 * @param origin The origin of the sound in 3D space.
 * @param soundTime The time at which the sound should be played.
 */
extern "C" PLUGIN_API void EmitSoundToClient(int clientIndex, int channel, const plg::string& sound, float volume, int soundLevel, int flags, int pitch, const Vector& origin, float soundTime)
{
	utils::PlaySoundToClient(CPlayerSlot(clientIndex), channel, sound.c_str(), volume, static_cast<soundlevel_t>(soundLevel), flags, pitch, origin, soundTime);
}

PLUGIFY_WARN_POP()
