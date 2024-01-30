#include "eventmanager.h"

namespace cs2sdk {
	CEventManager g_EventManager;
}

using namespace cs2sdk;

dyno::ReturnAction CEventManager::Hook_OnFireEvent(dyno::CallbackType type, dyno::IHook& hook) {

	return dyno::ReturnAction::Ignored;
}
