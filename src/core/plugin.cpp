#include "plugin.hpp"

#include <eiface.h>
#include <engine/igameeventsystem.h>
#include <entity2/entitysystem.h>
#include <igameevents.h>
#include <iserver.h>
#include <steam_gameserver.h>

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
#include "user_message_manager.hpp"

#include "sdk/entity/cgamerules.h"

#undef FindResource

static Source2SDK g_sdk;
EXPOSE_PLUGIN(PLUGIN_API, Source2SDK, &g_sdk)

//ISteamHTTP* g_http = nullptr;
CSteamGameServerAPIContext g_steamAPI;

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

	auto coreConfig = FindResource(S2SDK_NSTR("configs/core.txt"));
	if (!coreConfig.has_value()) {
		S2_LOG(LS_ERROR, "configs/core.txt not found!");
		return;
	}
	auto gameData = FindResource(S2SDK_NSTR("gamedata/s2sdk.games.txt"));
	if (!gameData.has_value()) {
		S2_LOG(LS_ERROR, "gamedata/s2sdk.games.txt not found!");
		return;
	}

	globals::Initialize(S2SDK_UTF8(*coreConfig), S2SDK_UTF8(*gameData));

	using enum poly::CallbackType;

	g_PH.AddHookMemFunc(&IGameEventManager2::FireEvent, g_pGameEventManager, Hook_FireEvent, Pre, Post);
	using PostEventAbstract = void(IGameEventSystem::*)(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64 *clients, INetworkMessageInternal *pEvent, const CNetMessage *pData, unsigned long nSize, NetChannelBufType_t bufType);
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

	using FireOutputInternal = void(*)(CEntityIOOutput* const, CEntityInstance*, CEntityInstance*, const CVariant* const, float);
	g_PH.AddHookDetourFunc<FireOutputInternal>("CEntityIOOutput_FireOutputInternal", Hook_FireOutputInternal, Pre, Post);

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
	return type == poly::CallbackType::Post ? g_EventManager.Hook_OnFireEvent_Post(params, count, ret) : g_EventManager.Hook_OnFireEvent(params, count, ret);
}

poly::ReturnAction Source2SDK::Hook_PostEvent(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	//S2_LOGF(LS_DEBUG, "[PostEvent] = %d, %d, %d, %lli\n", nSlot, bLocalOnly, nClientCount, clients );
	return g_UserMessageManager.Hook_PostEvent(params, count, ret, static_cast<HookMode>(type));
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

	g_ServerManager.OnGameFrame();
	g_TimerSystem.OnGameFrame(simulating);

	GetOnGameFrameListenerManager().Notify(simulating, bFirstTick, bLastTick);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientActive(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto bLoadGame = poly::GetArgument<bool>(params, 2);
	auto pszName = poly::GetArgument<const char*>(params, 3);
	auto xuid = (uint64) poly::GetArgument<uint64_t>(params, 4);

	//S2_LOGF(LS_DEBUG, "[OnClientActive] = %d, \"%s\", %lli\n", slot, pszName, xuid);

	g_PlayerManager.OnClientActive(slot, bLoadGame);

	GetOnClientActiveListenerManager().Notify(slot, bLoadGame);
	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientDisconnect(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, int reason, const char* pszName, uint64 xuid, const char* pszNetworkID
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto reason = (ENetworkDisconnectionReason) poly::GetArgument<int>(params, 2);
	auto pszName = poly::GetArgument<const char*>(params, 3);
	auto xuid = poly::GetArgument<uint64_t>(params, 4);
	auto pszNetworkID = poly::GetArgument<const char*>(params, 5);

	//S2_LOGF(LS_DEBUG, "[ClientDisconnect] = %d, %d, \"%s\", %lli, \"%s\"\n", slot, reason, pszName, xuid, pszNetworkID);

	if (type == poly::CallbackType::Pre) {
		g_PlayerManager.OnClientDisconnect(slot, reason);
	} else {
		g_PlayerManager.OnClientDisconnect_Post(slot, reason);
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction Source2SDK::Hook_ClientPutInServer(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	// CPlayerSlot slot, char const *pszName, int type, uint64 xuid
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto pszName = poly::GetArgument<const char*>(params, 2);
	auto conType = poly::GetArgument<int>(params, 3);
	auto xuid = poly::GetArgument<uint64_t>(params, 4);

	//S2_LOGF(LS_DEBUG, "[ClientPutInServer] = %d, \"%s\", %d, %d, %lli\n", slot, pszName, conType, xuid);

	g_PlayerManager.OnClientPutInServer(slot, pszName);
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
	// CPlayerSlot slot, const cha*r pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto pszName = poly::GetArgument<const char*>(params, 2);
	auto xuid = poly::GetArgument<uint64_t>(params, 3);
	auto pszNetworkID = poly::GetArgument<const char*>(params, 4);
	auto pszAddress = poly::GetArgument<const char*>(params, 5);
	auto bFakePlayer = poly::GetArgument<bool>(params, 6);

	//S2_LOGF(LS_DEBUG, "[OnClientConnected] = %d, \"%s\", %lli, \"%s\", \"%s\", %d\n", slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);

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
	// CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString *pRejectReason
	auto slot = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto pszName = poly::GetArgument<const char*>(params, 2);
	auto xuid = poly::GetArgument<uint64_t>(params, 3);
	auto pszNetworkID = poly::GetArgument<const char*>(params, 4);
	bool unk1 = poly::GetArgument<bool>(params, 5);
	auto pRejectReason = poly::GetArgument<CBufferString*>(params, 6);

	//S2_LOGF(LS_DEBUG, "[ClientConnect] = %d, \"%s\", %lli, \"%s\", %d, \"%s\" \n", slot, pszName, xuid, pszNetworkID, unk1, pRejectReason->Get());

	if (type == poly::CallbackType::Pre) {
		g_PlayerManager.OnClientConnect(slot, pszName, xuid, pszNetworkID);
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

	g_steamAPI.Init();
	//g_http = g_steamAPI.SteamHTTP();

	g_PlayerManager.OnSteamAPIActivated();

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
	return type == poly::CallbackType::Post ? g_OutputManager.Hook_FireOutputInternal_Post(params, count, ret) : g_OutputManager.Hook_FireOutputInternal(params, count, ret);
}

poly::ReturnAction Source2SDK::Hook_DispatchConCommand(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	return g_CommandManager.Hook_DispatchConCommand(params, count, ret, static_cast<HookMode>(type));
}

/*poly::ReturnAction Source2SDK::Hook_HostSay(poly::IHook& hook, poly::Params& params, int count, poly::Return& ret, poly::CallbackType type) {
	return g_ChatManager.Hook_HostSay(hook, params, count, ret, static_cast<HookMode>(type));
}*/

#if S2SDK_PLATFORM_WINDOWS

#if PLUGIFY_ARCH_BITS == 64
const WORD PE_FILE_MACHINE = IMAGE_FILE_MACHINE_AMD64;
const WORD PE_NT_OPTIONAL_HDR_MAGIC = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
#else
const WORD PE_FILE_MACHINE = IMAGE_FILE_MACHINE_I386;
const WORD PE_NT_OPTIONAL_HDR_MAGIC = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
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