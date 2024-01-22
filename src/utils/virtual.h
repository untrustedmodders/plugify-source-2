#pragma once

#include <cstdint>

#define CALL_VIRTUAL(retType, idx, ...) cs2sdk::CallVirtual<retType>(idx, __VA_ARGS__)

namespace cs2sdk {
	template <typename T = void*>
	inline T GetVMethod(uint32_t uIndex, void* pClass) {
		if (!pClass) {
			return T{};
		}

		void** pVTable = *static_cast<void ***>(pClass);
		if (!pVTable) {
			return T{};
		}

		if constexpr (std::is_same_v<T, void>) {
			return;
		} else {
			return reinterpret_cast<T>(pVTable[uIndex]);
		}
	}

	template <typename T, typename... Args>
	inline T CallVirtual(uint32_t uIndex, void* pClass, Args... args) {
#ifdef CS2SDK_PLATFORM_WINDOWS
		auto pFunc = GetVMethod<T(__thiscall *)(void*, Args...)>(uIndex, pClass);
#else
		auto pFunc = GetVMethod<T(*)(void*, Args...)>(uIndex, pClass);
#endif
		if (!pFunc) {
			return T{};
		}

		return pFunc(pClass, args...);
	}
}