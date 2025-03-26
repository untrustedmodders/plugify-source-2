#include <core/game_config.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

#undef GetModuleHandle

extern "C" PLUGIN_API void* GetModule(const plg::string& name) {
	return g_GameConfigManager.GetModule(name);
}

extern "C" PLUGIN_API void* GetModuleVirtualTableByName(Module* module, const plg::string& tableName, bool decorated) {
	return module->GetVirtualTableByName(tableName, decorated);
}

extern "C" PLUGIN_API void* GetModuleFunctionByName(Module* module, const plg::string& functionName) {
	return module->GetFunctionByName(functionName);
}

extern "C" PLUGIN_API plg::string GetModuleSectionNameByName(Module* module, const plg::string& sectionName) {
	return module->GetSectionByName(sectionName).m_svSectionName;
}

extern "C" PLUGIN_API void* GetModuleSectionBaseByName(Module* module, const plg::string& sectionName) {
	return module->GetSectionByName(sectionName).m_pSectionBase;
}

extern "C" PLUGIN_API uint64_t GetModuleSectionSizeByName(Module* module, const plg::string& sectionName) {
	return static_cast<uint64_t>(module->GetSectionByName(sectionName).m_nSectionSize);
}

extern "C" PLUGIN_API void* GetModuleHandle(Module* module) {
	return module->GetHandle();
}

extern "C" PLUGIN_API void* GetModuleBase(Module* module) {
	return module->GetBase();
}

extern "C" PLUGIN_API plg::string GetModulePath(Module* module) {
	return module->GetPath();
}

extern "C" PLUGIN_API plg::string GetModuleName(Module* module) {
	return module->GetName();
}

extern "C" PLUGIN_API plg::string ModuleGetLastError(Module* module) {
	return module->GetLastError();
}

PLUGIFY_WARN_POP()
