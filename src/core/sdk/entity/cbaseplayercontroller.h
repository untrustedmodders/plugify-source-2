#pragma once

#include "cbaseentity.h"
#include "ccsplayerpawn.h"
#include <ehandle.h>

class CCSPlayerPawn;

class CBasePlayerController : public CBaseEntity2
{
public:
	DECLARE_SCHEMA_CLASS(CBasePlayerController);

	SCHEMA_FIELD(uint64, m_steamID)
	SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hPawn)
	SCHEMA_FIELD_POINTER(char, m_iszPlayerName)

	void SetPawn(CCSPlayerPawn* pawn)
	{
		addresses::CBasePlayerController_SetPawn(this, pawn, true, false);
	}
};
