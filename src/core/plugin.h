#pragma once

#include <wizard/cpp_plugin.h>
#include <plugin_export.h>

namespace cs2sdk {
	class Source2SDK : public wizard::IPluginEntry {
	public:
		void OnPluginStart() override {
			std::cout << "Source2SDK Start!" << std::endl;
			globals::Initialize();
		}

		void OnPluginEnd() override {
			std::cout << "Source2SDK End!" << std::endl;
			globals::Terminate();
		}
	};

	extern Source2SDK g_sdk;
}