#pragma once

#include "listener_manager.h"
#include <core/sdk/utils.h>

#include <convar.h>

using ConVarChangeListenerCallback = void (*)(BaseConVar* pConVar, const std::string& newValue, const std::string& oldValue);

struct ConVarInfo
{
	explicit ConVarInfo(std::string name, std::string description = {});
	~ConVarInfo() = default;

	std::string name;
	std::string description;
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
	CConVarBaseData* CreateConVar(const std::string& name, const std::string& description, const T& defaultVal, int flags, bool hasMin = false, T min = {}, bool hasMax = {}, T max = {})
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
	
	bool RemoveConVar(const std::string& name);
	CConVarBaseData* FindConVar(const std::string& name);
	bool IsValidConVar(const std::string& name) const;
	void HookConVarChange(const std::string& name, ConVarChangeListenerCallback callback);
	void UnhookConVarChange(const std::string& name, ConVarChangeListenerCallback callback);

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
			std::stringstream newValue;
			newValue << pNewValue->r() << " " <<  pNewValue->g() << " " << pNewValue->b() << " " << pNewValue->a();
			std::stringstream oldValue;
			oldValue << pOldValue->r() << " " <<  pOldValue->g() << " " << pOldValue->b() << " " << pOldValue->a();
			conVarInfo.hook.Notify(ref, newValue.str(), oldValue.str());
		}
		else if constexpr (std::is_same_v<T, Vector2D>)
		{
			std::stringstream newValue;
			newValue << pNewValue->x << " " <<  pNewValue->y;
			std::stringstream oldValue;
			oldValue << pOldValue->x << " " <<  pOldValue->y;
			conVarInfo.hook.Notify(ref, newValue.str(), oldValue.str());
		}
		else if constexpr (std::is_same_v<T, Vector> || std::is_same_v<T, QAngle>)
		{
			std::stringstream newValue;
			newValue << pNewValue->x << " " <<  pNewValue->y << " " << pNewValue->z;
			std::stringstream oldValue;
			oldValue << pOldValue->x << " " <<  pOldValue->y << " " << pOldValue->z;
			conVarInfo.hook.Notify(ref, newValue.str(), oldValue.str());
		}
		else if constexpr (std::is_same_v<T, Vector4D>)
		{
			std::stringstream newValue;
			newValue << pNewValue->x << " " <<  pNewValue->y << " " << pNewValue->z << " " << pNewValue->w;
			std::stringstream oldValue;
			oldValue << pOldValue->x << " " <<  pOldValue->y << " " << pOldValue->z << " " << pOldValue->w;
			conVarInfo.hook.Notify(ref, newValue.str(), oldValue.str());
		}
		else
		{
			conVarInfo.hook.Notify(ref, std::to_string(*pNewValue), std::to_string(*pOldValue));
		}
	}
	
	static void ChangeGlobal(BaseConVar* ref, CSplitScreenSlot nSlot, const char *pNewValue, const char *pOldValue);
	
private:
	std::map<std::string, ConVarInfoPtr, utils::CaseInsensitiveComparator> m_cnvLookup;
	std::map<const BaseConVar*, const ConVarInfo*> m_cnvCache;
	CListenerManager<ConVarChangeListenerCallback> m_global;
};