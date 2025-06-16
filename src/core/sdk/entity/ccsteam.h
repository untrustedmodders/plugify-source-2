#pragma once

#include "cbaseentity.h"

class CTeam : public CBaseEntity {
public:
	DECLARE_SCHEMA_CLASS(CTeam);

	SCHEMA_FIELD(int, m_iScore);
};

class CCSTeam : public CTeam {
public:
	DECLARE_SCHEMA_CLASS(CCSTeam);

	SCHEMA_FIELD(bool, m_bSurrendered);
};