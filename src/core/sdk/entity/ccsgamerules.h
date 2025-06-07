#pragma once

#include "cbasegamerules.h"

enum CSRoundEndReason {
	TargetBombed = 1,	  /**< Target Successfully Bombed! */
	VIPEscaped,			  /**< The VIP has escaped! - Doesn't exist on CS:GO */
	VIPKilled,			  /**< VIP has been assassinated! - Doesn't exist on CS:GO */
	TerroristsEscaped,	  /**< The terrorists have escaped! */
	CTStoppedEscape,	  /**< The CTs have prevented most of the terrorists from escaping! */
	TerroristsStopped,	  /**< Escaping terrorists have all been neutralized! */
	BombDefused,		  /**< The bomb has been defused! */
	CTWin,				  /**< Counter-Terrorists Win! */
	TerroristWin,		  /**< Terrorists Win! */
	Draw,				  /**< Round Draw! */
	HostagesRescued,	  /**< All Hostages have been rescued! */
	TargetSaved,		  /**< Target has been saved! */
	HostagesNotRescued,	  /**< Hostages have not been rescued! */
	TerroristsNotEscaped, /**< Terrorists have not escaped! */
	VIPNotEscaped,		  /**< VIP has not escaped! - Doesn't exist on CS:GO */
	GameStart,			  /**< Game Commencing! */
	TerroristsSurrender,  /**< Terrorists Surrender */
	CTSurrender,		  /**< CTs Surrender */
	TerroristsPlanted,	  /**< Terrorists Planted the bomb */
	CTsReachedHostage,	  /**< CTs Reached the hostage */
	SurvivalWin,
	SurvivalDraw
};

class CCSGameRules : public CGameRules {
public:
	DECLARE_SCHEMA_CLASS(CCSGameRules)

	SCHEMA_FIELD(float, m_fMatchStartTime)
	SCHEMA_FIELD(float, m_flGameStartTime)
	SCHEMA_FIELD(int, m_totalRoundsPlayed)
	SCHEMA_FIELD(GameTime_t, m_fRoundStartTime)
	SCHEMA_FIELD(GameTime_t, m_flRestartRoundTime)
	SCHEMA_FIELD_POINTER(int, m_nEndMatchMapGroupVoteOptions)
	SCHEMA_FIELD(int, m_nEndMatchMapVoteWinner)
	SCHEMA_FIELD(int, m_iRoundTime)
	SCHEMA_FIELD_POINTER(CUtlVector<SpawnPoint*>, m_CTSpawnPoints)
	SCHEMA_FIELD_POINTER(CUtlVector<SpawnPoint*>, m_TerroristSpawnPoints)

	void TerminateRound(float flDelay, CSRoundEndReason reason) {
		addresses::CGameRules_TerminateRound(this, flDelay, reason, 0, 0);
	}
};

class CCSGameRulesProxy : public CBaseEntity {
public:
	DECLARE_SCHEMA_CLASS(CCSGameRulesProxy)

	SCHEMA_FIELD(CCSGameRules*, m_pGameRules)
};