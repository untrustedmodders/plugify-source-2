#pragma once

#include <core/player_manager.h>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/entity/ccsplayercontroller.h>
#include <core/sdk/utils.h>
#include <plugin_export.h>

extern "C" {

/**
 * @brief Retrieves a client's authentication string (SteamID).
 *
 * @param output Reference to a string where the authentication string will be stored.
 * @param clientIndex Index of the client whose authentication string is being retrieved.
 */
PLUGIN_API void GetClientAuthId(plg::string& output, int clientIndex);

/**
 * @brief Returns the client's Steam account ID, a unique number identifying a given Steam account.
 *
 * @param clientIndex Index of the client.
 * @return uint64_t Steam account ID.
 */
PLUGIN_API uint64_t GetClientAccountId(int clientIndex);

/**
 * @brief Retrieves a client's IP address.
 *
 * @param output Reference to a string where the IP address will be stored.
 * @param clientIndex Index of the client.
 */
PLUGIN_API void GetClientIp(plg::string& output, int clientIndex);

/**
 * @brief Returns the client's name.
 *
 * @param output Reference to a string where the client's name will be stored.
 * @param clientIndex Index of the client.
 */
PLUGIN_API void GetClientName(plg::string& output, int clientIndex);

/**
 * @brief Returns the client's connection time in seconds.
 *
 * @param clientIndex Index of the client.
 * @return float Connection time in seconds.
 */
PLUGIN_API float GetClientTime(int clientIndex);

/**
 * @brief Returns the client's current latency (RTT).
 *
 * @param clientIndex Index of the client.
 * @return float Latency value.
 */
PLUGIN_API float GetClientLatency(int clientIndex);

/**
 * @brief Returns the client's access flags.
 *
 * @param clientIndex Index of the client.
 * @return uint64 Access flags as a bitmask.
 */
PLUGIN_API uint64 GetUserFlagBits(int clientIndex);

/**
 * @brief Sets the access flags on a client using a bitmask.
 *
 * @param clientIndex Index of the client.
 * @param flags Bitmask representing the flags to be set.
 */
PLUGIN_API void SetUserFlagBits(int clientIndex, uint64 flags);

/**
 * @brief Adds access flags to a client.
 *
 * If the client is not an admin, they will be given a temporary AdminId.
 *
 * @param clientIndex Index of the client.
 * @param flags Bitmask representing the flags to be added.
 */
PLUGIN_API void AddUserFlags(int clientIndex, uint64 flags);

/**
 * @brief Removes access flags from a client.
 *
 * If the client is not an admin, this has no effect.
 *
 * @param clientIndex Index of the client.
 * @param flags Bitmask representing the flags to be removed.
 */
PLUGIN_API void RemoveUserFlags(int clientIndex, uint64 flags);

/**
 * @brief Checks if a certain player has been authenticated.
 *
 * @param clientIndex Index of the client.
 * @return true if the player is authenticated, false otherwise.
 */
PLUGIN_API bool IsClientAuthorized(int clientIndex);

/**
 * @brief Checks if a certain player is connected.
 *
 * @param clientIndex Index of the client.
 * @return true if the player is connected, false otherwise.
 */
PLUGIN_API bool IsClientConnected(int clientIndex);

/**
 * @brief Checks if a certain player has entered the game.
 *
 * @param clientIndex Index of the client.
 * @return true if the player is in the game, false otherwise.
 */
PLUGIN_API bool IsClientInGame(int clientIndex);

/**
 * @brief Checks if a certain player is the SourceTV bot.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is the SourceTV bot, false otherwise.
 */
PLUGIN_API bool IsClientSourceTV(int clientIndex);

/**
 * @brief Checks if the client is alive or dead.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is alive, false if dead.
 */
PLUGIN_API bool IsClientAlive(int clientIndex);

/**
 * @brief Checks if a certain player is a fake client.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is a fake client, false otherwise.
 */
PLUGIN_API bool IsFakeClient(int clientIndex);

/**
 * @brief Retrieves a client's team index.
 *
 * @param clientIndex Index of the client.
 * @return int The team index of the client.
 */
PLUGIN_API int GetClientTeam(int clientIndex);

/**
 * @brief Returns the client's health.
 *
 * @param clientIndex Index of the client.
 * @return int The health value of the client.
 */
PLUGIN_API int GetClientHealth(int clientIndex);

/**
 * @brief Returns the client's armor value.
 *
 * @param clientIndex Index of the client.
 * @return int The armor value of the client.
 */
PLUGIN_API int GetClientArmor(int clientIndex);

/**
 * @brief Retrieves the client's origin vector.
 *
 * @param output Reference to a Vector where the client's origin will be stored.
 * @param clientIndex Index of the client.
 */
PLUGIN_API void GetClientAbsOrigin(Vector& output, int clientIndex);

/**
 * @brief Retrieves the client's position angle.
 *
 * @param output Reference to a QAngle where the client's position angle will be stored.
 * @param clientIndex Index of the client.
 */
PLUGIN_API void GetClientAbsAngles(QAngle& output, int clientIndex);

/**
 * @brief Processes the target string to determine if one user can target another.
 *
 * @param output Reference to a vector where the result of the targeting operation will be stored.
 * @param caller Index of the client making the target request.
 * @param target The target string specifying the player or players to be targeted.
 */
PLUGIN_API void ProcessTargetString(std::vector<int>& output, int caller, const plg::string& target);

/**
 * @brief Changes a client's team.
 *
 * @param clientIndex Index of the client.
 * @param team The team index to assign the client to.
 */
PLUGIN_API void ChangeClientTeam(int clientIndex, int team);

/**
 * @brief Switches the player's team.
 *
 * @param clientIndex Index of the client.
 * @param team The team index to switch the client to.
 */
PLUGIN_API void SwitchClientTeam(int clientIndex, int team);

/**
 * @brief Respawns a player.
 *
 * @param clientIndex Index of the client to respawn.
 */
PLUGIN_API void RespawnClient(int clientIndex);

/**
 * @brief Forces a player to commit suicide.
 *
 * @param clientIndex Index of the client.
 * @param explode If true, the client will explode upon death.
 * @param force If true, the suicide will be forced.
 */
PLUGIN_API void ForcePlayerSuicide(int clientIndex, bool explode, bool force);

/**
 * @brief Disconnects a client from the server as soon as the next frame starts.
 *
 * @param clientIndex Index of the client to be kicked.
 */
PLUGIN_API void KickClient(int clientIndex);

/**
 * @brief Bans a client for a specified duration.
 *
 * @param clientIndex Index of the client to be banned.
 * @param duration Duration of the ban in seconds.
 * @param kick If true, the client will be kicked immediately after being banned.
 */
PLUGIN_API void BanClient(int clientIndex, float duration, bool kick);

/**
 * @brief Bans an identity (either an IP address or a Steam authentication string).
 *
 * @param steamId The Steam ID to ban.
 * @param duration Duration of the ban in seconds.
 * @param kick If true, the client will be kicked immediately after being banned.
 */
PLUGIN_API void BanIdentity(uint64_t steamId, float duration, bool kick);

}