#pragma once

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
	int GetOffset(const plg::string& name) const;
	CMemory GetAddress(const plg::string& name) const;
	CModule* GetModule(const plg::string& name) const;
	bool IsSymbol(const plg::string& name) const;
	CMemory ResolveSignature(const plg::string& name) const;

private:
	struct AddressConf {
		plg::string signature;
		plg::vector<std::pair<int, bool>> read;
		bool lastIsOffset;
	};

private:
	plg::string m_szGameDir;
	plg::vector<plg::string> m_szPaths;
	std::unordered_map<plg::string, int> m_umOffsets;
	std::unordered_map<plg::string, plg::string> m_umSignatures;
	std::unordered_map<plg::string, AddressConf> m_umAddresses;
	std::unordered_map<plg::string, plg::string> m_umLibraries;
	std::unordered_map<plg::string, plg::string> m_umPatches;
};

class CGameConfigManager {
public:
	CGameConfigManager() = default;
	~CGameConfigManager() = default;

	Handle LoadGameConfigFile(plg::vector<plg::string> paths);
	void CloseGameConfigFile(Handle handle);
	CGameConfig* GetGameConfig(Handle handle);

private:
	std::unordered_map<Handle, CGameConfig> m_configs;
};

extern CGameConfigManager g_pGameConfigManager;