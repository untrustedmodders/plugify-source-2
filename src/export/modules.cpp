#include <core/game_config.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

#undef GetModuleHandle

/**
 * @brief Retrieves a module by name.
 *
 * @param name The name of the module.
 * @return A pointer to the module.
 */
extern "C" PLUGIN_API void* GetModule(const plg::string& name) {
	return g_GameConfigManager.GetModule(name);
}

/**
 * @brief Retrieves the virtual table of a module by name.
 *
 * @param module Pointer to the module.
 * @param tableName Name of the virtual table.
 * @param decorated Whether the table name is decorated.
 * @return A pointer to the virtual table.
 */
extern "C" PLUGIN_API void* GetModuleVirtualTableByName(DynLibUtils::CModule* module, const plg::string& tableName, bool decorated) {
	return module->GetVirtualTableByName(tableName, decorated);
}

/**
 * @brief Retrieves a function from a module by name.
 *
 * @param module Pointer to the module.
 * @param functionName Name of the function.
 * @return A pointer to the function.
 */
extern "C" PLUGIN_API void* GetModuleFunctionByName(DynLibUtils::CModule* module, const plg::string& functionName) {
	return module->GetFunctionByName(functionName);
}

/**
 * @brief Retrieves the name of a section in a module.
 *
 * @param module Pointer to the module.
 * @param sectionName Name of the section.
 * @return The name of the section as a string.
 */
extern "C" PLUGIN_API plg::string GetModuleSectionNameByName(DynLibUtils::CModule* module, const plg::string& sectionName) {
	auto section = module->GetSectionByName(sectionName);
	if (!section) {
		S2_LOGF(LS_WARNING, "Failed to get the module section by '{}' name.\n", sectionName);
		return {};
	}

	return section->m_svSectionName;
}

/**
 * @brief Retrieves the base address of a section in a module.
 *
 * @param module Pointer to the module.
 * @param sectionName Name of the section.
 * @return A pointer to the base address of the section.
 */
extern "C" PLUGIN_API void* GetModuleSectionBaseByName(DynLibUtils::CModule* module, const plg::string& sectionName) {
	auto section = module->GetSectionByName(sectionName);
	if (!section) {
		S2_LOGF(LS_WARNING, "Failed to get the module section base by '{}' name.\n", sectionName);
		return nullptr;
	}

	return section->GetPtr();
}

/**
 * @brief Retrieves the size of a section in a module.
 *
 * @param module Pointer to the module.
 * @param sectionName Name of the section.
 * @return The size of the section as a 64-bit unsigned integer.
 */
extern "C" PLUGIN_API uint64_t GetModuleSectionSizeByName(DynLibUtils::CModule* module, const plg::string& sectionName) {
	auto section = module->GetSectionByName(sectionName);
	if (!section) {
		S2_LOGF(LS_WARNING, "Failed to get the module section size by '{}' name.\n", sectionName);
		return 0;
	}

	return static_cast<uint64_t>(section->m_nSectionSize);
}

/**
 * @brief Retrieves the handle of a module.
 *
 * @param module Pointer to the module.
 * @return A pointer to the module handle.
 */
extern "C" PLUGIN_API void* GetModuleHandle(DynLibUtils::CModule* module) {
	return module->GetHandle();
}

/**
 * @brief Retrieves the base address of a module.
 *
 * @param module Pointer to the module.
 * @return A pointer to the module base address.
 */
extern "C" PLUGIN_API void* GetModuleBase(DynLibUtils::CModule* module) {
	return module->GetBase();
}

/**
 * @brief Retrieves the file path of a module.
 *
 * @param module Pointer to the module.
 * @return The file path of the module as a string.
 */
extern "C" PLUGIN_API plg::string GetModulePath(DynLibUtils::CModule* module) {
	return module->GetPath();
}

/**
 * @brief Retrieves the name of a module.
 *
 * @param module Pointer to the module.
 * @return The name of the module as a string.
 */
extern "C" PLUGIN_API plg::string GetModuleName(DynLibUtils::CModule* module) {
	return module->GetName();
}

/**
 * @brief Retrieves the last error message from a module.
 *
 * @param module Pointer to the module.
 * @return The last error message as a string.
 */
extern "C" PLUGIN_API plg::string GetModuleLastError(DynLibUtils::CModule* module) {
	return module->GetLastError();
}

PLUGIFY_WARN_POP()
