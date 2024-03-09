#pragma once

extern "C" PLUGIN_API void SetClientListening(CBaseEntity* receiver, CBaseEntity* sender, int8_t listen)
{
	if (!receiver)
	{
		g_Logger.Warning("Receiver is a null pointer\n");
		return;
	}

	if (!sender)
	{
		g_Logger.Warning("Sender is a null pointer\n");
		return;
	}

	auto iSenderSlot = sender->GetEntityIndex().Get() - 1;
	if (iSenderSlot < 0 || iSenderSlot >= gpGlobals->maxClients)
	{
		g_Logger.Warning("Invalid sender\n");
		return;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(receiver->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return;
	}

	pPlayer->SetListen(iSenderSlot, static_cast<ListenOverride>(listen));
}

extern "C" PLUGIN_API int8_t GetClientListening(CBaseEntity* receiver, CBaseEntity* sender)
{
	if (!receiver)
	{
		g_Logger.Warning("Receiver is a null pointer\n");
		return Listen_Default;
	}

	if (!sender)
	{
		g_Logger.Warning("Sender is a null pointer\n");
		return Listen_Default;
	}

	auto iSenderSlot = sender->GetEntityIndex().Get() - 1;
	if (iSenderSlot < 0 || iSenderSlot >= gpGlobals->maxClients)
	{
		g_Logger.Warning("Invalid sender\n");
		return Listen_Default;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(receiver->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return Listen_Default;
	}

	return pPlayer->GetListen(iSenderSlot);
}

extern "C" PLUGIN_API void SetClientVoiceFlags(CBaseEntity* client, uint8_t flags)
{
	if (!client)
	{
		g_Logger.Warning("Receiver is a null pointer\n");
		return;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(client->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return;
	}

	pPlayer->SetVoiceFlags(flags);
}

extern "C" PLUGIN_API uint8_t GetClientVoiceFlags(CBaseEntity* client)
{
	if (!client)
	{
		g_Logger.Warning("Receiver is a null pointer\n");
		return 0;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(client->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return 0;
	}

	return pPlayer->GetVoiceFlags();
}