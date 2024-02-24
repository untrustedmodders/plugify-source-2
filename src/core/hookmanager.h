#pragma once

#include "utils.h"

#include <dynohook/dynohook.h>

class CHookManager
{
public:
	template <typename F, typename C, typename... T>
	void AddHookMemFunc(F func, void* ptr, C callback, T... types)
	{
		using trait = dyno::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();
		auto ihook = dyno::CHook::CreateHookVirtualByFunc(ptr, (void*&)func, std::vector(args.begin(), args.end()), ret);
		if (ihook == nullptr)
		{
			g_Logger.ErrorFormat("Could not hook \"'%s\".", utils::Demangle(typeid(func).name()).c_str());
		}
		auto& hook = m_Hooks.emplace_back(std::move(ihook));
		([&]()
		 { hook->AddCallback(types, callback); }(), ...);
	}

	void UnhookAll()
	{
		m_Hooks.clear();
	}

private:
	std::vector<std::unique_ptr<dyno::CHook>> m_Hooks;
};

extern CHookManager g_HookManager;