#pragma once

#include "cbasegamerules.h"

class CCitadelGameRules : public CGameRules {
public:
    DECLARE_SCHEMA_CLASS(CCitadelGameRules)

    void TerminateRound(float flDelay, int reason) {
    }
};

class CCitadelGameRulesProxy : public CBaseEntity {
public:
    DECLARE_SCHEMA_CLASS(CCitadelGameRulesProxy)

    SCHEMA_FIELD(CCitadelGameRules*, m_pGameRules)
};