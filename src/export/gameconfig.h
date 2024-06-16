#pragma once

#include <core/game_config.h>

extern "C" PLUGIN_API void CloseGameConfigFile(CGameConfig* pGameConfig)
{
	g_pGameConfigManager.CloseGameConfigFile(pGameConfig);
}

extern "C" PLUGIN_API CGameConfig* LoadGameConfigFile(const std::string& file)
{
	return g_pGameConfigManager.LoadGameConfigFile(file);
}

extern "C" PLUGIN_API void GetGameConfigPath(std::string& output, CGameConfig* pGameConfig)
{
	std::construct_at(&output, pGameConfig->GetPath());
}

extern "C" PLUGIN_API void GetGameConfigLibrary(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	std::construct_at(&output, pGameConfig->GetLibrary(name));
}

extern "C" PLUGIN_API void GetGameConfigSignature(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	std::construct_at(&output, pGameConfig->GetSignature(name));
}

extern "C" PLUGIN_API void GetGameConfigSymbol(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	std::construct_at(&output, pGameConfig->GetSymbol(name));
}

extern "C" PLUGIN_API void GetGameConfigPatch(std::string& output, CGameConfig* pGameConfig, const std::string& name)
{
	std::construct_at(&output, pGameConfig->GetPatch(name));
}

extern "C" PLUGIN_API int GetGameConfigOffset(CGameConfig* pGameConfig, const std::string& name)
{
	return pGameConfig->GetOffset(name);
}

extern "C" PLUGIN_API void* GetGameConfigAddress(CGameConfig* pGameConfig, const std::string& name)
{
	return pGameConfig->GetAddress(name);
}

extern "C" PLUGIN_API void* GetGameConfigMemSig(CGameConfig* pGameConfig, const std::string& name)
{
	return pGameConfig->ResolveSignature(name);
}
