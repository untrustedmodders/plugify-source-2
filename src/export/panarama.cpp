#include <core/panorama_vote.hpp>
#include <plugin_export.h>

/**
 * @brief Start a new Yes/No vote
 *
 * @param duration		Maximum time to leave vote active for
 * @param caller		Player slot of the vote caller. Use VOTE_CALLER_SERVER for 'Server'.
 * @param voteTitle		Translation string to use as the vote message. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param detailStr		Extra string used in some vote translation strings.
 * @param filter		Recipient filter with all the clients who are allowed to participate in the vote.
 * @param result		Called when a menu action is completed.
 * @param handler		Called when the vote has finished.
 */
extern "C" PLUGIN_API bool PanoramaSendYesNoVote(double duration, int caller, const plg::string& voteTitle, const plg::string& detailStr, IRecipientFilter* filter, YesNoVoteResult result, YesNoVoteHandler handler) {
    return g_PanoramaVoteHandler.SendYesNoVote(duration, caller, voteTitle, detailStr, filter, result, handler);
}

/**
 * @brief Start a new Yes/No vote with all players included
 *
 * @param duration		Maximum time to leave vote active for
 * @param caller		Player slot of the vote caller. Use VOTE_CALLER_SERVER for 'Server'.
 * @param voteTitle		Translation string to use as the vote message. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param detailStr		Extra string used in some vote translation strings.
 * @param result		Called when a menu action is completed.
 * @param handler		Called when the vote has finished.
 */
extern "C" PLUGIN_API bool PanoramaSendYesNoVoteToAll(double duration, int caller, const plg::string& voteTitle, const plg::string& detailStr, YesNoVoteResult result, YesNoVoteHandler handler) {
    return g_PanoramaVoteHandler.SendYesNoVoteToAll(duration, caller, voteTitle, detailStr, result, handler);
}

/**
 * @brief Removes a player from the current vote.
 *
 * @param playerSlot The slot/index of the player to remove from the vote.
 */
extern "C" PLUGIN_API void PanoramaRemovePlayerFromVote(int playerSlot) {
    g_PanoramaVoteHandler.RemovePlayerFromVote(playerSlot);
}

/**
 * @brief Checks if a player is in the vote pool.
 *
 * @param playerSlot The slot/index of the player to check.
 * @return true if the player is in the vote pool, false otherwise.
 */
extern "C" PLUGIN_API bool PanoramaIsPlayerInVotePool(int playerSlot) {
    return g_PanoramaVoteHandler.IsPlayerInVotePool(playerSlot);
}

/**
 * @brief Redraws the vote UI to a specific player client.
 *
 * @param playerSlot The slot/index of the player to update.
 * @return true if the vote UI was successfully redrawn, false otherwise.
 */
extern "C" PLUGIN_API bool PanoramaRedrawVoteToClient(int playerSlot) {
    return g_PanoramaVoteHandler.RedrawVoteToClient(playerSlot);
}

/**
 * @brief Checks if a vote is currently in progress.
 *
 * @return true if a vote is active, false otherwise.
 */
extern "C" PLUGIN_API bool PanoramaIsVoteInProgress() {
    return g_PanoramaVoteHandler.IsVoteInProgress();
}

/**
 * @brief Ends the current vote with a specified reason.
 *
 * @param reason The reason for ending the vote.
 */
extern "C" PLUGIN_API void PanoramaEndVote(VoteEndReason reason) {
    g_PanoramaVoteHandler.EndVote(reason);
}
