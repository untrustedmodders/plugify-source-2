#include <plugin_export.h>
#include <core/player_manager.h>

extern "C" PLUGIN_API void SetClientListening(int receiverIndex, int senderIndex, int8_t listen)
{
	if (senderIndex < 0 || senderIndex >= gpGlobals->maxClients)
	{
		g_Logger.Log(LS_WARNING, "Invalid sender\n");
		return;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(receiverIndex - 1));
	if (pPlayer == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return;
	}

	pPlayer->SetListen(CPlayerSlot(senderIndex - 1), static_cast<ListenOverride>(listen));
}

extern "C" PLUGIN_API int8_t GetClientListening(int receiverIndex, int senderIndex)
{
	if (senderIndex < 0 || senderIndex >= gpGlobals->maxClients)
	{
		g_Logger.Log(LS_WARNING, "Invalid sender\n");
		return Listen_Default;
	}

	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(receiverIndex - 1));
	if (pPlayer == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return Listen_Default;
	}

	return pPlayer->GetListen(CPlayerSlot(senderIndex - 1));
}

extern "C" PLUGIN_API void SetClientVoiceFlags(int clientIndex, uint8_t flags)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return;
	}

	pPlayer->SetVoiceFlags(flags);
}

extern "C" PLUGIN_API uint8_t GetClientVoiceFlags(int clientIndex)
{
	auto pPlayer = g_PlayerManager.GetPlayerBySlot(CPlayerSlot(clientIndex - 1));
	if (pPlayer == nullptr)
	{
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return 0;
	}

	return pPlayer->GetVoiceFlags();
}