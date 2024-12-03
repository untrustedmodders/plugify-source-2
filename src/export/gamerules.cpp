#include "core/sdk/entity/cgamerules.h"
#include <plugin_export.h>

extern CCSGameRules* g_pGameRules;

extern "C" PLUGIN_API void* GetGameRules()
{
	return g_pGameRules;
}

extern "C" PLUGIN_API void TerminateRound(float flDelay, CSRoundEndReason reason)
{
	g_pGameRules->TerminateRound(flDelay, reason);
}