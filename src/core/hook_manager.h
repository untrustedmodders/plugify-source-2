#pragma once

#include "game_config.h"
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
			g_Logger.ErrorFormat("Could not hook member function \"%s\".\n", utils::Demangle(typeid(func).name()).c_str());
		}
		auto& hook = m_hooks.emplace_back(std::move(ihook));
		([&]()
		 { hook->AddCallback(types, callback); }(), ...);
	}

	template <typename F, typename C, typename... T>
	void AddHookDetourFunc(const std::string& name, C callback, T... types)
	{
		auto addr = globals::g_GameConfig->ResolveSignature(name);
		if (!addr)
		{
			g_Logger.ErrorFormat("Could not hook detour function \"%s\".\n", name.c_str());
		}

		using trait = dyno::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();
		auto ihook = dyno::CHook::CreateDetourHook(addr, std::vector(args.begin(), args.end()), ret);
		if (ihook == nullptr)
		{
			g_Logger.ErrorFormat("Could not hook detour function \"%s\".\n", name.c_str());
		}

		auto& hook = m_hooks.emplace_back(std::move(ihook));
		([&]()
		 { hook->AddCallback(types, callback); }(), ...);
	}

	void UnhookAll()
	{
		m_hooks.clear();
	}

private:
	std::vector<std::unique_ptr<dyno::CHook>> m_hooks;
};

extern CHookManager g_HookManager;