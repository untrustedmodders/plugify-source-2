#include "game_config.h"
#include <utils/utils.h>

CGameConfig::CGameConfig(std::string game, std::string path) : m_szGameDir(std::move(game)), m_szPath(std::move(path)), m_pKeyValues(std::make_unique<KeyValues>("Games"))
{
}

CGameConfig::~CGameConfig() = default;

bool CGameConfig::Initialize(std::span<char> error)
{
	if (!m_pKeyValues->LoadFromFile(g_pFullFileSystem, m_szPath.c_str(), nullptr))
	{
		snprintf(error.data(), error.size(), "Failed to load gamedata file");
		return false;
	}

	KeyValues* game = m_pKeyValues->FindKey(m_szGameDir.c_str());
	if (game)
	{
		const char* platform = CS2SDK_PLATFORM;

		KeyValues* offsets = game->FindKey("Offsets");
		if (offsets)
		{
			FOR_EACH_SUBKEY(offsets, it)
			{
				m_umOffsets[it->GetName()] = it->GetInt(platform, -1);
			}
		}

		KeyValues* signatures = game->FindKey("Signatures");
		if (signatures)
		{
			FOR_EACH_SUBKEY(signatures, it)
			{
				m_umLibraries[it->GetName()] = std::string(it->GetString("library"));
				m_umSignatures[it->GetName()] = std::string(it->GetString(platform));
			}
		}

		KeyValues* patches = game->FindKey("Patches");
		if (patches)
		{
			FOR_EACH_SUBKEY(patches, it)
			{
				m_umPatches[it->GetName()] = std::string(it->GetString(platform));
			}
		}

		KeyValues* addresses = game->FindKey("Addresses");
		if (addresses)
		{
			FOR_EACH_SUBKEY(addresses, it)
			{
				KeyValues* reads = it->FindKey(platform);
				std::vector<int> read;
				bool lastIsOffset = false;
				FOR_EACH_SUBKEY(reads, it2)
				{
					const char* key = it->GetName();
					if (!strcmp(key, "read") || !strcmp(key, "offset"))
					{
						if (lastIsOffset)
						{
							snprintf(error.data(), error.size(), "Error parsing Address \"%s\", 'offset' entry must be the last entry", it->GetName());
							continue;
						}
						read.push_back(it2->GetInt());
						if (key[0] == 'o') // offset
						{
							lastIsOffset = true;
						}
					}
				}
				m_umAddresses[it->GetName()] = AddressConf{it->GetString("signature"), std::move(read), lastIsOffset};
			}
		}
	}
	else
	{
		snprintf(error.data(), error.size(), "Failed to find game: %s", m_szGameDir.c_str());
		return false;
	}

	return true;
}

const std::string& CGameConfig::GetPath() const
{
	return m_szPath;
}

std::string_view CGameConfig::GetSignature(const std::string& name) const
{
	auto it = m_umSignatures.find(name);
	if (it == m_umSignatures.end())
		return {};
	return std::get<std::string>(*it);
}

std::string_view CGameConfig::GetPatch(const std::string& name) const
{
	auto it = m_umPatches.find(name);
	if (it == m_umPatches.end())
		return {};
	return std::get<std::string>(*it);
}

int CGameConfig::GetOffset(const std::string& name) const
{
	auto it = m_umOffsets.find(name);
	if (it == m_umOffsets.end())
		return -1;
	return std::get<int>(*it);
}

std::string_view CGameConfig::GetLibrary(const std::string& name) const
{
	auto it = m_umLibraries.find(name);
	if (it == m_umLibraries.end())
		return {};
	return std::get<std::string>(*it);
}

// memory addresses below 0x10000 are automatically considered invalid for dereferencing
#define VALID_MINIMUM_MEMORY_ADDRESS ((void*)0x10000)

void* CGameConfig::GetAddress(const std::string& name) const
{
	auto it = m_umAddresses.find(name);
	if (it == m_umAddresses.end())
		return nullptr;

	const auto& addrConf = std::get<AddressConf>(*it);

	CMemory addr = ResolveSignature(addrConf.signature);
	if (!addr)
		return nullptr;

	size_t readCount = addrConf.read.size();
	for (size_t i = 0; i < readCount; ++i)
	{
		int offset = addrConf.read[i];

		// NULLs in the middle of an indirection chain are bad, end NULL is ok
		if (!addr || addr < VALID_MINIMUM_MEMORY_ADDRESS)
			return nullptr;

		// If lastIsOffset is set and this is the last iteration of the loop, don't deref
		if (addrConf.lastIsOffset && i == readCount - 1)
		{
			addr.OffsetSelf(offset);
		}
		else
		{
			addr.OffsetSelf(offset).DerefSelf();
		}
	}

	return addr;
}

ModuleRef CGameConfig::GetModule(const std::string& name) const
{
	const std::string_view library = GetLibrary(name);
	if (library.empty())
		return {};

	if (library == "engine")
		return *modules::engine;
	else if (library == "server")
		return *modules::server;
	else if (library == "schemasystem")
		return *modules::schemasystem;
	else if (library == "vscript")
		return *modules::vscript;
	else if (library == "tier0")
		return *modules::tier0;

	return {};
}

bool CGameConfig::IsSymbol(const std::string& name) const
{
	const std::string_view sigOrSymbol = GetSignature(name);
	if (sigOrSymbol.empty())
	{
		g_Logger.WarningFormat("Missing signature or symbol: %s\n", name.c_str());
		return false;
	}
	return sigOrSymbol[0] == '@';
}

std::string_view CGameConfig::GetSymbol(const std::string& name) const
{
	const std::string_view symbol = GetSignature(name);

	if (symbol.size() <= 1)
	{
		g_Logger.WarningFormat("Missing symbol: %s\n", name.c_str());
		return {};
	}

	return symbol.substr(1);
}

CMemory CGameConfig::ResolveSignature(const std::string& name) const
{
	auto moduleRef = GetModule(name);
	if (!moduleRef.has_value())
	{
		g_Logger.WarningFormat("Invalid module: %s\n", name.c_str());
		return {};
	}

	auto& module = moduleRef->get();
	CMemory address;

	if (IsSymbol(name))
	{
		const std::string_view symbol = GetSymbol(name);
		if (symbol.empty())
		{
			g_Logger.WarningFormat("Invalid symbol for %s\n", name.c_str());
			return {};
		}

		address = module.GetFunctionByName(symbol);
	}
	else
	{
		const std::string_view signature = GetSignature(name);
		if (signature.empty())
		{
			g_Logger.WarningFormat("Failed to find signature for %s\n", name.c_str());
			return {};
		}

		address = module.FindPattern(signature);
	}

	if (!address)
	{
		g_Logger.WarningFormat("Failed to find address for %s\n", name.c_str());
		return {};
	}

	return address;
}

CGameConfig* CGameConfigManager::LoadGameConfigFile(std::string path)
{
	path = utils::GamedataDirectory() + path;

	auto it = m_configs.find(path);
	if (it != m_configs.end())
	{
		return &std::get<CGameConfig>(*it);
	}

	char confError[255] = "";
	CGameConfig gameConfig("cs2", path);
	if (!gameConfig.Initialize(confError))
	{
		g_Logger.WarningFormat("Could not read \"%s\": %s\n", gameConfig.GetPath().c_str(), confError);
		return nullptr;
	}

	auto [_, result] = m_configs.emplace(std::move(path), std::move(gameConfig));
	return &std::get<CGameConfig>(*_);
}

void CGameConfigManager::CloseGameConfigFile(CGameConfig* pGameConfig)
{
	if (pGameConfig)
	{
		auto it = m_configs.find(pGameConfig->GetPath());
		if (it != m_configs.end())
		{
			m_configs.erase(it);
		}
	}
}

CGameConfigManager g_pGameConfigManager;