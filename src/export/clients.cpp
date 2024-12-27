#include <core/player_manager.hpp>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/entity/ccsplayercontroller.h>
#include <core/sdk/utils.h>
#include <plugify/cpp_plugin.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

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
	
	return pClients->Element(clientIndex);
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
		return pController->GetEntityIndex().Get();
	}

	return -1;
}*/

/**
 * @brief Retrieves a client's authentication string (SteamID).
 *
 * @param clientIndex Index of the client whose authentication string is being retrieved.
 * @return The authentication string.
 */
extern "C" PLUGIN_API plg::string GetClientAuthId(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
	if (pPlayer == nullptr || !pPlayer->m_bAuthorized)
	{
		return {};
	}

	auto pSteamId = pPlayer->GetSteamId();
	if (pSteamId == nullptr)
	{
		return {};
	}

	return pSteamId->Render();
}

/**
 * @brief Returns the client's Steam account ID, a unique number identifying a given Steam account.
 *
 * @param clientIndex Index of the client.
 * @return uint64_t Steam account ID.
 */
extern "C" PLUGIN_API uint64_t GetClientAccountId(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
extern "C" PLUGIN_API plg::string GetClientIp(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
	if (pPlayer == nullptr)
	{
		return {};
	}

	return pPlayer->GetIpAddress();
}

/**
 * @brief Returns the client's name.
 *
 * @param clientIndex Index of the client.
 * @return The client's name.
 */
extern "C" PLUGIN_API plg::string GetClientName(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
	if (pPlayer == nullptr)
	{
		return {};
	}

	return pPlayer->GetName();
}

/**
 * @brief Returns the client's connection time in seconds.
 *
 * @param clientIndex Index of the client.
 * @return float Connection time in seconds.
 */
extern "C" PLUGIN_API float GetClientTime(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return false;
	}

	return pController->m_bIsHLTV();
}

/**
 * @brief Checks if the client is alive or dead.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is alive, false if dead.
 */
extern "C" PLUGIN_API bool IsClientAlive(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return false;
	}

	return pController->IsAlive();
}

/**
 * @brief Checks if a certain player is a fake client.
 *
 * @param clientIndex Index of the client.
 * @return true if the client is a fake client, false otherwise.
 */
extern "C" PLUGIN_API bool IsFakeClient(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex));
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
 * @return The team index of the client.
 */
extern "C" PLUGIN_API int GetClientTeam(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return 0;
	}

	return pController->m_iTeamNum();
}

/**
 * @brief Returns the client's health.
 *
 * @param clientIndex Index of the client.
 * @return The health value of the client.
 */
extern "C" PLUGIN_API int GetClientHealth(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return 0;
	}

	return pController->m_iHealth();
}

/**
 * @brief Returns the client's armor value.
 *
 * @param clientIndex Index of the client.
 * @return The armor value of the client.
 */
extern "C" PLUGIN_API int GetClientArmor(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return 0;
	}

	return 0;//pController->m_Armor(); // TODO:
}

/**
 * @brief Retrieves the client's origin vector.
 *
 * @param clientIndex Index of the client.
 * @return A Vector where the client's origin will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetClientAbsOrigin(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return {};
	}

	const Vector& vec = pController->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Retrieves the client's position angle.
 *
 * @param clientIndex Index of the client.
 * @return A QAngle where the client's position angle will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetClientAbsAngles(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return {};
	}

	const QAngle& ang = pController->m_CBodyComponent->m_pSceneNode->m_angRotation();
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Retrieves the client's eye angle.
 *
 * @param clientIndex Index of the client.
 * @return A QAngle where the client's eye angle will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetClientEyeAngles(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		return {};
	}

	const QAngle& ang = static_cast<CCSPlayerPawn*>(pController->GetPawn())->m_angEyeAngles();
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Processes the target string to determine if one user can target another.
 *
 * @param caller Index of the client making the target request.
 * @param target The target string specifying the player or players to be targeted.
 * @return A vector where the result of the targeting operation will be stored.
 */
extern "C" PLUGIN_API plg::vector<int> ProcessTargetString(int caller, const plg::string& target)
{
	plg::vector<int> output;
	g_PlayerManager.TargetPlayerString(caller, target.c_str(), output);
	return output;
}

/**
 * @brief Changes a client's team.
 *
 * @param clientIndex Index of the client.
 * @param team The team index to assign the client to.
 */
extern "C" PLUGIN_API void ChangeClientTeam(int clientIndex, int team)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'ChangeClientTeam' on invalid client index: %d\n", clientIndex);
		return;
	}

	pController->ChangeTeam(team);
}

/**
 * @brief Switches the player's team.
 *
 * @param clientIndex Index of the client.
 * @param team The team index to switch the client to.
 */
extern "C" PLUGIN_API void SwitchClientTeam(int clientIndex, int team)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SwitchClientTeam' on invalid client index: %d\n", clientIndex);
		return;
	}

	pController->SwitchTeam(team);
}

/**
 * @brief Respawns a player.
 *
 * @param clientIndex Index of the client to respawn.
 */
extern "C" PLUGIN_API void RespawnClient(int clientIndex)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'RespawnClient' on invalid client index: %d\n", clientIndex);
		return;
	}

	if (pController->GetPawn()->IsAlive())
	{
		// TODO: Fix players spawning under spawn positions
		static_cast<CCSPlayerPawn*>(pController->GetPawn())->Respawn();
	}
	else
	{
		static_cast<CCSPlayerController*>(pController)->Respawn();
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
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'ForcePlayerSuicide' on invalid client index: %d\n", clientIndex);
		return;
	}

	pController->GetPawn()->CommitSuicide(explode, force);
}

/**
 * @brief Disconnects a client from the server as soon as the next frame starts.
 *
 * @param clientIndex Index of the client to be kicked.
 */
extern "C" PLUGIN_API void KickClient(int clientIndex)
{
	g_pEngineServer2->DisconnectClient(CPlayerSlot(clientIndex), NETWORK_DISCONNECT_KICKED);
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
	g_pEngineServer2->BanClient(CPlayerSlot(clientIndex), duration, kick);
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

/**
 * @brief Retrieves the handle of the client's currently active weapon.
 *
 * @param clientIndex The index of the client.
 * @return The entity handle of the active weapon, or INVALID_EHANDLE_INDEX if the client is invalid or has no active weapon.
 */
extern "C" PLUGIN_API int GetClientActiveWeapon(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientActiveWeapon' on invalid client index: %d\n", clientIndex);
		return INVALID_EHANDLE_INDEX;
	}

	auto pWeaponServices = pController->GetPawn()->m_pWeaponServices();
	if (!pWeaponServices)
	{
		return INVALID_EHANDLE_INDEX;
	}

	return pWeaponServices->m_hActiveWeapon().ToInt();
}

/**
 * @brief Retrieves a list of weapon handles owned by the client.
 *
 * @param clientIndex The index of the client.
 * @return A vector of entity handles for the client's weapons, or an empty vector if the client is invalid or has no weapons.
 */
extern "C" PLUGIN_API plg::vector<int> GetClientWeapons(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientWeapons' on invalid client index: %d\n", clientIndex);
		return {};
	}

	auto pWeaponServices = pController->GetPawn()->m_pWeaponServices();
	if (!pWeaponServices)
	{
		return {};
	}

	CUtlVector<CHandle<CBasePlayerWeapon>>* weapons = pWeaponServices->m_hMyWeapons();

	plg::vector<int> handles;
	handles.reserve(static_cast<size_t>(weapons->Count()));

	FOR_EACH_VEC(*weapons, i)
	{
		handles.emplace_back((*weapons)[i].ToInt());
	}

	return handles;
}

/**
 * @brief Removes all weapons from a client, with an option to remove the suit as well.
 *
 * @param clientIndex The index of the client.
 * @param removeSuit A boolean indicating whether to also remove the client's suit.
 */
extern "C" PLUGIN_API void StripWeapons(int clientIndex, bool removeSuit)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'StripWeapons' on invalid client index: %d\n", clientIndex);
		return;
	}

	auto pItemServices = pController->GetPawn()->m_pItemServices();
	if (!pItemServices)
	{
		return;
	}

	pItemServices->StripPlayerWeapons(removeSuit);
}

/**
 * @brief Forces a player to drop their weapon.
 *
 * @param clientIndex Index of the client.
 * @param weaponHandle Handle of weapon to drop.
 * @param target Target direction.
 * @param velocity Velocity to toss weapon or zero to just drop weapon.
 */
extern "C" PLUGIN_API void DropWeapon(int clientIndex, int weaponHandle, const plg::vec3& target, const plg::vec3& velocity)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'DropWeapon' on invalid client index: %d\n", clientIndex);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)weaponHandle)));
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'DropWeapon' on invalid weapon handle: %d\n", weaponHandle);
		return;
	}

	auto pWeaponServices = pController->GetPawn()->m_pWeaponServices();
	if (!pWeaponServices)
	{
		return;
	}

	pWeaponServices->Drop(pWeapon, reinterpret_cast<Vector*>(const_cast<plg::vec3*>(&target)), reinterpret_cast<Vector*>(const_cast<plg::vec3*>(&velocity)));
}

/**
 * @brief Bumps a player's weapon.
 *
 * @param clientIndex Index of the client.
 * @param weaponHandle Handle of weapon to bump.
 */
extern "C" PLUGIN_API void BumpWeapon(int clientIndex, int weaponHandle)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid client index: %d\n", clientIndex);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)weaponHandle)));
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid weapon handle: %d\n", weaponHandle);
		return;
	}

	auto pWeaponServices = pController->GetPawn()->m_pWeaponServices();
	if (!pWeaponServices)
	{
		return;
	}

	pWeaponServices->Bump(pWeapon);
}

/**
 * @brief Switches a player's weapon.
 *
 * @param clientIndex Index of the client.
 * @param weaponHandle Handle of weapon to switch.
 */
extern "C" PLUGIN_API void SwitchWeapon(int clientIndex, int weaponHandle)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid client index: %d\n", clientIndex);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)weaponHandle)));
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid weapon handle: %d\n", weaponHandle);
		return;
	}

	auto pWeaponServices = pController->GetPawn()->m_pWeaponServices();
	if (!pWeaponServices)
	{
		return;
	}

	pWeaponServices->Switch(pWeapon);
}

/**
 * @brief Removes a player's weapon.
 *
 * @param clientIndex Index of the client.
 * @param weaponHandle Handle of weapon to remove.
 */
extern "C" PLUGIN_API void RemoveWeapon(int clientIndex, int weaponHandle)
{
	auto pController = utils::GetController(CPlayerSlot(clientIndex));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'RemovePlayerItem' on invalid client index: %d\n", clientIndex);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)weaponHandle)));
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'RemovePlayerItem' on invalid weapon handle: %d\n", weaponHandle);
		return;
	}

	auto pWeaponServices = pController->GetPawn()->m_pWeaponServices();
	if (!pWeaponServices)
	{
		return;
	}

	pWeaponServices->RemoveItem(pWeapon);
}

/**
 * @brief Gives a named item (e.g., weapon) to a client.
 *
 * @param clientIndex The index of the client.
 * @param itemName The name of the item to give.
 * @return The entity handle of the created item, or INVALID_EHANDLE_INDEX if the client or item is invalid.
 */
extern "C" PLUGIN_API int GiveNamedItem(int clientIndex, const plg::string& itemName)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GiveNamedItem' on invalid client index: %d\n", clientIndex);
		return INVALID_EHANDLE_INDEX;
	}

	auto pItemServices = pController->GetPawn()->m_pItemServices();
	if (!pItemServices)
	{
		return INVALID_EHANDLE_INDEX;
	}

	CBaseEntity* pWeapon = pItemServices->GiveNamedItem(itemName.c_str());
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GiveNamedItem' with invalid item: %s\n", itemName.c_str());
		return INVALID_EHANDLE_INDEX;
	}

	return pWeapon->GetRefEHandle().ToInt();
}

/**
 * @brief Retrieves the state of a specific button for a client.
 *
 * @param clientIndex The index of the client.
 * @param buttonIndex The index of the button (0-2).
 * @return uint64_t The state of the specified button, or 0 if the client or button index is invalid.
 */
extern "C" PLUGIN_API uint64_t GetClientButtons(int clientIndex, int buttonIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientButtons' on invalid client index: %d\n", clientIndex);
		return  0;
	}

	if (buttonIndex > 2 || buttonIndex < 0)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientButtons' on invalid button index: %d\n", buttonIndex);
		return  0;
	}

	auto pMovementServices = pController->GetPawn()->m_pMovementServices();
	if (!pMovementServices)
	{
		return 0;
	}

	return pMovementServices->m_nButtons().m_pButtonStates[buttonIndex];
}

/**
 * @brief Retrieves the amount of money a client has.
 *
 * @param clientIndex The index of the client.
 * @return The amount of money the client has, or 0 if the client index is invalid.
 */
extern "C" PLUGIN_API int GetClientMoney(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientMoney' on invalid client index: %d\n", clientIndex);
		return  0;
	}

	auto pInGameMoneyServices = pController->m_pInGameMoneyServices();
	if (!pInGameMoneyServices)
	{
		return 0;
	}

	return pInGameMoneyServices->m_iAccount;
}

/**
 * @brief Sets the amount of money for a client.
 *
 * @param clientIndex The index of the client.
 * @param money The amount of money to set.
 */
extern "C" PLUGIN_API void SetClientMoney(int clientIndex, int money)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetClientMoney' on invalid client index: %d\n", clientIndex);
		return;
	}

	auto pInGameMoneyServices = pController->m_pInGameMoneyServices();
	if (!pInGameMoneyServices)
	{
		return;
	}

	pInGameMoneyServices->m_iAccount = money;
}

/**
 * @brief Retrieves the number of kills for a client.
 *
 * @param clientIndex The index of the client.
 * @return The number of kills the client has, or 0 if the client index is invalid.
 */
extern "C" PLUGIN_API int GetClientKills(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientKills' on invalid client index: %d\n", clientIndex);
		return 0;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iKills;
}

/**
 * @brief Sets the number of kills for a client.
 *
 * @param clientIndex The index of the client.
 * @param kills The number of kills to set.
 */
extern "C" PLUGIN_API void SetClientKills(int clientIndex, int kills)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetClientKills' on invalid client index: %d\n", clientIndex);
		return;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return;
	}

	pActionTrackingServices->m_matchStats().m_iKills = kills;
}

/**
 * @brief Retrieves the number of deaths for a client.
 *
 * @param clientIndex The index of the client.
 * @return The number of deaths the client has, or 0 if the client index is invalid.
 */
extern "C" PLUGIN_API int GetClientDeaths(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientDeaths' on invalid client index: %d\n", clientIndex);
		return  0;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iDeaths;
}

/**
 * @brief Sets the number of deaths for a client.
 *
 * @param clientIndex The index of the client.
 * @param deaths The number of deaths to set.
 */
extern "C" PLUGIN_API void SetClientDeaths(int clientIndex, int deaths)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetClientDeaths' on invalid client index: %d\n", clientIndex);
		return;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return;
	}

	pActionTrackingServices->m_matchStats().m_iDeaths = deaths;
}

/**
 * @brief Retrieves the number of assists for a client.
 *
 * @param clientIndex The index of the client.
 * @return The number of assists the client has, or 0 if the client index is invalid.
 */
extern "C" PLUGIN_API int GetClientAssists(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientAssists' on invalid client index: %d\n", clientIndex);
		return  0;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iAssists;
}

/**
 * @brief Sets the number of assists for a client.
 *
 * @param clientIndex The index of the client.
 * @param assists The number of assists to set.
 */
extern "C" PLUGIN_API void SetClientAssists(int clientIndex, int assists)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetClientAssists' on invalid client index: %d\n", clientIndex);
		return;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return;
	}

	pActionTrackingServices->m_matchStats().m_iAssists = assists;
}

/**
 * @brief Retrieves the total damage dealt by a client.
 *
 * @param clientIndex The index of the client.
 * @return The total damage dealt by the client, or 0 if the client index is invalid.
 */
extern "C" PLUGIN_API int GetClientDamage(int clientIndex)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'GetClientDamage' on invalid client index: %d\n", clientIndex);
		return  0;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iDamage;
}

/**
 * @brief Sets the total damage dealt by a client.
 *
 * @param clientIndex The index of the client.
 * @param damage The amount of damage to set.
 */
extern "C" PLUGIN_API void SetClientDamage(int clientIndex, int damage)
{
	auto pController = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(clientIndex)));
	if (!pController)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot execute 'SetClientDamage' on invalid client index: %d\n", clientIndex);
		return;
	}

	auto pActionTrackingServices = pController->m_pActionTrackingServices();
	if (!pActionTrackingServices)
	{
		return;
	}

	pActionTrackingServices->m_matchStats().m_iDamage = damage;
}
