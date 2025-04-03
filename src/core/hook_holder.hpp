#pragma once

#include "game_config.hpp"
#include <core/sdk/utils.h>

#include <plugify/polyhook.hpp>

class HookHolder {
public:
	template<typename F>
	bool AddHookMemFunc(F func, void* ptr, const std::function<void(const poly::Hook&)>& callback = {}, int varIndex = -1) {
		std::pair name{(void*&) func, ptr};

		auto it = m_vhooks.find(name);
		if (it != m_vhooks.end()) {
			callback(*it->second);
			return true;
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		auto ihook = poly::Hook::FindVirtualByFunc(ptr, (void*&) func);
		if (ihook != nullptr) {
			callback(*ihook);
			return true;
		}

		ihook = poly::Hook::CreateHookVirtualByFunc(ptr, (void*&) func, ret, plg::vector(args.begin(), args.end()), varIndex);
		if (ihook == nullptr) {
			S2_LOGF(LS_WARNING, "Could not hook member function \"%s\".\n", typeid(func).name());
			return false;
		}

		callback(*ihook);
		m_vhooks.emplace(name, std::move(ihook));
		return true;
	}

	template<typename F>
	bool AddHookDetourFunc(const plg::string& name, const std::function<void(poly::Hook&)>& callback = {}, int varIndex = -1) {
		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			callback(*it->second);
			return true;
		}

		auto addr = g_pGameConfig->ResolveSignature(name);
		if (!addr) {
			S2_LOGF(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return false;
		}

		auto ihook = poly::Hook::FindDetour(addr);
		if (ihook != nullptr) {
			callback(*ihook);
			return true;
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		ihook = poly::Hook::CreateDetourHook(addr, ret, plg::vector(args.begin(), args.end()), varIndex);
		if (ihook == nullptr) {
			S2_LOGF(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return false;
		}

		callback(*ihook);
		m_dhooks.emplace(name, std::move(ihook));
		return true;
	}

	template<typename F>
	bool AddHookDetourFunc(void* addr, const std::function<void(poly::Hook&)>& callback = {}, int varIndex = -1) {
		auto name = std::format("0x{:x}", reinterpret_cast<uintptr_t>(addr));

		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			callback(*it->second);
			return true;
		}

		auto ihook = poly::Hook::FindDetour(addr);
		if (ihook != nullptr) {
			callback(*ihook);
			return true;
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		ihook = poly::Hook::CreateDetourHook(addr, ret, plg::vector(args.begin(), args.end()), varIndex);
		if (ihook == nullptr) {
			S2_LOGF(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return false;
		}

		callback(*ihook);
		m_dhooks.emplace(std::move(name), std::move(ihook));
		return true;
	}

	template<typename F, int V = -1, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	bool AddHookMemFunc(F func, void* ptr, C callback, T... types) {
		return AddHookMemFunc<F>(func, ptr, [&](const poly::Hook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		}, V);
	}

	template<typename F, int V = -1, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	bool AddHookDetourFunc(const plg::string& name, C callback, T... types) {
		return AddHookDetourFunc<F>(name, [&](const poly::Hook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		}, V);
	}

	template<typename F, int V = -1, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	bool AddHookDetourFunc(void* addr, C callback, T... types) {
		return AddHookDetourFunc<F>(addr, [&](const poly::Hook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		}, V);
	}

	bool RemoveHookDetourFunc(const plg::string& name) {
		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			m_dhooks.erase(it);
			return true;
		}
		return false;
	}

	template<typename F>
	bool RemoveHookMemFunc(F func, void* ptr) {
		auto it = m_vhooks.find(std::pair{(void*&) func, ptr});
		if (it != m_vhooks.end()) {
			m_vhooks.erase(it);
			return true;
		}
		return false;
	}

	void UnhookAll() {
		m_dhooks.clear();
		m_vhooks.clear();
	}

	template<typename F>
	int GetVTableIndex(F func) {
		return poly::GetVTableIndex((void*&) func);
	}

private:
	std::unordered_map<plg::string, std::unique_ptr<poly::Hook>> m_dhooks;
	std::unordered_map<std::pair<void*, void*>, std::unique_ptr<poly::Hook>, utils::pair_hash<void*, void*>> m_vhooks;
};

extern HookHolder g_PH;