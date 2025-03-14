#include "con_command_manager.hpp"
#include "player_manager.hpp"
#include "core_config.hpp"

#include <core/sdk/entity/cbaseplayercontroller.h>
#include <core/sdk/entity/ccsplayercontroller.h>
#include <core/sdk/utils.h>

#include <icvar.h>
#include <igameevents.h>
#undef CreateEvent

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

bool CConCommandManager::AddCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	if (name.empty()) {
		m_globalCallbacks[static_cast<size_t>(mode)].Register(callback);
		return true;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end()) {
		ConCommandRef hFoundCommand = g_pCVar->FindConCommand(name.c_str());
		if (!hFoundCommand.IsValidRef()) {
			return false;
		}

		auto& commandInfo = *m_cmdLookup.emplace(name, std::make_unique<ConCommandInfo>(name)).first->second;
		commandInfo.command = g_pCVar->GetConCommandData(hFoundCommand);
		commandInfo.defaultCommand = true;
		commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	} else {
		auto& commandInfo = *std::get<CommandInfoPtr>(*it);
		commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	}
	return true;
}

bool CConCommandManager::RemoveCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	if (name.empty()) {
		m_globalCallbacks[static_cast<size_t>(mode)].Unregister(callback);
		return false;
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end()) {
		return false;
	}

	auto& commandInfo = *std::get<CommandInfoPtr>(*it);
	commandInfo.callbacks[static_cast<size_t>(mode)].Unregister(callback);
	return true;
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

	int size = args.ArgC();

	plg::vector<plg::string> arguments;
	arguments.reserve(static_cast<size_t>(size));
	for (int i = 0; i < size; ++i) {
		arguments.emplace_back(args.Arg(i));
	}

	CPlayerSlot caller = ctx.GetPlayerSlot();

	ResultType result = ResultType::Continue;

	const auto& globalCallback = m_globalCallbacks[static_cast<size_t>(mode)];

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

	const auto& commandInfo = *std::get<CommandInfoPtr>(*it);

	if (!CheckCommandAccess(caller, commandInfo.adminFlags)) {
		return result;
	}

	const auto& callback = commandInfo.callbacks[static_cast<size_t>(mode)];

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

poly::ReturnAction CConCommandManager::Hook_DispatchConCommand(poly::Params& params, int count, poly::Return& ret, HookMode mode) {
	// auto cmd = poly::GetArgument<ConCommandRef* const>(params, 1);
	auto ctx = poly::GetArgument<const CCommandContext*>(params, 2);
	auto args = poly::GetArgument<const CCommand*>(params, 3);
	if (ctx == nullptr || args == nullptr) {
		return poly::ReturnAction::Ignored;
	}

	std::string_view name = args->Arg(0);

	g_Logger.LogFormat(LS_DEBUG, "[ConCommandManager::Hook_DispatchConCommand]: %s\n", name);

	CCommand nargs;
	CommandCallingContext callingContext = CommandCallingContext::Console;
	if (name == "say" || name == "say_team") {
		std::string_view command = args->Arg(1);
		bool bSilent = g_pCoreConfig->IsSilentChatTrigger(command);
		bool bCommand = g_pCoreConfig->IsPublicChatTrigger(command) || bSilent;
		if (bCommand) {
			char* pszMessage = (char*)(args->ArgS() + 1);

			if (pszMessage[0] == '"' || pszMessage[0] == '!' || pszMessage[0] == '/')
				pszMessage += 1;

			size_t last = std::strlen(pszMessage);
			if (last && pszMessage[last - 1] == '"')
				pszMessage[last - 1] = '\0';

			nargs.Tokenize(pszMessage);
			name = nargs[0];
			args = &nargs;
			callingContext = CommandCallingContext::Chat;
		}
	}

	auto result = ExecuteCommandCallbacks(name, *ctx, *args, mode, callingContext);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

CConCommandManager g_CommandManager;