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
		return m_globalCallbacks[static_cast<size_t>(mode)].Register(callback);
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
		return commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	} else {
		auto& commandInfo = *std::get<CommandInfoPtr>(*it);
		return commandInfo.callbacks[static_cast<size_t>(mode)].Register(callback);
	}
}

bool CConCommandManager::RemoveCommandListener(const plg::string& name, CommandListenerCallback callback, HookMode mode) {
	std::lock_guard<std::mutex> lock(m_registerCmdLock);

	if (name.empty()) {
		return m_globalCallbacks[static_cast<size_t>(mode)].Unregister(callback);
	}

	auto it = m_cmdLookup.find(name);
	if (it == m_cmdLookup.end()) {
		return false;
	}

	auto& commandInfo = *std::get<CommandInfoPtr>(*it);
	return commandInfo.callbacks[static_cast<size_t>(mode)].Unregister(callback);
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
	//S2_LOGF(LS_DEBUG, "[ConCommandManager::ExecuteCommandCallbacks][%s]: %s\n", mode == HookMode::Pre ? "Pre" : "Post", name.c_str());

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

	const char* arg0 = args->Arg(0);

	//S2_LOGF(LS_DEBUG, "[ConCommandManager::Hook_DispatchConCommand]: %s\n", name.data());

	static const char sayCommand[] = "say";
	constexpr size_t sayNullTerminated = sizeof(sayCommand) - 1;
	if (!V_strncmp(arg0, sayCommand, sayNullTerminated)) {
		if (!arg0[sayNullTerminated] || !V_strcmp(&arg0[sayNullTerminated], "_team")) {
			const char* arg1 = args->Arg(1);
			while (*arg1 == ' ') {
				arg1++;
			}

			bool bSilent = g_pCoreConfig->IsSilentChatTrigger(arg1);
			if (bSilent || g_pCoreConfig->IsPublicChatTrigger(arg1)) {
				char* pszMessage = (char*) (args->ArgS() + 1);
				++pszMessage;

				// Trailing slashes are only removed if Host_Say has been called.
				bool bHostSay = bSilent && mode == HookMode::Pre;
				if (bHostSay) pszMessage[V_strlen(pszMessage) - 1] = 0;

				CCommand nargs;
				nargs.Tokenize(pszMessage);

				auto result = ExecuteCommandCallbacks(nargs[0], *ctx, nargs, mode, CommandCallingContext::Chat);
				if (result >= ResultType::Handled || bHostSay) {
					return poly::ReturnAction::Supercede;
				}
			}
		}
	}

	auto result = ExecuteCommandCallbacks(arg0, *ctx, *args, mode, CommandCallingContext::Console);
	if (result >= ResultType::Handled) {
		return poly::ReturnAction::Supercede;
	}

	return poly::ReturnAction::Ignored;
}

CConCommandManager g_CommandManager;