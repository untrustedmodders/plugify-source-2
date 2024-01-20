#include "globals.h"
#include "module.h"
#include "game_config.h"
#include "interfaces/cs2_interfaces.h"

namespace cs2sdk {
	namespace modules {
		Module* engine = nullptr;
		Module* tier0 = nullptr;
		Module* server = nullptr;
		Module* schemasystem = nullptr;
		Module* filesystem = nullptr;
		Module* vscript = nullptr;
	}

	namespace globals {
		IVEngineServer* engine = nullptr;
		IFileSystem* fileSystem = nullptr;

		GameConfig* gameConfig = nullptr;

		std::thread::id gameThreadId;

		void Initialize() {
			gameThreadId = std::this_thread::get_id();

			modules::engine = new modules::Module(CS2SDK_ROOT_BINARY, "engine2");
			modules::tier0 = new modules::Module(CS2SDK_ROOT_BINARY, "tier0");
			modules::server = new modules::Module(CS2SDK_GAME_BINARY, "server");
			modules::schemasystem = new modules::Module(CS2SDK_ROOT_BINARY, "schemasystem");
			modules::filesystem = new modules::Module(CS2SDK_ROOT_BINARY, "filesystem_stdio");
			modules::vscript = new modules::Module(CS2SDK_ROOT_BINARY, "vscript");

			interfaces::Initialize();

			engine = (IVEngineServer*)modules::engine->FindInterface(INTERFACEVERSION_VENGINESERVER);
			fileSystem = (IFileSystem*)modules::filesystem->FindInterface(FILESYSTEM_INTERFACE_VERSION);

			std::cout << "ENGINE " <<  engine << std::endl;
			std::cout << "ENGINE " <<  fileSystem << std::endl;

			/*std::string gamedataPath(utils::GamedataDirectory() + "/cs2sdk.games.txt");
			gameConfig = new GameConfig("csgo", gamedataPath);

			if (!globals::gameConfig->Initialize()) {
				ErrorIfNot("Could not read \"%s\". Error: {}", gamedataPath);
			}*/
		}

		void Terminate() {
			delete modules::engine;
			delete modules::tier0;
			delete modules::server;
			delete modules::schemasystem;
			delete modules::filesystem;
			delete modules::vscript;
		}

		CGlobalVars* getGlobalVars() {
			return nullptr;
		}

	}
}