#pragma once

#include <public/eiface.h>

namespace cs2sdk::utils {
	static std::string gameDirectory;

	inline std::string GameDirectory() {
		if (gameDirectory.empty()) {
			CBufferStringGrowable<255> gamePath;
			globals::engine->GetGameDir(gamePath);
			gameDirectory = std::string(gamePath.Get());
		}
		return gameDirectory;
	}

	inline std::string GetRootDirectory() { return GameDirectory() + "/addons/wizard/"; }
	inline std::string ConfigsDirectory() { return GameDirectory() + "/addons/wizard/configs/"; }
	inline std::string GamedataDirectory() { return GameDirectory() + "/addons/wizard/gamedata/"; }
}