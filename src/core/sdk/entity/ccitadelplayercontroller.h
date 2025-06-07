
#pragma once

#include "cbaseplayercontroller.h"
#include "ccitadelplayerpawn.h"
#include <sharedInterface.h>

class CCitadelPlayerController : public CBasePlayerController {
public:
	DECLARE_SCHEMA_CLASS(CCitadelPlayerController);

	SCHEMA_FIELD(CHandle<CCitadelPlayerPawn>, m_hHeroPawn)

	CCSPlayerController_InGameMoneyServices* m_pInGameMoneyServices() {
		return nullptr;
	}
	CCSPlayerController_ActionTrackingServices* m_pActionTrackingServices() {
		return nullptr;
	}

	// Returns the actual player pawn
	CCitadelPlayerPawn* GetPlayerPawn() {
		return m_hHeroPawn();
	}

	CCitadelPlayerPawn* GetObserverPawn() {
		return nullptr;
	}

	/*CServerSideClient* GetServerSideClient() {
		return utils::GetClientBySlot(GetPlayerSlot());
	}*/

	bool IsBot() {
		return m_fFlags() & FL_CONTROLLER_FAKECLIENT;
	}

	void ChangeTeam(int iTeam) {
		// TODO:
	}

	void SwitchTeam(int iTeam) {
		// TODO:
	}

	// Respawns the player if the player is not alive, does nothing otherwise.
	void Respawn() {
		// TODO:
	}
};


