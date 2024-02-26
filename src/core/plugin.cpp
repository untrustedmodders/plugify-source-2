#include "plugin.h"

#include <eiface.h>
#include <engine/igameeventsystem.h>
#include <entity2/entitysystem.h>
#include <igameevents.h>
#include <iserver.h>

#include <ISmmPlugin.h>

#include "con_command_manager.h"
#include "event_manager.h"
#include "hook_manager.h"
#include "listeners.h"
#include "output_manager.h"
#include "player_manager.h"
#include "server_manager.h"
#include "timer_system.h"
#include "voice_manager.h"

#include <export/commands.h>
#include <export/console.h>
#include <export/engine.h>
#include <export/entities.h>
#include <export/events.h>
#include <export/gameconfig.h>
#include <export/logger.h>
#include <export/schema.h>
#include <export/timers.h>
#include <export/voice.h>

Source2SDK g_sdk;

CGameEntitySystem* GameEntitySystem()
{
	static int offset = globals::g_GameConfig->GetOffset("GameEntitySystem");
	return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(g_pGameResourceServiceServer) + offset);
}

class CEntityListener : public IEntityListener
{
	void OnEntitySpawned(CEntityInstance* pEntity) override
	{
		GetOnEntitySpawnedListenerManager().Notify(pEntity);
	}
	void OnEntityCreated(CEntityInstance* pEntity) override
	{
		GetOnEntityCreatedListenerManager().Notify(pEntity);
	}
	void OnEntityDeleted(CEntityInstance* pEntity) override
	{
		GetOnEntityDeletedListenerManager().Notify(pEntity);
	}
	void OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent) override
	{
		GetOnEntityParentChangedListenerManager().Notify(pEntity, pNewParent);
	}
} g_pEntityListener;

void Source2SDK::OnPluginStart()
{
	g_Logger.Message("OnPluginStart!\n");
	globals::Initialize();

	using enum dyno::CallbackType;
	g_HookManager.AddHookMemFunc(&IMetamodListener::OnLevelInit, globals::g_MetamodListener, Hook_OnLevelInit, Post);
	g_HookManager.AddHookMemFunc(&IMetamodListener::OnLevelShutdown, globals::g_MetamodListener, Hook_OnLevelShutdown, Post);
	g_HookManager.AddHookMemFunc(&IServerGameDLL::GameFrame, g_pSource2Server, Hook_GameFrame, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientActive, g_pSource2GameClients, Hook_ClientActive, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientDisconnect, g_pSource2GameClients, Hook_ClientDisconnect, Pre, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientPutInServer, g_pSource2GameClients, Hook_ClientPutInServer, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientSettingsChanged, g_pSource2GameClients, Hook_ClientSettingsChanged, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::OnClientConnected, g_pSource2GameClients, Hook_OnClientConnected, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientFullyConnect, g_pSource2GameClients, Hook_ClientFullyConnect, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientConnect, g_pSource2GameClients, Hook_ClientConnect, Pre, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientCommand, g_pSource2GameClients, Hook_ClientCommand, Pre);
	g_HookManager.AddHookMemFunc(&INetworkServerService::StartupServer, g_pNetworkServerService, Hook_StartupServer, Post);
	// g_HookManager.AddHookMemFunc(&ISource2GameEntities::CheckTransmit, g_pSource2GameEntities, Hook_CheckTransmit, Post);
	using PostEventAbstract = void (IGameEventSystem::*)(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64* clients, INetworkSerializable* pEvent, const void* pData, unsigned long nSize, NetChannelBufType_t bufType);
	g_HookManager.AddHookMemFunc<PostEventAbstract>(&IGameEventSystem::PostEventAbstract, g_gameEventSystem, Hook_PostEvent, Post);
	g_HookManager.AddHookMemFunc(&IGameEventManager2::FireEvent, g_gameEventSystem, Hook_FireEvent, Pre, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::ServerHibernationUpdate, g_pSource2Server, Hook_ServerHibernationUpdate, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::GameServerSteamAPIActivated, g_pSource2Server, Hook_GameServerSteamAPIActivated, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::GameServerSteamAPIDeactivated, g_pSource2Server, Hook_GameServerSteamAPIDeactivated, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::OnHostNameChanged, g_pSource2Server, Hook_OnHostNameChanged, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::PreFatalShutdown, g_pSource2Server, Hook_PreFatalShutdown, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::UpdateWhenNotInGame, g_pSource2Server, Hook_UpdateWhenNotInGame, Post);
	g_HookManager.AddHookMemFunc(&ISource2Server::PreWorldUpdate, g_pSource2Server, Hook_PreWorldUpdate, Post);
	g_HookManager.AddHookMemFunc(&ICvar::DispatchConCommand, g_pCVar, Hook_DispatchConCommand, Pre, Post);
	g_HookManager.AddHookMemFunc(&IVEngineServer2::SetClientListening, g_pEngineServer2, Hook_SetClientListening, Pre);

	using FireOutputInternal = void (*)(CEntityIOOutput* const, CEntityInstance*, CEntityInstance*, const CVariant* const, float);
	g_HookManager.AddHookDetourFunc<FireOutputInternal>("CEntityIOOutput_FireOutputInternal", Hook_FireOutputInternal, Pre);
}

void Source2SDK::OnPluginEnd()
{
	globals::Terminate();
	g_HookManager.UnhookAll();
	int iListener = g_pEntitySystem->m_entityListeners.Find(&g_pEntityListener);
	if (iListener != -1)
	{
		g_pEntitySystem->m_entityListeners.Remove(iListener);
	}
	g_Logger.Message("OnPluginEnd!\n");
}

dyno::ReturnAction Source2SDK::Hook_StartupServer(dyno::CallbackType type, dyno::IHook& hook)
{
	g_Logger.Message("Startup server\n");

	g_pEntitySystem = GameEntitySystem();
	g_pEntitySystem->m_entityListeners.AddToTail(&g_pEntityListener);
	g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
	gpGlobals = g_pNetworkGameServer->GetGlobals();

	if (gpGlobals == nullptr)
	{
		g_Logger.Error("Failed to lookup gpGlobals\n");
		return dyno::ReturnAction::Ignored;
	}

	GetOnServerStartupListenerManager().Notify();

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_FireEvent(dyno::CallbackType type, dyno::IHook& hook)
{
	// g_Logger.MessageFormat("FireEvent = %s\n", event->GetName() );
	return type == dyno::CallbackType::Post ? g_EventManager.Hook_OnFireEvent_Post(hook) : g_EventManager.Hook_OnFireEvent(hook);
}

dyno::ReturnAction Source2SDK::Hook_PostEvent(dyno::CallbackType type, dyno::IHook& hook)
{
	// g_Logger.MessageFormat("PostEvent = %d, %d, %d, %lli\n", nSlot, bLocalOnly, nClientCount, clients );
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_OnLevelInit(dyno::CallbackType type, dyno::IHook& hook)
{
	auto pMapName = dyno::GetArgument<const char*>(hook, 1);
	auto pMapEntities = dyno::GetArgument<const char*>(hook, 2);
	g_Logger.MessageFormat("OnLevelInit = %s\n", pMapName);
	GetOnLevelInitListenerManager().Notify(pMapName, pMapEntities);
	return dyno::ReturnAction::Ignored;
};

dyno::ReturnAction Source2SDK::Hook_OnLevelShutdown(dyno::CallbackType type, dyno::IHook& hook)
{
	g_Logger.Message("OnLevelShutdown\n");
	g_TimerSystem.OnLevelShutdown();
	g_PlayerManager.OnLevelShutdown();
	GetOnLevelShutdownListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
};

dyno::ReturnAction Source2SDK::Hook_GameFrame(dyno::CallbackType type, dyno::IHook& hook)
{
	// bool simulating, bool bFirstTick, bool bLastTick
	auto simulating = dyno::GetArgument<bool>(hook, 1);
	auto bFirstTick = dyno::GetArgument<bool>(hook, 2);
	auto bLastTick = dyno::GetArgument<bool>(hook, 3);

	g_ServerManager.OnGameFrame();
	g_TimerSystem.OnGameFrame(simulating);
	g_PlayerManager.RunAuthChecks();

	GetOnGameFrameListenerManager().Notify(simulating, bFirstTick, bLastTick);
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientActive(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto bLoadGame = dyno::GetArgument<bool>(hook, 2);
	// auto pszName = dyno::GetArgument<const char*>(hook, 3);
	// auto xuid = dyno::GetArgument<uint64>(hook, 4);
	// g_Logger.MessageFormat("ClientActive = %d, %d, \"%s\", %lli\n", slot, bLoadGame, pszName, xuid);

	g_PlayerManager.OnClientActive(slot, bLoadGame);

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientDisconnect(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot, int reason, const char* pszName, uint64 xuid, const char* pszNetworkID
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto reason = (ENetworkDisconnectionReason)dyno::GetArgument<int>(hook, 2);
	auto pszName = dyno::GetArgument<const char*>(hook, 3);
	auto xuid = dyno::GetArgument<uint64>(hook, 4);
	auto pszNetworkID = dyno::GetArgument<const char*>(hook, 5);
	// g_Logger.MessageFormat("ClientDisconnect - %d, %d, \"%s\", %lli, \"%s\"\n", slot, reason, pszName, xuid, pszNetworkID);

	if (type == dyno::CallbackType::Pre)
	{
		g_PlayerManager.OnClientDisconnect(slot, reason, pszName, xuid, pszNetworkID);
	}
	else
	{
		g_PlayerManager.OnClientDisconnect_Post(slot, reason, pszName, xuid, pszNetworkID);
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientPutInServer(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot, char const *pszName, int type, uint64 xuid
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto pszName = dyno::GetArgument<const char*>(hook, 2);
	auto conType = dyno::GetArgument<int>(hook, 3);
	auto xuid = dyno::GetArgument<uint64>(hook, 4);
	// g_Logger.MessageFormat("ClientPutInServer - %d, \"%s\", %d, %d, %lli\n", slot, pszName, conType, xuid);

	g_PlayerManager.OnClientPutInServer(slot, pszName, conType, xuid);

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientSettingsChanged(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	g_Logger.MessageFormat("ClientSettingsChanged - %d\n", slot);
	GetOnClientSettingsChangedListenerManager().Notify(slot.Get());
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_OnClientConnected(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot, const cha*r pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto pszName = dyno::GetArgument<const char*>(hook, 2);
	auto xuid = dyno::GetArgument<uint64>(hook, 3);
	auto pszNetworkID = dyno::GetArgument<const char*>(hook, 4);
	auto pszAddress = dyno::GetArgument<const char*>(hook, 5);
	auto bFakePlayer = dyno::GetArgument<bool>(hook, 6);
	// g_Logger.MessageFormat("OnClientConnected = %d, \"%s\", %lli, \"%s\", \"%s\", %d\n", slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	g_PlayerManager.OnClientConnected(slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientFullyConnect(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	g_Logger.MessageFormat("ClientFullyConnect = %d\n", slot);
	GetOnClientFullyConnectListenerManager().Notify(slot.Get());
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientConnect(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString *pRejectReason
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto pszName = dyno::GetArgument<const char*>(hook, 2);
	auto xuid = dyno::GetArgument<uint64>(hook, 3);
	auto pszNetworkID = dyno::GetArgument<const char*>(hook, 4);
	bool unk1 = dyno::GetArgument<bool>(hook, 5);
	auto pRejectReason = dyno::GetArgument<CBufferString*>(hook, 6);

	// g_Logger.MessageFormat("ClientConnect = %d, \"%s\", %lli, \"%s\", %d, \"%s\" \n", slot, pszName, xuid, pszNetworkID, unk1, pRejectReason->ToGrowable()->Get());

	if (type == dyno::CallbackType::Pre)
	{
		g_PlayerManager.OnClientConnect(slot, pszName, xuid, pszNetworkID, unk1, pRejectReason);
	}
	else
	{
		bool origRet = dyno::GetReturn<bool>(hook);
		bool newRet = g_PlayerManager.OnClientConnect_Post(slot, pszName, xuid, pszNetworkID, unk1, pRejectReason, origRet);
		if (newRet != origRet)
		{
			dyno::SetReturn<bool>(hook, newRet);
			return dyno::ReturnAction::Handled;
		}
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientCommand(dyno::CallbackType type, dyno::IHook& hook)
{
	// CPlayerSlot nSlot, const CCommand& _cmd
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto args = dyno::GetArgument<const CCommand&>(hook, 2);

	g_Logger.MessageFormat("ClientCommand = %d, \"%s\"\n", slot, args.GetCommandString());
	const char* cmd = args.Arg(0);

	g_PlayerManager.OnClientCommand(slot, args);

	auto result = g_CommandManager.ExecuteCommandCallbacks(cmd, CCommandContext(CommandTarget_t::CT_NO_TARGET, slot), args, HookMode::Pre, CommandCallingContext::Console);
	if (result >= ResultType::Handled)
	{
		return dyno::ReturnAction::Supercede;
	}

	return dyno::ReturnAction::Ignored;
}

/*dyno::ReturnAction Source2SDK::Hook_CheckTransmit(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot nSlot, const CCommand& _cmd
	GetOnCheckTransmitListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
}*/

dyno::ReturnAction Source2SDK::Hook_ServerHibernationUpdate(dyno::CallbackType type, dyno::IHook& hook)
{
	// bool bHibernating
	auto bHibernating = dyno::GetArgument<bool>(hook, 1);
	g_Logger.MessageFormat("ServerHibernationUpdate = %d\n", bHibernating);
	GetOnServerHibernationUpdateListenerManager().Notify(bHibernating);
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_GameServerSteamAPIActivated(dyno::CallbackType type, dyno::IHook& hook)
{
	g_Logger.Message("GameServerSteamAPIActivated\n");
	GetOnGameServerSteamAPIActivatedListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_GameServerSteamAPIDeactivated(dyno::CallbackType type, dyno::IHook& hook)
{
	g_Logger.Message("GameServerSteamAPIDeactivated\n");
	GetOnGameServerSteamAPIDeactivatedListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_OnHostNameChanged(dyno::CallbackType type, dyno::IHook& hook)
{
	// const char *pHostname
	auto pHostname = dyno::GetArgument<const char*>(hook, 1);
	g_Logger.MessageFormat("OnHostNameChanged = %s\n", pHostname);
	GetOnHostNameChangedListenerManager().Notify(pHostname);
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_PreFatalShutdown(dyno::CallbackType type, dyno::IHook& hook)
{
	g_Logger.Message("PreFatalShutdown\n");
	GetOnPreFatalShutdownListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_UpdateWhenNotInGame(dyno::CallbackType type, dyno::IHook& hook)
{
	// float flFrameTime
	auto flFrameTime = dyno::GetArgument<float>(hook, 1);
	// g_Logger.MessageFormat("UpdateWhenNotInGame = %f\n", flFrameTime);
	GetOnUpdateWhenNotInGameListenerManager().Notify(flFrameTime);
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_PreWorldUpdate(dyno::CallbackType type, dyno::IHook& hook)
{
	// bool simulating
	auto simulating = dyno::GetArgument<bool>(hook, 1);
	// g_Logger.MessageFormat("PreWorldUpdate = %d\n", simulating);

	g_ServerManager.OnPreWorldUpdate();

	GetOnPreWorldUpdateListenerManager().Notify(simulating);
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_FireOutputInternal(dyno::CallbackType type, dyno::IHook& hook)
{
	return type == dyno::CallbackType::Post ? g_OutputManager.Hook_FireOutputInternal_Post(hook) : g_OutputManager.Hook_FireOutputInternal(hook);
}

dyno::ReturnAction Source2SDK::Hook_DispatchConCommand(dyno::CallbackType type, dyno::IHook& hook)
{
	return type == dyno::CallbackType::Post ? g_CommandManager.Hook_DispatchConCommand_Post(hook) : g_CommandManager.Hook_DispatchConCommand(hook);
}

dyno::ReturnAction Source2SDK::Hook_SetClientListening(dyno::CallbackType type, dyno::IHook& hook)
{
	return VoiceManager::Hook_SetClientListening(hook);
}

EXPOSE_PLUGIN(PLUGIN_API, &g_sdk)