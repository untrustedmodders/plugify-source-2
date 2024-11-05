#pragma once

#include "listener_manager.hpp"
#include <core/sdk/utils.h>

#include <convar.h>
#include <plugify/polyhook.hpp>

enum CommandCallingContext
{
	Invalid = -1,
	Console = 0,
	Chat = 1,
};

using CommandListenerCallback = ResultType (*)(int caller, int context, const plg::vector<plg::string>& arguments);

struct ConCommandInfo
{
	ConCommandInfo() = default;
	explicit ConCommandInfo(plg::string name, plg::string description = {});
	~ConCommandInfo() = default;

	plg::string name;
	plg::string description;
	uint64 adminFlags{};
	ConCommand* command{};
	std::unique_ptr<ConCommand> commandRef;
	CListenerManager<CommandListenerCallback> callbackPre;
	CListenerManager<CommandListenerCallback> callbackPost;
};

using CommandInfoPtr = std::unique_ptr<ConCommandInfo>;

class CConCommandManager
{
public:
	CConCommandManager() = default;
	~CConCommandManager() = default;

	void AddCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode);
	void RemoveCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode);
	bool IsValidValveCommand(const plg::string& name) const;
	bool AddValveCommand(const plg::string& name, const plg::string& description, int64 flags = 0, uint64 adminFlags = 0);
	bool RemoveValveCommand(const plg::string& name);

	poly::ReturnAction Hook_DispatchConCommand(poly::Params& params, int count, poly::Return& ret);
	poly::ReturnAction Hook_DispatchConCommand_Post(poly::Params& params, int count, poly::Return& ret);
	ResultType ExecuteCommandCallbacks(const plg::string& name, const CCommandContext& ctx, const CCommand& args, HookMode mode, CommandCallingContext callingContext);

private:
	//std::vector<ConCommandInfo*> m_cmdList;
	std::map<plg::string, CommandInfoPtr, utils::CaseInsensitiveComparator> m_cmdLookup;
	CListenerManager<CommandListenerCallback> m_globalPre;
	CListenerManager<CommandListenerCallback> m_globalPost;
};

extern CConCommandManager g_CommandManager;