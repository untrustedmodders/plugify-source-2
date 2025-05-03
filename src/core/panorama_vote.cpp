#include "panorama_vote.hpp"
#include "timer_system.hpp"
#include "server_manager.hpp"

#include <cstrike15_usermessages.pb.h>
#include <engine/igameeventsystem.h>
#include <networksystem/inetworkmessages.h>

#include "event_listener.hpp"
#include "server_manager.hpp"
#include "sdk/entity/ccsplayercontroller.h"
#include "sdk/entity/recipientfilters.h"

void CPanoramaVoteHandler::Reset() {
	m_voteInProgress = false;
	m_voteController = nullptr;
	m_voteHandler = nullptr;
	m_voteResult = nullptr;
	m_currentVoteTitle.clear();
	m_currentVoteDetailStr.clear();
}

void CPanoramaVoteHandler::Init() {
	if (m_voteInProgress)
		return;

	CVoteController* pVoteController = nullptr;
	while (nullptr != (pVoteController = static_cast<CVoteController *>(utils::FindEntityByClassname(pVoteController, "vote_controller")))) {
		m_voteController = pVoteController->GetHandle();
	}
}

// Called by vote_cast event
void CPanoramaVoteHandler::VoteCast(IGameEvent *event) {
	if (!m_voteController || !m_voteInProgress)
		return;

	if (m_voteHandler != nullptr) {
		CCSPlayerController* controller = static_cast<CCSPlayerController*>(event->GetPlayerController("userid"));
		if (!controller)
			return;

		m_voteHandler(VoteAction::Vote, controller->GetPlayerSlot(), event->GetInt("vote_option"));
	}

	CheckForEarlyVoteClose();
}

void CPanoramaVoteHandler::RemovePlayerFromVote(CPlayerSlot slot) {
	if (!m_voteInProgress)
		return;

	bool found = false;
	for (int i = 0; i < m_voterCount; i++)
		if (m_voters[i] == slot)
			found = true;
		else if (found)
			m_voters[i - 1] = m_voters[i];

	if (found) {
		m_voters[--m_voterCount] = -1;
	}
}

bool CPanoramaVoteHandler::IsPlayerInVotePool(CPlayerSlot slot) const {
	if (!m_voteInProgress)
		return false;

	if (slot < 0 || slot > MAXPLAYERS)
		return false;

	for (int i = 0; i < m_voterCount; i++)
		if (m_voters[i] == slot)
			return true;

	return false;
}

// Removes a client's vote and redraws the vote
bool CPanoramaVoteHandler::RedrawVoteToClient(CPlayerSlot slot) {
	if (!m_voteController)
		return false;

	int myVote = m_voteController->m_nVotesCast[slot];
	if (myVote != VOTE_UNCAST) {
		m_voteController->m_nVotesCast[slot] = VOTE_UNCAST;
		--m_voteController->m_nVoteOptionCount[myVote];

		UpdateVoteCounts();
	}

	CRecipientFilter filter;
	filter.AddRecipient(CPlayerSlot(slot));
	SendVoteStartUM(&filter);

	return true;
}

void CPanoramaVoteHandler::UpdateVoteCounts() const {
	if (IGameEvent* event = g_pGameEventManager->CreateEvent("vote_changed")) {
		event->SetInt("vote_option1", m_voteController->m_nVoteOptionCount[VOTE_OPTION1]);
		event->SetInt("vote_option2", m_voteController->m_nVoteOptionCount[VOTE_OPTION2]);
		event->SetInt("vote_option3", m_voteController->m_nVoteOptionCount[VOTE_OPTION3]);
		event->SetInt("vote_option4", m_voteController->m_nVoteOptionCount[VOTE_OPTION4]);
		event->SetInt("vote_option5", m_voteController->m_nVoteOptionCount[VOTE_OPTION5]);
		event->SetInt("potentialVotes", m_voterCount);

		g_pGameEventManager->FireEvent(event, false);
	}
}

bool CPanoramaVoteHandler::IsVoteInProgress() const {
	return m_voteInProgress;
}

bool CPanoramaVoteHandler::SendYesNoVote(double duration, int caller, const plg::string& voteTitle, const plg::string& detailStr,
                                         IRecipientFilter *filter, YesNoVoteResult result,
                                         YesNoVoteHandler handler = nullptr) {
	if (!m_voteController|| m_voteInProgress)
		return false;

	if (filter->GetRecipientCount() <= 0)
		return false;

	if (result == nullptr)
		return false;

	S2_LOGF(LS_MESSAGE, "[Vote Start] Starting a new vote [id:{}]. Duration:{} Caller:{} NumClients:{}", m_voteCount, duration, caller, filter->GetRecipientCount());

	m_voteInProgress = true;

	InitVoters(filter);

	m_voteController->m_nPotentialVotes = m_voterCount;
	m_voteController->m_bIsYesNoVote = true;
	m_voteController->m_iActiveIssueIndex = 2;

	m_voteController->m_iOnlyTeamToVote = -1; // use the recipient filter param to handle who votes

	m_voteResult = result;
	m_voteHandler = handler;

	m_currentVoteCaller = caller;
	m_currentVoteTitle = voteTitle;
	m_currentVoteDetailStr = detailStr;

	UpdateVoteCounts();

	SendVoteStartUM(filter);

	if (m_voteHandler != nullptr)
		m_voteHandler(VoteAction::Start, -1, VOTE_NOTINCLUDED);

	g_TimerSystem.CreateTimer(duration, [](uint32_t, const plg::vector<plg::any>& args) {
		int32_t voteNum = plg::get<int32_t>(args[0]);
		// Ensure we dont end the wrong vote
		if (voteNum == g_PanoramaVoteHandler.m_voteCount)
			g_PanoramaVoteHandler.EndVote(VoteEndReason::TimeUp);
	}, {}, {m_voteCount});

	return true;
}

void CPanoramaVoteHandler::SendVoteStartUM(IRecipientFilter* filter) {
	INetworkMessageInternal *pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("VoteStart");
	auto data = pNetMsg->AllocateMessage()->ToPB<CCSUsrMsg_VoteStart>();

	data->set_team(-1);
	data->set_player_slot(m_currentVoteCaller);
	data->set_vote_type(-1);
	data->set_disp_str(m_currentVoteTitle);
	data->set_details_str(m_currentVoteDetailStr);
	data->set_is_yes_no_vote(true);

	g_pGameEventSystem->PostEventAbstract(-1, false, filter, pNetMsg, data, 0);
}

void CPanoramaVoteHandler::InitVoters(IRecipientFilter* filter) {
	// Clear any old info
	m_voterCount = 0;

	for (int i = 0; i < MAXPLAYERS; ++i) {
		m_voters[i] = -1;
		m_voteController->m_nVotesCast[i] = VOTE_UNCAST;
	}

	for (int i = 0; i < VOTE_UNCAST; ++i) {
		m_voteController->m_nVoteOptionCount[i] = 0;
	}

	m_voterCount = filter->GetRecipientCount();
	for (int i = 0, j = 0; i < m_voterCount; ++i) {
		CPlayerSlot slot = filter->GetRecipientIndex(i);
		if (slot != INVALID_PLAYER_SLOT) {
			m_voters[j++] = slot;
		}
	}
}

void CPanoramaVoteHandler::CheckForEarlyVoteClose() const {
	int votes = 0;
	votes += m_voteController->m_nVoteOptionCount[VOTE_OPTION1];
	votes += m_voteController->m_nVoteOptionCount[VOTE_OPTION2];

	if (votes >= m_voterCount) {
		// Do this next frame to prevent a crash
		g_ServerManager.AddTaskForNextFrame([](const plg::vector<plg::any>&) {
			g_PanoramaVoteHandler.EndVote(VoteEndReason::AllVotes);
		});
	}
}

void CPanoramaVoteHandler::EndVote(VoteEndReason reason) {
	if (!m_voteInProgress)
		return;

	m_voteInProgress = false;

	switch (reason) {
		case VoteEndReason::AllVotes: {
			S2_LOGF(LS_MESSAGE, "[Vote Ending] [id:{}] All possible players voted.", m_voteCount);
			break;
		}
		case VoteEndReason::TimeUp: {
			S2_LOGF(LS_MESSAGE, "[Vote Ending] [id:{}] Time ran out.", m_voteCount);
			break;
		}
		case VoteEndReason::Cancelled: {
			S2_LOGF(LS_MESSAGE, "[Vote Ending] [id:{}] The vote has been cancelled.", m_voteCount);
			break;
		}
	}

	// Cycle global vote counter
	++m_voteCount;

	if (m_voteHandler != nullptr)
		m_voteHandler(VoteAction::End, -1, static_cast<int>(reason));

	if (!m_voteController) {
		SendVoteFailed();
		return;
	}

	if (m_voteResult == nullptr || reason == VoteEndReason::Cancelled) {
		SendVoteFailed();
		m_voteController->m_iActiveIssueIndex = -1;
		return;
	}

	int numClients = m_voterCount;
	int yesVotes = m_voteController->m_nVoteOptionCount[VOTE_OPTION1];
	int noVotes = m_voteController->m_nVoteOptionCount[VOTE_OPTION2];
	int numVotes = yesVotes + noVotes;

	plg::vector<int> clientInfoSlot;
	plg::vector<int> clientInfoItem;

	clientInfoSlot.reserve(numClients);
	clientInfoItem.reserve(numClients);

	for (int i = 0; i < numClients; ++i) {
		clientInfoSlot.emplace_back(m_voters[i]);
		clientInfoItem.emplace_back(m_voteController->m_nVotesCast[m_voters[i]]);
	}

	bool passed = m_voteResult(numVotes, yesVotes, noVotes, numClients, clientInfoSlot, clientInfoItem);
	if (passed) {
		SendVotePassed();
	} else {
		SendVoteFailed();
	}
}

void CPanoramaVoteHandler::SendVoteFailed() {
	INetworkMessageInternal *pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("VoteFailed");

	auto data = pNetMsg->AllocateMessage()->ToPB<CCSUsrMsg_VoteFailed>();

	data->set_reason(0);
	data->set_team(-1);

	CRecipientFilter filter;
	filter.AddAllPlayers();
	g_pGameEventSystem->PostEventAbstract(-1, false, &filter, pNetMsg, data, 0);
}

void CPanoramaVoteHandler::SendVotePassed() {
	INetworkMessageInternal *pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("VotePass");

	auto data = pNetMsg->AllocateMessage()->ToPB<CCSUsrMsg_VotePass>();

	data->set_team(-1);
	data->set_vote_type(2);
	data->set_disp_str("#SFUI_Vote_None");
	data->set_details_str("");

	CRecipientFilter filter;
	filter.AddAllPlayers();
	g_pGameEventSystem->PostEventAbstract(-1, false, &filter, pNetMsg, data, 0);
}

GAME_EVENT_F(vote_cast) {
	g_PanoramaVoteHandler.VoteCast(event);
}

GAME_EVENT_F(round_start) {
	g_PanoramaVoteHandler.Init();
}

CPanoramaVoteHandler g_PanoramaVoteHandler;