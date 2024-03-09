#pragma once

#include <core/sdk/entity/cbaseentity.h>

extern "C" PLUGIN_API void PrintServer(const std::string& msg)
{
	ConMsg("%s", msg.c_str());
}

extern "C" PLUGIN_API void PrintConsole(int clientIndex, const std::string& message)
{
	utils::PrintConsole(CPlayerSlot(clientIndex), message.c_str());
}

extern "C" PLUGIN_API void PrintChat(int clientIndex, const std::string& message)
{
	utils::PrintChat(CPlayerSlot(clientIndex), message.c_str());
}

extern "C" PLUGIN_API void PrintCentre(int clientIndex, const std::string& message)
{
	utils::PrintCentre(CPlayerSlot(clientIndex), message.c_str());
}

extern "C" PLUGIN_API void PrintAlert(int clientIndex, const std::string& message)
{
	utils::PrintAlert(CPlayerSlot(clientIndex), message.c_str());
}

extern "C" PLUGIN_API void PrintHTMLCentre(int clientIndex, const std::string& message)
{
	utils::PrintHTMLCentre(CPlayerSlot(clientIndex), message.c_str());
}

extern "C" PLUGIN_API void PrintConsoleAll(const std::string& message)
{
	utils::PrintConsoleAll(message.c_str());
}

extern "C" PLUGIN_API void PrintChatAll(const std::string& message)
{
	utils::PrintChatAll(message.c_str());
}

extern "C" PLUGIN_API void PrintCentreAll(const std::string& message)
{
	utils::PrintCentreAll(message.c_str());
}

extern "C" PLUGIN_API void PrintAlertAll(const std::string& message)
{
	utils::PrintAlertAll(message.c_str());
}

extern "C" PLUGIN_API void PrintHTMLCentreAll(const std::string& message)
{
	utils::PrintHTMLCentreAll(message.c_str());
}

extern "C" PLUGIN_API void PrintChatColored(int clientIndex, const std::string& message)
{
	utils::CPrintChat(CPlayerSlot(clientIndex), message.c_str());
}

extern "C" PLUGIN_API void PrintChatColoredAll(const std::string& message)
{
	utils::CPrintChatAll(message.c_str());
}
