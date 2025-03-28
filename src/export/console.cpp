#include <core/sdk/utils.h>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Sends a message to the server console.
 * @param msg The message to be sent to the server console.
 */
extern "C" PLUGIN_API void PrintToServer(const plg::string& msg) {
	ConMsg("%s", msg.c_str());
}

/**
 * @brief Sends a message to a client's console.
 * @param playerSlot The index of the player's slot to whom the message will be sent.
 * @param message The message to be sent to the client's console.
 */
extern "C" PLUGIN_API void PrintToConsole(int playerSlot, const plg::string& message) {
	utils::PrintConsole(playerSlot, message.c_str());
}

/**
 * @brief Prints a message to a specific client in the chat area.
 * @param playerSlot The index of the player's slot to whom the message will be sent.
 * @param message The message to be printed in the chat area.
 */
extern "C" PLUGIN_API void PrintToChat(int playerSlot, const plg::string& message) {
	utils::PrintChat(playerSlot, message.c_str());
}

/**
 * @brief Prints a message to a specific client in the center of the screen.
 * @param playerSlot The index of the player's slot to whom the message will be sent.
 * @param message The message to be printed in the center of the screen.
 */
extern "C" PLUGIN_API void PrintCenterText(int playerSlot, const plg::string& message) {
	utils::PrintCentre(playerSlot, message.c_str());
}

/**
 * @brief Prints a message to a specific client with an alert box.
 * @param playerSlot The index of the player's slot to whom the message will be sent.
 * @param message The message to be printed in the alert box.
 */
extern "C" PLUGIN_API void PrintAlertText(int playerSlot, const plg::string& message) {
	utils::PrintAlert(playerSlot, message.c_str());
}

/**
 * @brief Prints a html message to a specific client in the center of the screen.
 * @param playerSlot The index of the player's slot to whom the message will be sent.
 * @param message The HTML-formatted message to be printed.
 */
extern "C" PLUGIN_API void PrintCentreHtml(int playerSlot, const plg::string& message) {
	utils::PrintHtmlCentre(playerSlot, message.c_str());
}

/**
 * @brief Sends a message to every client's console.
 * @param message The message to be sent to all clients' consoles.
 */
extern "C" PLUGIN_API void PrintToConsoleAll(const plg::string& message) {
	utils::PrintConsoleAll(message.c_str());
}

/**
 * @brief Prints a message to all clients in the chat area.
 * @param message The message to be printed in the chat area for all clients.
 */
extern "C" PLUGIN_API void PrintToChatAll(const plg::string& message) {
	utils::PrintChatAll(message.c_str());
}

/**
 * @brief Prints a message to all clients in the center of the screen.
 * @param message The message to be printed in the center of the screen for all clients.
 */
extern "C" PLUGIN_API void PrintCenterTextAll(const plg::string& message) {
	utils::PrintCentreAll(message.c_str());
}

/**
 * @brief Prints a message to all clients with an alert box.
 * @param message The message to be printed in an alert box for all clients.
 */
extern "C" PLUGIN_API void PrintAlertTextAll(const plg::string& message) {
	utils::PrintAlertAll(message.c_str());
}

/**
 * @brief Prints a html message to all clients in the center of the screen.
 * @param message The HTML-formatted message to be printed in the center of the screen for all clients.
 */
extern "C" PLUGIN_API void PrintCentreHtmlAll(const plg::string& message) {
	utils::PrintHtmlCentreAll(message.c_str());
}

/**
 * @brief Prints a colored message to a specific client in the chat area.
 * @param playerSlot The index of the player's slot to whom the message will be sent.
 * @param message The message to be printed in the chat area with color.
 */
extern "C" PLUGIN_API void PrintToChatColored(int playerSlot, const plg::string& message) {
	utils::CPrintChat(playerSlot, message.c_str());
}

/**
 * @brief Prints a colored message to all clients in the chat area.
 * @param message The colored message to be printed in the chat area for all clients.
 */
extern "C" PLUGIN_API void PrintToChatColoredAll(const plg::string& message) {
	utils::CPrintChatAll(message.c_str());
}
