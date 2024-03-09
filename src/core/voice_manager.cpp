#include "voice_manager.h"
#include "player_manager.h"

#include <entity2/entitysystem.h>
#include <utils/schema.h>

dyno::ReturnAction CVoiceManager::Hook_SetClientListening(dyno::IHook& hook)
{
	// CPlayerSlot iReceiver, CPlayerSlot iSender, bool bListen
	auto iReceiver = (CPlayerSlot)dyno::GetArgument<int>(hook, 1);
	auto iSender = (CPlayerSlot)dyno::GetArgument<int>(hook, 2);
	auto bListen = dyno::GetArgument<bool>(hook, 3);

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
			static auto classKey = hash_32_fnv1a_const("CBaseEntity");
			static auto memberKey = hash_32_fnv1a_const("m_iTeamNum");
			const static auto m_key = schema::GetOffset("CBaseEntity", classKey, "m_iTeamNum", memberKey);

			auto receiverController = g_pEntitySystem->GetBaseEntity(CEntityIndex(iReceiver.Get() + 1));
			auto senderController = g_pEntitySystem->GetBaseEntity(CEntityIndex(iSender.Get() + 1));

			if (receiverController && senderController)
			{
				auto receiverTeam = *reinterpret_cast<std::add_pointer_t<unsigned int>>((uintptr_t)(receiverController) + m_key.offset);
				auto senderTeam = *reinterpret_cast<std::add_pointer_t<unsigned int>>((uintptr_t)(senderController) + m_key.offset);

				dyno::SetArgument<bool>(hook, 3, receiverTeam == senderTeam);
				return dyno::ReturnAction::Handled;
			}
		}
	}

	return dyno::ReturnAction::Ignored;
}