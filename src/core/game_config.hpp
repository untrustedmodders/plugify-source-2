#pragma once

#include <core/sdk/utils.h>
#include <dynlibutils/memaddr.hpp>
#include <dynlibutils/module.hpp>

using CMemory = DynLibUtils::CMemory;
using CModule = DynLibUtils::CModule;

class CGameConfig {
public:
	CGameConfig(plg::string game, plg::vector<plg::string> path);
	CGameConfig(CGameConfig&& other) = default;
	~CGameConfig();

	bool Initialize();

	const plg::vector<plg::string>& GetPaths() const;
	std::string_view GetLibrary(const plg::string& name) const;
	std::string_view GetSignature(const plg::string& name) const;
	std::string_view GetSymbol(const plg::string& name) const;
	std::string_view GetPatch(const plg::string& name) const;
	int32_t GetOffset(const plg::string& name) const;
	CMemory GetAddress(const plg::string& name) const;
	const CModule* GetModule(const plg::string& name) const;
	bool IsSymbol(const plg::string& name) const;
	CMemory ResolveSignature(const plg::string& name) const;

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
};

class CGameConfigManager {
public:
	CGameConfigManager();
	~CGameConfigManager() = default;

	Handle LoadGameConfigFile(plg::vector<plg::string> paths);
	void CloseGameConfigFile(Handle handle);
	CGameConfig* GetGameConfig(Handle handle);
	CModule* GetModule(std::string_view name);

private:
	std::unordered_map<Handle, CGameConfig> m_configs;
	std::unordered_map<plg::string, CModule, utils::string_hash,std::equal_to<>> m_modules;
};

extern CGameConfigManager g_pGameConfigManager;