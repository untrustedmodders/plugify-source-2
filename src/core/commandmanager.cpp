#include "commandmanager.h"

void CommandInfo::CommandCallback(const CCommandContext &context, const CCommand &args)
{
	int size = args.ArgC();
	std::vector<std::string> arguments;
	arguments.reserve((size_t) size);
	for (int i = 0; i < size; ++i)
	{
		arguments.emplace_back(args.Arg(i));
	}

	listener.Notify(arguments, context.GetPlayerSlot().Get());
}

void CCommandManager::AddCommand(FnCommandListenerCallback callback, const std::string &name, const std::string &description, int64 flags)
{
	auto it = m_Commands.find(name);
	if (it != m_Commands.end())
	{
		std::get<CommandInfo>(*it).listener.Register(callback);
		return;
	}

	auto [_, result] = m_Commands.emplace(name, CommandInfo(name, description, flags));
	std::get<CommandInfo>(*_).listener.Register(callback);
}

bool CCommandManager::DeleteCommand(FnCommandListenerCallback callback, const std::string &name)
{
	auto it = m_Commands.find(name);
	if (it == m_Commands.end())
	{
		return false;
	}

	auto &command = std::get<CommandInfo>(*it);

	if (!command.listener.Unregister(callback))
	{
		return false;
	}

	if (!command.listener.GetCount())
	{
		m_Commands.erase(it);
	}

	return true;
}

CommandInfo::CommandInfo(std::string name, std::string description, int64 flags) : name(std::move(name)), description(std::move(description)), commandRef(), command(&commandRef, name.c_str(), this, description.empty() ? nullptr : description.c_str(), flags)
{
}
