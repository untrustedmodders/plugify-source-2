#pragma once

#include <string>

namespace wizard {
	constexpr int kApiVersion = 1;

	using InitFunc = int (*)(void**, int);
	using StartFunc = void (*)();
	using EndFunc = void (*)();

	using GetMethodFn = void* (*)(const std::string&);

	class IPluginEntry {
	protected:
		~IPluginEntry() = default;

	public:
		virtual void OnPluginStart() {};
		virtual void OnPluginEnd() {};
	};

	IPluginEntry* GetPluginEntry();

	extern GetMethodFn GetMethod;
}

#define EXPOSE_PLUGIN(plugin_api, interface_addr) namespace wizard { \
	GetMethodFn GetMethod{ nullptr }; \
	extern "C" \
	plugin_api int Wizard_Init(void** api, int version) { \
		if (version < kApiVersion) { \
			return kApiVersion; \
		} \
		GetMethod = reinterpret_cast<GetMethodFn>(api[0]); \
		return 0; \
	} \
	extern "C" \
	plugin_api void Wizard_PluginStart() { \
		GetPluginEntry()->OnPluginStart(); \
	} \
	extern "C" \
	plugin_api void Wizard_PluginEnd() { \
		GetPluginEntry()->OnPluginEnd(); \
	} \
	wizard::IPluginEntry* GetPluginEntry() { \
		return interface_addr; \
	} \
}
