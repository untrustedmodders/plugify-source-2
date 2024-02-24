#pragma once

enum HudDestination
{
	Notify = 1,
	Console = 2,
	Chat = 3,
	Center = 4,
	Alert = 6
};

extern "C" PLUGIN_API void ClientPrint(CBasePlayerController* player, HudDestination hud_dest, const std::string& msg)
{
	addresses::ClientPrint(player, hud_dest, msg.c_str(), nullptr, nullptr, nullptr, nullptr);
}

extern "C" PLUGIN_API void ClientPrintAll(HudDestination hud_dest, const std::string& msg)
{
	addresses::UTIL_ClientPrintAll(hud_dest, msg.c_str(), nullptr, nullptr, nullptr, nullptr);
}