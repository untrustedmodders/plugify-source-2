/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2024 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "cbaseplayercontroller.h"
#include <sharedInterface.h>

class CCSPlayerController : public CBasePlayerController
{
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerController);
	SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hPlayerPawn);

	void ChangeTeam(int iTeam)
	{
		static int offset = g_pGameConfig->GetOffset("ControllerChangeTeam");
		CALL_VIRTUAL(void, offset, this, iTeam);
	}

	void SwitchTeam(int iTeam)
	{
		if (!IsController())
			return;

		if (iTeam == CS_TEAM_SPECTATOR)
		{
			ChangeTeam(iTeam);
		}
		else
		{
			addresses::CCSPlayerController_SwitchTeam(this, iTeam);
		}
	}

	// Respawns the player if the player is not alive, does nothing otherwise.
	void Respawn()
	{
		CCSPlayerPawn* pawn = m_hPlayerPawn.Get();
		if (!pawn || pawn->IsAlive())
			return;

		SetPawn(pawn);
		if (this->m_iTeamNum() != CS_TEAM_CT && this->m_iTeamNum() != CS_TEAM_T)
		{
			SwitchTeam(RandomInt(CS_TEAM_T, CS_TEAM_CT));
		}
		static int offset = g_pGameConfig->GetOffset("ControllerRespawn");
		CALL_VIRTUAL(void, offset, this);
	}
};
