#include "con_var_manager.hpp"

CConVarManager::~CConVarManager()
{
	if (!g_pCVar)
	{
		return;
	}

	g_pCVar->RemoveGlobalChangeCallback(&ChangeGlobal);
	for (const auto& [cv, _] : m_cnvCache)
	{
		g_pCVar->UnregisterConVar(cv->GetHandle());
	}
}

ConVarInfo::ConVarInfo(plg::string name, plg::string description) : name(std::move(name)), description(std::move(description))
{
}

bool CConVarManager::RemoveConVar(const plg::string& name)
{
	auto it = m_cnvLookup.find(name);
	if (it == m_cnvLookup.end())
	{
		return false;
	}

	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	m_cnvCache.erase(std::get<ConVarInfoPtr>(*it)->conVar.get());
	m_cnvLookup.erase(it);

	return true;
}

BaseConVar* CConVarManager::FindConVar(const plg::string& name)
{
	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end())
	{
		return std::get<ConVarInfoPtr>(*it)->conVar.get();
	}

	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	auto& conVarInfo = *m_cnvLookup.emplace(name, std::make_unique<ConVarInfo>(name, "")).first->second;
	conVarInfo.conVar = std::make_unique<ConVarRef<bool>>(name.c_str());
	m_cnvCache.emplace(conVarInfo.conVar.get(), &conVarInfo);

	return conVarInfo.conVar.get();
}

void CConVarManager::HookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback)
{
	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	if (name.empty())
	{
		if (m_global.Empty()) 
		{
			g_pCVar->InstallGlobalChangeCallback(&ChangeGlobal);
		}
		m_global.Register(callback);
		return;
	}
	
	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end())
	{
		auto& conVarInfo = *std::get<ConVarInfoPtr>(*it);
		conVarInfo.hook.Register(callback);
	}
}

void CConVarManager::UnhookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback)
{
	std::lock_guard<std::mutex> lock(m_registerCnvLock);

	if (name.empty())
	{
		m_global.Unregister(callback);
		if (m_global.Empty()) 
		{
			g_pCVar->RemoveGlobalChangeCallback(&ChangeGlobal);
		}
		return;
	}
	
	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end())
	{
		auto& conVarInfo = *std::get<ConVarInfoPtr>(*it);
		conVarInfo.hook.Unregister(callback);
	}
}

void CConVarManager::ChangeGlobal(BaseConVar* ref, CSplitScreenSlot nSlot, const char* pNewValue, const char* pOldValue)
{
	g_ConVarManager.m_global.Notify(ref, pNewValue, pOldValue);
}

CConVarManager g_ConVarManager;