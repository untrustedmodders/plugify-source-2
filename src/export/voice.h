#pragma once

extern "C" PLUGIN_API void SetClientListening(int receiver, int sender, int8_t listen)
{
	if (sender < 0 || sender >= gpGlobals->maxClients)
	{
		g_Logger.Warning("Invalid sender\n");
		return;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(receiver);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return;
	}

	pPlayer->SetListen(sender, static_cast<ListenOverride>(listen));
}

extern "C" PLUGIN_API int8_t GetClientListening(int receiver, int sender)
{
	if (sender < 0 || sender >= gpGlobals->maxClients)
	{
		g_Logger.Warning("Invalid sender\n");
		return Listen_Default;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(receiver);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return Listen_Default;
	}

	return pPlayer->GetListen(sender);
}

extern "C" PLUGIN_API void SetClientVoiceFlags(int clientIndex, uint8_t flags)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return;
	}

	pPlayer->SetVoiceFlags(flags);
}

extern "C" PLUGIN_API uint8_t GetClientVoiceFlags(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(clientIndex);
	if (pPlayer == nullptr)
	{
		g_Logger.Warning("Invalid receiver\n");
		return 0;
	}

	return pPlayer->GetVoiceFlags();
}