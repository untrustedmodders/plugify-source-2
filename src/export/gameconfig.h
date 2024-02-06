#pragma once

#include <core/gameconfig.h>

extern "C"
PLUGIN_API void GameConfig_Close(CGameConfig* pGameConfig) {
	g_GameConfigManager.CloseGameConfigFile(pGameConfig);
}

extern "C"
PLUGIN_API CGameConfig* GameConfig_Load(const char* file) {
	return g_GameConfigManager.LoadGameConfigFile(file);
}

extern "C"
PLUGIN_API const char* GameConfig_GetPath(CGameConfig* pGameConfig) {
	return pGameConfig->GetPath().c_str();
}

extern "C"
PLUGIN_API const char* GameConfig_GetLibrary(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetLibrary(name).data();
}

extern "C"
PLUGIN_API const char* GameConfig_GetSignature(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetSignature(name).data();
}

extern "C"
PLUGIN_API const char* GameConfig_GetSymbol(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetSymbol(name).data();
}

extern "C"
PLUGIN_API const char* GameConfig_GetPatch(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetPatch(name).data();
}

extern "C"
PLUGIN_API int GameConfig_GetOffset(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetOffset(name);
}

extern "C"
PLUGIN_API void* GameConfig_GetAddress(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->GetAddress(name);
}

extern "C"
PLUGIN_API void* GameConfig_ResolveSignature(CGameConfig* pGameConfig, const char* name) {
	return pGameConfig->ResolveSignature(name);
}
