#include "game_config.hpp"
#include <core/sdk/utils.h>
#include <corecrt_io.h>
#include <plugify-configs/plugify-configs.hpp>

CGameConfig::CGameConfig(plg::string game, plg::vector<plg::string> paths) : m_gameDir(std::move(game)), m_paths(std::move(paths)) {
}

CGameConfig::~CGameConfig() = default;

bool CGameConfig::Initialize() {
	std::vector<std::string_view> paths;
	paths.reserve(m_paths.size());
	for (const auto& path : m_paths) {
		paths.emplace_back(path);
	}
	auto config = ReadConfigs(paths);
	if (!config) {
		S2_LOGF(LS_ERROR, "Failed to load configuration file: \"%s\"\n", GetError().data());
		return false;
	}

	config->JumpKey(m_gameDir);

	if (config->JumpKey("Signatures")) {
		if (config->IsObject() && config->JumpFirst()) {
			do {
				if (config->IsObject()) {
					m_libraries[config->GetName()] = config->GetString("library");
					m_signatures[config->GetName()] = config->GetString(S2SDK_PLATFORM);
				}
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	if (config->JumpKey("Offsets")) {
		if (config->IsObject() && config->JumpFirst()) {
			do {
				if (config->IsObject()) {
					m_offsets[config->GetName()] = config->GetAsInt32(S2SDK_PLATFORM, -1);
				}
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	if (config->JumpKey("Patches")) {
		if (config->IsObject() && config->JumpFirst()) {
			do {
				if (config->IsObject()) {
					m_patches[config->GetName()] = config->GetString(S2SDK_PLATFORM);
				}
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	auto getAddressConf = [](std::unique_ptr<pcf::Config>& config) -> AddressConf {
		AddressConf conf{config->GetString("signature"), {}, false};
		if (config->JumpKey(S2SDK_PLATFORM)) {
			if (config->IsArray() && config->JumpFirst()) {
				do {
					if (config->IsObject() && config->JumpFirst()) {
						auto name = config->GetName();
						if (conf.lastIsOffset) {
							S2_LOGF(LS_WARNING, "Error parsing Address \"%s\", 'offset' entry must be the last entry\n", name.c_str());
							continue;
						}
						conf.read.emplace_back(config->GetAsInt32(), name == "read_offs32");
						if (name == "offset") {
							conf.lastIsOffset = true;
						}
						config->JumpBack();
					}
				} while (config->JumpNext());
				config->JumpBack();
			}
			config->JumpBack();
		}
		return conf;
	};

	if (config->JumpKey("Addresses")) {
		if (config->IsObject() && config->JumpFirst()) {
			do {
				if (config->IsObject()) {
					m_addresses[config->GetName()] = getAddressConf(config);
				}
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	return true;
}

const plg::vector<plg::string>& CGameConfig::GetPaths() const {
	return m_paths;
}

std::string_view CGameConfig::GetSignature(const plg::string& name) const {
	auto it = m_signatures.find(name);
	if (it == m_signatures.end())
		return {};
	return std::get<plg::string>(*it);
}

std::string_view CGameConfig::GetPatch(const plg::string& name) const {
	auto it = m_patches.find(name);
	if (it == m_patches.end())
		return {};
	return std::get<plg::string>(*it);
}

int32_t CGameConfig::GetOffset(const plg::string& name) const {
	auto it = m_offsets.find(name);
	if (it == m_offsets.end())
		return -1;
	return std::get<int32_t>(*it);
}

std::string_view CGameConfig::GetLibrary(const plg::string& name) const {
	auto it = m_libraries.find(name);
	if (it == m_libraries.end())
		return {};
	return std::get<plg::string>(*it);
}

// memory addresses below 0x10000 are automatically considered invalid for dereferencing
#define VALID_MINIMUM_MEMORY_ADDRESS ((void*) 0x10000)

CMemory CGameConfig::GetAddress(const plg::string& name) const {
	auto it = m_addresses.find(name);
	if (it == m_addresses.end())
		return {};

	const auto& addrConf = std::get<AddressConf>(*it);

	CMemory addr = ResolveSignature(addrConf.signature);
	if (!addr)
		return {};

	size_t readCount = addrConf.read.size();
	for (size_t i = 0; i < readCount; ++i) {
		const auto& [offset, rel] = addrConf.read[i];

		// NULLs in the middle of an indirection chain are bad, end NULL is ok
		if (!addr || addr < VALID_MINIMUM_MEMORY_ADDRESS)
			return nullptr;

		if (rel) {
			auto target = addr.Offset(offset);
			if (!target || target < VALID_MINIMUM_MEMORY_ADDRESS)
				return nullptr;

			addr.OffsetSelf(offset)
					.OffsetSelf(sizeof(int32_t))
					.OffsetSelf(target.GetValue<int32_t>());
		} else {
			addr.OffsetSelf(offset);

			// If lastIsOffset is set and this is the last iteration of the loop, don't deref
			if (!addrConf.lastIsOffset || i != readCount - 1) {
				addr.DerefSelf();
			}
		}
	}

	return addr;
}

const CModule* CGameConfig::GetModule(const plg::string& name) const {
	const std::string_view library = GetLibrary(name);
	if (library.empty())
		return {};

	return g_pGameConfigManager.GetModule(library);
}

bool CGameConfig::IsSymbol(const plg::string& name) const {
	const std::string_view sigOrSymbol = GetSignature(name);
	if (sigOrSymbol.empty()) {
		S2_LOGF(LS_WARNING, "Missing signature or symbol: %s\n", name.c_str());
		return false;
	}
	return sigOrSymbol[0] == '@';
}

std::string_view CGameConfig::GetSymbol(const plg::string& name) const {
	const std::string_view symbol = GetSignature(name);

	if (symbol.size() <= 1) {
		S2_LOGF(LS_WARNING, "Missing symbol: %s\n", name.c_str());
		return {};
	}

	return symbol.substr(1);
}

CMemory CGameConfig::ResolveSignature(const plg::string& name) const {
	auto module = GetModule(name);
	if (!module) {
		S2_LOGF(LS_WARNING, "Invalid module: %s\n", name.c_str());
		return {};
	}

	CMemory address;

	if (IsSymbol(name)) {
		const std::string_view symbol = GetSymbol(name);
		if (symbol.empty()) {
			S2_LOGF(LS_WARNING, "Invalid symbol for %s\n", name.c_str());
			return {};
		}

		address = module->GetFunctionByName(symbol);
	} else {
		const std::string_view signature = GetSignature(name);
		if (signature.empty()) {
			S2_LOGF(LS_WARNING, "Failed to find signature for %s\n", name.c_str());
			return {};
		}

		address = module->FindPattern(signature);
	}

	if (!address) {
		S2_LOGF(LS_WARNING, "Failed to find address for %s\n", name.c_str());
		return {};
	}

	return address;
}

CGameConfigManager::CGameConfigManager() {
	m_modules.emplace("engine2", CModule(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "engine2"));
	m_modules.emplace("server", CModule(utils::GameDirectory() + S2SDK_GAME_BINARY S2SDK_LIBRARY_PREFIX "server"));
	/*m_modules.emplace("tier0", CModule(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "tier0"));
	m_modules.emplace("schemasystem", CModule(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "schemasystem"));
	m_modules.emplace("filesystem", CModule(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "filesystem_stdio"));
	m_modules.emplace("vscript", CModule(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "vscript"));
	m_modules.emplace("networksystem", CModule(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "networksystem"));*/
}

Handle CGameConfigManager::LoadGameConfigFile(plg::vector<plg::string> paths) {
	for (const auto& [handle, config] : m_configs) {
		if (config.GetPaths() == paths) {
			return handle;
		}
	}

	CGameConfig gameConfig("csgo", std::move(paths));
	if (!gameConfig.Initialize()) {
		return InvalidHandle();
	}

	Handle handle = CreateHandle();
	m_configs.emplace(handle, std::move(gameConfig));
	return handle;
}

void CGameConfigManager::CloseGameConfigFile(Handle handle) {
	auto it = m_configs.find(handle);
	if (it != m_configs.end()) {
		m_configs.erase(it);
	}
}

CGameConfig* CGameConfigManager::GetGameConfig(Handle handle) {
	auto it = m_configs.find(handle);
	if (it != m_configs.end()) {
		return &std::get<CGameConfig>(*it);
	}
	return nullptr;
}

CModule* CGameConfigManager::GetModule(std::string_view name) {
	auto it = m_modules.find(name);
	if (it != m_modules.end()) {
		return &std::get<CModule>(*it);
	}

	auto system = globals::FindModule(name);
	if (system != nullptr) {
		return &m_modules.emplace(name, CModule(system)).first->second;
	}

	return nullptr;
}

CGameConfigManager g_pGameConfigManager;