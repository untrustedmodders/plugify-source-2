#include <core/player_manager.h>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/entity/ccsplayercontroller.h>
#include <core/sdk/utils.h>
#include <plugin_export.h>

//! 
extern "C" PLUGIN_API int GetClientIndexFromEntityPointer(CBaseEntity* entity)
{
	return utils::GetEntityPlayerSlot(entity).Get() + 1;
}

/*//! 
extern "C" PLUGIN_API void* GetClientFromIndex(int clientIndex)
{
	CUtlVector<CServerSideClient *>* pClients = utils::GetClientList();
	if (!pClients)
	{
		return nullptr;
	}
	
	return pClients->Element(clientIndex - 1);
}

//! 
extern "C" PLUGIN_API int GetIndexFromClient(CServerSideClient* client)
{
	CUtlVector<CServerSideClient *>* pClients = utils::GetClientList();
	if (!pClients)
	{
		return -1;
	}
	
	if (pClients->Find(client) != -1)
	{
		return client->GetEntityIndex().Get();
	}

	return -1;
}*/

/**
 * @brief Retrieves a client's authentication string (SteamID).
 *
 * @param clientIndex Index of the client whose authentication string is being retrieved.
 * @return The authentication string.
 */
extern "C" PLUGIN_API void GetClientAuthId(plg::string& output, int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr || !pPlayer->m_bAuthorized)
	{
		return;
	}

	auto pSteamId = pPlayer->GetSteamId();
	if (pSteamId == nullptr)
	{
		return;
	}

	std::construct_at(&output, pSteamId->Render());
}

/**
 * @brief Returns the client's Steam account ID, a unique number identifying a given Steam account.
 *
 * @param clientIndex Index of the client.
 * @return uint64_t Steam account ID.
 */
extern "C" PLUGIN_API uint64_t GetClientAccountId(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr || !pPlayer->m_bAuthorized)
	{
		return 0;
	}

	auto pSteamId = pPlayer->GetSteamId();
	if (pSteamId == nullptr)
	{
		return 0;
	}

	return pSteamId->ConvertToUint64();
}

/**
 * @brief Retrieves a client's IP address.
 *
 * @param clientIndex Index of the client.
 * @return The IP address.
 */
extern "C" PLUGIN_API void GetClientIp(plg::string& output, int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return;
	}

	std::construct_at(&output, pPlayer->GetIpAddress());
}

/**
 * @brief Returns the client's name.
 *
 * @param clientIndex Index of the client.
 * @return The client's name.
 */
extern "C" PLUGIN_API void GetClientName(plg::string& output, int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return;
	}

	std::construct_at(&output, pPlayer->GetName());
}

/**
 * @brief Returns the client's connection time in seconds.
 *
 * @param clientIndex Index of the client.
 * @return float Connection time in seconds.
 */
extern "C" PLUGIN_API float GetClientTime(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return -1.0f;
	}

	return pPlayer->GetTimeConnected();
}

/**
 * @brief Returns the client's current latency (RTT).
 *
 * @param clientIndex Index of the client.
 * @return float Latency value.
 */
extern "C" PLUGIN_API float GetClientLatency(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return 0.0f;
	}

	return pPlayer->GetLatency();
}

/**
 * @brief Returns the client's access flags.
 *
 * @param clientIndex Index of the client.
 * @return uint64 Access flags as a bitmask.
 */
extern "C" PLUGIN_API uint64 GetUserFlagBits(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->GetAdminFlags();
}

/**
 * @brief Sets the access flags on a client using a bitmask.
 *
 * @param clientIndex Index of the client.
 * @param flags Bitmask representing the flags to be set.
 */
extern "C" PLUGIN_API void SetUserFlagBits(int clientIndex, uint64 flags)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return;
	}

	pPlayer->SetAdminFlags(flags);
}

/**
 * @brief Adds access flags to a client.
 *
 * If the client is not an admin, they will be given a temporary AdminId.
 *
 * @param clientIndex Index of the client.
 * @param flags Bitmask representing the flags to be added.
 */
extern "C" PLUGIN_API void AddUserFlags(int clientIndex, uint64 flags)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return;
	}

	pPlayer->SetAdminFlags(pPlayer->GetAdminFlags() | flags);
}

/**
 * @brief Removes access flags from a client.
 *
 * If the client is not an admin, this has no effect.
 *
 * @param clientIndex Index of the client.
 * @param flags Bitmask representing the flags to be removed.
 */
extern "C" PLUGIN_API void RemoveUserFlags(int clientIndex, uint64 flags)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return;
	}

	pPlayer->SetAdminFlags(pPlayer->GetAdminFlags() & ~flags);
}

/**
 * @brief Checks if a certain player has been authenticated.
 *
 * @param clientIndex Index of the client.
 * @return true if the player is authenticated, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientAuthorized(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsAuthorized();
}

/**
 * @brief Checks if a certain player is connected.
 *
 * @param clientIndex Index of the client.
 * @return true if the player is connected, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientConnected(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsConnected();
}

/**
 * @brief Checks if a certain player has entered the game.
 *
 * @param clientIndex Index of the client.
 * @return true if the player is in the game, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientInGame(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsInGame();
}

/**
 * @brief Checks if a certain player is the SourceTV bot.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is the SourceTV bot, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientSourceTV(int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return false;
	}

	return client->m_bIsHLTV();
}

/**
 * @brief Checks if the client is alive or dead.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is alive, false if dead.
 */
extern "C" PLUGIN_API bool IsClientAlive(int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return false;
	}

	return client->IsAlive();
}

/**
 * @brief Checks if a certain player is a fake client.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is a fake client, false otherwise.
 */
extern "C" PLUGIN_API bool IsFakeClient(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		return false;
	}

	return pPlayer->IsFakeClient();
}

/////

/**
 * @brief Retrieves a client's team index.
 *
 * @param clientIndex Index of the client.
 * @return int The team index of the client.
 */
extern "C" PLUGIN_API int GetClientTeam(int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return 0;
	}

	return client->m_iTeamNum();
}

/**
 * @brief Returns the client's health.
 *
 * @param clientIndex Index of the client.
 * @return int The health value of the client.
 */
extern "C" PLUGIN_API int GetClientHealth(int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return 0;
	}

	return client->m_iHealth();
}

/**
 * @brief Returns the client's armor value.
 *
 * @param clientIndex Index of the client.
 * @return int The armor value of the client.
 */
extern "C" PLUGIN_API int GetClientArmor(int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return 0;
	}

	return client->m_Armor();
}

/**
 * @brief Retrieves the client's origin vector.
 *
 * @param output Reference to a Vector where the client's origin will be stored.
 * @param clientIndex Index of the client.
 */
extern "C" PLUGIN_API void GetClientAbsOrigin(Vector& output, int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return;
	}

	output = client->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
}

/**
 * @brief Retrieves the client's position angle.
 *
 * @param output Reference to a QAngle where the client's position angle will be stored.
 * @param clientIndex Index of the client.
 */
extern "C" PLUGIN_API void GetClientAbsAngles(QAngle& output, int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return;
	}

	std::construct_at(&output, client->m_CBodyComponent->m_pSceneNode->m_angRotation());
}

/**
 * @brief Processes the target string to determine if one user can target another.
 *
 * @param output Reference to a vector where the result of the targeting operation will be stored.
 * @param caller Index of the client making the target request.
 * @param target The target string specifying the player or players to be targeted.
 */
extern "C" PLUGIN_API void ProcessTargetString(std::vector<int>& output, int caller, const plg::string& target)
{
	g_PlayerManager.TargetPlayerString(caller, target.c_str(), output);
}

/**
 * @brief Changes a client's team.
 *
 * @param clientIndex Index of the client.
 * @param team The team index to assign the client to.
 */
extern "C" PLUGIN_API void ChangeClientTeam(int clientIndex, int team)
{
	auto client = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex - 1)));
	if (!client)
	{
		return;
	}

	client->ChangeTeam(team);
}

/**
 * @brief Switches the player's team.
 *
 * @param clientIndex Index of the client.
 * @param team The team index to switch the client to.
 */
extern "C" PLUGIN_API void SwitchClientTeam(int clientIndex, int team)
{
	auto client = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex - 1)));
	if (!client)
	{
		return;
	}

	client->SwitchTeam(team);
}

/**
 * @brief Respawns a player.
 *
 * @param clientIndex Index of the client to respawn.
 */
extern "C" PLUGIN_API void RespawnClient(int clientIndex)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return;
	}

	if (client->GetPawn()->IsAlive())
	{
		// TODO: Fix players spawning under spawn positions
		static_cast<CCSPlayerPawn*>(client->GetPawn())->Respawn();
	}
	else
	{
		static_cast<CCSPlayerController*>(client)->Respawn();
	}
}

/**
 * @brief Forces a player to commit suicide.
 *
 * @param clientIndex Index of the client.
 * @param explode If true, the client will explode upon death.
 * @param force If true, the suicide will be forced.
 */
extern "C" PLUGIN_API void ForcePlayerSuicide(int clientIndex, bool explode, bool force)
{
	auto client = utils::GetController(CPlayerSlot(clientIndex - 1));
	if (!client)
	{
		return;
	}

	client->GetPawn()->CommitSuicide(explode, force);
}

/**
 * @brief Disconnects a client from the server as soon as the next frame starts.
 *
 * @param clientIndex Index of the client to be kicked.
 */
extern "C" PLUGIN_API void KickClient(int clientIndex)
{
	g_pEngineServer2->DisconnectClient(CPlayerSlot(clientIndex - 1), NETWORK_DISCONNECT_KICKED);
}

/**
 * @brief Bans a client for a specified duration.
 *
 * @param clientIndex Index of the client to be banned.
 * @param duration Duration of the ban in seconds.
 * @param kick If true, the client will be kicked immediately after being banned.
 */
extern "C" PLUGIN_API void BanClient(int clientIndex, float duration, bool kick)
{
	g_pEngineServer2->BanClient(CPlayerSlot(clientIndex - 1), duration, kick);
}

/**
 * @brief Bans an identity (either an IP address or a Steam authentication string).
 *
 * @param steamId The Steam ID to ban.
 * @param duration Duration of the ban in seconds.
 * @param kick If true, the client will be kicked immediately after being banned.
 */
extern "C" PLUGIN_API void BanIdentity(uint64_t steamId, float duration, bool kick)
{
	g_pEngineServer2->BanClient(CSteamID(static_cast<uint64>(steamId)), duration, kick);
}

