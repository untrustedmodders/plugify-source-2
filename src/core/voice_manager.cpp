#include "voice_manager.hpp"
#include "player_manager.hpp"

#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/schema.h>

poly::ReturnAction CVoiceManager::Hook_SetClientListening(poly::Params& params, int count, poly::Return& ret) {
	// CPlayerSlot iReceiver, CPlayerSlot iSender, bool bListen
	auto iReceiver = (CPlayerSlot) poly::GetArgument<int>(params, 1);
	auto iSender = (CPlayerSlot) poly::GetArgument<int>(params, 2);
	//auto bListen = poly::GetArgument<bool>(params, 3);

	auto pReceiver = g_PlayerManager.ToPlayer(iReceiver);
	auto pSender = g_PlayerManager.ToPlayer(iSender);

	if (pReceiver && pSender) {
		auto listenOverride = pReceiver->GetListen(iSender);
		auto senderFlags = pSender->GetVoiceFlags();
		auto receiverFlags = pReceiver->GetVoiceFlags();

		if (pReceiver->GetSelfMutes().test(iSender.Get())) {
			poly::SetArgument<bool>(params, 3, false);
			return poly::ReturnAction::Handled;
		}

		if (senderFlags & Speak_Muted) {
			poly::SetArgument<bool>(params, 3, false);
			return poly::ReturnAction::Handled;
		}

		if (listenOverride == Listen_Mute) {
			poly::SetArgument<bool>(params, 3, false);
			return poly::ReturnAction::Handled;
		}

		if (listenOverride == Listen_Hear) {
			poly::SetArgument<bool>(params, 3, true);
			return poly::ReturnAction::Handled;
		}

		if ((senderFlags & Speak_All) || (receiverFlags & Speak_ListenAll)) {
			poly::SetArgument<bool>(params, 3, true);
			return poly::ReturnAction::Handled;
		}

		if ((senderFlags & Speak_Team) || (receiverFlags & Speak_ListenTeam)) {
			CBaseEntity* pReceiverController = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityIndex(iReceiver.Get() + 1)));
			CBaseEntity* pSenderController = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityIndex(iSender.Get() + 1)));

			if (pReceiverController && pSenderController) {
				poly::SetArgument<bool>(params, 3, pReceiverController->m_iTeamNum() == pSenderController->m_iTeamNum());
				return poly::ReturnAction::Handled;
			}
		}
	}

	return poly::ReturnAction::Ignored;
}

CON_COMMAND_F(vban, "voice ban", FCVAR_SPONLY | FCVAR_LINKED_CONCOMMAND) {
	auto pPlayer = g_PlayerManager.ToPlayer(context.GetPlayerSlot());
	if (!pPlayer) {
		return;
	}

	if (args.ArgC() > 1 && V_stricmp(args.Arg(0), "vban") == 0) {
		pPlayer->GetSelfMutes().set(std::stoi(args.Arg(1)));
	}
}