#pragma once

#include "game_config.h"
#include <core/sdk/utils.h>

#include <plugify/polyhook.h>

class CHookHolder
{
public:
	template <typename F>
	void AddHookMemFunc(F func, void* ptr, const std::function<void(const poly::CHook&)>& callback = {})
	{
		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		auto ihook = poly::CHook::FindVirtualByFunc(ptr, (void*&)func);
		if (ihook != nullptr)
		{
			return;
		}

		ihook = poly::CHook::CreateHookVirtualByFunc(ptr, (void*&)func, ret, std::vector(args.begin(), args.end()));
		if (ihook == nullptr)
		{
			g_Logger.LogFormat(LS_WARNING, "Could not hook member function \"%s\".\n", typeid(func).name());
			return;
		}

		//void* p = ihook->GetAddress();
		callback(*m_hooks.emplace_back(std::move(ihook)).get());
		//g_Logger.LogFormat(LS_DEBUG, "%s - %p.\n", p, typeid(func).name());
	}

	template <typename F>
	void AddHookDetourFunc(const plg::string& name, const std::function<void(poly::CHook&)>& callback = {})
	{
		auto addr = g_pGameConfig->ResolveSignature(name);
		if (!addr)
		{
			g_Logger.LogFormat(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return;
		}

		auto ihook = poly::CHook::FindDetour(addr);
		if (ihook != nullptr)
		{
			return;
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		ihook = poly::CHook::CreateDetourHook(addr, ret, std::vector(args.begin(), args.end()));
		if (ihook == nullptr)
		{
			g_Logger.LogFormat(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return;
		}

		//void* ptr = ihook->GetAddress();
		callback(*m_hooks.emplace_back(std::move(ihook)).get());
		//g_Logger.LogFormat(LS_DEBUG, "%s - %p.\n", ptr, name.c_str());
	}

	template <typename F, typename C, typename... T> requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	void AddHookMemFunc(F func, void* ptr, C callback, T... types)
	{
		AddHookMemFunc<F>(func, ptr, [&](const poly::CHook& hook)
		{
			([&](){ hook.AddCallback(types, callback); }(), ...);
		});
	}

	template <typename F, typename C, typename... T> requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	void AddHookDetourFunc(const plg::string& name, C callback, T... types)
	{
		AddHookDetourFunc<F>(name, [&](const poly::CHook& hook)
		{
			([&](){ hook.AddCallback(types, callback); }(), ...);
		});
	}

	void UnhookAll()
	{
		m_hooks.clear();
	}

private:
	std::vector<std::unique_ptr<poly::CHook>> m_hooks;
};

extern CHookHolder g_PH;