#include <plugin_export.h>

#include <core/sdk/entity/cgamerules.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

extern CBaseGameRulesProxy* g_pGameRulesProxy;
extern CBaseGameRules* g_pGameRules;

/**
 * @brief Retrieves the pointer to the current game rules proxy entity instance.
 *
 * This function provides access to the global game rules object,
 * which contains various methods and properties defining the
 * rules and logic for the game.
 *
 * @return A pointer to the game rules proxy entity instance.
 */
extern "C" PLUGIN_API void* GetGameRulesProxy() {
	return g_pGameRulesProxy;
}

/**
 * @brief Retrieves the pointer to the current game rules instance.
 *
 * This function provides access to the global game rules object,
 * which contains various methods and properties defining the
 * rules and logic for the game.
 *
 * @return A pointer to the game rules object.
 */
extern "C" PLUGIN_API void* GetGameRules() {
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
extern "C" PLUGIN_API void TerminateRound(float delay, CSRoundEndReason reason) {
	if (g_pGameRules == nullptr) {
		S2_LOG(LS_WARNING, "cs_gamerules not instantiated yet.\n");
		return;
	}

	g_pGameRules->TerminateRound(delay, reason);
}

PLUGIFY_WARN_POP()
