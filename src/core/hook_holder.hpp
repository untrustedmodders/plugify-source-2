#pragma once

#include "game_config.hpp"
#include <core/sdk/utils.h>

#include <polyhook/polyhook.hpp>

class HookHolder {
public:
	template<typename F, typename C>
	void* AddHookMemFunc(F func, void* ptr, const C& callback, int varIndex = -1) {
		std::pair name{(void*&) func, ptr};

		auto it = m_vhooks.find(name);
		if (it != m_vhooks.end()) {
			callback(*it->second);
			return it->second->GetAddress();
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		auto ihook = poly::Hook::FindVirtualByFunc(ptr, (void*&) func);
		if (ihook != nullptr) {
			callback(*ihook);
			return ihook->GetAddress();
		}

		ihook = poly::Hook::CreateHookVirtualByFunc(ptr, (void*&) func, ret, plg::vector(args.begin(), args.end()), varIndex);
		if (ihook == nullptr) {
#if PLUGIFY_IS_DEBUG
			S2_LOGF(LS_WARNING, "Could not hook member function \"{}\".\n", ptr);
#else
			S2_LOGF(LS_WARNING, "Could not hook member function \"{}\".\n", typeid(func).name());
#endif
			return nullptr;
		}

		callback(*ihook);
		void* orig = ihook->GetAddress();
		m_vhooks.emplace(name, std::move(ihook));
		return orig;
	}

	template<typename F, typename C>
	void* AddHookDetourFunc(std::string_view name, const C& callback, int varIndex = -1) {
		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			callback(*it->second);
			return it->second->GetAddress();
		}

		auto addr = g_pGameConfig->ResolveSignature(name);
		if (!addr) {
			S2_LOGF(LS_WARNING, "Could not hook detour function \"{}\".\n", name);
			return nullptr;
		}

		auto ihook = poly::Hook::FindDetour(addr);
		if (ihook != nullptr) {
			callback(*ihook);
			return ihook->GetAddress();
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		ihook = poly::Hook::CreateDetourHook(addr, ret, plg::vector(args.begin(), args.end()), varIndex);
		if (ihook == nullptr) {
			S2_LOGF(LS_WARNING, "Could not hook detour function \"{}\".\n", name);
			return nullptr;
		}

		callback(*ihook);
		void* orig = ihook->GetAddress();
		m_dhooks.emplace(name, std::move(ihook));
		return orig;
	}

	template<typename F, typename C>
	void* AddHookDetourFunc(uintptr_t addr, const C& callback, int varIndex = -1) {
		plg::string name = std::format("0x{:x}", addr);

		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			callback(*it->second);
			return it->second->GetAddress();
		}

		auto ihook = poly::Hook::FindDetour((void*)addr);
		if (ihook != nullptr) {
			callback(*ihook);
			return ihook->GetAddress();
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		ihook = poly::Hook::CreateDetourHook((void*)addr, ret, plg::vector(args.begin(), args.end()), varIndex);
		if (ihook == nullptr) {
			S2_LOGF(LS_WARNING, "Could not hook detour function \"{}\".\n", name);
			return nullptr;
		}

		callback(*ihook);
		void* orig = ihook->GetAddress();
		m_dhooks.emplace(std::move(name), std::move(ihook));
		return orig;
	}

	template<typename F, int V = -1, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	void* AddHookMemFunc(F func, void* ptr, C callback, T... types) {
		return AddHookMemFunc<F>(func, ptr, [&](const poly::Hook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		}, V);
	}

	template<typename F, int V = -1, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	void* AddHookDetourFunc(std::string_view name, C callback, T... types) {
		return AddHookDetourFunc<F>(name, [&](const poly::Hook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		}, V);
	}

	template<typename F, int V = -1, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	void* AddHookDetourFunc(uintptr_t addr, C callback, T... types) {
		return AddHookDetourFunc<F>(addr, [&](const poly::Hook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		}, V);
	}

	bool RemoveHookDetourFunc(std::string_view name) {
		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			m_dhooks.erase(it);
			return true;
		}
		return false;
	}

	template<typename F>
	bool RemoveHookMemFunc(F func, void* ptr) {
		auto it = m_vhooks.find({(void*&) func, ptr});
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
	std::unordered_map<plg::string, std::unique_ptr<poly::Hook>, utils::string_hash,std::equal_to<>> m_dhooks;
	std::unordered_map<std::pair<void*, void*>, std::unique_ptr<poly::Hook>, utils::pair_hash<void*, void*>> m_vhooks;
};

extern HookHolder g_PH;