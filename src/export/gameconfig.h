#pragma once

#include <core/gameconfig.h>
#include <core/utils.h>

extern "C"
PLUGIN_API void GameConfig_Delete(cs2sdk::CGameConfig* pGameConfig) {
	delete pGameConfig;
}

extern "C"
PLUGIN_API cs2sdk::CGameConfig* GameConfig_Create(const char* file) {
	auto pGameConfig = new cs2sdk::CGameConfig("csgo", cs2sdk::utils::GamedataDirectory() + file);
	if (!pGameConfig->Initialize()) {
		Log_Error(LOG_GENERAL, "Could not read \"%s\".", pGameConfig->GetPath().c_str());
		delete pGameConfig;
		return nullptr;
	}
	return pGameConfig;
}

extern "C"
PLUGIN_API const char* GameConfig_GetPath(cs2sdk::CGameConfig* pGameConfig) {
	return pGameConfig->GetPath().c_str();
}

extern "C"
PLUGIN_API const char* GameConfig_GetLibrary(cs2sdk::CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetLibrary(name).data();
}

extern "C"
PLUGIN_API const char* GameConfig_GetSignature(cs2sdk::CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetSignature(name).data();
}

extern "C"
PLUGIN_API const char* GameConfig_GetSymbol(cs2sdk::CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetSymbol(name).data();
}

extern "C"
PLUGIN_API const char* GameConfig_GetPatch(cs2sdk::CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetPatch(name).data();
}

extern "C"
PLUGIN_API int GameConfig_GetOffset(cs2sdk::CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetOffset(name);
}

extern "C"
PLUGIN_API void* GameConfig_ResolveSignature(cs2sdk::CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->ResolveSignature(name);
}