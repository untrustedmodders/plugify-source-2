#include "con_command_manager.h"

#include <icvar.h>

void CommandCallback(const CCommandContext& context, const CCommand& command)
{
}

ConCommandInfo::ConCommandInfo(std::string name, std::string description) : name(std::move(name)), description(std::move(description))
{
}

void CConCommandManager::AddCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode)
{
	if (name.empty())
	{
		if (mode == HookMode::Pre)
		{
			m_globalPre.Register(callback);
		}
		else
		{
			m_globalPost.Register(callback);
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
			commandInfo.callbackPre.Register(callback);
		}
		else
		{
			commandInfo.callbackPost.Register(callback);
		}
	}
	else
	{
		auto& commandInfo = *std::get<CommandInfoPtr>(*it);
		if (mode == HookMode::Pre)
		{
			commandInfo.callbackPre.Register(callback);
		}
		else
		{
			commandInfo.callbackPost.Register(callback);
		}
	}
}

void CConCommandManager::RemoveCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode)
{
	if (name.empty())
	{
		if (mode == HookMode::Pre)
		{
			m_globalPre.Unregister(callback);
		}
		else
		{
			m_globalPre.Unregister(callback);
		}
		return;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end())
	{
		return;
	}

	auto& commandInfo = *std::get<CommandInfoPtr>(*it);
	if (mode == HookMode::Pre)
	{
		commandInfo.callbackPre.Unregister(callback);
	}
	else
	{
		commandInfo.callbackPost.Unregister(callback);
	}
}

bool CConCommandManager::AddValveCommand(const std::string& name, const std::string& description, int64 flags)
{
	if (name.empty() || g_pCVar->FindConVar(name.c_str()).IsValid())
	{
		return false;
	}

	if (g_pCVar->FindCommand(name.c_str()).IsValid())
	{
		return false;
	}

	auto it = m_cmdLookup.find(name);
	if (it != m_cmdLookup.end())
	{
		return false;
	}

	auto& commandInfo = *m_cmdLookup.emplace(name, std::make_unique<ConCommandInfo>(name, description)).first->second;
	commandInfo.commandRef = std::make_unique<ConCommand>(commandInfo.name.c_str(), CommandCallback, commandInfo.description.c_str(), flags);
	commandInfo.command = commandInfo.commandRef.get();

	return true;
}

bool CConCommandManager::RemoveValveCommand(const std::string& name)
{
	auto hFoundCommand = g_pCVar->FindCommand(name.c_str());
	if (!hFoundCommand.IsValid())
	{
		return false;
	}

	auto it = m_cmdLookup.find(name);
	if (it != m_cmdLookup.end())
	{
		m_cmdLookup.erase(it);
		return true;
	}
	else
	{
		g_pCVar->UnregisterConCommand(hFoundCommand);
	}

	return true;
}

bool CConCommandManager::IsValidValveCommand(const std::string& name) const
{
	ConCommandHandle hFoundCommand = g_pCVar->FindCommand(name.c_str());
	return hFoundCommand.IsValid();
}

ResultType CConCommandManager::ExecuteCommandCallbacks(const std::string& name, const CCommandContext& ctx, const CCommand& args, HookMode mode, CommandCallingContext callingContext)
{
	g_Logger.LogFormat(LS_DEBUG, "[ConCommandManager::ExecuteCommandCallbacks][%s]: %s\n", mode == HookMode::Pre ? "Pre" : "Post", name.c_str());

	ResultType result = ResultType::Continue;

	auto globalCallback = mode == HookMode::Pre ? m_globalPre : m_globalPost;

	int size = args.ArgC();

	std::vector<std::string> arguments;
	arguments.reserve(static_cast<size_t>(size));
	for (int i = 0; i < size; ++i)
	{
		arguments.emplace_back(args.Arg(i));
	}
	
	int caller = ctx.GetPlayerSlot().Get() + 1;

	for (size_t i = 0; i < globalCallback.GetCount(); ++i)
	{
		auto thisResult = globalCallback.Notify(i, caller, callingContext, arguments);
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

	auto& commandInfo = *std::get<CommandInfoPtr>(*it);
	auto callback = mode == HookMode::Pre ? commandInfo.callbackPre : commandInfo.callbackPost;

	for (size_t i = 0; i < callback.GetCount(); ++i)
	{
		auto thisResult = callback.Notify(i, caller, callingContext, arguments);
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

dyno::ReturnAction CConCommandManager::Hook_DispatchConCommand(dyno::IHook& hook)
{
	// auto cmd = dyno::GetArgument<ConCommandHandle* const>(hook, 1);
	auto ctx = dyno::GetArgument<const CCommandContext*>(hook, 2);
	auto args = dyno::GetArgument<const CCommand*>(hook, 3);

	const char* name = args->Arg(0);

	g_Logger.LogFormat(LS_DEBUG, "[ConCommandManager::Hook_DispatchConCommand]: %s\n", name);

	auto result = ExecuteCommandCallbacks(name, *ctx, *args, HookMode::Pre, CommandCallingContext::Console);
	if (result >= ResultType::Handled)
	{
		return dyno::ReturnAction::Supercede;
	}

	return dyno::ReturnAction::Ignored;
}

dyno::ReturnAction CConCommandManager::Hook_DispatchConCommand_Post(dyno::IHook& hook)
{
	// auto cmd = dyno::GetArgument<ConCommandHandle* const>(hook, 1);
	auto ctx = dyno::GetArgument<const CCommandContext*>(hook, 2);
	auto args = dyno::GetArgument<const CCommand*>(hook, 3);

	const char* name = args->Arg(0);

	auto result = ExecuteCommandCallbacks(name, *ctx, *args, HookMode::Post, CommandCallingContext::Console);
	if (result >= ResultType::Handled)
	{
		return dyno::ReturnAction::Supercede;
	}

	return dyno::ReturnAction::Ignored;
}

CConCommandManager g_CommandManager;