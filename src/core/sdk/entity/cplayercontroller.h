#pragma once

#if defined(CS2)
#include "ccsplayercontroller.h"
using CPlayerController = CCSPlayerController;
#elif defined(DEADLOCK)
#include "ccitadelplayercontroller.h"
using CPlayerController = CCitadelPlayerController;
#endif