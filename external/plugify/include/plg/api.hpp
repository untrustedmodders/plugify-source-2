#pragma once

#include <cstdint>

namespace plg {
	constexpr int kApiVersion = 1;

	extern "C" {
		struct PluginContext {
			bool hasUpdate{};
			bool hasStart{};
			bool hasEnd{};
			bool hasDebug{};
		};
	}
}