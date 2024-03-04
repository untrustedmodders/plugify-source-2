#include "con_var_manager.h"

ConVarInfo::ConVarInfo(std::string name, std::string description) : name(std::move(name)), description(std::move(description))
{
}

bool ConVarManager::RemoveConVar(const std::string& name)
{
	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end())
	{
		m_cnvCache.erase(std::get<ConVarInfoPtr>(*it)->conVar.get());
		m_cnvLookup.erase(it);
		return true;
	}

	return false;
}

CConVarBaseData* ConVarManager::FindConVar(const std::string& name)
{
	auto it = m_cnvLookup.find(name);
	if (it != m_cnvLookup.end())
	{
		return std::get<ConVarInfoPtr>(*it)->conVar->GetConVarData();
	}

	ConVarHandle hCvarHandle = g_pCVar->FindConVar(name.c_str());
	if (!hCvarHandle.IsValid())
	{
		return nullptr;
	}

	return g_pCVar->GetConVar(hCvarHandle);
}

bool ConVarManager::IsValidConVar(const std::string& name) const
{
	ConVarHandle hFoundConVar = g_pCVar->FindConVar(name.c_str());
	return hFoundConVar.IsValid();
}

void ConVarManager::HookConVarChange(const std::string& name, ConVarChangeListenerCallback callback)
{
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

void ConVarManager::UnhookConVarChange(const std::string& name, ConVarChangeListenerCallback callback)
{
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

void ConVarManager::ChangeGlobal(BaseConVar* ref, CSplitScreenSlot nSlot, const char* pNewValue, const char* pOldValue)
{
	g_ConVarManager.m_global.Notify(ref, pNewValue, pOldValue);
}

ConVarManager g_ConVarManager;