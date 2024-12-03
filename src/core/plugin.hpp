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

	static poly::ReturnAction Hook_SetupHookLoop_Pre(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_SetupHookLoop_Post(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);

	//static poly::ReturnAction Hook_GameEventManagerInit(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_StartupServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ActivateServer(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_FireEvent(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	//static poly::ReturnAction Hook_PostEvent(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
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
	// static poly::ReturnAction Hook_CheckTransmit(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_ServerHibernationUpdate(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_GameServerSteamAPIActivated(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_GameServerSteamAPIDeactivated(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_OnHostNameChanged(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_PreFatalShutdown(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_UpdateWhenNotInGame(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_PreWorldUpdate(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_FireOutputInternal(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	//static poly::ReturnAction Hook_SayText2Filter(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret); // TODO: TEMP
	static poly::ReturnAction Hook_DispatchConCommand(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);
	static poly::ReturnAction Hook_SetClientListening(poly::CallbackType type, poly::Params& params, int count, poly::Return& ret);

/*public: // CBaseGameSystem
	GS_EVENT(GameInit);
	GS_EVENT(GameShutdown);
	GS_EVENT(GamePostInit);
	GS_EVENT(GamePreShutdown);
	GS_EVENT(BuildGameSessionManifest);
	GS_EVENT(GameActivate);
	GS_EVENT(ClientFullySignedOn);
	GS_EVENT(Disconnect);
	GS_EVENT(GameDeactivate);
	GS_EVENT(SpawnGroupPrecache);
	GS_EVENT(SpawnGroupUncache);
	GS_EVENT(PreSpawnGroupLoad);
	GS_EVENT(PostSpawnGroupLoad);
	GS_EVENT(PreSpawnGroupUnload);
	GS_EVENT(PostSpawnGroupUnload);
	GS_EVENT(ActiveSpawnGroupChanged);
	GS_EVENT(ClientPostDataUpdate);
	GS_EVENT(ClientPreRender);
	GS_EVENT(ClientPreEntityThink);
	GS_EVENT(ClientUpdate);
	GS_EVENT(ClientPostRender);
	GS_EVENT(ServerPreEntityThink);
	GS_EVENT(ServerPostEntityThink);
	GS_EVENT(ServerPreClientUpdate);
	GS_EVENT(ServerGamePostSimulate);
	GS_EVENT(ClientGamePostSimulate);
	GS_EVENT(GameFrameBoundary);
	GS_EVENT(OutOfGameFrameBoundary);
	GS_EVENT(SaveGame);
	GS_EVENT(RestoreGame);*/

private:
	IGameSystemFactory *m_pFactory = NULL;
};

extern Source2SDK g_sdk;