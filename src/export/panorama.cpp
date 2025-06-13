#include <core/panorama_vote.hpp>
#include <plugin_export.h>
#include <core/sdk/entity/recipientfilters.h>

/**
 * @brief Start a new Yes/No vote
 *
 * @param duration		Maximum time to leave vote active for
 * @param caller		Player slot of the vote caller. Use VOTE_CALLER_SERVER for 'Server'.
 * @param voteTitle		Translation string to use as the vote message. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param detailStr		Extra string used in some vote translation strings.
 * @param voteTitle		Translation string to use as the vote message. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param votePassTitle	Translation string to use as the vote message when the vote passes. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param detailPassStr Extra string used in some vote translation strings when the vote passes.
 * @param failReason	Reason for the vote to fail, used in some translation strings.
 * @param result		Called when a menu action is completed.
 * @param handler		Called when the vote has finished.
 */
extern "C" PLUGIN_API bool PanoramaSendYesNoVote(double duration, int caller, const plg::string& voteTitle, const plg::string& detailStr, const plg::string& votePassTitle, const plg::string& detailPassStr, int failReason, uint64 recipientMask, YesNoVoteResult result, YesNoVoteHandler handler) {
    return g_PanoramaVoteHandler.SendYesNoVote(duration, caller, voteTitle, detailStr, votePassTitle, detailPassStr, failReason, recipientMask, result, handler);
}

/**
 * @brief Start a new Yes/No vote with all players included
 *
 * @param duration		Maximum time to leave vote active for
 * @param caller		Player slot of the vote caller. Use VOTE_CALLER_SERVER for 'Server'.
 * @param voteTitle		Translation string to use as the vote message. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param detailStr		Extra string used in some vote translation strings.
 * @param votePassTitle	Translation string to use as the vote message when the vote passes. (Only '#SFUI_vote' or '#Panorama_vote' strings)
 * @param detailPassStr Extra string used in some vote translation strings when the vote passes.
 * @param failReason	Reason for the vote to fail, used in some translation strings.
 * @param result		Called when a menu action is completed.
 * @param handler		Called when the vote has finished.
 */
extern "C" PLUGIN_API bool PanoramaSendYesNoVoteToAll(double duration, int caller, const plg::string& voteTitle, const plg::string& detailStr, const plg::string& votePassTitle, const plg::string& detailPassStr, int failReason, YesNoVoteResult result, YesNoVoteHandler handler) {
    return g_PanoramaVoteHandler.SendYesNoVote(duration, caller, voteTitle, detailStr, votePassTitle, detailPassStr, failReason, static_cast<uint64>(-1), result, handler);
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
