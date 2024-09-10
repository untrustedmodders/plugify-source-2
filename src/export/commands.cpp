#include <core/con_command_manager.h>
#include <plugin_export.h>

extern "C" PLUGIN_API void AddAdminCommand(const plg::string& name, int64_t adminFlags, const plg::string& description, int64_t flags, CommandListenerCallback callback)
{
	g_Logger.LogFormat(LS_DEBUG, "Adding admin command %s, %d, %s, %d, %p\n", name.c_str(), (int)adminFlags, description.c_str(), (int)flags, (void*)callback);

	g_CommandManager.AddValveCommand(name, description, flags, adminFlags);
	g_CommandManager.AddCommandListener(name, callback, HookMode::Pre);
}

extern "C" PLUGIN_API void AddConsoleCommand(const plg::string& name, const plg::string& description, int64_t flags, CommandListenerCallback callback)
{
	g_Logger.LogFormat(LS_DEBUG, "Adding command %s, %s, %d, %p\n", name.c_str(), description.c_str(), (int)flags, (void*)callback);

	g_CommandManager.AddValveCommand(name, description, flags);
	g_CommandManager.AddCommandListener(name, callback, HookMode::Pre);
}

extern "C" PLUGIN_API void RemoveCommand(const plg::string& name, CommandListenerCallback callback)
{
	g_CommandManager.RemoveCommandListener(name, callback, HookMode::Pre);
	g_CommandManager.RemoveValveCommand(name);
}

extern "C" PLUGIN_API void AddCommandListener(const plg::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.AddCommandListener(name, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API void RemoveCommandListener(const plg::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.RemoveCommandListener(name, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API void ServerCommand(const plg::string& command)
{
	auto cleanCommand = command;
	cleanCommand.append("\n\0");
	g_pEngineServer2->ServerCommand(cleanCommand.c_str());
}

extern "C" PLUGIN_API void ClientCommand(int clientIndex, const plg::string& command)
{
	auto cleanCommand = command;
	cleanCommand.append("\n\0");
	g_pEngineServer2->ClientCommand(CPlayerSlot(clientIndex - 1), "%s", cleanCommand.c_str());
}

extern "C" PLUGIN_API void ClientCommandFromServer(int clientIndex, const plg::string& command)
{
	CCommand args;
	args.Tokenize(command.c_str(), CCommand::DefaultBreakSet());

	auto handle = g_pCVar->FindCommand(args.Arg(0));
	if (!handle.IsValid())
		return;

	CCommandContext context(CommandTarget_t::CT_NO_TARGET, CPlayerSlot(clientIndex - 1));

	g_pCVar->DispatchConCommand(handle, context, args);
}