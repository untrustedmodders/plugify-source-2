#pragma once

#include <core/gameconfig.h>

extern "C" PLUGIN_API void GameConfig_Close(CGameConfig* pGameConfig)
{
	g_GameConfigManager.CloseGameConfigFile(pGameConfig);
}

extern "C" PLUGIN_API CGameConfig* GameConfig_Load(const std::string& file)
{
	return g_GameConfigManager.LoadGameConfigFile(file);
}

extern "C" PLUGIN_API void GameConfig_GetPath(std::string& output, CGameConfig* pGameConfig)
{
	output = pGameConfig->GetPath();
}

extern "C" PLUGIN_API void GameConfig_GetLibrary(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	output = pGameConfig->GetLibrary(name).data();
}

extern "C" PLUGIN_API void GameConfig_GetSignature(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	output = pGameConfig->GetSignature(name).data();
}

extern "C" PLUGIN_API void GameConfig_GetSymbol(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	output = pGameConfig->GetSymbol(name);
}

extern "C" PLUGIN_API void GameConfig_GetPatch(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	output = pGameConfig->GetPatch(name);
}

extern "C" PLUGIN_API int GameConfig_GetOffset(CGameConfig* pGameConfig, const std::string& name)
{
	return pGameConfig->GetOffset(name);
}

extern "C" PLUGIN_API void* GameConfig_GetAddress(CGameConfig* pGameConfig, const std::string& name)
{
	return pGameConfig->GetAddress(name);
}

extern "C" PLUGIN_API void* GameConfig_ResolveSignature(CGameConfig* pGameConfig, const std::string& name)
{
	return pGameConfig->ResolveSignature(name);
}
