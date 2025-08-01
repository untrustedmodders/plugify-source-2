#include "game_config.hpp"

#include <core/sdk/utils.h>
#include <plugify-configs/plugify-configs.hpp>

static plugify::LoadFlag defaultFlags = plugify::LoadFlag::Noload | plugify::LoadFlag::Lazy | plugify::LoadFlag::DontResolveDllReferences;
static plugify::Assembly::SearchDirs noDirs{};

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
constexpr plugify::MemAddr VALID_MINIMUM_MEMORY_ADDRESS(0x10000);

plugify::MemAddr GameConfig::GetAddress(std::string_view name) const {
	auto it = m_addresses.find(name);
	if (it == m_addresses.end())
		return {};

	const auto& addrConf = std::get<AddressConf>(*it);

	auto addr = ResolveSignature(addrConf.signature);
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

const plugify::Assembly* GameConfig::GetModule(std::string_view name) const {
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

plugify::MemAddr GameConfig::ResolveSignature(std::string_view name) const {
	auto module = GetModule(name);
	if (!module) {
		S2_LOGF(LS_WARNING, "Invalid module: {}\n", name);
		return {};
	}

	plugify::MemAddr address;

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

		address = module->FindPattern(signature);
	}

	if (!address) {
		S2_LOGF(LS_WARNING, "Failed to find address for {}\n", name);
		return {};
	}

	return address;
}

GameConfigManager::GameConfigManager() {
	// metamod workaround
	if (plugify::Assembly("metamod.2.cs2").GetHandle() != nullptr) {
		m_modules.try_emplace("engine2", utils::GameDirectory() + S2SDK_ROOT_BINARY S2SDK_LIBRARY_PREFIX "engine2", defaultFlags, noDirs, true);
		m_modules.try_emplace("server",  utils::GameDirectory() + S2SDK_GAME_BINARY S2SDK_LIBRARY_PREFIX "server", defaultFlags, noDirs, true);
	} else {
		m_modules.try_emplace("engine2", "engine2", defaultFlags, noDirs, true);
		m_modules.try_emplace("server", "server", defaultFlags, noDirs, true);
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

	GameConfig gameConfig(S2SDK_GAME_NAME, std::move(paths));
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

plugify::Assembly* GameConfigManager::GetModule(std::string_view name) {
	auto it = m_modules.find(name);
	if (it != m_modules.end()) {
		return &std::get<plugify::Assembly>(*it);
	}

	auto system = globals::FindModule(name);
	if (system != nullptr) {
		return &m_modules.try_emplace(name, plugify::Assembly::Handle{system}, defaultFlags, noDirs, true).first->second;
	}

	return nullptr;
}

GameConfigManager g_GameConfigManager;