#include "con_command_manager.h"

#include <icvar.h>

void CommandCallback(const CCommandContext& context, const CCommand& command)
{
}

ConCommandInfo::ConCommandInfo(std::string name, std::string description) : name(std::move(name)), description(std::move(description)), commandRef()
{
}

void ConCommandManager::AddCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode)
{
	if (name.empty())
	{
		if (mode == HookMode::Pre)
		{
			m_globalCmd.callbackPre.Register(callback);
		}
		else
		{
			m_globalCmd.callbackPost.Register(callback);
		}
		return;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end())
	{
		auto& commandInfo = *m_cmdLookup.emplace(name, std::make_unique<ConCommandInfo>(name)).first->second;

		ConCommandHandle hExistingCommand = g_pCVar->FindCommand(name.c_str());
		if (hExistingCommand.IsValid())
		{
			commandInfo.command = g_pCVar->GetCommand(hExistingCommand);
		}

		if (mode == HookMode::Pre)
		{
			commandInfo.callbackPre.Unregister(callback);
		}
		else
		{
			commandInfo.callbackPost.Unregister(callback);
		}
	}
	else
	{
		auto& commandInfo = *std::get<CommandInfo>(*it);
		if (mode == HookMode::Pre)
		{
			commandInfo.callbackPre.Unregister(callback);
		}
		else
		{
			commandInfo.callbackPost.Unregister(callback);
		}
	}
}

void ConCommandManager::RemoveCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode)
{
	if (name.empty())
	{
		if (mode == HookMode::Pre)
		{
			m_globalCmd.callbackPre.Unregister(callback);
		}
		else
		{
			m_globalCmd.callbackPost.Unregister(callback);
		}
		return;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end())
	{
		return;
	}

	auto& commandInfo = *std::get<CommandInfo>(*it);
	if (mode == HookMode::Pre)
	{
		commandInfo.callbackPre.Unregister(callback);
	}
	else
	{
		commandInfo.callbackPost.Unregister(callback);
	}
}

bool ConCommandManager::AddValveCommand(const std::string& name, const std::string& description, int64 flags)
{
	ConCommandHandle hExistingCommand = g_pCVar->FindCommand(name.c_str());
	if (hExistingCommand.IsValid())
	{
		return false;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end())
	{
		auto& commandInfo = *m_cmdLookup.emplace(name, std::make_unique<ConCommandInfo>(name, description)).first->second;
		commandInfo.commandStorage = std::make_unique<ConCommand>(&commandInfo.commandRef, commandInfo.name.c_str(), CommandCallback, commandInfo.description.c_str(), flags);
		commandInfo.command = commandInfo.commandStorage.get();
	}
	else
	{
		auto& commandInfo = *std::get<CommandInfo>(*it);
		commandInfo.name = name;
		commandInfo.description = description;
		commandInfo.commandRef = {};
		commandInfo.commandStorage = std::make_unique<ConCommand>(&commandInfo.commandRef, commandInfo.name.c_str(), CommandCallback, commandInfo.description.c_str(), flags);
		commandInfo.command = commandInfo.commandStorage.get();
	}

	return true;
}

bool ConCommandManager::RemoveValveCommand(const std::string& name)
{
	auto hFoundCommand = g_pCVar->FindCommand(name.c_str());
	if (!hFoundCommand.IsValid())
	{
		return false;
	}

	g_pCVar->UnregisterConCommand(hFoundCommand);

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end())
	{
		return true;
	}

	auto& commandInfo = std::get<CommandInfo>(*it);
	commandInfo->commandStorage = nullptr;
	commandInfo->command = nullptr;

	return true;
}

bool ConCommandManager::IsValidValveCommand(const std::string& name)
{
	ConCommandHandle pCmd = g_pCVar->FindCommand(name.c_str());
	return pCmd.IsValid();
}

ResultType ConCommandManager::ExecuteCommandCallbacks(const std::string& name, const CCommandContext& ctx,  const CCommand& args, HookMode mode, CommandCallingContext callingContext)
{
	g_Logger.MessageFormat("[ConCommandManager::ExecuteCommandCallbacks][%s]: %s", mode == HookMode::Pre ? "Pre" : "Post", name.c_str());

	ResultType result = ResultType::Continue;

	auto globalCallback = mode == HookMode::Pre ? m_globalCmd.callbackPre : m_globalCmd.callbackPost;

	int size = args.ArgC();

	std::vector<std::string> arguments;
	arguments.reserve((size_t)size);
	for (int i = 0; i < size; ++i)
	{
		arguments.emplace_back(args.Arg(i));
	}

	for (size_t i = 0; i < globalCallback.GetCount(); ++i)
	{
		auto thisResult = globalCallback.Notify(i, ctx.GetPlayerSlot().Get(), name, arguments, callingContext);
		if (thisResult >= ResultType::Stop)
		{
			if (mode == HookMode::Pre)
			{
				return ResultType::Stop;
			}

			result = thisResult;
			break;
		}

		if (thisResult >= ResultType::Handled)
		{
			result = thisResult;
		}
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end())
	{
		return result;
	}

	auto& commandInfo = *std::get<CommandInfo>(*it);
	auto callback = mode == HookMode::Pre ? commandInfo.callbackPre : commandInfo.callbackPost;

	for (size_t i = 0; i < callback.GetCount(); ++i)
	{
		auto thisResult = globalCallback.Notify(i, ctx.GetPlayerSlot().Get(), name, arguments, callingContext);
		if (thisResult >= ResultType::Handled)
		{
			return thisResult;
		}
		else if (thisResult > result)
		{
			result = thisResult;
		}
	}

	return result;
}

dyno::ReturnAction ConCommandManager::Hook_DispatchConCommand(dyno::IHook& hook)
{
	//auto cmd = dyno::GetArgument<ConCommandHandle* const>(hook, 1);
	auto ctx = dyno::GetArgument<const CCommandContext&>(hook, 2);
	auto args = dyno::GetArgument<const CCommand&>(hook, 3);

	const char* name = args.Arg(0);

	g_Logger.MessageFormat("[ConCommandManager::Hook_DispatchConCommand]: %s", name);

	auto result = ExecuteCommandCallbacks(name, ctx, args, HookMode::Pre, CommandCallingContext::Console);
	if (result >= ResultType::Handled)
	{
		return dyno::ReturnAction::Supercede;
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction ConCommandManager::Hook_DispatchConCommand_Post(dyno::IHook& hook)
{
	//auto cmd = dyno::GetArgument<ConCommandHandle* const>(hook, 1);
	auto ctx = dyno::GetArgument<const CCommandContext&>(hook, 2);
	auto args = dyno::GetArgument<const CCommand&>(hook, 3);

	const char* name = args.Arg(0);

	auto result = ExecuteCommandCallbacks(name, ctx, args, HookMode::Post, CommandCallingContext::Console);
	if (result >= ResultType::Handled)
	{
		return dyno::ReturnAction::Supercede;
	}

	return dyno::ReturnAction::Ignored;
}

ConCommandManager g_CommandManager;