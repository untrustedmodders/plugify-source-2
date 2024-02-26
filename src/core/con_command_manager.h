#pragma once

#include "listener_manager.h"
#include "utils.h"

#include <convar.h>
#include <dynohook/dynohook.h>

enum CommandCallingContext
{
	Console = 0,
	Chat = 1,
};

using CommandListenerCallback = ResultType (*)(int playerSlot, const std::string& name, const std::vector<std::string>& args, CommandCallingContext ctx);

struct ConCommandInfo {
	explicit ConCommandInfo(std::string name, std::string description = {});
	~ConCommandInfo() = default;

	std::string name;
	std::string description;
	ConCommandRefAbstract commandRef;
	ConCommand* command{};
	std::unique_ptr<ConCommand> commandStorage;
	CListenerManager<CommandListenerCallback> callbackPre;
	CListenerManager<CommandListenerCallback> callbackPost;
};

using CommandInfo = std::unique_ptr<ConCommandInfo>;

class ConCommandManager {
public:
	ConCommandManager() = default;
	~ConCommandManager() = default;

	void AddCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode);
	void RemoveCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode);
	bool IsValidValveCommand(const std::string& name);
	bool AddValveCommand(const std::string& name, const std::string& description, int64 flags);
	bool RemoveValveCommand(const std::string& name);

	dyno::ReturnAction Hook_DispatchConCommand(dyno::IHook& hook);
	dyno::ReturnAction Hook_DispatchConCommand_Post(dyno::IHook& hook);
	ResultType ExecuteCommandCallbacks(const std::string& name, const CCommandContext& ctx, const CCommand& args, HookMode mode, CommandCallingContext callingContext);

private:
	std::vector<ConCommandInfo*> m_cmdList;
	std::map<std::string, CommandInfo, utils::CaseInsensitiveComparator> m_cmdLookup;
	ConCommandInfo m_globalCmd{"global"};
};

extern ConCommandManager g_CommandManager;