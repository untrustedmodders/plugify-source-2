#pragma once

#include <plugify/cpp_plugin.hpp>
#include <plugify/polyhook.hpp>
#include <plugin_export.h>

#include <igamesystem.h>
#include <igamesystemfactory.h>
#include <networksystem/inetworkmessages.h>

class Source2SDK : public plg::IPluginEntry, public CBaseGameSystem
{
public:
	void OnPluginStart() override;
	void OnPluginEnd() override;
	static void OnServerStartup();

	static poly::ReturnAction Hook_RegisterLoopMode(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_UnregisterLoopMode(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_StartupServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ActivateServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_FireEvent(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_OnLevelInit(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_OnLevelShutdown(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_GameFrame(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientActive(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientDisconnect(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientPutInServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientSettingsChanged(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_OnClientConnected(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientFullyConnect(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientConnect(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ClientCommand(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_UpdateWhenNotInGame(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_PreWorldUpdate(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_FireOutputInternal(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_DispatchConCommand(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_SetClientListening(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
};

extern Source2SDK g_sdk;