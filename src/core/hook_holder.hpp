#pragma once

#include "game_config.hpp"
#include <core/sdk/utils.h>

#include <plugify/polyhook.hpp>

class CHookHolder {
public:
	template<typename F>
	bool AddHookMemFunc(F func, void* ptr, const std::function<void(const poly::CHook&)>& callback = {}) {
		std::pair name{(void*&) func, ptr};

		auto it = m_vhooks.find(name);
		if (it != m_vhooks.end()) {
			callback(*it->second);
			return true;
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		auto ihook = poly::CHook::FindVirtualByFunc(ptr, (void*&) func);
		if (ihook != nullptr) {
			callback(*ihook);
			return true;
		}

		ihook = poly::CHook::CreateHookVirtualByFunc(ptr, (void*&) func, ret, plg::vector(args.begin(), args.end()));
		if (ihook == nullptr) {
			g_Logger.LogFormat(LS_WARNING, "Could not hook member function \"%s\".\n", typeid(func).name());
			return false;
		}

		callback(*ihook);
		std::lock_guard<std::mutex> lock(m_mutex);
		m_vhooks.emplace(name, std::move(ihook));
		return true;
	}

	template<typename F>
	bool AddHookDetourFunc(const plg::string& name, const std::function<void(poly::CHook&)>& callback = {}) {
		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			callback(*it->second);
			return true;
		}

		auto addr = g_pGameConfig->ResolveSignature(name);
		if (!addr) {
			g_Logger.LogFormat(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return false;
		}

		auto ihook = poly::CHook::FindDetour(addr);
		if (ihook != nullptr) {
			callback(*ihook);
			return true;
		}

		using trait = poly::details::function_traits<F>;
		auto args = trait::args();
		auto ret = trait::ret();

		ihook = poly::CHook::CreateDetourHook(addr, ret, plg::vector(args.begin(), args.end()));
		if (ihook == nullptr) {
			g_Logger.LogFormat(LS_WARNING, "Could not hook detour function \"%s\".\n", name.c_str());
			return false;
		}

		callback(*ihook);
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dhooks.emplace(name, std::move(ihook));
		return true;
	}

	template<typename F, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	bool AddHookMemFunc(F func, void* ptr, C callback, T... types) {
		return AddHookMemFunc<F>(func, ptr, [&](const poly::CHook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		});
	}

	template<typename F, typename C, typename... T>
		requires(std::is_pointer_v<C> && std::is_function_v<std::remove_pointer_t<C>>)
	bool AddHookDetourFunc(const plg::string& name, C callback, T... types) {
		return AddHookDetourFunc<F>(name, [&](const poly::CHook& hook) {
			([&]() { hook.AddCallback(types, callback); }(), ...);
		});
	}

	bool RemoveHookDetourFunc(const plg::string& name) {
		auto it = m_dhooks.find(name);
		if (it != m_dhooks.end()) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_dhooks.erase(it);
			return true;
		}
		return false;
	}

	template<typename F>
	bool RemoveHookMemFunc(F func, void* ptr) {
		auto it = m_vhooks.find(std::pair{(void*&) func, ptr});
		if (it != m_vhooks.end()) {
			std::lock_guard<std::mutex> lock(m_mutex);
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
	std::unordered_map<plg::string, std::unique_ptr<poly::CHook>> m_dhooks;
	std::unordered_map<std::pair<void*, void*>, std::unique_ptr<poly::CHook>, utils::PairHash<void*, void*>> m_vhooks;
	std::mutex m_mutex;
};

extern CHookHolder g_PH;