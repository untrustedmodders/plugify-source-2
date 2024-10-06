#include <core/sdk/utils.h>
#include <plugin_export.h>

/**
 * @brief Sends a message to the server console.
 * @param msg The message to be sent to the server console.
 */
extern "C" PLUGIN_API void PrintToServer(const plg::string& msg)
{
	ConMsg("%s", msg.c_str());
}

/**
 * @brief Sends a message to a client's console.
 * @param clientIndex Index of the client to whom the message will be sent.
 * @param message The message to be sent to the client's console.
 */
extern "C" PLUGIN_API void PrintToConsole(int clientIndex, const plg::string& message)
{
	utils::PrintConsole(CPlayerSlot(clientIndex - 1), message.c_str());
}

/**
 * @brief Prints a message to a specific client in the chat area.
 * @param clientIndex Index of the client to whom the message will be sent.
 * @param message The message to be printed in the chat area.
 */
extern "C" PLUGIN_API void PrintToChat(int clientIndex, const plg::string& message)
{
	utils::PrintChat(CPlayerSlot(clientIndex - 1), message.c_str());
}

/**
 * @brief Prints a message to a specific client in the center of the screen.
 * @param clientIndex Index of the client to whom the message will be sent.
 * @param message The message to be printed in the center of the screen.
 */
extern "C" PLUGIN_API void PrintCenterText(int clientIndex, const plg::string& message)
{
	utils::PrintCentre(CPlayerSlot(clientIndex - 1), message.c_str());
}

/**
 * @brief Prints a message to a specific client with an alert box.
 * @param clientIndex Index of the client to whom the message will be sent.
 * @param message The message to be printed in the alert box.
 */
extern "C" PLUGIN_API void PrintAlertText(int clientIndex, const plg::string& message)
{
	utils::PrintAlert(CPlayerSlot(clientIndex - 1), message.c_str());
}

/**
 * @brief Prints a html message to a specific client in the center of the screen.
 * @param clientIndex Index of the client to whom the message will be sent.
 * @param message The HTML-formatted message to be printed.
 */
extern "C" PLUGIN_API void PrintCentreHtml(int clientIndex, const plg::string& message)
{
	utils::PrintHtmlCentre(CPlayerSlot(clientIndex - 1), message.c_str());
}

/**
 * @brief Sends a message to every client's console.
 * @param message The message to be sent to all clients' consoles.
 */
extern "C" PLUGIN_API void PrintToConsoleAll(const plg::string& message)
{
	utils::PrintConsoleAll(message.c_str());
}

/**
 * @brief Prints a message to all clients in the chat area.
 * @param message The message to be printed in the chat area for all clients.
 */
extern "C" PLUGIN_API void PrintToChatAll(const plg::string& message)
{
	utils::PrintChatAll(message.c_str());
}

/**
 * @brief Prints a message to all clients in the center of the screen.
 * @param message The message to be printed in the center of the screen for all clients.
 */
extern "C" PLUGIN_API void PrintCenterTextAll(const plg::string& message)
{
	utils::PrintCentreAll(message.c_str());
}

/**
 * @brief Prints a message to all clients with an alert box.
 * @param message The message to be printed in an alert box for all clients.
 */
extern "C" PLUGIN_API void PrintAlertTextToAll(const plg::string& message)
{
	utils::PrintAlertAll(message.c_str());
}

/**
 * @brief Prints a html message to all clients in the center of the screen.
 * @param message The HTML-formatted message to be printed in the center of the screen for all clients.
 */
extern "C" PLUGIN_API void PrintCentreHtmlAll(const plg::string& message)
{
	utils::PrintHtmlCentreAll(message.c_str());
}

/**
 * @brief Prints a colored message to a specific client in the chat area.
 * @param clientIndex Index of the client to whom the message will be sent.
 * @param message The message to be printed in the chat area with color.
 */
extern "C" PLUGIN_API void PrintToChatColored(int clientIndex, const plg::string& message)
{
	utils::CPrintChat(CPlayerSlot(clientIndex - 1), message.c_str());
}

/**
 * @brief Prints a colored message to all clients in the chat area.
 * @param message The colored message to be printed in the chat area for all clients.
 */
extern "C" PLUGIN_API void PrintToChatColoredAll(const plg::string& message)
{
	utils::CPrintChatAll(message.c_str());
}
