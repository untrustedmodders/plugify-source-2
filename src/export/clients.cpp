#include <core/player_manager.hpp>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/entity/cplayercontroller.h>
#include <core/sdk/utils.h>
#include <plugify/cpp_plugin.hpp>
#include <plugin_export.h>

#include <core/sdk/entity/cplayerpawn.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Retrieves the player slot from a given entity pointer.
 *
 * This function checks if the provided entity pointer is valid and returns
 * the corresponding player slot. If the entity pointer is invalid, it returns -1.
 *
 * @param entity A pointer to the entity (CBaseEntity*).
 * @return The player slot if valid, otherwise -1.
 */
extern "C" PLUGIN_API int GetPlayerSlotFromEntityPointer(CBaseEntity* entity) {
	if (!g_pGameEntitySystem->IsEntityPtr(entity)) {
		return -1;
	}

	return utils::GetEntityPlayerSlot(entity);
}

/**
 * @brief Returns a pointer to the entity instance by player slot index.
 *
 * This function retrieves a pointer to the entity instance associated with the specified player slot index.
 * If the client list is unavailable or the slot index is invalid, it returns nullptr.
 *
 * @param playerSlot Index of the player slot.
 * @return Pointer to the entity instance, or nullptr if the slot is invalid.
 */
extern "C" PLUGIN_API CEntityInstance* GetEntityPointerFromPlayerSlot(int playerSlot) {
	CUtlClientVector* pClients = utils::GetClientList();
	if (!pClients || !(0 <= playerSlot && playerSlot < pClients->Count())) {
		return nullptr;
	}

	return g_pGameEntitySystem->GetEntityInstance(pClients->Element(playerSlot)->GetEntityIndex());
}

/**
 * @brief Retrieves the client object from a given player slot.
 *
 * This function fetches the client object associated with the specified index.
 * If the client list is unavailable, or the index is invalid, it returns nullptr.
 *
 * @param playerSlot The index of the player's slot (0-based).
 * @return A pointer to the client object if found, otherwise nullptr.
 */
extern "C" PLUGIN_API void* GetClientBaseFromPlayerSlot(int playerSlot) {
	CUtlClientVector* pClients = utils::GetClientList();
	if (!pClients || !(0 <= playerSlot && playerSlot < pClients->Count())) {
		return nullptr;
	}
	
	return pClients->Element(playerSlot);
}

/**
 * @brief Retrieves the index of a given client object.
 *
 * This function finds the index of a specified client object within the client list.
 * If the client is not found or the list is unavailable, it returns -1.
 *
 * @param client A pointer to the client object (CServerSideClient*).
 * @return The player slot if found, otherwise -1.
 */
extern "C" PLUGIN_API int GetPlayerSlotFromClientBase(CServerSideClient* client) {
	CUtlClientVector* pClients = utils::GetClientList();
	if (!pClients) {
		return -1;
	}
	
	if (pClients->Find(client) != INVALID_PLAYER_SLOT) {
		return client->GetPlayerSlot();
	}

	return -1;
}

/**
 * @brief Returns the entity index for a given player slot.
 *
 * This function retrieves the entity index associated with the specified player slot.
 * If the client list is unavailable or the slot index is invalid, it returns 0.
 *
 * @param playerSlot The index of the player's slot.
 * @return The entity index if valid, otherwise 0.
 */
extern "C" PLUGIN_API int GetClientIndexFromPlayerSlot(int playerSlot) {
	CUtlClientVector* pClients = utils::GetClientList();
	if (!pClients || !(0 <= playerSlot && playerSlot < pClients->Count())) {
		return 0;
	}

	return pClients->Element(playerSlot)->GetEntityIndex();
}

/**
 * @brief Retrieves the player slot from a given client index.
 *
 * This function converts a client index to the corresponding player slot.
 * If the controller is not found, it returns -1.
 *
 * @param clientIndex The index of the client.
 * @return The player slot if valid, otherwise -1.
 */
extern "C" PLUGIN_API int GetPlayerSlotFromClientIndex(int clientIndex) {
	auto pController = g_PlayerManager.ToPlayer(CEntityIndex(clientIndex));
	if (!pController) {
		return -1;
	}

	return pController->GetPlayerSlot();
}

/**
 * @brief Retrieves a client's authentication string (SteamID).
 *
 * @param playerSlot The index of the player's slot whose authentication string is being retrieved.
 * @return The authentication string.
 */
extern "C" PLUGIN_API plg::string GetClientAuthId(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return {};
	}

	return pPlayer->GetSteamId().Render();
}

/**
 * @brief Returns the client's Steam account ID, a unique number identifying a given Steam account.
 *
 * @param playerSlot The index of the player's slot.
 * @return uint64_t Steam account ID.
 */
extern "C" PLUGIN_API uint64_t GetClientAccountId(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return 0;
	}

	return pPlayer->GetSteamId().ConvertToUint64();
}

/**
 * @brief Retrieves a client's IP address.
 *
 * @param playerSlot The index of the player's slot.
 * @return The IP address.
 */
extern "C" PLUGIN_API plg::string GetClientIp(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return {};
	}

	return pPlayer->GetIpAddress();
}

/**
 * @brief Returns the client's name.
 *
 * @param playerSlot The index of the player's slot.
 * @return The client's name.
 */
extern "C" PLUGIN_API plg::string GetClientName(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return {};
	}

	return pPlayer->GetName();
}

/**
 * @brief Returns the client's connection time in seconds.
 *
 * @param playerSlot The index of the player's slot.
 * @return float Connection time in seconds.
 */
extern "C" PLUGIN_API float GetClientTime(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return -1.0f;
	}

	return pPlayer->GetTimeConnected();
}

/**
 * @brief Returns the client's current latency (RTT).
 *
 * @param playerSlot The index of the player's slot.
 * @return float Latency value.
 */
extern "C" PLUGIN_API float GetClientLatency(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return 0.0f;
	}

	return pPlayer->GetLatency();
}

/**
 * @brief Returns the client's access flags.
 *
 * @param playerSlot The index of the player's slot.
 * @return uint64 Access flags as a bitmask.
 */
extern "C" PLUGIN_API uint64 GetUserFlagBits(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return 0;//pPlayer->GetAdminFlags();
}

/**
 * @brief Sets the access flags on a client using a bitmask.
 *
 * @param playerSlot The index of the player's slot.
 * @param flags Bitmask representing the flags to be set.
 */
extern "C" PLUGIN_API void SetUserFlagBits(int playerSlot, uint64 flags) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return;
	}

	//pPlayer->SetAdminFlags(flags);
}

/**
 * @brief Adds access flags to a client.
 *
 * If the client is not an admin, they will be given a temporary AdminId.
 *
 * @param playerSlot The index of the player's slot.
 * @param flags Bitmask representing the flags to be added.
 */
extern "C" PLUGIN_API void AddUserFlags(int playerSlot, uint64 flags) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return;
	}

	//pPlayer->SetAdminFlags(pPlayer->GetAdminFlags() | flags);
}

/**
 * @brief Removes access flags from a client.
 *
 * If the client is not an admin, this has no effect.
 *
 * @param playerSlot The index of the player's slot.
 * @param flags Bitmask representing the flags to be removed.
 */
extern "C" PLUGIN_API void RemoveUserFlags(int playerSlot, uint64 flags) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return;
	}

	//pPlayer->SetAdminFlags(pPlayer->GetAdminFlags() & ~flags);
}

/**
 * @brief Checks if a certain player has been authenticated.
 *
 * @param playerSlot The index of the player's slot.
 * @return true if the player is authenticated, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientAuthorized(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return pPlayer->IsAuthenticated();
}

/**
 * @brief Checks if a certain player is connected.
 *
 * @param playerSlot The index of the player's slot.
 * @return true if the player is connected, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientConnected(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return pPlayer->IsConnected();
}

/**
 * @brief Checks if a certain player has entered the game.
 *
 * @param playerSlot The index of the player's slot.
 * @return true if the player is in the game, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientInGame(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return pPlayer->IsInGame();
}

/**
 * @brief Checks if a certain player is the SourceTV bot.
 *
 * @param playerSlot The index of the player's slot.
 * @return true if the client is the SourceTV bot, false otherwise.
 */
extern "C" PLUGIN_API bool IsClientSourceTV(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return pPlayer->IsSourceTV();
}

/**
 * @brief Checks if the client is alive or dead.
 *
 * @param playerSlot The index of the player's slot.
 * @return true if the client is alive, false if dead.
 */
extern "C" PLUGIN_API bool IsClientAlive(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return pPlayer->IsAlive();
}

/**
 * @brief Checks if a certain player is a fake client.
 *
 * @param playerSlot The index of the player's slot.
 * @return true if the client is a fake client, false otherwise.
 */
extern "C" PLUGIN_API bool IsFakeClient(int playerSlot) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(playerSlot));
	if (pPlayer == nullptr) {
		return false;
	}

	return pPlayer->IsFakeClient();
}

/////

/**
 * @brief Retrieves a client's team index.
 *
 * @param playerSlot The index of the player's slot.
 * @return The team index of the client.
 */
extern "C" PLUGIN_API int GetClientTeam(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		return 0;
	}

	return pController->m_iTeamNum;
}

/**
 * @brief Returns the client's health.
 *
 * @param playerSlot The index of the player's slot.
 * @return The health value of the client.
 */
extern "C" PLUGIN_API int GetClientHealth(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		return 0;
	}

	return pController->m_iHealth;
}

/**
 * @brief Returns the client's armor value.
 *
 * @param playerSlot The index of the player's slot.
 * @return The armor value of the client.
 */
extern "C" PLUGIN_API int GetClientArmor(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		return 0;
	}

	return pController->GetCurrentPawn()->m_ArmorValue;
}

/**
 * @brief Retrieves the client's origin vector.
 *
 * @param playerSlot The index of the player's slot.
 * @return A Vector where the client's origin will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetClientAbsOrigin(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		return {};
	}

	const Vector& vec = pController->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin;
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Retrieves the client's position angle.
 *
 * @param playerSlot The index of the player's slot.
 * @return A QAngle where the client's position angle will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetClientAbsAngles(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		return {};
	}

	const QAngle& ang = pController->m_CBodyComponent->m_pSceneNode->m_angRotation;
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Retrieves the client's eye angle.
 *
 * @param playerSlot The index of the player's slot.
 * @return A QAngle where the client's eye angle will be stored.
 */
extern "C" PLUGIN_API plg::vec3 GetClientEyeAngles(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		return {};
	}

	const QAngle& ang = static_cast<CPlayerPawn*>(pController->GetCurrentPawn())->m_angEyeAngles;
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Processes the target string to determine if one user can target another.
 *
 * @param caller The index of the player's slot making the target request.
 * @param target The target string specifying the player or players to be targeted.
 * @return A vector where the result of the targeting operation will be stored.
 */
extern "C" PLUGIN_API plg::vector<int> ProcessTargetString(int caller, const plg::string& target) {
	plg::vector<int> output;
	g_PlayerManager.TargetPlayerString(caller, target, output);
	return output;
}

/**
 * @brief Changes a client's team.
 *
 * @param playerSlot The index of the player's slot.
 * @param team The team index to assign the client to.
 */
extern "C" PLUGIN_API void ChangeClientTeam(int playerSlot, int team) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'ChangeClientTeam' on invalid player slot: {}\n", playerSlot);
		return;
	}

	pController->ChangeTeam(team);
}

/**
 * @brief Switches the player's team.
 *
 * @param playerSlot The index of the player's slot.
 * @param team The team index to switch the client to.
 */
extern "C" PLUGIN_API void SwitchClientTeam(int playerSlot, int team) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SwitchClientTeam' on invalid player slot: {}\n", playerSlot);
		return;
	}

	pController->SwitchTeam(team);
}

/**
 * @brief Respawns a player.
 *
 * @param playerSlot The index of the player's slot to respawn.
 */
extern "C" PLUGIN_API void RespawnClient(int playerSlot) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'RespawnClient' on invalid player slot: {}\n", playerSlot);
		return;
	}

	if (pController->GetCurrentPawn()->IsAlive()) {
		// TODO: Fix players spawning under spawn positions
		static_cast<CPlayerPawn*>(pController->GetCurrentPawn())->Respawn();
	} else {
		static_cast<CPlayerController*>(pController)->Respawn();
	}
}

/**
 * @brief Forces a player to commit suicide.
 *
 * @param playerSlot The index of the player's slot.
 * @param explode If true, the client will explode upon death.
 * @param force If true, the suicide will be forced.
 */
extern "C" PLUGIN_API void ForcePlayerSuicide(int playerSlot, bool explode, bool force) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'ForcePlayerSuicide' on invalid player slot: {}\n", playerSlot);
		return;
	}

	pController->GetCurrentPawn()->CommitSuicide(explode, force);
}

/**
 * @brief Disconnects a client from the server as soon as the next frame starts.
 *
 * @param playerSlot The index of the player's slot to be kicked.
 */
extern "C" PLUGIN_API void KickClient(int playerSlot) {
	g_pEngineServer->DisconnectClient(playerSlot, NETWORK_DISCONNECT_KICKED);
}

/**
 * @brief Bans a client for a specified duration.
 *
 * @param playerSlot The index of the player's slot to be banned.
 * @param duration Duration of the ban in seconds.
 * @param kick If true, the client will be kicked immediately after being banned.
 */
extern "C" PLUGIN_API void BanClient(int playerSlot, float duration, bool kick) {
	g_pEngineServer->BanClient(CPlayerSlot(playerSlot), duration, kick);
}

/**
 * @brief Bans an identity (either an IP address or a Steam authentication string).
 *
 * @param steamId The Steam ID to ban.
 * @param duration Duration of the ban in seconds.
 * @param kick If true, the client will be kicked immediately after being banned.
 */
extern "C" PLUGIN_API void BanIdentity(uint64_t steamId, float duration, bool kick) {
	g_pEngineServer->BanClient(CSteamID(static_cast<uint64>(steamId)), duration, kick);
}

/**
 * @brief Retrieves the handle of the client's currently active weapon.
 *
 * @param playerSlot The index of the player's slot.
 * @return The entity handle of the active weapon, or INVALID_EHANDLE_INDEX if the client is invalid or has no active weapon.
 */
extern "C" PLUGIN_API int GetClientActiveWeapon(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientActiveWeapon' on invalid player slot: {}\n", playerSlot);
		return INVALID_EHANDLE_INDEX;
	}

	CCSPlayer_WeaponServices* pWeaponServices = pController->GetCurrentPawn()->m_pWeaponServices;
	if (!pWeaponServices) {
		return INVALID_EHANDLE_INDEX;
	}

	return pWeaponServices->m_hActiveWeapon().ToInt();
}

/**
 * @brief Retrieves a list of weapon handles owned by the client.
 *
 * @param playerSlot The index of the player's slot.
 * @return A vector of entity handles for the client's weapons, or an empty vector if the client is invalid or has no weapons.
 */
extern "C" PLUGIN_API plg::vector<int> GetClientWeapons(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientWeapons' on invalid player slot: {}\n", playerSlot);
		return {};
	}

	CCSPlayer_WeaponServices* pWeaponServices = pController->GetCurrentPawn()->m_pWeaponServices;
	if (!pWeaponServices) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientWeapons' on m_pWeaponServices: {}\n", playerSlot);
		return {};
	}

	CUtlVector<CHandle<CBasePlayerWeapon>>* weapons = pWeaponServices->m_hMyWeapons;
	if (!weapons) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientWeapons' on m_hMyWeapons: {}\n", playerSlot);
		return {};
	}

	plg::vector<int> handles;
	handles.reserve(static_cast<size_t>(weapons->Count()));

	FOR_EACH_VEC(*weapons, i) {
		handles.emplace_back((*weapons)[i].ToInt());
	}

	return handles;
}

/**
 * @brief Removes all weapons from a client, with an option to remove the suit as well.
 *
 * @param playerSlot The index of the player's slot.
 * @param removeSuit A boolean indicating whether to also remove the client's suit.
 */
extern "C" PLUGIN_API void StripWeapons(int playerSlot, bool removeSuit) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'StripWeapons' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CCSPlayer_ItemServices* pItemServices = pController->GetCurrentPawn()->m_pItemServices;
	if (!pItemServices) {
		return;
	}

	pItemServices->StripPlayerWeapons(removeSuit);
}

/**
 * @brief Forces a player to drop their weapon.
 *
 * @param playerSlot The index of the player's slot.
 * @param weaponHandle The handle of weapon to drop.
 * @param target Target direction.
 * @param velocity Velocity to toss weapon or zero to just drop weapon.
 */
extern "C" PLUGIN_API void DropWeapon(int playerSlot, int weaponHandle, const plg::vec3& target, const plg::vec3& velocity) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'DropWeapon' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) weaponHandle)));
	if (!pWeapon) {
		S2_LOGF(LS_WARNING, "Cannot execute 'DropWeapon' on invalid weapon handle: {}\n", weaponHandle);
		return;
	}

	CCSPlayer_WeaponServices* pWeaponServices = pController->GetCurrentPawn()->m_pWeaponServices;
	if (!pWeaponServices) {
		return;
	}

	pWeaponServices->Drop(pWeapon, reinterpret_cast<Vector*>(const_cast<plg::vec3*>(&target)), reinterpret_cast<Vector*>(const_cast<plg::vec3*>(&velocity)));
}

/**
 * @brief Bumps a player's weapon.
 *
 * @param playerSlot The index of the player's slot.
 * @param weaponHandle The handle of weapon to bump.
 */
extern "C" PLUGIN_API void BumpWeapon(int playerSlot, int weaponHandle) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) weaponHandle)));
	if (!pWeapon) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid weapon handle: {}\n", weaponHandle);
		return;
	}

	CCSPlayer_WeaponServices* pWeaponServices = pController->GetCurrentPawn()->m_pWeaponServices;
	if (!pWeaponServices) {
		return;
	}

	pWeaponServices->Bump(pWeapon);
}

/**
 * @brief Switches a player's weapon.
 *
 * @param playerSlot The index of the player's slot.
 * @param weaponHandle The handle of weapon to switch.
 */
extern "C" PLUGIN_API void SwitchWeapon(int playerSlot, int weaponHandle) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) weaponHandle)));
	if (!pWeapon) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SwitchWeapon' on invalid weapon handle: {}\n", weaponHandle);
		return;
	}

	CCSPlayer_WeaponServices* pWeaponServices = pController->GetCurrentPawn()->m_pWeaponServices;
	if (!pWeaponServices) {
		return;
	}

	pWeaponServices->Switch(pWeapon);
}

/**
 * @brief Removes a player's weapon.
 *
 * @param playerSlot The index of the player's slot.
 * @param weaponHandle The handle of weapon to remove.
 */
extern "C" PLUGIN_API void RemoveWeapon(int playerSlot, int weaponHandle) {
	auto pController = utils::GetController(playerSlot);
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'RemovePlayerItem' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32) weaponHandle)));
	if (!pWeapon) {
		S2_LOGF(LS_WARNING, "Cannot execute 'RemovePlayerItem' on invalid weapon handle: {}\n", weaponHandle);
		return;
	}

	CCSPlayer_WeaponServices* pWeaponServices = pController->GetCurrentPawn()->m_pWeaponServices;
	if (!pWeaponServices) {
		return;
	}

	pWeaponServices->RemoveItem(pWeapon);
}

/**
 * @brief Gives a named item (e.g., weapon) to a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param itemName The name of the item to give.
 * @return The entity handle of the created item, or INVALID_EHANDLE_INDEX if the client or item is invalid.
 */
extern "C" PLUGIN_API int GiveNamedItem(int playerSlot, const plg::string& itemName) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GiveNamedItem' on invalid player slot: {}\n", playerSlot);
		return INVALID_EHANDLE_INDEX;
	}

	CCSPlayer_ItemServices* pItemServices = pController->GetCurrentPawn()->m_pItemServices;
	if (!pItemServices) {
		return INVALID_EHANDLE_INDEX;
	}

	CBaseEntity* pWeapon = pItemServices->GiveNamedItem(itemName.c_str());
	if (!pWeapon) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GiveNamedItem' with invalid item: {}\n", itemName);
		return INVALID_EHANDLE_INDEX;
	}

	return pWeapon->GetRefEHandle().ToInt();
}

/**
 * @brief Retrieves the state of a specific button for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param buttonIndex The index of the button (0-2).
 * @return uint64_t The state of the specified button, or 0 if the client or button index is invalid.
 */
extern "C" PLUGIN_API uint64_t GetClientButtons(int playerSlot, int buttonIndex) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientButtons' on invalid player slot: {}\n", playerSlot);
		return 0;
	}

	if (buttonIndex > 2 || buttonIndex < 0) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientButtons' on invalid button index: {}\n", buttonIndex);
		return 0;
	}

	CPlayer_MovementServices* pMovementServices = pController->GetCurrentPawn()->m_pMovementServices;
	if (!pMovementServices) {
		return 0;
	}

	return pMovementServices->m_nButtons().m_pButtonStates[buttonIndex];
}

/**
 * @brief Retrieves the amount of money a client has.
 *
 * @param playerSlot The index of the player's slot.
 * @return The amount of money the client has, or 0 if the player slot is invalid.
 */
extern "C" PLUGIN_API int GetClientMoney(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientMoney' on invalid player slot: {}\n", playerSlot);
		return 0;
	}

	CCSPlayerController_InGameMoneyServices* pInGameMoneyServices = pController->m_pInGameMoneyServices;
	if (!pInGameMoneyServices) {
		return 0;
	}

	return pInGameMoneyServices->m_iAccount;
}

/**
 * @brief Sets the amount of money for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param money The amount of money to set.
 */
extern "C" PLUGIN_API void SetClientMoney(int playerSlot, int money) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SetClientMoney' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CCSPlayerController_InGameMoneyServices* pInGameMoneyServices = pController->m_pInGameMoneyServices;
	if (!pInGameMoneyServices) {
		return;
	}

	pInGameMoneyServices->m_iAccount = money;
}

/**
 * @brief Retrieves the number of kills for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @return The number of kills the client has, or 0 if the player slot is invalid.
 */
extern "C" PLUGIN_API int GetClientKills(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientKills' on invalid player slot: {}\n", playerSlot);
		return 0;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iKills;
}

/**
 * @brief Sets the number of kills for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param kills The number of kills to set.
 */
extern "C" PLUGIN_API void SetClientKills(int playerSlot, int kills) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SetClientKills' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return;
	}

	pActionTrackingServices->m_matchStats().m_iKills = kills;
}

/**
 * @brief Retrieves the number of deaths for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @return The number of deaths the client has, or 0 if the player slot is invalid.
 */
extern "C" PLUGIN_API int GetClientDeaths(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientDeaths' on invalid player slot: {}\n", playerSlot);
		return 0;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iDeaths;
}

/**
 * @brief Sets the number of deaths for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param deaths The number of deaths to set.
 */
extern "C" PLUGIN_API void SetClientDeaths(int playerSlot, int deaths) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SetClientDeaths' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return;
	}

	pActionTrackingServices->m_matchStats().m_iDeaths = deaths;
}

/**
 * @brief Retrieves the number of assists for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @return The number of assists the client has, or 0 if the player slot is invalid.
 */
extern "C" PLUGIN_API int GetClientAssists(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientAssists' on invalid player slot: {}\n", playerSlot);
		return 0;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iAssists;
}

/**
 * @brief Sets the number of assists for a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param assists The number of assists to set.
 */
extern "C" PLUGIN_API void SetClientAssists(int playerSlot, int assists) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SetClientAssists' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return;
	}

	pActionTrackingServices->m_matchStats().m_iAssists = assists;
}

/**
 * @brief Retrieves the total damage dealt by a client.
 *
 * @param playerSlot The index of the player's slot.
 * @return The total damage dealt by the client, or 0 if the player slot is invalid.
 */
extern "C" PLUGIN_API int GetClientDamage(int playerSlot) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientDamage' on invalid player slot: {}\n", playerSlot);
		return 0;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return 0;
	}

	return pActionTrackingServices->m_matchStats().m_iDamage;
}

/**
 * @brief Sets the total damage dealt by a client.
 *
 * @param playerSlot The index of the player's slot.
 * @param damage The amount of damage to set.
 */
extern "C" PLUGIN_API void SetClientDamage(int playerSlot, int damage) {
	auto pController = static_cast<CPlayerController*>(utils::GetController(playerSlot));
	if (!pController) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SetClientDamage' on invalid player slot: {}\n", playerSlot);
		return;
	}

	CCSPlayerController_ActionTrackingServices* pActionTrackingServices = pController->m_pActionTrackingServices;
	if (!pActionTrackingServices) {
		return;
	}

	pActionTrackingServices->m_matchStats().m_iDamage = damage;
}
