#pragma once

#include "KeyValues.h"

#include "module.h"
#include "memaddr.h"

using CMemory = DynLibUtils::CMemory;
using ModuleRef = std::optional<std::reference_wrapper<DynLibUtils::CModule>>;

class CGameConfig
{
public:
	CGameConfig(std::string game, std::string path);
	CGameConfig(CGameConfig &&other) = default;
	~CGameConfig();

	bool Initialize(std::span<char> error);

	const std::string &GetPath() const;
	std::string_view GetLibrary(const std::string &name) const;
	std::string_view GetSignature(const std::string &name) const;
	std::string_view GetSymbol(const std::string &name) const;
	std::string_view GetPatch(const std::string &name) const;
	int GetOffset(const std::string &name) const;
	void *GetAddress(const std::string &name) const;
	ModuleRef GetModule(const std::string &name) const;
	bool IsSymbol(const std::string &name) const;
	CMemory ResolveSignature(const std::string &name) const;

	static std::vector<uint8_t> HexToByte(std::string_view hexString);

private:
	struct AddressConf
	{
		std::string signature;
		std::vector<int> read;
		bool lastIsOffset;
	};

private:
	std::string m_szGameDir;
	std::string m_szPath;
	std::unique_ptr<KeyValues> m_pKeyValues;
	std::unordered_map<std::string, int> m_umOffsets;
	std::unordered_map<std::string, std::string> m_umSignatures;
	std::unordered_map<std::string, AddressConf> m_umAddresses;
	std::unordered_map<std::string, std::string> m_umLibraries;
	std::unordered_map<std::string, std::string> m_umPatches;
};

class CGameConfigManager
{
public:
	CGameConfigManager() = default;
	~CGameConfigManager() = default;

	CGameConfig *LoadGameConfigFile(std::string file);
	void CloseGameConfigFile(CGameConfig *pGameConfig);

private:
	std::unordered_map<std::string, CGameConfig> m_Configs;
};

extern CGameConfigManager g_GameConfigManager;