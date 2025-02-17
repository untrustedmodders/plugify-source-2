#include "con_command_manager.hpp"
#include "player_manager.hpp"

#include <icvar.h>

CConCommandManager::~CConCommandManager() {
	if (!g_pCVar) {
		return;
	}

	for (const auto& [_, command] : m_cmdLookup) {
		if (!command->defaultCommand) {
			g_pCVar->UnregisterConCommandCallbacks(*command->commandRef);
		}
	}
}

void CommandCallback(const CCommandContext&, const CCommand&) {
}

ConCommandInfo::ConCommandInfo(plg::string name, plg::string description) : name(std::move(name)), description(std::move(description)) {
}

void CConCommandManager::AddCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	if (name.empty()) {
		if (mode == HookMode::Pre) {
			m_globalPre.Register(callback);
		} else {
			m_globalPost.Register(callback);
		}
		return;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end()) {
		ConCommandRef hFoundCommand = g_pCVar->FindConCommand(name.c_str());
		if (!hFoundCommand.IsValidRef()) {
			return;
		}

		auto& commandInfo = *m_cmdLookup.emplace(name, std::make_unique<ConCommandInfo>(name)).first->second;
		commandInfo.command = g_pCVar->GetConCommandData(hFoundCommand);
		commandInfo.defaultCommand = true;

		if (mode == HookMode::Pre) {
			commandInfo.callbackPre.Register(callback);
		} else {
			commandInfo.callbackPost.Register(callback);
		}
	} else {
		auto& commandInfo = *std::get<CommandInfoPtr>(*it);
		if (mode == HookMode::Pre) {
			commandInfo.callbackPre.Register(callback);
		} else {
			commandInfo.callbackPost.Register(callback);
		}
	}
}

void CConCommandManager::RemoveCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	if (name.empty()) {
		if (mode == HookMode::Pre) {
			m_globalPre.Unregister(callback);
		} else {
			m_globalPost.Unregister(callback);
		}
		return;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end()) {
		return;
	}

	auto& commandInfo = *std::get<CommandInfoPtr>(*it);
	if (mode == HookMode::Pre) {
		commandInfo.callbackPre.Unregister(callback);
	} else {
		commandInfo.callbackPost.Unregister(callback);
	}
}

bool CConCommandManager::AddValveCommand(const plg::string& name, const plg::string& description, ConVarFlag flags, uint64 adminFlags) {
	if (name.empty() || g_pCVar->FindConVar(name.c_str()).IsValidRef()) {
		return false;
	}

	if (g_pCVar->FindConCommand(name.c_str()).IsValidRef()) {
		return false;
	}

	auto it = m_cmdLookup.find(name);
	if (it != m_cmdLookup.end()) {
		return false;
	}

	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	auto& commandInfo = *m_cmdLookup.emplace(name, std::make_unique<ConCommandInfo>(name, description)).first->second;
	commandInfo.commandRef = std::make_unique<ConCommand>(commandInfo.name.c_str(), CommandCallback, commandInfo.description.c_str(), flags);
	commandInfo.command = commandInfo.commandRef->GetRawData();
	commandInfo.adminFlags = adminFlags;

	return true;
}

bool CConCommandManager::RemoveValveCommand(const plg::string& name) {
	auto hFoundCommand = g_pCVar->FindConCommand(name.c_str());
	if (!hFoundCommand.IsValidRef()) {
		return false;
	}

	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	auto it = m_cmdLookup.find(name);
	if (it != m_cmdLookup.end()) {
		m_cmdLookup.erase(it);
		return true;
	} else {
		g_pCVar->UnregisterConCommandCallbacks(hFoundCommand);
	}

	return true;
}

bool CConCommandManager::IsValidValveCommand(const plg::string& name) const {
	ConCommandRef hFoundCommand = g_pCVar->FindConCommand(name.c_str());
	return hFoundCommand.IsValidRef();
}

static bool CheckCommandAccess(CPlayerSlot slot, uint64 flags) {
	if (!flags) {
		return true;
	}

	auto pPlayer = g_PlayerManager.ToPlayer(slot);
	if (pPlayer == nullptr) {
		return false;
	}

	/*if (!pPlayer->IsAdminFlagSet(flags)) {
		utils::PrintChat(slot, "You don't have access to this command.");
		return false;
	}*/

	return true;
}

ResultType CConCommandManager::ExecuteCommandCallbacks(const plg::string& name, const CCommandContext& ctx, const CCommand& args, HookMode mode, CommandCallingContext callingContext) {
	g_Logger.LogFormat(LS_DEBUG, "[ConCommandManager::ExecuteCommandCallbacks][%s]: %s\n", mode == HookMode::Pre ? "Pre" : "Post", name.c_str());

	ResultType result = ResultType::Continue;

	const auto& globalCallback = mode == HookMode::Pre ? m_globalPre : m_globalPost;

	int size = args.ArgC();

	plg::vector<plg::string> arguments;
	arguments.reserve(static_cast<size_t>(size));
	for (int i = 0; i < size; ++i) {
		arguments.emplace_back(args.Arg(i));
	}

	CPlayerSlot caller = ctx.GetPlayerSlot();

	for (size_t i = 0; i < globalCallback.GetCount(); ++i) {
		auto thisResult = globalCallback.Notify(i, caller, callingContext, arguments);
		if (thisResult >= ResultType::Stop) {
			if (mode == HookMode::Pre) {
				return ResultType::Stop;
			}

			result = thisResult;
			break;
		}

		if (thisResult >= ResultType::Handled) {
			result = thisResult;
		}
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end()) {
		return result;
	}

	auto& commandInfo = *std::get<CommandInfoPtr>(*it);

	if (!CheckCommandAccess(caller, commandInfo.adminFlags)) {
		return result;
	}

	const auto& callback = mode == HookMode::Pre ? commandInfo.callbackPre : commandInfo.callbackPost;

	for (size_t i = 0; i < callback.GetCount(); ++i) {
		auto thisResult = callback.Notify(i, caller, callingContext, arguments);
		if (thisResult >= ResultType::Handled) {
			return thisResult;
		} else if (thisResult > result) {
			result = thisResult;
		}
	}

	return result;
}

poly::ReturnAction CConCommandManager::Hook_DispatchConCommand(poly::Params& params, int count, poly::Return& ret) {
	// auto cmd = poly::GetArgument<ConCommandRef* const>(params, 1);
	auto ctx = poly::GetArgument<const CCommandContext*>(params, 2);
	auto args = poly::GetArgument<const CCommand*>(params, 3);
	if (ctx == nullptr || args == nullptr) {
		return poly::ReturnAction::Ignored;
	}

	const char* name = args->Arg(0);

	g_Logger.LogFormat(LS_DEBUG, "[ConCommandManager::Hook_DispatchConCommand]: %s\n", name);

	auto result = ExecuteCommandCallbacks(name, *ctx, *args, HookMode::Pre, CommandCallingContext::Console);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

poly::ReturnAction CConCommandManager::Hook_DispatchConCommand_Post(poly::Params& params, int count, poly::Return& ret) {
	// auto cmd = poly::GetArgument<ConCommandRef* const>(params, 1);
	auto ctx = poly::GetArgument<const CCommandContext*>(params, 2);
	auto args = poly::GetArgument<const CCommand*>(params, 3);
	if (ctx == nullptr || args == nullptr) {
		return poly::ReturnAction::Ignored;
	}

	const char* name = args->Arg(0);

	auto result = ExecuteCommandCallbacks(name, *ctx, *args, HookMode::Post, CommandCallingContext::Console);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

CConCommandManager g_CommandManager;