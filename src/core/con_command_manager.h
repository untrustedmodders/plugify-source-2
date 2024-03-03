#pragma once

#include "listener_manager.h"
#include "utils.h"

#include <convar.h>
#include <dynohook/dynohook.h>

enum CommandCallingContext
{
	Invalid = -1,
	Console = 0,
	Chat = 1,
};

using CommandListenerCallback = ResultType (*)(int playerSlot, const CCommand& command);

struct ConCommandInfo
{
	ConCommandInfo() = default;
	explicit ConCommandInfo(std::string name, std::string description = {});
	~ConCommandInfo() = default;

	std::string name;
	std::string description;
	ConCommand* command{};
	std::unique_ptr<ConCommand> commandRef;
	CListenerManager<CommandListenerCallback> callbackPre;
	CListenerManager<CommandListenerCallback> callbackPost;
};

using CommandInfoPtr = std::unique_ptr<ConCommandInfo>;

class ConCommandManager
{
public:
	ConCommandManager() = default;
	~ConCommandManager() = default;

	void AddCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode);
	void RemoveCommandListener(const std::string& name, CommandListenerCallback callback, HookMode mode);
	bool IsValidValveCommand(const std::string& name) const;
	bool AddValveCommand(const std::string& name, const std::string& description, int64 flags);
	bool RemoveValveCommand(const std::string& name);

	dyno::ReturnAction Hook_DispatchConCommand(dyno::IHook& hook);
	dyno::ReturnAction Hook_DispatchConCommand_Post(dyno::IHook& hook);
	ResultType ExecuteCommandCallbacks(const std::string& name, const CCommandContext& ctx, const CCommand& args, HookMode mode, CommandCallingContext callingContext);
	CommandCallingContext GetCommandCallingContext(CCommand* args) const;

private:
	std::vector<ConCommandInfo*> m_cmdList;
	std::map<std::string, CommandInfoPtr, utils::CaseInsensitiveComparator> m_cmdLookup;
	std::map<const CCommand*, CommandCallingContext> m_cmdContexts;
	CListenerManager<CommandListenerCallback> m_globalPre;
	CListenerManager<CommandListenerCallback> m_globalPost;
};

extern ConCommandManager g_CommandManager;