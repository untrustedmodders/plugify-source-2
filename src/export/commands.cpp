#include <core/con_command_manager.hpp>
#include <plugin_export.h>

/**
 * @brief Creates a console command as an administrative command.
 * If the command does not exist, it is created. When this command is invoked,
 * the access rights of the player are automatically checked before allowing it to continue.
 *
 * @param name The name of the console command.
 * @param adminFlags The admin flags that indicate which admin level can use this command.
 * @param description A brief description of what the command does.
 * @param flags Command flags that define the behavior of the command.
 * @param callback A callback function that is invoked when the command is executed.
 */
extern "C" PLUGIN_API void AddAdminCommand(const plg::string& name, int64_t adminFlags, const plg::string& description, int64_t flags, CommandListenerCallback callback)
{
	g_Logger.LogFormat(LS_DEBUG, "Adding admin command %s, %d, %s, %d, %p\n", name.c_str(), (int)adminFlags, description.c_str(), (int)flags, (void*)callback);

	g_CommandManager.AddValveCommand(name, description, flags, adminFlags);
	g_CommandManager.AddCommandListener(name, callback, HookMode::Pre);
}

/**
 * @brief Creates a console command or hooks an already existing one.
 *
 * @param name The name of the console command.
 * @param description A brief description of what the command does.
 * @param flags Command flags that define the behavior of the command.
 * @param callback A callback function that is invoked when the command is executed.
 */
extern "C" PLUGIN_API void AddConsoleCommand(const plg::string& name, const plg::string& description, int64_t flags, CommandListenerCallback callback)
{
	g_Logger.LogFormat(LS_DEBUG, "Adding command %s, %s, %d, %p\n", name.c_str(), description.c_str(), (int)flags, (void*)callback);

	g_CommandManager.AddValveCommand(name, description, flags);
	g_CommandManager.AddCommandListener(name, callback, HookMode::Pre);
}

/**
 * @brief Removes a console command from the system.
 *
 * @param name The name of the command to be removed.
 * @param callback The callback function associated with the command to be removed.
 */
extern "C" PLUGIN_API void RemoveCommand(const plg::string& name, CommandListenerCallback callback)
{
	g_CommandManager.RemoveCommandListener(name, callback, HookMode::Pre);
	g_CommandManager.RemoveValveCommand(name);
}

/**
 * @brief Adds a callback that will fire when a command is sent to the server.
 *
 * @param name The name of the command.
 * @param callback The callback function that will be invoked when the command is executed.
 * @param post A boolean indicating whether the callback should fire after the command is executed.
 */
extern "C" PLUGIN_API void AddCommandListener(const plg::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.AddCommandListener(name, callback, static_cast<HookMode>(post));
}

/**
 * @brief Removes a callback that fires when a command is sent to the server.
 *
 * @param name The name of the command.
 * @param callback The callback function to be removed.
 * @param post A boolean indicating whether the callback should be removed for post-execution.
 */
extern "C" PLUGIN_API void RemoveCommandListener(const plg::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.RemoveCommandListener(name, callback, static_cast<HookMode>(post));
}

/**
 * @brief Executes a server command as if it were run on the server console or through RCON.
 *
 * @param command The command to execute on the server.
 */
extern "C" PLUGIN_API void ServerCommand(const plg::string& command)
{
	auto cleanCommand = command;
	cleanCommand.append("\n\0");
	g_pEngineServer2->ServerCommand(cleanCommand.c_str());
}

/**
 * @brief Executes a client command.
 * This will only work if clients have `cl_restrict_server_commands` set to 0.
 *
 * @param clientIndex The index of the client executing the command.
 * @param command The command to execute on the client.
 */
extern "C" PLUGIN_API void ClientCommand(int clientIndex, const plg::string& command)
{
	auto cleanCommand = command;
	cleanCommand.append("\n\0");
	g_pEngineServer2->ClientCommand(CPlayerSlot(clientIndex), "%s", cleanCommand.c_str());
}

/**
 * @brief Executes a client command on the server without network communication.
 *
 * @param clientIndex The index of the client.
 * @param command The command to be executed by the client.
 */
extern "C" PLUGIN_API void FakeClientCommand(int clientIndex, const plg::string& command)
{
	CCommand args;
	args.Tokenize(command.c_str(), CCommand::DefaultBreakSet());

	auto handle = g_pCVar->FindCommand(args.Arg(0));
	if (!handle.IsValid())
		return;

	CCommandContext context(CommandTarget_t::CT_NO_TARGET, CPlayerSlot(clientIndex));

	g_pCVar->DispatchConCommand(handle, context, args);
}
