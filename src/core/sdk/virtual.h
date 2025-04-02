#pragma once

#define CALL_VIRTUAL(retType, idx, ...) CallVirtual<retType>(idx, __VA_ARGS__)

template<typename T = void*>
T GetVMethod(uint32_t uIndex, void* pClass) {
	if (!pClass) {
		return T{};
	}

	void** pVTable = *static_cast<void***>(pClass);
	if (!pVTable) {
		return T{};
	}

	return reinterpret_cast<T>(pVTable[uIndex]);
}

template<typename T, typename... Args>
T CallVirtual(uint32_t uIndex, void* pClass, Args... args) {
#if S2SDK_PLATFORM_WINDOWS
	auto pFunc = GetVMethod<T(__thiscall*)(void*, Args...)>(uIndex, pClass);
#else
	auto pFunc = GetVMethod<T (*)(void*, Args...)>(uIndex, pClass);
#endif
	if (!pFunc) {
		if constexpr (std::is_same_v<T, void>) {
			return;
		} else {
			return T{};
		}
	}

	if constexpr (std::is_same_v<T, void>) {
		pFunc(pClass, args...);
	} else {
		return pFunc(pClass, args...);
	}
}