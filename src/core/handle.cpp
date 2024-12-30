#include "handle.hpp"

static std::atomic<Handle> s_nextHandle = static_cast<Handle>(-1);

Handle CreateHandle() {
	return ++s_nextHandle;
}
