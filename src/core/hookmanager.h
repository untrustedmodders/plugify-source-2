#pragma once

#include <dynohook/dynohook.h>
#include "utils.h"

namespace cs2sdk {
	class CHookManager {
	public:
		template<typename F, typename C, typename... T>
		void AddHookMemFunc(F func, void* ptr, C callback, T... types) {
			using trait = dyno::details::function_traits<F>;
			auto args = trait::args();
			auto ret = trait::ret();
			auto ihook = dyno::CHook::CreateHookVirtualByFunc(ptr, (void*&) func, args, ret);
			if (ihook == nullptr) {
				Log_Error(LOG_GENERAL, "Could not hook \"'%s\".", utils::Demangle(typeid(func).name()).c_str());
			}
			auto& hook = m_hooks.emplace_back(std::move(ihook));
			([&]() { hook->AddCallback(types, callback); }(), ...);
		}

		void UnhookAll() {
			m_hooks.clear();
		}

	private:
		std::vector<std::unique_ptr<dyno::CHook>> m_hooks;
	};

	extern CHookManager g_HookManager;
}