#pragma once

#include "listenermanager.h"

#include <convar.h>

class ConCommand;
class CCommand;

typedef ResultType (*FnCommandListenerCallback)(const CCommand &args, int slot);

struct CommandInfo : public ICommandCallback {
	std::string name;
	std::string description;
	CListenerManager<FnCommandListenerCallback> listener;
	ConCommandRefAbstract commandRef;
	ConCommand command;

	explicit CommandInfo(const char* name, const char* description = nullptr, int64 flags = 0);

	void CommandCallback(const CCommandContext& context, const CCommand& command) override;
};

class CCommandManager {
public:
	void AddCommand(FnCommandListenerCallback callback, const char* name, const char* description = nullptr, int64 flags = 0);
	bool DeleteCommand(FnCommandListenerCallback callback, const char* name);

private:
	std::unordered_map<std::string, CommandInfo> m_Commands;
};

extern CCommandManager g_CommandManager;