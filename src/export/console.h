#pragma once

enum class HudDestination : int
{
	Notify = 1,
	Console = 2,
	Chat = 3,
	Center = 4,
	Alert = 6
};

extern "C" PLUGIN_API void ClientPrint(CBasePlayerController* player, int hudDest, const std::string& msg)
{
	addresses::ClientPrint(player, hudDest, msg.c_str(), nullptr, nullptr, nullptr, nullptr);
}

extern "C" PLUGIN_API void ClientPrintAll(int hudDest, const std::string& msg)
{
	addresses::UTIL_ClientPrintAll(hudDest, msg.c_str(), nullptr, nullptr, nullptr, nullptr);
}

extern "C" PLUGIN_API void PrintToConsole(int clientIndex, const std::string& msg)
{
	// g_pEngineServer2->ClientPrintf(CPlayerSlot(clientIndex - 1), msg.c_str());
}

extern "C" PLUGIN_API void PrintToConsoleAll(const std::string& msg)
{
	// g_pEngineServer2->ClientPrintf(client, msg.c_str());
}

extern "C" PLUGIN_API void PrintToServer(const std::string& msg)
{
	ConMsg("%s", msg.c_str());
}