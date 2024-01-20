#pragma once

#define protected public
#define private public
#include <tier1/convar.h>
#undef protected
#undef private

#include <thread>

#include <eiface.h>
#include <iserver.h>

class IFileSystem;

namespace cs2sdk {
	class GameConfig;

	namespace globals {
		extern IVEngineServer* engine;
		extern IFileSystem* fileSystem;

		extern GameConfig* gameConfig;

		extern std::thread::id gameThreadId;

		void Initialize();
		void Terminate();

		CGlobalVars* getGlobalVars();
	}

	namespace modules {
		class Module;

		extern Module* engine;
		extern Module* tier0;
		extern Module* server;
		extern Module* schemasystem;
		extern Module* filesystem;
		extern Module* vscript;
	}
}