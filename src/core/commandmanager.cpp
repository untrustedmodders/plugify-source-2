#include "commandmanager.h"

void CommandInfo::CommandCallback(const CCommandContext& context, const CCommand& args) {
	listener.Notify(args, context.GetPlayerSlot().Get());
}

void CCommandManager::AddCommand(FnCommandListenerCallback callback, const char* name, const char* description, int64 flags) {
	std::string commandName(name);

	auto it = m_Commands.find(commandName);
	if (it != m_Commands.end()) {
		std::get<CommandInfo>(*it).listener.Register(callback);
		return;
	}

	auto [_, result] = m_Commands.emplace(std::move(commandName), CommandInfo(name, description, flags));
	std::get<CommandInfo>(*_).listener.Register(callback);
}

bool CCommandManager::DeleteCommand(FnCommandListenerCallback callback, const char* name) {
	auto it = m_Commands.find(name);
	if (it == m_Commands.end()) {
		return false;
	}

	auto& command = std::get<CommandInfo>(*it);

	if (!command.listener.Unregister(callback)) {
		return false;
	}

	if (!command.listener.GetCount()) {
		m_Commands.erase(it);
	}

	return true;
}

CommandInfo::CommandInfo(const char* _name, const char* _description, int64 _flags) : name{_name}, description{_description}, commandRef(), command(&commandRef, name.c_str(), this, description.c_str(), _flags) {
}
