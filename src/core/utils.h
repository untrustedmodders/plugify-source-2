#pragma once

#include <eiface.h>

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

	inline std::string Demangle(const char* name)
	{
#if CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
		int status = 0;

		std::unique_ptr<char, void (*)(void*)> res{
			abi::__cxa_demangle(name, nullptr, nullptr, &status),
			std::free};

		std::string_view ret((status == 0) ? res.get() : name);
#else
		std::string_view ret(name);
#endif
		if (ret.substr(ret.size() - 3) == " ()")
			ret.remove_suffix(3);

		return std::string(ret);
	}

	/**
	 * Combines a seed into a hash and modifies the seed by the new hash.
	 * @param seed The seed.
	 * @param v The value to hash.
	 * https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
	 */
	inline void hash_combine(size_t& seed) {}

	template <typename T, typename... Rest>
	inline void hash_combine(size_t& seed, const T& v, Rest... rest)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
		hash_combine(seed, rest...);
	}

	template<typename T1, typename T2>
	struct PairHash
	{
		std::size_t operator()(std::pair<T1, T2> const& p) const
		{
			std::size_t seed(0);
			hash_combine(seed, p.first);
			hash_combine(seed, p.second);
			return seed;
		}
	};

	struct CaseInsensitiveComparator {
		bool operator()(const std::string& lhs, const std::string& rhs) const {
			return std::lexicographical_compare(
				lhs.begin(), lhs.end(),
				rhs.begin(), rhs.end(),
				[](char a, char b) { return std::tolower(a) < std::tolower(b); }
			);
		}
	};

} // namespace utils