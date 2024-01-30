#pragma once

#define protected public
#define private public
#include <tier1/convar.h>
#undef protected
#undef private

#include <thread>

#include <eiface.h>
#include <iserver.h>
#include <igameeventsystem.h>
#include <igameevents.h>
#include <entitysystem.h>

#include <ISmmPlugin.h>

namespace cs2sdk {
	class CGameConfig;

	namespace globals {
		extern IMetamodListener* metamodListener;
		extern CGameConfig* gameConfig;
		extern std::thread::id gameThreadId;

		void Initialize();
		void Terminate();

		CGlobalVars* GetGameGlobals();
	}

	namespace modules {
		class CModule;

		extern CModule* engine;
		extern CModule* tier0;
		extern CModule* server;
		extern CModule* schemasystem;
		extern CModule* filesystem;
		extern CModule* vscript;
	}
}

class CGameResourceService;
class CSchemaSystem;

extern IGameEventSystem* g_gameEventSystem;
extern CGlobalVars* gpGlobals;
extern IVEngineServer2* g_pEngineServer2;
extern CGameResourceService* pGameResourceServiceServer;
extern CSchemaSystem* pSchemaSystem;
extern CGameEntitySystem* g_pEntitySystem;
