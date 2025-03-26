#pragma once

#include <core/sdk/utils.h>
#include <dynlibutils/memaddr.hpp>
#include <dynlibutils/module.hpp>

using Memory = DynLibUtils::CMemory;
using Module = DynLibUtils::CModule;

class GameConfig {
	friend class GameConfigManager;
public:
	GameConfig(plg::string game, plg::vector<plg::string> path);
	GameConfig(GameConfig&& other) = default;
	~GameConfig();

	bool Initialize();

	const plg::vector<plg::string>& GetPaths() const;
	std::string_view GetLibrary(const plg::string& name) const;
	std::string_view GetSignature(const plg::string& name) const;
	std::string_view GetSymbol(const plg::string& name) const;
	std::string_view GetPatch(const plg::string& name) const;
	int32_t GetOffset(const plg::string& name) const;
	Memory GetAddress(const plg::string& name) const;
	const Module* GetModule(const plg::string& name) const;
	bool IsSymbol(const plg::string& name) const;
	Memory ResolveSignature(const plg::string& name) const;

private:
	struct AddressConf {
		plg::string signature;
		plg::vector<std::pair<int, bool>> read;
		bool lastIsOffset;
	};

private:
	plg::string m_gameDir;
	plg::vector<plg::string> m_paths;
	std::unordered_map<plg::string, int32_t> m_offsets;
	std::unordered_map<plg::string, plg::string> m_signatures;
	std::unordered_map<plg::string, AddressConf> m_addresses;
	std::unordered_map<plg::string, plg::string> m_libraries;
	std::unordered_map<plg::string, plg::string> m_patches;
	size_t m_refCount = 1;
};

class GameConfigManager {
public:
	GameConfigManager();
	~GameConfigManager() = default;

	uint32_t LoadGameConfigFile(plg::vector<plg::string> paths);
	void CloseGameConfigFile(uint32_t id);
	GameConfig* GetGameConfig(uint32_t id);
	Module* GetModule(std::string_view name);

private:
	std::unordered_map<uint32_t, GameConfig> m_configs;
	std::unordered_map<plg::string, Module, utils::string_hash,std::equal_to<>> m_modules;
	static inline uint32_t s_nextId = static_cast<uint32_t>(-1);
};

extern GameConfigManager g_GameConfigManager;