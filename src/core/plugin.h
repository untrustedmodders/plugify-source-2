#pragma once

#include <plugify/cpp_plugin.h>
#include <plugify/dynohook.h>
#include <plugin_export.h>

class Source2SDK : public plugify::IPluginEntry
{
public:
	void OnPluginStart() override;
	void OnPluginEnd() override;
	static void OnServerStartup();

	static dyno::ReturnAction Hook_StartupServer(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_FireEvent(dyno::CallbackType type, dyno::IHook& hook);
	//static dyno::ReturnAction Hook_PostEvent(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_OnLevelInit(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_OnLevelShutdown(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_GameFrame(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientActive(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientDisconnect(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientPutInServer(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientSettingsChanged(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_OnClientConnected(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientFullyConnect(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientConnect(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ClientCommand(dyno::CallbackType type, dyno::IHook& hook);
	// static dyno::ReturnAction Hook_CheckTransmit(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_ServerHibernationUpdate(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_GameServerSteamAPIActivated(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_GameServerSteamAPIDeactivated(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_OnHostNameChanged(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_PreFatalShutdown(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_UpdateWhenNotInGame(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_PreWorldUpdate(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_FireOutputInternal(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_DispatchConCommand(dyno::CallbackType type, dyno::IHook& hook);
	static dyno::ReturnAction Hook_SetClientListening(dyno::CallbackType type, dyno::IHook& hook);
};

extern Source2SDK g_sdk;