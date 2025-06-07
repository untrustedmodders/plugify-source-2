#pragma once

#if defined(CS2)
#include "ccsgamerules.h"
using CBaseGameRules = CCSGameRules;
using CBaseGameRulesProxy = CCSGameRulesProxy;
#elif defined(DEADLOCK)
#include "ccitadelgamerules.h"
using CBaseGameRules = CCitadelGameRules;
using CBaseGameRulesProxy = CCitadelGameRulesProxy;
enum CSRoundEndReason
{
};
#endif