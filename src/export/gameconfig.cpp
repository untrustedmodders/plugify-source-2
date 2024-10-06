#include <core/game_config.h>
#include <plugin_export.h>

/**
 * @brief Closes a game configuration file.
 *
 * This function closes the specified game configuration file.
 *
 * @param pGameConfig A pointer to the game configuration to be closed.
 */
extern "C" PLUGIN_API void CloseGameConfigFile(CGameConfig* pGameConfig)
{
	g_pGameConfigManager.CloseGameConfigFile(pGameConfig);
}

/**
 * @brief Loads a game configuration file.
 *
 * This function loads a game configuration file from the specified file path.
 *
 * @param file The path to the game configuration file to be loaded.
 * @return A pointer to the loaded CGameConfig object, or nullptr if loading fails.
 */
extern "C" PLUGIN_API CGameConfig* LoadGameConfigFile(const plg::string& file)
{
	return g_pGameConfigManager.LoadGameConfigFile(file);
}

/**
 * @brief Retrieves the path of a game configuration.
 *
 * This function retrieves the path of the specified game configuration and
 * stores it in the provided output string.
 *
 * @param output A reference to a string where the path will be stored.
 * @param pGameConfig A pointer to the game configuration whose path is to be retrieved.
 */
extern "C" PLUGIN_API void GetGameConfigPath(plg::string& output, CGameConfig* pGameConfig)
{
	std::construct_at(&output, pGameConfig->GetPath());
}

/**
 * @brief Retrieves a library associated with the game configuration.
 *
 * This function retrieves a library by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param output A reference to a string where the library will be stored.
 * @param pGameConfig A pointer to the game configuration from which to retrieve the library.
 * @param name The name of the library to be retrieved.
 */
extern "C" PLUGIN_API void GetGameConfigLibrary(plg::string& output, CGameConfig* pGameConfig, const plg::string& name)
{
	std::construct_at(&output, pGameConfig->GetLibrary(name));
}

/**
 * @brief Retrieves the signature associated with the game configuration.
 *
 * This function retrieves a signature by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param output A reference to a string where the signature will be stored.
 * @param pGameConfig A pointer to the game configuration from which to retrieve the signature.
 * @param name The name of the signature to be retrieved.
 */
extern "C" PLUGIN_API void GetGameConfigSignature(plg::string& output, CGameConfig* pGameConfig, const plg::string& name)
{
	std::construct_at(&output, pGameConfig->GetSignature(name));
}

/**
 * @brief Retrieves a symbol associated with the game configuration.
 *
 * This function retrieves a symbol by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param output A reference to a string where the symbol will be stored.
 * @param pGameConfig A pointer to the game configuration from which to retrieve the symbol.
 * @param name The name of the symbol to be retrieved.
 */
extern "C" PLUGIN_API void GetGameConfigSymbol(plg::string& output, CGameConfig* pGameConfig, const plg::string& name)
{
	std::construct_at(&output, pGameConfig->GetSymbol(name));
}

/**
 * @brief Retrieves a patch associated with the game configuration.
 *
 * This function retrieves a patch by name from the specified game configuration
 * and stores it in the provided output string.
 *
 * @param output A reference to a string where the patch will be stored.
 * @param pGameConfig A pointer to the game configuration from which to retrieve the patch.
 * @param name The name of the patch to be retrieved.
 */
extern "C" PLUGIN_API void GetGameConfigPatch(plg::string& output, CGameConfig* pGameConfig, const plg::string& name)
{
	std::construct_at(&output, pGameConfig->GetPatch(name));
}

/**
 * @brief Retrieves the offset associated with a name from the game configuration.
 *
 * This function retrieves the offset of the specified name from the game configuration.
 *
 * @param pGameConfig A pointer to the game configuration from which to retrieve the offset.
 * @param name The name whose offset is to be retrieved.
 * @return The offset associated with the specified name.
 */
extern "C" PLUGIN_API int GetGameConfigOffset(CGameConfig* pGameConfig, const plg::string& name)
{
	return pGameConfig->GetOffset(name);
}

/**
 * @brief Retrieves the address associated with a name from the game configuration.
 *
 * This function retrieves the address of the specified name from the game configuration.
 *
 * @param pGameConfig A pointer to the game configuration from which to retrieve the address.
 * @param name The name whose address is to be retrieved.
 * @return A pointer to the address associated with the specified name.
 */
extern "C" PLUGIN_API void* GetGameConfigAddress(CGameConfig* pGameConfig, const plg::string& name)
{
	return pGameConfig->GetAddress(name);
}

/**
 * @brief Retrieves the memory signature associated with a name from the game configuration.
 *
 * This function resolves and retrieves the memory signature of the specified name from the game configuration.
 *
 * @param pGameConfig A pointer to the game configuration from which to retrieve the memory signature.
 * @param name The name whose memory signature is to be resolved and retrieved.
 * @return A pointer to the memory signature associated with the specified name.
 */
extern "C" PLUGIN_API void* GetGameConfigMemSig(CGameConfig* pGameConfig, const plg::string& name)
{
	return pGameConfig->ResolveSignature(name);
}
