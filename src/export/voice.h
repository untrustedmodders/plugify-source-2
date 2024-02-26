#pragma once

extern "C" PLUGIN_API void SetClientListening(CBaseEntity* receiver, CBaseEntity* sender, ListenOverride listen)
{
	if (!receiver)
	{
		g_Logger.Error("Receiver is a null pointer");
		return;
	}

	if (!sender)
	{
		g_Logger.Error("Sender is a null pointer");
		return;
	}

	auto iSenderSlot = sender->GetEntityIndex().Get() - 1;
	if (iSenderSlot < 0 || iSenderSlot >= gpGlobals->maxClients)
	{
		g_Logger.Error("Invalid sender");
		return;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(receiver->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Error("Invalid receiver");
		return;
	}

	pPlayer->SetListen(iSenderSlot, listen);
}

extern "C" PLUGIN_API ListenOverride GetClientListening(CBaseEntity* receiver, CBaseEntity* sender)
{
	if (!receiver)
	{
		g_Logger.Error("Receiver is a null pointer");
		return Listen_Default;
	}

	if (!sender)
	{
		g_Logger.Error("Sender is a null pointer");
		return Listen_Default;
	}

	auto iSenderSlot = sender->GetEntityIndex().Get() - 1;
	if (iSenderSlot < 0 || iSenderSlot >= gpGlobals->maxClients)
	{
		g_Logger.Error("Invalid sender");
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(receiver->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Error("Invalid receiver");
		return Listen_Default;
	}

	return pPlayer->GetListen(iSenderSlot);
}

extern "C" PLUGIN_API void SetClientVoiceFlags(CBaseEntity* client, VoiceFlag_t flags)
{
	if (!client)
	{
		g_Logger.Error("Receiver is a null pointer");
		return;
	}
	
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(client->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Error("Invalid receiver");
		return;
	}

	pPlayer->SetVoiceFlags(flags);
}

extern "C" PLUGIN_API VoiceFlag_t GetClientVoiceFlags(CBaseEntity* client)
{
	if (!client)
	{
		g_Logger.Error("Receiver is a null pointer");
		return {};
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(client->GetEntityIndex().Get() - 1);
	if (pPlayer == nullptr)
	{
		g_Logger.Error("Invalid receiver");
		return {};
	}

	return pPlayer->GetVoiceFlags();
}