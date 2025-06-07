#pragma once
/*
#include <igameevents.h>
#include <utlqueue.h>
#include <utlvector.h>
#include <networksystem/inetworkserializer.h>
#include <steam/steam_api_common.h>
#include <steam/isteamugc.h>

class CServerSideClient;

class MultiAddonManager {
public:
	std::string BuildAddonPath(uint64_t addon);
	bool MountAddon(uint64_t addon, bool addToTail = false);
	bool UnmountAddon(uint64_t addon, bool remove = true);
	bool AddAddon(uint64_t addon, bool refresh = false);
	bool RemoveAddon(uint64_t addon, bool refresh = false);
	bool DownloadAddon(uint64_t addon, bool important = false, bool force = false);
	void PrintDownloadProgress() const;
	void RefreshAddons(bool reloadMap = false);
	void ClearAddons();
	void ReloadMap();

	uint64_t GetCurrentWorkshopMap() const { return m_currentWorkshopMap; }
	void SetCurrentWorkshopMap(uint64_t workshopID) { m_currentWorkshopMap = workshopID; }
	void ClearCurrentWorkshopMap() { m_currentWorkshopMap = k_PublishedFileIdInvalid; }

	static bool HasUGCConnection();
	void AddClientAddon(uint64_t addon, uint64 steamID64 = 0, bool refresh = false);
	void RemoveClientAddon(uint64_t addon, uint64 steamID64 = 0);
	void ClearClientAddons(uint64 steamID64 = 0);
	std::vector<uint64_t> GetClientAddons(uint64 steamID64 = 0);

public:
	void OnSteamAPIActivated();
	void OnStartupServer();
	void OnClientConnect(CPlayerSlot slot, const char* name, uint64 steamID64, const char* networkID);
	void OnClientDisconnect(CPlayerSlot slot, const char* name, uint64 steamID64, const char* networkID);
	void OnClientActive(CPlayerSlot slot, bool loadGame, const char* name, uint64 steamID64);
	void OnGameFrame();
	void OnPostEvent(INetworkMessageInternal* message, CNetMessage* data, uint64_t* clients);
	void* OnSendNetMessage(CServerSideClient* client, CNetMessage* data, NetChannelBufType_t bufType);
	void OnHostStateRequest(CHostStateMgr* pMgrDoNotUse, CHostStateRequest* pRequest);
	void OnReplyConnection(INetworkGameServer* server, CServerSideClient* client);

public:
	std::vector<uint64_t> m_extraAddons;
	std::vector<uint64_t> m_mountedAddons;
	std::vector<uint64_t> m_globalClientAddons;

private:
	CUtlVector<uint64_t> m_importantDownloads; // Important addon downloads that will trigger a map reload when finished
	CUtlQueue<uint64_t> m_downloadQueue; // Queue of all addon downloads to print progress

	STEAM_GAMESERVER_CALLBACK_MANUAL(MultiAddonManager, OnAddonDownloaded, DownloadItemResult_t, m_CallbackDownloadItemResult);

	// Used when reloading current map
	uint64_t m_currentWorkshopMap{k_PublishedFileIdInvalid};
	bool m_callbackRegistered{};
};

extern MultiAddonManager g_MultiAddonManager;*/