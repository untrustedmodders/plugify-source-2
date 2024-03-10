#pragma once

extern "C" PLUGIN_API void AddCommand(const std::string& name, const std::string& description, int64_t flags, CommandListenerCallback callback)
{
	g_Logger.MessageFormat("Adding command %s, %s, %d, %p\n", name.c_str(), description.c_str(), flags, (void*)callback);

	g_CommandManager.AddValveCommand(name, description, flags);
	g_CommandManager.AddCommandListener(name, callback, HookMode::Pre);
}

extern "C" PLUGIN_API void RemoveCommand(const std::string& name, CommandListenerCallback callback)
{
	g_CommandManager.RemoveCommandListener(name, callback, HookMode::Pre);
	g_CommandManager.RemoveValveCommand(name);
}

extern "C" PLUGIN_API void AddCommandListener(const std::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.AddCommandListener(name, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API void RemoveCommandListener(const std::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.RemoveCommandListener(name, callback, static_cast<HookMode>(post));
}

extern "C" PLUGIN_API void ServerCommand(const std::string& command)
{
	auto cleanCommand = command;
	cleanCommand.append("\n\0");
	g_pEngineServer2->ServerCommand(cleanCommand.c_str());
}

extern "C" PLUGIN_API void ClientCommand(int clientIndex, const std::string& command)
{
	auto cleanCommand = command;
	cleanCommand.append("\n\0");
	g_pEngineServer2->ClientCommand(clientIndex, cleanCommand.c_str());
}

extern "C" PLUGIN_API void ClientCommandFromServer(int clientIndex, const std::string& command)
{
	CCommand args;
	args.Tokenize(command.c_str(), CCommand::DefaultBreakSet());

	auto handle = g_pCVar->FindCommand(args.Arg(0));
	if (!handle.IsValid())
		return;

	CCommandContext context(CommandTarget_t::CT_NO_TARGET, clientIndex);

	g_pCVar->DispatchConCommand(handle, context, args);
}