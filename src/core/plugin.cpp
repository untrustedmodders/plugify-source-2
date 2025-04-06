#include "plugin.hpp"

#include <eiface.h>
#include <engine/igameeventsystem.h>
#include <entity2/entitysystem.h>
#include <igameevents.h>
#include <iserver.h>
#include <steam_gameserver.h>

#include <ISmmPlugin.h>

#include "core_config.hpp"
#include "con_command_manager.hpp"
#include "con_var_manager.hpp"
#include "event_manager.hpp"
#include "hook_holder.hpp"
#include "listeners.hpp"
#include "multi_addon_manager.hpp"
#include "output_manager.hpp"
#include "player_manager.hpp"
#include "server_manager.hpp"
#include "timer_system.hpp"
#include "user_message_manager.hpp"

#include "sdk/entity/cgamerules.h"

#undef FindResource

Source2SDK g_sdk;
EXPOSE_PLUGIN(PLUGIN_API, Source2SDK, &g_sdk)

//ISteamHTTP* g_http = nullptr;
CSteamGameServerAPIContext g_SteamAPI;

CGameEntitySystem* GameEntitySystem() {
	static int offset = g_pGameConfig->GetOffset("GameEntitySystem");
	return *reinterpret_cast<CGameEntitySystem**>((uintptr_t) (g_pGameResourceServiceServer) + offset);
}

CEntityInstance* CEntityHandle::Get() const {
	return g_pGameEntitySystem->GetEntityInstance(*this);
}

class CEntityListener : public IEntityListener {
	void OnEntityCreated(CEntityInstance* pEntity) override {
		std::string_view name(pEntity->GetClassname());
		if (name == "cs_gamerules")
			g_pGameRules = ((CCSGameRulesProxy*) pEntity)->m_pGameRules;
		GetOnEntityCreatedListenerManager().Notify(pEntity->GetRefEHandle().ToInt());
	}
	void OnEntityDeleted(CEntityInstance* pEntity) override {
		std::string_view name(pEntity->GetClassname());
		if (name == "cs_gamerules")
			g_pGameRules = nullptr;
		GetOnEntityDeletedListenerManager().Notify(pEntity->GetRefEHandle().ToInt());
	}
	void OnEntitySpawned(CEntityInstance* pEntity) override {
		GetOnEntitySpawnedListenerManager().Notify(pEntity->GetRefEHandle().ToInt());
	}
	void OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent) override {
		GetOnEntityParentChangedListenerManager().Notify(pEntity->GetRefEHandle().ToInt(), pNewParent ? pNewParent->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX);
	}
} g_pEntityListener;

void Source2SDK::OnPluginStart() {
	S2_LOG(LS_DEBUG, "[OnPluginStart] - Source2SDK!\n");

	globals::Initialize({
		{ "base", GetBaseDir() },
		{ "configs", GetConfigsDir() },
		{ "data", GetDataDir() },
	});

	using enum poly::CallbackType;

	g_PH.AddHookMemFunc(&IGameEventManager2::FireEvent, g_pGameEventManager, Hook_FireEvent, Pre, Post);
	using PostEventAbstract = void(IGameEventSystem::*)(CSplitScreenSlot nSlot, bool bLocalOnly, IRecipientFilter *pFilter, INetworkMessageInternal *pEvent, const CNetMessage *pData, unsigned long nSize);
	g_PH.AddHookMemFunc<PostEventAbstract>(&IGameEventSystem::PostEventAbstract, g_pGameEventSystem, Hook_PostEvent, Pre, Post);

	if (g_pMetamodListener != nullptr) {
		g_PH.AddHookMemFunc(&IMetamodListener::OnLevelInit, g_pMetamodListener, Hook_OnLevelInit, Post);
		g_PH.AddHookMemFunc(&IMetamodListener::OnLevelShutdown, g_pMetamodListener, Hook_OnLevelShutdown, Post);
	} else {
		g_PH.AddHookMemFunc(&IEngineServiceMgr::RegisterLoopMode, g_pEngineServiceMgr, Hook_RegisterLoopMode, Post);
		g_PH.AddHookMemFunc(&IEngineServiceMgr::UnregisterLoopMode, g_pEngineServiceMgr, Hook_UnregisterLoopMode, Post);
	}

	g_PH.AddHookMemFunc(&IServerGameClients::ClientCommand, g_pSource2GameClients, Hook_ClientCommand, Pre);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientActive, g_pSource2GameClients, Hook_ClientActive, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientDisconnect, g_pSource2GameClients, Hook_ClientDisconnect, Pre, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientPutInServer, g_pSource2GameClients, Hook_ClientPutInServer, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientSettingsChanged, g_pSource2GameClients, Hook_ClientSettingsChanged, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::OnClientConnected, g_pSource2GameClients, Hook_OnClientConnected, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientFullyConnect, g_pSource2GameClients, Hook_ClientFullyConnect, Post);
	g_PH.AddHookMemFunc(&IServerGameClients::ClientConnect, g_pSource2GameClients, Hook_ClientConnect, Pre, Post);
	g_PH.AddHookMemFunc(&INetworkServerService::StartupServer, g_pNetworkServerService, Hook_StartupServer, Post);
	g_PH.AddHookMemFunc(&ISource2Server::GameServerSteamAPIActivated, g_pSource2Server, Hook_GameServerSteamAPIActivated, Post);
	g_PH.AddHookMemFunc(&ISource2Server::GameServerSteamAPIDeactivated, g_pSource2Server, Hook_GameServerSteamAPIDeactivated, Post);
	g_PH.AddHookMemFunc(&ISource2Server::UpdateWhenNotInGame, g_pSource2Server, Hook_UpdateWhenNotInGame, Post);
	g_PH.AddHookMemFunc(&ISource2Server::PreWorldUpdate, g_pSource2Server, Hook_PreWorldUpdate, Post);
	g_PH.AddHookMemFunc(&IServerGameDLL::GameFrame, g_pSource2Server, Hook_GameFrame, Post);
	g_PH.AddHookMemFunc(&ICvar::DispatchConCommand, g_pCVar, Hook_DispatchConCommand, Pre, Post);

	//using Host_Say = void(*)(CEntityInstance*, CCommand&, bool, int, const char*);
	//g_PH.AddHookDetourFunc<Host_Say>("Host_Say", Hook_HostSay, Pre, Post);

	using FireOutputInternal = void(*)(CEntityIOOutput* const, CEntityInstance*, CEntityInstance*, const CVariant*, float);
	g_PH.AddHookDetourFunc<FireOutputInternal>("CEntityIOOutput_FireOutputInternal", Hook_FireOutputInternal, Pre, Post);

	//using LogDirect = LoggingResponse_t (*)(void* loggingSystem, LoggingChannelID_t channel, LoggingSeverity_t severity, LeafCodeInfo_t*, LoggingMetaData_t*, Color, char const*, va_list*);
	//g_PH.AddHookDetourFunc<LogDirect>("LogDirect", Hook_LogDirect, Pre);

	using HostStateRequest = void* (*)(void *a1, void **pRequest);
	g_PH.AddHookDetourFunc<HostStateRequest>("HostStateRequest", Hook_HostStateRequest, Pre);

	// We're using funchook even though it's a virtual function because it can be called on a different thread and SourceHook isn't thread-safe
	auto pServerSideClientVTable = g_GameConfigManager.GetModule("engine2")->GetVirtualTableByName("CServerSideClient").CCast<void**>();
	auto fSendNetMessage = &CServerSideClientBase::SendNetMessage;
	int iSendNetMessageOffset = poly::GetVTableIndex((void*&) fSendNetMessage);
	using SendNetMessage = decltype(&CServerSideClientBase::SendNetMessage);
	g_PH.AddHookDetourFunc<SendNetMessage>(pServerSideClientVTable[iSendNetMessageOffset], Hook_SendNetMessage, Pre);

#if S2SDK_PLATFORM_WINDOWS
	using PreloadLibrary = void(*)(void* const);
	g_PH.AddHookDetourFunc<PreloadLibrary>("PreloadLibrary", Hook_PreloadLibrary, Pre);
#endif

	OnServerStartup();// for late load*/
}

void Source2SDK::OnPluginEnd() {
	globals::Terminate();
	g_PH.UnhookAll();

	if (g_pGameEntitySystem != nullptr) {
		g_pGameEntitySystem->RemoveListenerEntity(&g_pEntityListener);
	}

	S2_LOG(LS_DEBUG, "[OnPluginEnd] = Source2SDK!\n");
}

void Source2SDK::OnServerStartup() {
	if (g_pNetworkGameServer != nullptr) {
		g_PH.RemoveHookMemFunc(&INetworkGameServer::ActivateServer, g_pNetworkGameServer);
	}

	g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
	if (g_pNetworkGameServer != nullptr) {
		gpGlobals = g_pNetworkGameServer->GetGlobals();
		g_PH.AddHookMemFunc(&INetworkGameServer::ActivateServer, g_pNetworkGameServer, Hook_ActivateServer, poly::CallbackType::Post);
	}

	g_pGameEntitySystem = GameEntitySystem();
	if (g_pGameEntitySystem != nullptr) {
		g_pGameEntitySystem->AddListenerEntity(&g_pEntityListener);
	}

	g_MultiAddonManager.OnStartupServer();
}

poly::ReturnAction Source2SDK::Hook_StartupServer(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	//auto config = poly::GetArgument<const GameSessionConfiguration_t *>(params, 1);
	//auto pWorldSession = poly::GetArgument<ISource2WorldSession*>(params, 2);
	auto pMapName = poly::GetArgument<const char*>(params, 3);

	//S2_LOGF(LS_DEBUG, "[StartupServer] = %s\n", pMapName);

	OnServerStartup();

	if (gpGlobals == nullptr) {
		S2_LOG(LS_ERROR, "Failed to lookup gpGlobals\n");
		return poly::ReturnAction::Ignored;
	}

	GetOnServerStartupListenerManager().Notify();

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ActivateServer(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	//S2_LOGF(LS_DEBUG, "[ActivateServer]\n");

	GetOnServerActivateListenerManager().Notify();

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_FireEvent(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	//S2_LOGF(LS_DEBUG, "FireEvent = %s\n", event->GetName() );
	auto event = poly::GetArgument<IGameEvent*>(params, 1);
	auto dontBroadcast = poly::GetArgument<bool>(params, 2);

	auto result = type == poly::CallbackType::Post ? g_EventManager.OnFireEvent_Post(event, dontBroadcast) : g_EventManager.OnFireEvent(event, dontBroadcast);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	if (result == ResultType::Changed) {
		poly::SetArgument<bool>(params, 2, !dontBroadcast);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_PostEvent(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	//S2_LOGF(LS_DEBUG, "[PostEvent] = %d, %d, %d, %lli\n", nSlot, bLocalOnly, nClientCount, clients );
	auto clientCount = poly::GetArgument<int>(params, 3);
	auto clients = poly::GetArgument<uint64_t*>(params, 4);
	auto message = poly::GetArgument<INetworkMessageInternal*>(params, 5);
	auto pData = poly::GetArgument<CNetMessage*>(params, 6);

	if (type == poly::CallbackType::Pre) {
		g_MultiAddonManager.OnPostEvent(message, pData, clients);
	}

	auto result = g_UserMessageManager.ExecuteMessageCallbacks(message, pData, clientCount, clients, static_cast<HookMode>(type));
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_OnLevelInit(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	auto pMapName = poly::GetArgument<const char*>(params, 1);
	auto pMapEntities = poly::GetArgument<const char*>(params, 2);
	//S2_LOGF(LS_DEBUG, "[OnLevelInit] = %s\n", pMapName);
	GetOnLevelInitListenerManager().Notify(pMapName, pMapEntities);
	return poly::ReturnAction::Ignored;
};

poly::ReturnAction Source2SDK::Hook_OnLevelShutdown(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	S2_LOG(LS_DEBUG, "[OnLevelShutdown]\n");
	g_TimerSystem.OnLevelShutdown();
	GetOnLevelShutdownListenerManager().Notify();
	return poly::ReturnAction::Ignored;
};

poly::ReturnAction Source2SDK::Hook_RegisterLoopMode(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	auto pszLoopModeName = poly::GetArgument<const char*>(params, 1);
	auto pLoopModeFactory = poly::GetArgument<ILoopModeFactory*>(params, 2);

	S2_LOG(LS_DEBUG, "[RegisterLoopMode]\n");

	if (!strcmp(pszLoopModeName, "game")) {
		g_PH.AddHookMemFunc(&ILoopMode::LoopInit, pLoopModeFactory, Hook_OnLevelInit, poly::CallbackType::Post);
		g_PH.AddHookMemFunc(&ILoopMode::LoopShutdown, pLoopModeFactory, Hook_OnLevelShutdown, poly::CallbackType::Post);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_UnregisterLoopMode(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	auto pszLoopModeName = poly::GetArgument<const char*>(params, 1);
	auto pLoopModeFactory = poly::GetArgument<ILoopModeFactory*>(params, 2);

	S2_LOG(LS_DEBUG, "[UnregisterLoopMode]\n");

	if (!strcmp(pszLoopModeName, "game")) {
		g_PH.RemoveHookMemFunc(&ILoopMode::LoopShutdown, pLoopModeFactory);
		g_PH.RemoveHookMemFunc(&ILoopMode::LoopInit, pLoopModeFactory);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_GameFrame(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// bool simulating, bool bFirstTick, bool bLastTick
	auto simulating = poly::GetArgument<bool>(params, 1);
	auto bFirstTick = poly::GetArgument<bool>(params, 2);
	auto bLastTick = poly::GetArgument<bool>(params, 3);

	g_MultiAddonManager.OnGameFrame();
	g_ServerManager.OnGameFrame();
	g_TimerSystem.OnGameFrame(simulating);

	GetOnGameFrameListenerManager().Notify(simulating, bFirstTick, bLastTick);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientActive(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, bool bLoadGame, const char* name, uint64 xuid
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto bLoadGame = poly::GetArgument<bool>(params, 2);
	auto name = poly::GetArgument<const char*>(params, 3);
	auto xuid = (uint64) poly::GetArgument<uint64_t>(params, 4);

	//S2_LOGF(LS_DEBUG, "[OnClientActive] = %d, \"%s\", %lli\n", slot, name, xuid);

	g_PlayerManager.OnClientActive(slot, bLoadGame);

	GetOnClientActiveListenerManager().Notify(slot, bLoadGame);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientDisconnect(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, int reason, const char* name, uint64 xuid, const char* networkID
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto reason = (ENetworkDisconnectionReason) poly::GetArgument<int>(params, 2);
	auto name = poly::GetArgument<const char*>(params, 3);
	auto xuid = poly::GetArgument<uint64_t>(params, 4);
	auto networkID = poly::GetArgument<const char*>(params, 5);

	//S2_LOGF(LS_DEBUG, "[ClientDisconnect] = %d, %d, \"%s\", %lli, \"%s\"\n", slot, reason, name, xuid, networkID);

	if (type == poly::CallbackType::Pre) {
		g_PlayerManager.OnClientDisconnect(slot, reason);
	} else {
		g_PlayerManager.OnClientDisconnect_Post(slot, reason);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientPutInServer(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, char const *name, int type, uint64 xuid
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto name = poly::GetArgument<const char*>(params, 2);
	auto conType = poly::GetArgument<int>(params, 3);
	auto xuid = poly::GetArgument<uint64_t>(params, 4);

	//S2_LOGF(LS_DEBUG, "[ClientPutInServer] = %d, \"%s\", %d, %d, %lli\n", slot, name, conType, xuid);

	g_PlayerManager.OnClientPutInServer(slot, name);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientSettingsChanged(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);

	//S2_LOGF(LS_DEBUG, "[ClientSettingsChanged] = %d\n", slot);

	GetOnClientSettingsChangedListenerManager().Notify(slot);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_OnClientConnected(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, const cha*r name, uint64 xuid, const char* networkID, const char* pszAddress, bool bFakePlayer
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto name = poly::GetArgument<const char*>(params, 2);
	auto xuid = poly::GetArgument<uint64_t>(params, 3);
	auto networkID = poly::GetArgument<const char*>(params, 4);
	auto pszAddress = poly::GetArgument<const char*>(params, 5);
	auto bFakePlayer = poly::GetArgument<bool>(params, 6);

	//S2_LOGF(LS_DEBUG, "[OnClientConnected] = %d, \"%s\", %lli, \"%s\", \"%s\", %d\n", slot, name, xuid, networkID, pszAddress, bFakePlayer);

	g_PlayerManager.OnClientConnected(slot);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientFullyConnect(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);

	//S2_LOGF(LS_DEBUG, "[ClientFullyConnect] = %d\n", slot);

	GetOnClientFullyConnectListenerManager().Notify(slot);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientConnect(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, const char* name, uint64 xuid, const char* networkID, bool unk1, CBufferString *pRejectReason
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto name = poly::GetArgument<const char*>(params, 2);
	auto xuid = poly::GetArgument<uint64_t>(params, 3);
	auto networkID = poly::GetArgument<const char*>(params, 4);
	//bool unk1 = poly::GetArgument<bool>(params, 5);
	//auto pRejectReason = poly::GetArgument<CBufferString*>(params, 6);

	//S2_LOGF(LS_DEBUG, "[ClientConnect] = %d, \"%s\", %lli, \"%s\", %d, \"%s\" \n", slot, name, xuid, networkID, unk1, pRejectReason->Get());

	if (type == poly::CallbackType::Pre) {
		g_MultiAddonManager.OnClientConnect(slot, name, xuid, networkID);
		g_PlayerManager.OnClientConnect(slot, name, xuid, networkID);
	} else {
		bool origRet = poly::GetReturn<bool>(ret);
		bool newRet = g_PlayerManager.OnClientConnect_Post(slot, origRet);
		if (newRet != origRet) {
			poly::SetReturn<bool>(ret, newRet);
			return poly::ReturnAction::Handled;
		}
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientCommand(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot nSlot, const CCommand& _cmd
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto args = poly::GetArgument<const CCommand*>(params, 2);
	if (args == nullptr) {
		return poly::ReturnAction::Ignored;
	}

	//S2_LOGF(LS_DEBUG, "[ClientCommand] = %d, \"%s\"\n", slot, args->GetCommandString());

	const char* cmd = args->Arg(0);

	auto result = g_CommandManager.ExecuteCommandCallbacks(cmd, CCommandContext(CommandTarget_t::CT_NO_TARGET, slot), *args, HookMode::Pre, CommandCallingContext::Console);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_GameServerSteamAPIActivated(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	S2_LOG(LS_DEBUG, "[GameServerSteamAPIActivated]\n");

	g_SteamAPI.Init();
	//g_http = g_steamAPI.SteamHTTP();

	g_PlayerManager.OnSteamAPIActivated();
	g_MultiAddonManager.OnSteamAPIActivated();

	//GetOnGameServerSteamAPIActivatedListenerManager().Notify();
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_GameServerSteamAPIDeactivated(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	S2_LOG(LS_DEBUG, "[GameServerSteamAPIDeactivated]\n");

	//g_http = nullptr;

	//GetOnGameServerSteamAPIDeactivatedListenerManager().Notify();
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_UpdateWhenNotInGame(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// float flFrameTime
	auto flFrameTime = poly::GetArgument<float>(params, 1);
	//S2_LOGF(LS_DEBUG, "UpdateWhenNotInGame = %f\n", flFrameTime);
	GetOnUpdateWhenNotInGameListenerManager().Notify(flFrameTime);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_PreWorldUpdate(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// bool simulating
	auto simulating = poly::GetArgument<bool>(params, 1);
	//S2_LOGF(LS_DEBUG, "PreWorldUpdate = %d\n", simulating);

	g_ServerManager.OnPreWorldUpdate();

	GetOnPreWorldUpdateListenerManager().Notify(simulating);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_FireOutputInternal(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CEntityIOOutput* const pThis, CEntityInstance* pActivator, CEntityInstance* pCaller, const CVariant* const value, float flDelay
	auto pThis = poly::GetArgument<CEntityIOOutput* const>(params, 0);
	auto pActivator = poly::GetArgument<CEntityInstance*>(params, 1);
	auto pCaller = poly::GetArgument<CEntityInstance*>(params, 2);
	//auto value = poly::GetArgument<const CVariant* const>(params, 3);
	auto flDelay = poly::GetArgument<float>(params, 4);

	ResultType result = type == poly::CallbackType::Post ? g_OutputManager.FireOutputInternal_Post(pThis, pActivator, pCaller, flDelay) : g_OutputManager.FireOutputInternal(pThis, pActivator, pCaller, flDelay);

	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_DispatchConCommand(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// auto cmd = poly::GetArgument<ConCommandRef* const>(params, 1);
	auto ctx = poly::GetArgument<const CCommandContext*>(params, 2);
	auto args = poly::GetArgument<const CCommand*>(params, 3);
	auto result = g_CommandManager.DispatchConCommand(ctx, args, static_cast<HookMode>(type));
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_LogDirect(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	/*volatile auto loggingSystem = poly::GetArgument<void*>(params, 0);
	volatile auto channel = (LoggingChannelID_t) poly::GetArgument<int32_t>(params, 1);
	volatile auto severity = (LoggingSeverity_t) poly::GetArgument<int32_t>(params, 2);
	volatile auto LeafCod = poly::GetArgument<LeafCodeInfo_t*>(params, 3);
	volatile auto LoggingMetaData = poly::GetArgument<LoggingMetaData_t*>(params, 4);
	volatile auto color = poly::GetArgument<int32_t>(params, 5);
	volatile auto str = poly::GetArgument<char const*>(params, 6);
	volatile auto args = poly::GetArgument<va_list*>(params, 7);

	printf("loggingSystem: %p\n", loggingSystem);
	printf("channel: %d\n", channel);
	printf("severity: %d\n", severity);
	printf("LeafCod: %p\n", LeafCod);
	printf("LoggingMetaData: %p\n", LoggingMetaData);
	printf("color: %d\n", color);
	printf("str: %s\n", str);
	if (args) {
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), str, *args);
		printf("args: %s\n", buffer);
	} else {
		printf("args: (null)\n");
	}*/

	/*auto str = poly::GetArgument<char const*>(params, 6);
	auto args = poly::GetArgument<va_list*>(params, 7);

	char buffer[MAX_LOGGING_MESSAGE_LENGTH];
	bool matched = false;

	if (args) {
		auto len = static_cast<size_t>(V_vsnprintf(buffer, sizeof buffer, str, *args));
		matched = g_pCoreConfig->IsRegexMatch(std::string_view(buffer, len));
	} else {
		matched = g_pCoreConfig->IsRegexMatch(str);
	}

	if (matched) {
		poly::SetReturn<int>(ret, LoggingResponse_t::LR_ABORT);
		return poly::ReturnAction::Supercede;
	}*/

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_HostStateRequest(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	auto pRequest = poly::GetArgument<void**>(params, 1);
	// This offset hasn't changed in 6 years so it should be safe
	return g_MultiAddonManager.OnHostStateRequest(reinterpret_cast<CUtlString *>(pRequest + 11)) ? poly::ReturnAction::Handled : poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_SendNetMessage(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	auto pClient = poly::GetArgument<CServerSideClient*>(params, 0);
	auto pData = poly::GetArgument<CNetMessage*>(params, 1);
	auto bufType = (NetChannelBufType_t) poly::GetArgument<int8_t>(params, 2);
	return g_MultiAddonManager.OnSendNetMessage(pClient, pData, bufType) ? poly::ReturnAction::Handled : poly::ReturnAction::Ignored;
}

/*poly::ReturnAction Source2SDK::Hook_HostSay(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	return g_ChatManager.Hook_HostSay(hook, params, count, ret, static_cast<HookMode>(type));
}*/

#if S2SDK_PLATFORM_WINDOWS

#if PLUGIFY_ARCH_BITS == 64
constexpr WORD PE_FILE_MACHINE = IMAGE_FILE_MACHINE_AMD64;
constexpr WORD PE_NT_OPTIONAL_HDR_MAGIC = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
#else
constexpr WORD PE_FILE_MACHINE = IMAGE_FILE_MACHINE_I386;
constexpr WORD PE_NT_OPTIONAL_HDR_MAGIC = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
#endif // PLUGIFY_ARCH_BITS

poly::ReturnAction Source2SDK::Hook_PreloadLibrary(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	HMODULE hModule = (HMODULE) poly::GetArgument<void*>(params, 0);

	IMAGE_DOS_HEADER* pDOSHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(hModule);
	IMAGE_NT_HEADERS* pNTHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(hModule) + pDOSHeader->e_lfanew);
	IMAGE_OPTIONAL_HEADER* pOptionalHeader = &pNTHeaders->OptionalHeader;

	IMAGE_DATA_DIRECTORY clrRuntimeHeader = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
	if (clrRuntimeHeader.VirtualAddress != 0 && clrRuntimeHeader.Size != 0) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

#endif