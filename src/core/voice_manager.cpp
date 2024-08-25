#include "voice_manager.h"
#include "player_manager.h"

#include <core/sdk/schema.h>
#include <core/sdk/entity/cbaseentity.h>

dyno::ReturnAction CVoiceManager::Hook_SetClientListening(dyno::IHook& hook)
{
	// CPlayerSlot iReceiver, CPlayerSlot iSender, bool bListen
	auto iReceiver = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto iSender = (CPlayerSlot)dyno::GetArgument<int>(hook, 2);
	//auto bListen = dyno::GetArgument<bool>(hook, 3);

	auto pReceiver = g_PlayerManager.GetPlayerBySlot(iReceiver.Get());
	auto pSender = g_PlayerManager.GetPlayerBySlot(iSender.Get());

	if (pReceiver && pSender)
	{
		auto listenOverride = pReceiver->GetListen(iSender);
		auto senderFlags = pSender->GetVoiceFlags();
		auto receiverFlags = pReceiver->GetVoiceFlags();

		if (pReceiver->m_selfMutes.Get(iSender.Get()))
		{
			dyno::SetArgument<bool>(hook, 3, false);
			return dyno::ReturnAction::Handled;
		}

		if (senderFlags & Speak_Muted)
		{
			dyno::SetArgument<bool>(hook, 3, false);
			return dyno::ReturnAction::Handled;
		}

		if (listenOverride == Listen_Mute)
		{
			dyno::SetArgument<bool>(hook, 3, false);
			return dyno::ReturnAction::Handled;
		}
		else if (listenOverride == Listen_Hear)
		{
			dyno::SetArgument<bool>(hook, 3, true);
			return dyno::ReturnAction::Handled;
		}

		if ((senderFlags & Speak_All) || (receiverFlags & Speak_ListenAll))
		{
			dyno::SetArgument<bool>(hook, 3, true);
			return dyno::ReturnAction::Handled;
		}

		if ((senderFlags & Speak_Team) || (receiverFlags & Speak_ListenTeam))
		{
			CBaseEntity* receiverController = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(iReceiver.Get() + 1)));
			CBaseEntity* senderController = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(iSender.Get() + 1)));

			if (receiverController && senderController)
			{
				dyno::SetArgument<bool>(hook, 3, receiverController->m_iTeamNum() == senderController->m_iTeamNum());
				return dyno::ReturnAction::Handled;
			}
		}
	}

	return dyno::ReturnAction::Ignored;
}