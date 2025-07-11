#pragma once

#include "cbaseplayerpawn.h"

enum CSPlayerState {
	STATE_ACTIVE = 0x0,
	STATE_WELCOME = 0x1,
	STATE_PICKINGTEAM = 0x2,
	STATE_PICKINGCLASS = 0x3,
	STATE_DEATH_ANIM = 0x4,
	STATE_DEATH_WAIT_FOR_KEY = 0x5,
	STATE_OBSERVER_MODE = 0x6,
	STATE_GUNGAME_RESPAWN = 0x7,
	STATE_DORMANT = 0x8,
	NUM_PLAYER_STATES = 0x9,
};

class CCitadelPlayerPawnBase : public CBasePlayerPawn {
public:
	DECLARE_SCHEMA_CLASS(CCitadelPlayerPawnBase);

	// No schema binary for binding
};

class CCitadelPlayerPawn : public CCitadelPlayerPawnBase {
public:
	DECLARE_SCHEMA_CLASS(CCitadelPlayerPawn);

	SCHEMA_FIELD(QAngle, m_angEyeAngles)

	void Respawn() {
		// TODO:
	}
};
