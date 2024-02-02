#pragma once

#include <thread>

namespace SourceMM {
	class IMetamodListener;
}

class IGameEventSystem;
class CGlobalVars;
class IVEngineServer2;
class CGameResourceService;
class CSchemaSystem;
class CGameEntitySystem;

extern IGameEventSystem* g_gameEventSystem;
extern CGlobalVars* gpGlobals;
extern IVEngineServer2* g_pEngineServer2;
extern CGameResourceService* pGameResourceServiceServer;
extern CSchemaSystem* pSchemaSystem;
extern CGameEntitySystem* g_pEntitySystem;

namespace cs2sdk {
	class CGameConfig;

	namespace globals {
		extern SourceMM::IMetamodListener* metamodListener;
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