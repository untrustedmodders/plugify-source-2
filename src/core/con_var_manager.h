#pragma once

#include "listener_manager.h"
#include <core/sdk/utils.h>

#include <convar.h>

using ConVarChangeListenerCallback = void (*)(BaseConVar* pConVar, const plg::string& newValue, const plg::string& oldValue);

struct ConVarInfo
{
	explicit ConVarInfo(plg::string name, plg::string description = {});
	~ConVarInfo() = default;

	plg::string name;
	plg::string description;
	std::unique_ptr<BaseConVar> conVar;
	CListenerManager<ConVarChangeListenerCallback> hook;
};

class CConVarManager;
extern CConVarManager g_ConVarManager;

using ConVarInfoPtr = std::unique_ptr<ConVarInfo>;

class CConVarManager
{
public:
	CConVarManager() = default;
	~CConVarManager() = default;

	template<typename T>
	CConVarBaseData* CreateConVar(const plg::string& name, const plg::string& description, const T& defaultVal, int flags, bool hasMin = false, T min = {}, bool hasMax = {}, T max = {})
	{
		if (name.empty() || g_pCVar->FindCommand(name.c_str()).IsValid())
		{
			return nullptr;
		}

		auto it = m_cnvLookup.find(name);
		if (it != m_cnvLookup.end())
		{
			return std::get<ConVarInfoPtr>(*it)->conVar->GetConVarData();
		}

		ConVarHandle hCvarHandle = g_pCVar->FindConVar(name.c_str());
		if (hCvarHandle.IsValid())
		{
			return g_pCVar->GetConVar(hCvarHandle);
		}

		auto& conVarInfo = *m_cnvLookup.emplace(name, std::make_unique<ConVarInfo>(name, description)).first->second;
		conVarInfo.conVar = std::unique_ptr<BaseConVar>(new ConVar<T>(conVarInfo.name.c_str(), flags, conVarInfo.description.c_str(), defaultVal, hasMin, min, hasMax, max, &ChangeCallback));
		m_cnvCache.emplace(conVarInfo.conVar.get(), &conVarInfo);
		return conVarInfo.conVar->GetConVarData();
	}
	
	bool RemoveConVar(const plg::string& name);
	CConVarBaseData* FindConVar(const plg::string& name);
	bool IsValidConVar(const plg::string& name) const;
	void HookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback);
	void UnhookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback);

	template<typename T>
	static void ChangeCallback(ConVar<T>* ref, const CSplitScreenSlot nSlot, const T* pNewValue, const T* pOldValue)
	{
		auto it = g_ConVarManager.m_cnvCache.find(ref);
		if (it == g_ConVarManager.m_cnvCache.end())
			return;

		auto& conVarInfo = *std::get<const ConVarInfo*>(*it);
		
		if constexpr (std::is_same_v<T, bool>) 
		{
			conVarInfo.hook.Notify(ref, *pNewValue ? "1" : "0", *pOldValue ? "1" : "0");
		}
		else if constexpr (std::is_same_v<T, const char*>)
		{
			conVarInfo.hook.Notify(ref, *pNewValue, *pOldValue);
		}
		else if constexpr (std::is_same_v<T, Color>)
		{
			plg::string newValue = std::format("{} {} {} {}", pNewValue->r(), pNewValue->g(), pNewValue->b(), pNewValue->a());
			plg::string oldValue = std::format("{} {} {} {}", pOldValue->r(), pOldValue->g(), pOldValue->b(), pOldValue->a());
			conVarInfo.hook.Notify(ref, newValue, oldValue);
		}
		else if constexpr (std::is_same_v<T, Vector2D>)
		{
			plg::string newValue = std::format("{} {}", pNewValue->x, pNewValue->y);
			plg::string oldValue = std::format("{} {}", pOldValue->x, pOldValue->y);
			conVarInfo.hook.Notify(ref, newValue, oldValue);
		}
		else if constexpr (std::is_same_v<T, Vector> || std::is_same_v<T, QAngle>)
		{
			plg::string newValue = std::format("{} {} {}", pNewValue->x, pNewValue->y, pNewValue->z);
			plg::string oldValue = std::format("{} {} {}", pOldValue->x, pOldValue->y, pOldValue->z);
			conVarInfo.hook.Notify(ref, newValue, oldValue);
		}
		else if constexpr (std::is_same_v<T, Vector4D>)
		{
			plg::string newValue = std::format("{} {} {} {}", pNewValue->x, pNewValue->y, pNewValue->z, pNewValue->w);
			plg::string oldValue = std::format("{} {} {} {}", pOldValue->x, pOldValue->y, pOldValue->z, pOldValue->w);
			conVarInfo.hook.Notify(ref, newValue, oldValue);
		}
		else
		{
			conVarInfo.hook.Notify(ref, std::to_string(*pNewValue), std::to_string(*pOldValue));
		}
	}
	
	static void ChangeGlobal(BaseConVar* ref, CSplitScreenSlot nSlot, const char *pNewValue, const char *pOldValue);
	
private:
	std::map<plg::string, ConVarInfoPtr, utils::CaseInsensitiveComparator> m_cnvLookup;
	std::map<const BaseConVar*, const ConVarInfo*> m_cnvCache;
	CListenerManager<ConVarChangeListenerCallback> m_global;
};