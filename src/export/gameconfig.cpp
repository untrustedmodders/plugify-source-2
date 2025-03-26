#include <core/game_config.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Closes a game configuration file.
 *
 * This function closes the specified game configuration file.
 *
 * @param id An id to the GameConfig object to be closed.
 */
extern "C" PLUGIN_API void CloseGameConfigFile(uint32_t id) {
	g_GameConfigManager.CloseGameConfigFile(id);
}

/**
 * @brief Loads a game configuration file.
 *
 * This function loads a game configuration file from the specified file path.
 *
 * @param paths The paths to the game configuration file to be loaded.
 * @return A handle to the loaded CGameConfig object, or nullptr if loading fails.
 */
extern "C" PLUGIN_API uint32_t LoadGameConfigFile(const plg::vector<plg::string>& paths) {
	return g_GameConfigManager.LoadGameConfigFile(paths);
}

/**
 * @brief Retrieves the path of a game configuration.
 *
 * This function retrieves the paths of the specified game configuration and
 * stores it in the provided output array.
 *
 * @param id An id to the GameConfig object whose path is to be retrieved.
 * @return Am array of paths where the configuration is stored.
 */
extern "C" PLUGIN_API plg::vector<plg::string> GetGameConfigPaths(uint32_t id) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetPaths();
	}
	return {};
}

/**
 * @brief Retrieves a library associated with the game configuration.
 *
 * This function retrieves a library by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param id An id to the GameConfig object from which to retrieve the library.
 * @param name The name of the library to be retrieved.
 * @return A string where the library will be stored.
 */
extern "C" PLUGIN_API plg::string GetGameConfigLibrary(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetLibrary(name);
	}
	return {};
}

/**
 * @brief Retrieves the signature associated with the game configuration.
 *
 * This function retrieves a signature by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param id An id to the GameConfig object from which to retrieve the signature.
 * @param name The name of the signature to be retrieved.
 * @return A string where the signature will be stored.
 */
extern "C" PLUGIN_API plg::string GetGameConfigSignature(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetSignature(name);
	}
	return {};
}

/**
 * @brief Retrieves a symbol associated with the game configuration.
 *
 * This function retrieves a symbol by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param id An id to the GameConfig object from which to retrieve the symbol.
 * @param name The name of the symbol to be retrieved.
 * @return A string where the symbol will be stored.
 */
extern "C" PLUGIN_API plg::string GetGameConfigSymbol(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetSymbol(name);
	}
	return {};
}

/**
 * @brief Retrieves a patch associated with the game configuration.
 *
 * This function retrieves a patch by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param id An id to the GameConfig object from which to retrieve the patch.
 * @param name The name of the patch to be retrieved.
 * @return A string where the patch will be stored.
 */
extern "C" PLUGIN_API plg::string GetGameConfigPatch(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetPatch(name);
	}
	return {};
}

/**
 * @brief Retrieves the offset associated with a name from the game configuration.
 *
 * This function retrieves the offset of the specified name from the game configuration.
 *
 * @param id An id to the GameConfig object from which to retrieve the offset.
 * @param name The name whose offset is to be retrieved.
 * @return The offset associated with the specified name.
 */
extern "C" PLUGIN_API int GetGameConfigOffset(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetOffset(name);
	}
	return -1;
}

/**
 * @brief Retrieves the address associated with a name from the game configuration.
 *
 * This function retrieves the address of the specified name from the game configuration.
 *
 * @param id An id to the GameConfig object from which to retrieve the address.
 * @param name The name whose address is to be retrieved.
 * @return A pointer to the address associated with the specified name.
 */
extern "C" PLUGIN_API void* GetGameConfigAddress(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->GetAddress(name);
	}
	return nullptr;
}

/**
 * @brief Retrieves the memory signature associated with a name from the game configuration.
 *
 * This function resolves and retrieves the memory signature of the specified name from the game configuration.
 *
 * @param id An id to the GameConfig object from which to retrieve the memory signature.
 * @param name The name whose memory signature is to be resolved and retrieved.
 * @return A pointer to the memory signature associated with the specified name.
 */
extern "C" PLUGIN_API void* GetGameConfigMemSig(uint32_t id, const plg::string& name) {
	if (auto gameConfig = g_GameConfigManager.GetGameConfig(id)) {
		return gameConfig->ResolveSignature(name);
	}
	return nullptr;
}

PLUGIFY_WARN_POP()
