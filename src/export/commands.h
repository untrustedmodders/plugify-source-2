#pragma once

extern "C" PLUGIN_API void AddCommand(const std::string& name, const std::string& description, int64 flags, CommandListenerCallback callback)
{
	g_Logger.MessageFormat("Adding command %s, %s, %d, %p", name.c_str(), description.c_str(), flags, (void*)callback);

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
	g_CommandManager.AddCommandListener(name, callback, post ? HookMode::Post : HookMode::Pre);
}

extern "C" PLUGIN_API void RemoveCommandListener(const std::string& name, CommandListenerCallback callback, bool post)
{
	g_CommandManager.RemoveCommandListener(name, callback, post ? HookMode::Post : HookMode::Pre);
}

/*extern "C" PLUGIN_API int CommandGetArgCount(CCommand* command)
{
	if (!command) {
		g_Logger.Error("Invalid command.");
		return -1;
	}

	return command->ArgC();
}

extern "C" PLUGIN_API void CommandGetArgString(std::string& output, CCommand* command)
{
	if (!command) {
		g_Logger.Error("Invalid command.");
		return;
	}

	output = command->ArgS();
}

extern "C" PLUGIN_API void CommandGetCommandString(std::string& output, CCommand* command)
{
	if (!command) {
		g_Logger.Error("Invalid command.");
		return;
	}

	output = command->GetCommandString();
}

extern "C" PLUGIN_API void CommandGetArgByIndex(std::string& output, CCommand* command, int index)
{
	if (!command) {
		g_Logger.Error("Invalid command.");
		return;
	}

	output = command->Arg(index);
}

extern "C" PLUGIN_API CommandCallingContext CommandGetCallingContext(CCommand* command)
{
	return g_CommandManager.GetCommandCallingContext(command);
}*/

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
	g_pEngineServer2->ClientCommand(CPlayerSlot(clientIndex - 1), cleanCommand.c_str());
}

extern "C" PLUGIN_API void ClientCommandFromServer(int slot, const std::string& command)
{
	CCommand args;
	args.Tokenize(command.c_str());

	auto handle = g_pCVar->FindCommand(args.Arg(0));
	if (!handle.IsValid())
		return;

	CCommandContext context(CommandTarget_t::CT_NO_TARGET, CPlayerSlot(slot));

	g_pCVar->DispatchConCommand(handle, context, args);
}

// TODO: Implement cvars

extern "C" PLUGIN_API void GetClientConVarValue(std::string& output, int playerSlot, const std::string& convarName)
{
	output = g_pEngineServer2->GetClientConVarValue(CPlayerSlot(playerSlot), convarName.c_str());
}

extern "C" PLUGIN_API void SetFakeClientConVarValue(int playerSlot, const std::string& convarName, const std::string& convarValue)
{
	g_pEngineServer2->SetFakeClientConVarValue(CPlayerSlot(playerSlot), convarName.c_str(), convarValue.c_str());
}

extern "C" PLUGIN_API ConVar* FindConVar(const std::string& name)
{
	auto hCvarHandle = g_pCVar->FindConVar(name.c_str(), true);
	if (!hCvarHandle.IsValid())
	{
		return nullptr;
	}

	return g_pCVar->GetConVar(hCvarHandle);
}

extern "C" PLUGIN_API void SetConVarStringValue(ConVar* pCvar, const std::string& value)
{
	if (!pCvar)
	{
		g_Logger.Error("Invalid cvar.");
		return;
	}

	// TODO: Seems leak
	pCvar->values = reinterpret_cast<CVValue_t**>(strdup(value.c_str()));
}