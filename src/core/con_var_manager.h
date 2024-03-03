#pragma once

#include "listener_manager.h"
#include "utils.h"

#include <convar.h>

using ConVarChangeListenerCallback = void (*)(BaseConVar* pConVar, const std::string& newValue, const std::string& flOldValue);

struct ConVarInfo
{
	explicit ConVarInfo(std::string name, std::string description = {});
	~ConVarInfo() = default;

	std::string name;
	std::string description;
	std::unique_ptr<BaseConVar> conVar;
	CListenerManager<ConVarChangeListenerCallback> hook;
};

class ConVarManager;
extern ConVarManager g_ConVarManager;

using ConVarInfoPtr = std::unique_ptr<ConVarInfo>;

class ConVarManager
{
public:
	ConVarManager() = default;
	~ConVarManager() = default;

	template<typename T>
	CConVarBaseData* CreateConVar(const std::string& name,const std::string& description, const T& defaultVal, int flags, bool hasMin, T min, bool hasMax, T max);
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

		const ConVarInfo* conVarInfo = *it;
		
		if constexpr (std::is_same_v<T, bool>) 
		{
			conVarInfo->hook(ref, *pNewValue ? "1" : "0", *pOldValue ? "1" : "0");
		}
		else if constexpr (std::is_same_v<T, char*>)
		{
			conVarInfo->hook(ref, *pNewValue, *pOldValue);
		}
		else if constexpr (std::is_same_v<T, Color>)
		{
			std::stringstream newValue;
			newValue << pNewValue->r() << " " <<  pNewValue->g() << " " << pNewValue->b() << " " << pNewValue->a();
			std::stringstream oldValue;
			oldValue << pOldValue->r() << " " <<  pOldValue->g() << " " << pOldValue->b() << " " << pOldValue->a();
			conVarInfo->hook(ref, newValue.str(), oldValue.str());
		}
		else if constexpr (std::is_same_v<T, Vector2D>)
		{
			std::stringstream newValue;
			newValue << pNewValue->x << " " <<  pNewValue->y;
			std::stringstream oldValue;
			oldValue << pOldValue->x << " " <<  pOldValue->y;
			conVarInfo->hook(ref, newValue.str(), oldValue.str());
		}
		else if constexpr (std::is_same_v<T, Vector> || std::is_same_v<T, QAngle>)
		{
			std::stringstream newValue;
			newValue << pNewValue->x << " " <<  pNewValue->y << " " << pNewValue->z;
			std::stringstream oldValue;
			oldValue << pOldValue->x << " " <<  pOldValue->y << " " << pOldValue->z;
			conVarInfo->hook(ref, newValue.str(), oldValue.str());
		}
		else if constexpr (std::is_same_v<T, Vector4D>)
		{
			std::stringstream newValue;
			newValue << pNewValue->x << " " <<  pNewValue->y << " " << pNewValue->z << " " << pNewValue->w;
			std::stringstream oldValue;
			oldValue << pOldValue->x << " " <<  pOldValue->y << " " << pOldValue->z << " " << pOldValue->w;
			conVarInfo->hook(ref, newValue.str(), oldValue.str());
		}
		else
		{
			conVarInfo->hook(ref, std::to_string(*pNewValue), std::to_string(*pOldValue));
		}
	}
	
	static void ChangeGlobal(BaseConVar* ref, CSplitScreenSlot nSlot, const char *pNewValue, const char *pOldValue);
	
private:
	std::map<std::string, ConVarInfoPtr, utils::CaseInsensitiveComparator> m_cnvLookup;
	std::map<const BaseConVar*, const ConVarInfo*> m_cnvCache;
	CListenerManager<ConVarChangeListenerCallback> m_global;
};