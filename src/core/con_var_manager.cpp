#include "con_var_manager.hpp"

ConVarManager::~ConVarManager() {
	if (!g_pCVar) {
		return;
	}

	g_pCVar->RemoveGlobalChangeCallback(&ChangeGlobal);
	for (const auto& [cv, _]: m_cnvCache) {
		g_pCVar->UnregisterConVarCallbacks(*cv);
	}
}

ConVarInfo::ConVarInfo(plg::string name, plg::string description) : name(std::move(name)), description(std::move(description)) {
}

bool ConVarManager::RemoveConVar(const plg::string& name) {
	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end()) {
		m_cnvCache.erase(std::get<ConVarInfoPtr>(*it)->conVar.get());
		m_cnvLookup.erase(it);
		return true;
	}

	return false;
}

ConVarRef ConVarManager::FindConVar(const plg::string& name) {
	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end()) {
		return *std::get<ConVarInfoPtr>(*it)->conVar;
	}

	auto& conVarInfo = *m_cnvLookup.emplace(name, std::make_unique<ConVarInfo>(name, "")).first->second;
	conVarInfo.conVar = std::make_unique<ConVarRefAbstract>(name.c_str(), true);
	m_cnvCache.emplace(conVarInfo.conVar.get(), &conVarInfo);

	if (!conVarInfo.conVar->IsValidRef()) {
		S2_LOGF(LS_WARNING, "Failed to find \"{}\" convar\n", name);
		return {};
	}

	return *conVarInfo.conVar;
}

void ConVarManager::HookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback) {
	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	if (name.empty()) {
		if (m_global.Empty()) {
			g_pCVar->InstallGlobalChangeCallback(&ChangeGlobal);
		}
		m_global.Register(callback);
		return;
	}

	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end()) {
		auto& conVarInfo = *std::get<ConVarInfoPtr>(*it);
		conVarInfo.hook.Register(callback);
	}
}

void ConVarManager::UnhookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback) {
	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	if (name.empty()) {
		m_global.Unregister(callback);
		if (m_global.Empty()) {
			g_pCVar->RemoveGlobalChangeCallback(&ChangeGlobal);
		}
		return;
	}

	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end()) {
		auto& conVarInfo = *std::get<ConVarInfoPtr>(*it);
		conVarInfo.hook.Unregister(callback);
	}
}

void ConVarManager::ChangeGlobal(ConVarRefAbstract* ref, CSplitScreenSlot nSlot, const char* pNewValue, const char* pOldValue) {
	g_ConVarManager.m_global.Notify(*ref, pNewValue, pOldValue);
}

ConVarManager g_ConVarManager;