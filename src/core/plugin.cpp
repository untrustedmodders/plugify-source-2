#include "plugin.h"

namespace cs2sdk {
	Source2SDK g_sdk;
}

EXPOSE_PLUGIN(PLUGIN_API, &cs2sdk::g_sdk)