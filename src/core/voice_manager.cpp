#include "voice_manager.h"
#include "player_manager.h"

#include <core/sdk/schema.h>
#include <core/sdk/entity/cbaseentity.h>

poly::ReturnAction CVoiceManager::Hook_SetClientListening(poly::Params& params, int count, poly::Return& ret)
{
	// CPlayerSlot iReceiver, CPlayerSlot iSender, bool bListen
	auto iReceiver = (CPlayerSlot)poly::GetArgument<int>(params, 1);
	auto iSender = (CPlayerSlot)poly::GetArgument<int>(params, 2);
	//auto bListen = poly::GetArgument<bool>(params, 3);

	auto pReceiver = g_PlayerManager.GetPlayerBySlot(iReceiver.Get());
	auto pSender = g_PlayerManager.GetPlayerBySlot(iSender.Get());

	if (pReceiver && pSender)
	{
		auto listenOverride = pReceiver->GetListen(iSender);
		auto senderFlags = pSender->GetVoiceFlags();
		auto receiverFlags = pReceiver->GetVoiceFlags();

		if (pReceiver->m_selfMutes.Get(iSender.Get()))
		{
			poly::SetArgument<bool>(params, 3, false);
			return poly::ReturnAction::Handled;
		}

		if (senderFlags & Speak_Muted)
		{
			poly::SetArgument<bool>(params, 3, false);
			return poly::ReturnAction::Handled;
		}

		if (listenOverride == Listen_Mute)
		{
			poly::SetArgument<bool>(params, 3, false);
			return poly::ReturnAction::Handled;
		}

		if (listenOverride == Listen_Hear)
		{
			poly::SetArgument<bool>(params, 3, true);
			return poly::ReturnAction::Handled;
		}

		if ((senderFlags & Speak_All) || (receiverFlags & Speak_ListenAll))
		{
			poly::SetArgument<bool>(params, 3, true);
			return poly::ReturnAction::Handled;
		}

		if ((senderFlags & Speak_Team) || (receiverFlags & Speak_ListenTeam))
		{
			CBaseEntity* receiverController = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(iReceiver.Get() + 1)));
			CBaseEntity* senderController = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(iSender.Get() + 1)));

			if (receiverController && senderController)
			{
				poly::SetArgument<bool>(params, 3, receiverController->m_iTeamNum() == senderController->m_iTeamNum());
				return poly::ReturnAction::Handled;
			}
		}
	}

	return poly::ReturnAction::Ignored;
}