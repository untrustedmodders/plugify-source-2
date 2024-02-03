#pragma once

#include <plugify/cpp_plugin.h>
#include <dynohook/dynohook.h>
#include <plugin_export.h>

namespace cs2sdk {
	class Source2SDK : public plugify::IPluginEntry {
	public:
		void OnPluginStart() override;
		void OnPluginEnd() override;

		static dyno::ReturnAction Hook_StartupServer(dyno::CallbackType type, dyno::IHook& hook);
		static dyno::ReturnAction Hook_FireEvent(dyno::CallbackType type, dyno::IHook& hook);
		static dyno::ReturnAction Hook_PostEvent(dyno::CallbackType type, dyno::IHook& hook);
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
		/*static dyno::ReturnAction Hook_ClientCommand(dyno::CallbackType type, dyno::IHook& hook);
		static dyno::ReturnAction Hook_CheckTransmit(dyno::CallbackType type, dyno::IHook& hook); */

	};

	extern Source2SDK g_sdk;
}