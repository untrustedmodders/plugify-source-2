#include "game_config.hpp"

#include <core/sdk/utils.h>
#include <plugify-configs/plugify-configs.hpp>

#if S2SDK_PLATFORM_LINUX
#include <link.h>
#endif

GameConfig::GameConfig(plg::string game, plg::vector<plg::string> paths) : m_gameDir(std::move(game)), m_paths(std::move(paths)) {
}

GameConfig::~GameConfig() = default;

bool GameConfig::Initialize() {
	std::vector<std::string_view> paths;
	paths.reserve(m_paths.size());
	for (const auto& path : m_paths) {
		paths.emplace_back(path);
	}
	auto config = pcf::ReadConfigs(paths);
	if (!config) {
		S2_LOGF(LS_ERROR, "Failed to load configuration file: \"{}\"\n", pcf::GetError());
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
							S2_LOGF(LS_WARNING, "Error parsing Address \"{}\", 'offset' entry must be the last entry\n", name);
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

const plg::vector<plg::string>& GameConfig::GetPaths() const {
	return m_paths;
}

std::string_view GameConfig::GetSignature(std::string_view name) const {
	auto it = m_signatures.find(name);
	if (it == m_signatures.end())
		return {};
	return std::get<plg::string>(*it);
}

std::string_view GameConfig::GetPatch(std::string_view name) const {
	auto it = m_patches.find(name);
	if (it == m_patches.end())
		return {};
	return std::get<plg::string>(*it);
}

int32_t GameConfig::GetOffset(std::string_view name) const {
	auto it = m_offsets.find(name);
	if (it == m_offsets.end())
		return -1;
	return std::get<int32_t>(*it);
}

std::string_view GameConfig::GetLibrary(std::string_view name) const {
	auto it = m_libraries.find(name);
	if (it == m_libraries.end())
		return {};
	return std::get<plg::string>(*it);
}

// memory addresses below 0x10000 are automatically considered invalid for dereferencing
#define VALID_MINIMUM_MEMORY_ADDRESS ((ptrdiff_t) 0x10000)

Memory GameConfig::GetAddress(std::string_view name) const {
	auto it = m_addresses.find(name);
	if (it == m_addresses.end())
		return {};

	const auto& addrConf = std::get<AddressConf>(*it);

	auto memory = ResolveSignature(addrConf.signature);
	if (!memory)
		return {};

	size_t readCount = addrConf.read.size();
	for (size_t i = 0; i < readCount; ++i) {
		const auto& [offset, rel] = addrConf.read[i];

		// NULLs in the middle of an indirection chain are bad, end NULL is ok
		auto addr = memory.GetAddr();
		if (!addr || addr < VALID_MINIMUM_MEMORY_ADDRESS)
			return nullptr;

		if (rel) {
			auto target = memory.Offset(offset);

			auto taddr = target.GetAddr();
			if (!taddr || taddr < VALID_MINIMUM_MEMORY_ADDRESS)
				return nullptr;

			memory.OffsetSelf(offset)
					.OffsetSelf(sizeof(int32_t))
					.OffsetSelf(target.Get<int32_t>());
		} else {
			memory.OffsetSelf(offset);

			// If lastIsOffset is set and this is the last iteration of the loop, don't deref
			if (!addrConf.lastIsOffset || i != readCount - 1) {
				memory.DerefSelf();
			}
		}
	}

	return memory;
}

const Module* GameConfig::GetModule(std::string_view name) const {
	const std::string_view library = GetLibrary(name);
	if (library.empty())
		return nullptr;

	return g_GameConfigManager.GetModule(library);
}

bool GameConfig::IsSymbol(std::string_view name) const {
	const std::string_view sigOrSymbol = GetSignature(name);
	if (sigOrSymbol.empty()) {
		S2_LOGF(LS_WARNING, "Missing signature or symbol: {}\n", name);
		return false;
	}
	return sigOrSymbol[0] == '@';
}

std::string_view GameConfig::GetSymbol(std::string_view name) const {
	const std::string_view symbol = GetSignature(name);

	if (symbol.size() <= 1) {
		S2_LOGF(LS_WARNING, "Missing symbol: {}\n", name);
		return {};
	}

	return symbol.substr(1);
}

Memory GameConfig::ResolveSignature(std::string_view name) const {
	auto module = GetModule(name);
	if (!module) {
		S2_LOGF(LS_WARNING, "Invalid module: {}\n", name);
		return {};
	}

	Memory address;

	if (IsSymbol(name)) {
		const std::string_view symbol = GetSymbol(name);
		if (symbol.empty()) {
			S2_LOGF(LS_WARNING, "Invalid symbol for {}\n", name);
			return {};
		}

		address = module->GetFunctionByName(symbol);
	} else {
		const std::string_view signature = GetSignature(name);
		if (signature.empty()) {
			S2_LOGF(LS_WARNING, "Failed to find signature for {}\n", name);
			return {};
		}

		address = module->FindPattern(DynLibUtils::ParsePattern(signature));
	}

	if (!address) {
		S2_LOGF(LS_WARNING, "Failed to find address for {}\n", name);
		return {};
	}

	return address;
}

GameConfigManager::GameConfigManager() {
	// metamod workaround
	if (Module("metamod.2.cs2").GetHandle() != nullptr) {
		m_modules.emplace("engine2", Module(utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "engine2"));
		m_modules.emplace("server", Module(utils::GameDirectory() + S2SDK_GAME_BINARY S2SDK_LIBRARY_PREFIX "server"));
	} else {
		m_modules.emplace("engine2", Module("engine2"));
		m_modules.emplace("server", Module("server"));
	}
	// add more for preload
}

uint32_t GameConfigManager::LoadGameConfigFile(plg::vector<plg::string> paths) {
	for (auto& [id, config] : m_configs) {
		if (config.GetPaths() == paths) {
			++config.m_refCount;
			return id;
		}
	}

	GameConfig gameConfig("csgo", std::move(paths));
	if (!gameConfig.Initialize()) {
		return static_cast<uint32_t>(-1);
	}

	uint32_t id = ++s_nextId;
	m_configs.emplace(id, std::move(gameConfig));
	return id;
}

void GameConfigManager::CloseGameConfigFile(uint32_t id) {
	auto it = m_configs.find(id);
	if (it != m_configs.end()) {
		auto& config = std::get<GameConfig>(*it);
		if (--config.m_refCount == 0) {
			m_configs.erase(it);
		}
	}
}

GameConfig* GameConfigManager::GetGameConfig(uint32_t id) {
	auto it = m_configs.find(id);
	if (it != m_configs.end()) {
		return &std::get<GameConfig>(*it);
	}
	return nullptr;
}

Module* GameConfigManager::GetModule(std::string_view name) {
	auto it = m_modules.find(name);
	if (it != m_modules.end()) {
		return &std::get<Module>(*it);
	}

	auto system = globals::FindModule(name);
	if (system != nullptr) {
#if S2SDK_PLATFORM_LINUX
		return &m_modules.emplace(name, Module(reinterpret_cast<link_map*>(system)->l_addr)).first->second;
#else
		return &m_modules.emplace(name, Module(system)).first->second;
#endif
	}

	return nullptr;
}

GameConfigManager g_GameConfigManager;