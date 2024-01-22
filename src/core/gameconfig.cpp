#include "gameconfig.h"
#include "module.h"

using namespace cs2sdk;

GameConfig::GameConfig(std::string game, std::string path) : m_szGameDir{std::move(game)}, m_szPath{std::move(path)}, m_pKeyValues{"Games"} {
}

GameConfig::~GameConfig() = default;

bool GameConfig::Initialize() {
	if (!m_pKeyValues.LoadFromFile(globals::fileSystem, m_szPath.c_str(), nullptr)) {
		//snprintf(conf_error, conf_error_size, "Failed to load gamedata file");
		return false;
	}

	const KeyValues* game = m_pKeyValues.FindKey(m_szGameDir.c_str());
	if (game) {
		const char* platform = CS2SDK_PLATFORM;

		const KeyValues* offsets = game->FindKey("Offsets");
		if (offsets) {
			FOR_EACH_SUBKEY(offsets, it) {
				m_umOffsets[it->GetName()] = it->GetInt(platform, -1);
			}
		}

		const KeyValues* signatures = game->FindKey("Signatures");
		if (signatures) {
			FOR_EACH_SUBKEY(signatures, it) {
				m_umLibraries[it->GetName()] = std::string(it->GetString("library"));
				m_umSignatures[it->GetName()] = std::string(it->GetString(platform));
			}
		}

		const KeyValues* patches = game->FindKey("Patches");
		if (patches) {
			FOR_EACH_SUBKEY(patches, it) {
				m_umPatches[it->GetName()] = std::string(it->GetString(platform));
			}
		}
	} else {
		//snprintf(conf_error, conf_error_size, "Failed to find game: %s", m_szGameDir.c_str());
		return false;
	}

	return true;
}

const std::string& GameConfig::GetPath() const {
	return m_szPath;
}

std::string_view GameConfig::GetSignature(const std::string& name) const {
	auto it = m_umSignatures.find(name);
	if (it == m_umSignatures.end())
		return {};
	return std::get<std::string>(*it);
}

std::string_view GameConfig::GetPatch(const std::string& name) const {
	auto it = m_umPatches.find(name);
	if (it == m_umPatches.end())
		return {};
	return std::get<std::string>(*it);
}

int GameConfig::GetOffset(const std::string& name) const {
	auto it = m_umOffsets.find(name);
	if (it == m_umOffsets.end())
		return -1;
	return std::get<int>(*it);
}

std::string_view GameConfig::GetLibrary(const std::string& name) const {
	auto it = m_umLibraries.find(name);
	if (it == m_umLibraries.end())
		return {};
	return std::get<std::string>(*it);
}

ModuleRef GameConfig::GetModule(const std::string& name) const {
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

bool GameConfig::IsSymbol(const std::string& name) const {
	const std::string_view sigOrSymbol = GetSignature(name);
	if (sigOrSymbol.empty()) {
		Error("Missing signature or symbol: %s\n", name.c_str());
		return false;
	}
	return sigOrSymbol[0] == '@';
}

std::string_view GameConfig::GetSymbol(const std::string& name) const {
	const std::string_view symbol = GetSignature(name);

	if (symbol.size() <= 1) {
		Error("Missing symbol: %s\n", name.c_str());
		return {};
	}

	return symbol.substr(1);
}

void* GameConfig::ResolveSignature(const std::string& name) const {
	auto moduleRef = GetModule(name);
	if (!moduleRef.has_value()) {
		Error("Invalid module: %s\n", name.c_str());
		return nullptr;
	}

	auto& module = moduleRef->get();
	void* address;

	if (IsSymbol(name)) {
		const std::string_view symbol = GetSymbol(name);
		if (symbol.empty()) {
			Error("Invalid symbol for %s\n", name.c_str());
			return nullptr;
		}
		address = module.GetAssembly().GetFunction(symbol.data());
	} else {
		const std::string_view signature = GetSignature(name);
		if (signature.empty()) {
			Error("Failed to find signature for %s\n", name.c_str());
			return nullptr;
		}
		auto sig = GameConfig::HexToByte(signature);
		if (sig.empty()) {
			Error("Invalid hex string\n");
			return nullptr;
		}
		address = module.FindSignature(sig);
	}

	if (address == nullptr) {
		Error("Failed to find address for %s\n", name.c_str());
		return nullptr;
	}

	return address;
}

std::vector<byte> GameConfig::HexToByte(std::string_view hexString) {
	if (hexString.empty())
		return {};

	size_t byteCount = hexString.size() / 4; // Each "\\x" represents one byte.
	if (hexString.size() % 4 != 0 || byteCount == 0) {
		printf("Invalid hex string format or byte count.\n");
		return {};
	}

	std::vector<byte> byteArray(byteCount + 1);

	for (size_t i = 0; i < hexString.size(); i += 4) {
		auto hexSubstring = hexString.substr(i + 2, 2); // Skip "\\x" and take the next two characters.
		auto [p, ec] = std::from_chars(hexSubstring.data(), hexSubstring.data() + hexSubstring.size(), byteArray[i / 4], 16);
		if (ec != std::errc() || p != hexSubstring.data() + hexSubstring.size()) {
			//printf("Failed to parse hex string at position " + i + "." + make_error_code(ec).message());
			return {}; // Return an error code.
		}
	}

	byteArray[byteCount] = '\0'; // Add a null-terminating character.

	return byteArray;
}