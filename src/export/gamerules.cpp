#include "core/sdk/entity/cgamerules.h"
#include <plugin_export.h>

extern CCSGameRules* g_pGameRules;

/**
 * @brief Retrieves the pointer to the current game rules instance.
 *
 * This function provides access to the global game rules object,
 * which contains various methods and properties defining the
 * rules and logic for the game.
 *
 * @return A pointer to the game rules object.
 */
extern "C" PLUGIN_API void* GetGameRules()
{
	return g_pGameRules;
}

/**
 * @brief Forces the round to end with a specified reason and delay.
 *
 * This function allows the caller to end the current round, specifying
 * the reason for the termination and the time delay before a new round begins.
 *
 * @param delay Time (in seconds) to delay before the next round starts.
 * @param reason The reason for ending the round, defined by the CSRoundEndReason enum.
 */
extern "C" PLUGIN_API void TerminateRound(float delay, CSRoundEndReason reason)
{
	g_pGameRules->TerminateRound(delay, reason);
}