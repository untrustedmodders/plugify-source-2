#pragma once

#include <public/eiface.h>

namespace utils
{
	static std::string gameDirectory;

	inline std::string GameDirectory()
	{
		if (gameDirectory.empty())
		{
			CBufferStringGrowable<255> gamePath;
			g_pEngineServer2->GetGameDir(gamePath);
			gameDirectory = std::string(gamePath.Get());
		}
		return gameDirectory;
	}

	inline std::string GetRootDirectory() { return GameDirectory() + "/addons/plugify/"; }
	inline std::string GetBinDirectory() { return GameDirectory() + "/addons/plugify/bin/" CS2SDK_BINARY "/"; }
	inline std::string ConfigsDirectory() { return GameDirectory() + "/addons/plugify/configs/"; }
	inline std::string GamedataDirectory() { return GameDirectory() + "/addons/plugify/gamedata/"; }

	inline std::string Demangle(const char *name)
	{
#if CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
		int status = 0;

		std::unique_ptr<char, void (*)(void *)> res{
			abi::__cxa_demangle(name, nullptr, nullptr, &status),
			std::free
		};

		std::string_view ret{ (status == 0) ? res.get() : name };
#else
		std::string_view ret{ name };
#endif
		if (ret.substr(ret.size() - 3) == " ()")
			ret.remove_suffix(3);

		return std::string(ret);
	}

} // namespace utils