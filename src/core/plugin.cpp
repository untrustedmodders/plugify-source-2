#include "plugin.h"
#include "fwd.h"

#include "interfaces/cgameresourceserviceserver.h"
#include "interfaces/cschemasystem.h"
#include "listenermanager.h"
#include "hookmanager.h"
#include "eventmanager.h"

#include <export/keyvalues.h>
#include <export/gameconfig.h>

#if _DEBUG
void Message(const char* msg, ...) {
	va_list args;
	va_start(args, msg);

	char buf[1024] = {};
	V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

	ConColorMsg(Color(255, 0, 255, 255), "[cs2sdk] %s\n", buf);

	va_end(args);
}
#else
#define Message(...)
#endif

namespace cs2sdk {
	Source2SDK g_sdk;
}

using namespace cs2sdk;

#define DEFINE_MANAGER_ACCESSOR(name, ret, ...) \
    typedef ret (*Fn##name)(__VA_ARGS__); \
    auto& Get##name##ListenerManager() { \
		static cs2sdk::CListenerManager<Fn##name> s_##name; \
		return s_##name; \
	} \
	extern "C" \
	PLUGIN_API void name##_Register(Fn##name func) { \
		Get##name##ListenerManager().Register(func); \
	} \
	extern "C" \
	PLUGIN_API void name##_Unregister(Fn##name func) { \
		Get##name##ListenerManager().Unregister(func); \
	}

DEFINE_MANAGER_ACCESSOR(OnClientActive, void, int, bool, const char*, uint64)
DEFINE_MANAGER_ACCESSOR(OnClientConnect, bool, int, const char*, uint64, const char*, bool, const char*)
DEFINE_MANAGER_ACCESSOR(OnClientConnected, void, int, const char*, uint64, const char*, const char*, bool)
DEFINE_MANAGER_ACCESSOR(OnClientFullyConnect, void, int)
DEFINE_MANAGER_ACCESSOR(OnClientDisconnect, void, int, int, const char*, uint64, const char*)
DEFINE_MANAGER_ACCESSOR(OnClientPutInServer, void, int, char const*, int, uint64)
DEFINE_MANAGER_ACCESSOR(OnClientSettingsChanged, void, int)
DEFINE_MANAGER_ACCESSOR(OnLevelInit, void, const char*, const char*)
DEFINE_MANAGER_ACCESSOR(OnLevelShutdown, void)
/*DEFINE_MANAGER_ACCESSOR(OnNetworkidValidated)
DEFINE_MANAGER_ACCESSOR(OnEdictAllocated)
DEFINE_MANAGER_ACCESSOR(OnEdictFreed)
DEFINE_MANAGER_ACCESSOR(OnQueryCvarValueFinished)
DEFINE_MANAGER_ACCESSOR(OnServerActivate)
DEFINE_MANAGER_ACCESSOR(OnGameFrame)
DEFINE_MANAGER_ACCESSOR(OnEntityPreSpawned)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntityPreSpawned)
DEFINE_MANAGER_ACCESSOR(OnEntityCreated)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntityCreated)
DEFINE_MANAGER_ACCESSOR(OnEntitySpawned)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntitySpawned)
DEFINE_MANAGER_ACCESSOR(OnEntityDeleted)
DEFINE_MANAGER_ACCESSOR(OnNetworkedEntityDeleted)
DEFINE_MANAGER_ACCESSOR(OnDataLoaded)
DEFINE_MANAGER_ACCESSOR(OnCombinerPreCache)
DEFINE_MANAGER_ACCESSOR(OnDataUnloaded)
DEFINE_MANAGER_ACCESSOR(OnPlayerRunCommand)
DEFINE_MANAGER_ACCESSOR(OnPlayerPostRunCommand)
DEFINE_MANAGER_ACCESSOR(OnButtonStateChanged)*/

void Source2SDK::OnPluginStart() {
	Message("OnPluginStart!\n");
	globals::Initialize();

	using enum dyno::CallbackType;
	g_HookManager.AddHookMemFunc(&IMetamodListener::OnLevelInit, globals::metamodListener, Hook_OnLevelInit, Post);
	g_HookManager.AddHookMemFunc(&IMetamodListener::OnLevelShutdown, globals::metamodListener, Hook_OnLevelShutdown, Post);
	g_HookManager.AddHookMemFunc(&IServerGameDLL::GameFrame, g_pSource2Server, Hook_GameFrame, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientActive, g_pSource2GameClients, Hook_ClientActive, Post);
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientDisconnect, g_pSource2GameClients, Hook_ClientDisconnect, Post); // May be Pre too >
	g_HookManager.AddHookMemFunc(&IServerGameClients::ClientPutInServer, g_pSource2GameClients, Hook_ClientPutInServer, Post);
	/**/g_HookManager.AddHookMemFunc(&IServerGameClients::ClientSettingsChanged, g_pSource2GameClients, Hook_ClientSettingsChanged, Post);
	/**/g_HookManager.AddHookMemFunc(&IServerGameClients::OnClientConnected, g_pSource2GameClients, Hook_OnClientConnected, Post);
	/**/g_HookManager.AddHookMemFunc(&IServerGameClients::ClientFullyConnect, g_pSource2GameClients, Hook_ClientFullyConnect, Post);
	/**/g_HookManager.AddHookMemFunc(&IServerGameClients::ClientConnect, g_pSource2GameClients, Hook_ClientConnect, Post);
	//AddHookMemFunc(&IServerGameClients::ClientCommand, g_pSource2GameClients, Hook_ClientCommand, Post);
	g_HookManager.AddHookMemFunc(&INetworkServerService::StartupServer, g_pNetworkServerService, Hook_StartupServer, Post);
	//AddHookMemFunc(&ISource2GameEntities::CheckTransmit, g_pSource2GameEntities, Hook_CheckTransmit, Post);
	using Fn = void(IGameEventSystem::*)(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64 *clients, INetworkSerializable *pEvent, const void *pData, unsigned long nSize, NetChannelBufType_t bufType);
	g_HookManager.AddHookMemFunc<Fn>(&IGameEventSystem::PostEventAbstract, g_gameEventSystem, Hook_PostEvent, Post);
	g_HookManager.AddHookMemFunc(&IGameEventManager2::FireEvent, g_gameEventSystem, Hook_FireEvent, Pre, Post);
}

void Source2SDK::OnPluginEnd() {
	Message("OnPluginEnd!\n");
	globals::Terminate();
	g_HookManager.UnhookAll();
}

dyno::ReturnAction Source2SDK::Hook_StartupServer(dyno::CallbackType type, dyno::IHook& hook) {
	Message("Startup server\n");

	gpGlobals = globals::GetGameGlobals();

	if (gpGlobals == nullptr) {
		Error("Failed to lookup gpGlobals\n");
	}

	g_pEntitySystem = pGameResourceServiceServer->GetGameEntitySystem();
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_FireEvent(dyno::CallbackType type, dyno::IHook& hook) {
	// Message("FireEvent(%s)\n", event->GetName() );
	return type == dyno::CallbackType::Post ? g_EventManager.Hook_OnFireEvent_Post(hook) : g_EventManager.Hook_OnFireEvent(hook);
}

dyno::ReturnAction Source2SDK::Hook_PostEvent(dyno::CallbackType type, dyno::IHook& hook) {
	// Message("Hook_PostEvent(%d, %d, %d, %lli)\n", nSlot, bLocalOnly, nClientCount, clients );
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_OnLevelInit(dyno::CallbackType type, dyno::IHook& hook) {
	auto pMapName = dyno::GetArgument<const char*>(hook, 1);
	auto pMapEntities = dyno::GetArgument<const char*>(hook, 2);
	Message("OnLevelInit(%s)\n", pMapName);
	GetOnLevelInitListenerManager().Notify(pMapName, pMapEntities);
	return dyno::ReturnAction::Ignored;
};

dyno::ReturnAction Source2SDK::Hook_OnLevelShutdown(dyno::CallbackType type, dyno::IHook& hook) {
	Message("OnLevelShutdown\n");
	GetOnLevelShutdownListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
};

dyno::ReturnAction Source2SDK::Hook_GameFrame(dyno::CallbackType type, dyno::IHook& hook) {
	//bool simulating, bool bFirstTick, bool bLastTick

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientActive(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto bLoadGame = dyno::GetArgument<bool>(hook, 2);
	auto pszName = dyno::GetArgument<const char*>(hook, 3);
	auto xuid = dyno::GetArgument<uint64>(hook, 4);
	Message("Hook_ClientActive(%d, %d, \"%s\", %lli)\n", slot, bLoadGame, pszName, xuid);
	GetOnClientActiveListenerManager().Notify(slot.Get(), bLoadGame, pszName, xuid);
	return dyno::ReturnAction::Ignored;
}
dyno::ReturnAction Source2SDK::Hook_ClientDisconnect(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot, int reason, const char* pszName, uint64 xuid, const char* pszNetworkID
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto reason = dyno::GetArgument<int>(hook, 2);
	auto pszName = dyno::GetArgument<const char*>(hook, 3);
	auto xuid = dyno::GetArgument<uint64>(hook, 4);
	auto pszNetworkID = dyno::GetArgument<const char*>(hook, 5);
	Message("Hook_ClientDisconnect(%d, %d, \"%s\", %lli, \"%s\")\n", slot, reason, pszName, xuid, pszNetworkID);
	GetOnClientDisconnectListenerManager().Notify(slot.Get(), reason, pszName, xuid, pszNetworkID);
	return dyno::ReturnAction::Ignored;
}
dyno::ReturnAction Source2SDK::Hook_ClientPutInServer(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot, char const *pszName, int type, uint64 xuid
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto pszName = dyno::GetArgument<const char*>(hook, 2);
	auto conType = dyno::GetArgument<int>(hook, 3);
	auto xuid = dyno::GetArgument<uint64>(hook, 4);
	Message("Hook_ClientPutInServer(%d, \"%s\", %d, %d, %lli)\n", slot, pszName, conType, xuid);
	GetOnClientPutInServerListenerManager().Notify(slot.Get(), pszName, conType, xuid);
	return dyno::ReturnAction::Ignored;
}
dyno::ReturnAction Source2SDK::Hook_ClientSettingsChanged(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	Message("Hook_ClientSettingsChanged(%d)\n", slot);
	GetOnClientSettingsChangedListenerManager().Notify(slot.Get());
	return dyno::ReturnAction::Ignored;
}
dyno::ReturnAction Source2SDK::Hook_OnClientConnected(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot, const cha*r pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto pszName = dyno::GetArgument<const char*>(hook, 2);
	auto xuid = dyno::GetArgument<uint64>(hook, 3);
	auto pszNetworkID = dyno::GetArgument<const char*>(hook, 4);
	auto pszAddress = dyno::GetArgument<const char*>(hook, 5);
	auto bFakePlayer = dyno::GetArgument<bool>(hook, 6);
	Message("Hook_OnClientConnected(%d, \"%s\", %lli, \"%s\", \"%s\", %d)\n", slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	GetOnClientConnectedListenerManager().Notify(slot.Get(), pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	return dyno::ReturnAction::Ignored;
}
dyno::ReturnAction Source2SDK::Hook_ClientFullyConnect(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	Message("Hook_ClientFullyConnect(%d)\n", slot);
	GetOnClientFullyConnectListenerManager().Notify(slot.Get());
	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction Source2SDK::Hook_ClientConnect(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, CBufferString *pRejectReason
	auto slot = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto pszName = dyno::GetArgument<const char*>(hook, 2);
	auto xuid = dyno::GetArgument<uint64>(hook, 3);
	auto pszNetworkID = dyno::GetArgument<const char*>(hook, 4);
	bool unk1 = dyno::GetArgument<bool>(hook, 5);
	auto pRejectReason = dyno::GetArgument<CBufferString*>(hook, 6);
	Message("Hook_ClientConnect(%d, \"%s\", %lli, \"%s\", %d, \"%s\")\n", slot, pszName, xuid, pszNetworkID, unk1, pRejectReason->ToGrowable()->Get());
	GetOnClientConnectListenerManager().Notify(slot.Get(), pszName, xuid, pszNetworkID, unk1, pRejectReason->ToGrowable()->Get());
	return dyno::ReturnAction::Ignored;
}
/*dyno::ReturnAction Source2SDK::Hook_ClientCommand(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot nSlot, const CCommand& _cmd
	//Message("Hook_ClientCommand(%d, \"%s\")\n", slot, args.GetCommandString());
	GetOnClientCommandListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
}
dyno::ReturnAction Source2SDK::Hook_CheckTransmit(dyno::CallbackType type, dyno::IHook& hook) {
	//CPlayerSlot nSlot, const CCommand& _cmd
	GetOnCheckTransmitListenerManager().Notify();
	return dyno::ReturnAction::Ignored;
}*/

EXPOSE_PLUGIN(PLUGIN_API, &cs2sdk::g_sdk)