#include "plugin.hpp"

#include <eiface.h>
#include <engine/igameeventsystem.h>
#include <entity2/entitysystem.h>
#include <igameevents.h>
#include <iserver.h>

#include <ISmmPlugin.h>

#include "con_command_manager.hpp"
#include "con_var_manager.hpp"
#include "event_manager.hpp"
#include "hook_holder.hpp"
#include "listeners.hpp"
#include "output_manager.hpp"
#include "player_manager.hpp"
#include "server_manager.hpp"
#include "timer_system.hpp"
#include "voice_manager.hpp"

#include "sdk/entity/cgamerules.h"

#undef FindResource

Source2SDK g_sdk;
EXPOSE_PLUGIN(PLUGIN_API, &g_sdk)

CGameEntitySystem* GameEntitySystem()
{
	static int offset = g_pGameConfig->GetOffset("GameEntitySystem");
	return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(g_pGameResourceServiceServer) + offset);
}

CEntityInstance* CEntityHandle::Get() const
{
	return g_pGameEntitySystem->GetEntityInstance(*this);
}

class CEntityListener : public IEntityListener
{
	void OnEntitySpawned(CEntityInstance* pEntity) override
	{
		GetOnEntitySpawnedListenerManager().Notify(pEntity);
	}
	void OnEntityCreated(CEntityInstance* pEntity) override
	{
		if (!V_strcmp("cs_gamerules", pEntity->GetClassname()))
			g_pGameRules = ((CCSGameRulesProxy*)pEntity)->m_pGameRules;
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
	g_Logger.Log(LS_DEBUG, "OnPluginStart!\n");

	auto coreConfig = FindResource(S2SDK_NSTR("configs/core.txt"));
	if (!coreConfig.has_value())
	{
		g_Logger.Log(LS_ERROR, "configs/core.txt not found!");
		return;
	}
	auto gameData = FindResource(S2SDK_NSTR("gamedata/s2sdk.games.txt"));
	if (!gameData.has_value())
	{
		g_Logger.Log(LS_ERROR, "gamedata/s2sdk.games.txt not found!");
		return;
	}

	globals::Initialize(S2SDK_UTF8(*coreConfig), S2SDK_UTF8(*gameData));

	using enum poly::CallbackType;

	if (g_pGameEventManager != nullptr)
	{
		g_PH.AddHookMemFunc(&IGameEventManager2::FireEvent, g_pGameEventManager, Hook_FireEvent, Pre, Post);
	}

	if (g_pMetamodListener != nullptr)
	{
		g_PH.AddHookMemFunc(&IMetamodListener::OnLevelInit, g_pMetamodListener, Hook_OnLevelInit, Post);
		g_PH.AddHookMemFunc(&IMetamodListener::OnLevelShutdown, g_pMetamodListener, Hook_OnLevelShutdown, Post);
	}
	else
	{
		g_PH.AddHookMemFunc(&IEngineServiceMgr::RegisterLoopMode, g_pEngineServiceMgr, Hook_RegisterLoopMode, Post);
		g_PH.AddHookMemFunc(&IEngineServiceMgr::UnregisterLoopMode, g_pEngineServiceMgr, Hook_UnregisterLoopMode, Post);
	}

	g_PH.AddHookMemFunc(&IServerGameClients::ClientCommand, g_pSource2GameClients, Hook_ClientCommand, Pre);
	g_PH.AddHookMemFunc(&IServerGameDLL::GameFrame, g_pSource2Server, Hook_GameFrame, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientActive, g_pSource2GameClients, Hook_ClientActive, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientDisconnect, g_pSource2GameClients, Hook_ClientDisconnect, Pre, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientPutInServer, g_pSource2GameClients, Hook_ClientPutInServer, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientSettingsChanged, g_pSource2GameClients, Hook_ClientSettingsChanged, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::OnClientConnected, g_pSource2GameClients, Hook_OnClientConnected, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientFullyConnect, g_pSource2GameClients, Hook_ClientFullyConnect, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientConnect, g_pSource2GameClients, Hook_ClientConnect, Pre, Post);
	g_PH.AddHookMemFunc(&INetworkServerService::StartupServer, g_pNetworkServerService, Hook_StartupServer, Post);
	//g_PH.AddHookMemFunc(&ISource2Server::ServerHibernationUpdate, g_pSource2Server, Hook_ServerHibernationUpdate, Post);
	//g_PH.AddHookMemFunc(&ISource2Server::GameServerSteamAPIActivated, g_pSource2Server, Hook_GameServerSteamAPIActivated, Post);
	//g_PH.AddHookMemFunc(&ISource2Server::GameServerSteamAPIDeactivated, g_pSource2Server, Hook_GameServerSteamAPIDeactivated, Post);
	//g_PH.AddHookMemFunc(&ISource2Server::OnHostNameChanged, g_pSource2Server, Hook_OnHostNameChanged, Post);
	//g_PH.AddHookMemFunc(&ISource2Server::PreFatalShutdown, g_pSource2Server, Hook_PreFatalShutdown, Post);
	g_PH.AddHookMemFunc(&ISource2Server::UpdateWhenNotInGame, g_pSource2Server, Hook_UpdateWhenNotInGame, Post);
	g_PH.AddHookMemFunc(&ISource2Server::PreWorldUpdate, g_pSource2Server, Hook_PreWorldUpdate, Post);
	g_PH.AddHookMemFunc(&ICvar::DispatchConCommand, g_pCVar, Hook_DispatchConCommand, Pre, Post);
	g_PH.AddHookMemFunc(&IVEngineServer2::SetClientListening, g_pEngineServer2, Hook_SetClientListening, Pre);

	using FireOutputInternal = void (*)(CEntityIOOutput* const, CEntityInstance*, CEntityInstance*, const CVariant* const, float);
	g_PH.AddHookDetourFunc<FireOutputInternal>("CEntityIOOutput_FireOutputInternal", Hook_FireOutputInternal, Pre);

	OnServerStartup(); // for late load*/
}

void Source2SDK::OnPluginEnd()
{
	globals::Terminate();
	g_PH.UnhookAll();
	if (g_pGameEntitySystem != nullptr)
	{
		int iListener = g_pGameEntitySystem->m_entityListeners.Find(&g_pEntityListener);
		if (iListener != -1)
		{
			g_pGameEntitySystem->m_entityListeners.Remove(iListener);
		}
	}
	g_Logger.Log(LS_DEBUG, "OnPluginEnd!\n");
}

void Source2SDK::OnServerStartup()
{
	if (g_pNetworkGameServer != nullptr)
	{
		g_PH.RemoveHookMemFunc(&INetworkGameServer::ActivateServer, g_pNetworkGameServer);
	}
	g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
	if (g_pNetworkGameServer != nullptr)
	{
		gpGlobals = g_pNetworkGameServer->GetGlobals();
		g_PH.AddHookMemFunc(&INetworkGameServer::ActivateServer, g_pNetworkGameServer, Hook_ActivateServer, poly::CallbackType::Post);
	}

	if (gpGlobals != nullptr)
	{
		g_pGameEntitySystem = GameEntitySystem();
		if (g_pGameEntitySystem != nullptr)
		{
			if (g_pGameEntitySystem->m_entityListeners.Find(&g_pEntityListener) == -1)
			{
				g_pGameEntitySystem->m_entityListeners.AddToTail(&g_pEntityListener);
			}
		}
	}
}

poly::ReturnAction Source2SDK::Hook_StartupServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	//auto config = poly::GetArgument<const GameSessionConfiguration_t *>(params, 1);
	//auto pWorldSession = poly::GetArgument<ISource2WorldSession*>(params, 2);
	auto pMapName = poly::GetArgument<const char *>(params, 3);

	g_Logger.LogFormat(LS_DEBUG, "Startup server: %s\n", pMapName);

	OnServerStartup();
	
	if (gpGlobals == nullptr)
	{
		g_Logger.Log(LS_ERROR, "Failed to lookup gpGlobals\n");
		return poly::ReturnAction::Ignored;
	}

	GetOnServerStartupListenerManager().Notify();

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ActivateServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	g_Logger.LogFormat(LS_DEBUG, "Activate server\n");

	GetOnServerActivateListenerManager().Notify();

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_FireEvent(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	//g_Logger.LogFormat(LS_DEBUG, "FireEvent = %s\n", event->GetName() );
	return type == poly::CallbackType::Post ? g_EventManager.Hook_OnFireEvent_Post(params, count, ret) : g_EventManager.Hook_OnFireEvent(params, count, ret);
}

/*poly::ReturnAction Source2SDK::Hook_PostEvent(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// g_Logger.LogFormat(LS_DEBUG, "PostEvent = %d, %d, %d, %lli\n", nSlot, bLocalOnly, nClientCount, clients );
	return poly::ReturnAction::Ignored;
}*/

poly::ReturnAction Source2SDK::Hook_OnLevelInit(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	auto pMapName = poly::GetArgument<const char*>(params, 1);
	auto pMapEntities = poly::GetArgument<const char*>(params, 2);
	g_Logger.LogFormat(LS_DEBUG, "OnLevelInit = %s\n", pMapName);
	GetOnLevelInitListenerManager().Notify(pMapName, pMapEntities);
	return poly::ReturnAction::Ignored;
};

poly::ReturnAction Source2SDK::Hook_OnLevelShutdown(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	g_Logger.Log(LS_DEBUG, "OnLevelShutdown\n");
	g_TimerSystem.OnLevelShutdown();
	g_PlayerManager.OnLevelShutdown();
	GetOnLevelShutdownListenerManager().Notify();
	return poly::ReturnAction::Ignored;
};

poly::ReturnAction Source2SDK::Hook_RegisterLoopMode(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	auto pszLoopModeName = poly::GetArgument<const char*>(params, 1);
	auto pLoopModeFactory = poly::GetArgument<ILoopModeFactory*>(params, 2);

	if (!strcmp(pszLoopModeName, "game"))
	{
		g_PH.AddHookMemFunc(&ILoopMode::LoopInit, pLoopModeFactory, Hook_OnLevelInit, poly::CallbackType::Post);
		g_PH.AddHookMemFunc(&ILoopMode::LoopShutdown, pLoopModeFactory, Hook_OnLevelShutdown, poly::CallbackType::Post);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_UnregisterLoopMode(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	auto pszLoopModeName = poly::GetArgument<const char*>(params, 1);
	auto pLoopModeFactory = poly::GetArgument<ILoopModeFactory*>(params, 2);

	if (!strcmp(pszLoopModeName, "game"))
	{
		g_PH.RemoveHookMemFunc(&ILoopMode::LoopShutdown, pLoopModeFactory);
		g_PH.RemoveHookMemFunc(&ILoopMode::LoopInit, pLoopModeFactory);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_GameFrame(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// bool simulating, bool bFirstTick, bool bLastTick
	auto simulating = poly::GetArgument<bool>(params, 1);
	auto bFirstTick = poly::GetArgument<bool>(params, 2);
	auto bLastTick = poly::GetArgument<bool>(params, 3);

	g_ServerManager.OnGameFrame();
	g_TimerSystem.OnGameFrame(simulating);
	g_PlayerManager.RunAuthChecks();

	GetOnGameFrameListenerManager().Notify(simulating, bFirstTick, bLastTick);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientActive(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto bLoadGame = poly::GetArgument<bool>(params, 2);
	// auto pszName = poly::GetArgument<const char*>(hook, 3);
	// auto xuid = poly::GetArgument<uint64>(hook, 4);
	// g_Logger.LogFormat(LS_DEBUG, "ClientActive = %d, %d, \"%s\", %lli\n", slot, bLoadGame, pszName, xuid);

	g_PlayerManager.OnClientActive(slot, bLoadGame);

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientDisconnect(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot, int reason, const char* pszName, uint64 xuid, const char* pszNetworkID
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto reason = (ENetworkDisconnectionReason)poly::GetArgument<int>(params, 2);
	auto pszName = poly::GetArgument<const char*>(params, 3);
	auto xuid = poly::GetArgument<uint64_t>(params, 4);
	auto pszNetworkID = poly::GetArgument<const char*>(params, 5);
	// g_Logger.LogFormat(LS_DEBUG, "ClientDisconnect - %d, %d, \"%s\", %lli, \"%s\"\n", slot, reason, pszName, xuid, pszNetworkID);

	if (type == poly::CallbackType::Pre)
	{
		g_PlayerManager.OnClientDisconnect(slot, reason, pszName, xuid, pszNetworkID);
	}
	else
	{
		g_PlayerManager.OnClientDisconnect_Post(slot, reason, pszName, xuid, pszNetworkID);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientPutInServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot, char const *pszName, int type, uint64 xuid
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto pszName = poly::GetArgument<const char*>(params, 2);
	auto conType = poly::GetArgument<int>(params, 3);
	auto xuid = poly::GetArgument<uint64_t>(params, 4);
	// g_Logger.LogFormat(LS_DEBUG, "ClientPutInServer - %d, \"%s\", %d, %d, %lli\n", slot, pszName, conType, xuid);

	g_PlayerManager.OnClientPutInServer(slot, pszName, conType, xuid);

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientSettingsChanged(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	g_Logger.LogFormat(LS_DEBUG, "ClientSettingsChanged - %d\n", slot.Get());
	GetOnClientSettingsChangedListenerManager().Notify(slot.Get());
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_OnClientConnected(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot, const cha*r pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto pszName = poly::GetArgument<const char*>(params, 2);
	auto xuid = poly::GetArgument<uint64_t>(params, 3);
	auto pszNetworkID = poly::GetArgument<const char*>(params, 4);
	auto pszAddress = poly::GetArgument<const char*>(params, 5);
	auto bFakePlayer = poly::GetArgument<bool>(params, 6);
	// g_Logger.LogFormat(LS_DEBUG, "OnClientConnected = %d, \"%s\", %lli, \"%s\", \"%s\", %d\n", slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	g_PlayerManager.OnClientConnected(slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientFullyConnect(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	g_Logger.LogFormat(LS_DEBUG, "ClientFullyConnect = %d\n", slot.Get());
	GetOnClientFullyConnectListenerManager().Notify(slot.Get());
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientConnect(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString *pRejectReason
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto pszName = poly::GetArgument<const char*>(params, 2);
	auto xuid = poly::GetArgument<uint64_t>(params, 3);
	auto pszNetworkID = poly::GetArgument<const char*>(params, 4);
	bool unk1 = poly::GetArgument<bool>(params, 5);
	auto pRejectReason = poly::GetArgument<CBufferString*>(params, 6);

	// g_Logger.LogFormat(LS_DEBUG, "ClientConnect = %d, \"%s\", %lli, \"%s\", %d, \"%s\" \n", slot, pszName, xuid, pszNetworkID, unk1, pRejectReason->ToGrowable()->Get());

	if (type == poly::CallbackType::Pre)
	{
		g_PlayerManager.OnClientConnect(slot, pszName, xuid, pszNetworkID, unk1, pRejectReason);
	}
	else
	{
		bool origRet = poly::GetReturn<bool>(ret);
		bool newRet = g_PlayerManager.OnClientConnect_Post(slot, pszName, xuid, pszNetworkID, unk1, pRejectReason, origRet);
		if (newRet != origRet)
		{
			poly::SetReturn<bool>(ret, newRet);
			return poly::ReturnAction::Handled;
		}
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientCommand(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot nSlot, const CCommand& _cmd
	auto slot = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto args = poly::GetArgument<const CCommand*>(params, 2);
	if (args == nullptr)
	{
		return poly::ReturnAction::Ignored;
	}

	g_Logger.LogFormat(LS_DEBUG, "ClientCommand = %d, \"%s\"\n", slot.Get(), args->GetCommandString());
	const char* cmd = args->Arg(0);

	auto result = g_CommandManager.ExecuteCommandCallbacks(cmd, CCommandContext(CommandTarget_t::CT_NO_TARGET, slot), *args, HookMode::Pre, CommandCallingContext::Console);
	if (result >= ResultType::Handled)
	{
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

/*poly::ReturnAction Source2SDK::Hook_CheckTransmit(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret) {
	//CPlayerSlot nSlot, const CCommand& _cmd
	GetOnCheckTransmitListenerManager().Notify();
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ServerHibernationUpdate(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// bool bHibernating
	auto bHibernating = poly::GetArgument<bool>(params, 1);
	g_Logger.LogFormat(LS_DEBUG, "ServerHibernationUpdate = %d\n", bHibernating);
	GetOnServerHibernationUpdateListenerManager().Notify(bHibernating);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_GameServerSteamAPIActivated(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	g_Logger.Log(LS_DEBUG, "GameServerSteamAPIActivated\n");
	GetOnGameServerSteamAPIActivatedListenerManager().Notify();
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_GameServerSteamAPIDeactivated(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	g_Logger.Log(LS_DEBUG, "GameServerSteamAPIDeactivated\n");
	GetOnGameServerSteamAPIDeactivatedListenerManager().Notify();
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_OnHostNameChanged(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// const char *pHostname
	auto pHostname = poly::GetArgument<const char*>(params, 1);
	g_Logger.LogFormat(LS_DEBUG, "OnHostNameChanged = %s\n", pHostname);
	GetOnHostNameChangedListenerManager().Notify(pHostname);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_PreFatalShutdown(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	g_Logger.Log(LS_DEBUG, "PreFatalShutdown\n");
	GetOnPreFatalShutdownListenerManager().Notify();
	return poly::ReturnAction::Ignored;
}*/

poly::ReturnAction Source2SDK::Hook_UpdateWhenNotInGame(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// float flFrameTime
	auto flFrameTime = poly::GetArgument<float>(params, 1);
	// g_Logger.LogFormat(LS_DEBUG, "UpdateWhenNotInGame = %f\n", flFrameTime);
	GetOnUpdateWhenNotInGameListenerManager().Notify(flFrameTime);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_PreWorldUpdate(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	// bool simulating
	auto simulating = poly::GetArgument<bool>(params, 1);
	// g_Logger.LogFormat(LS_DEBUG, "PreWorldUpdate = %d\n", simulating);

	g_ServerManager.OnPreWorldUpdate();

	GetOnPreWorldUpdateListenerManager().Notify(simulating);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_FireOutputInternal(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	return type == poly::CallbackType::Post ? g_OutputManager.Hook_FireOutputInternal_Post(params, count, ret) : g_OutputManager.Hook_FireOutputInternal(params, count, ret);
}

poly::ReturnAction Source2SDK::Hook_DispatchConCommand(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	return type == poly::CallbackType::Post ? g_CommandManager.Hook_DispatchConCommand_Post(params, count, ret) : g_CommandManager.Hook_DispatchConCommand(params, count, ret);
}

poly::ReturnAction Source2SDK::Hook_SetClientListening(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret)
{
	return CVoiceManager::Hook_SetClientListening(params, count, ret);
}
