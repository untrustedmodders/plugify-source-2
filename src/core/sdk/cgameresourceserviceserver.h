#pragma once
#include <core/game_config.h>

class CGameEntitySystem;
extern CGameConfig *g_pGameConfig;

class CGameResourceService
{
public:
	CGameEntitySystem *GetGameEntitySystem()
	{
		return *reinterpret_cast<CGameEntitySystem **>((uintptr_t)(this) + g_pGameConfig->GetOffset("GameEntitySystem"));
	}
};
