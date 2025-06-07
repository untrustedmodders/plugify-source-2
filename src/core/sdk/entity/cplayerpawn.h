#pragma once

#if defined(CS2)
#include "ccsplayerpawn.h"
using CPlayerPawn = CCSPlayerPawn;
#elif defined(DEADLOCK)
#include "ccitadelplayerpawn.h"
using CPlayerPawn = CCitadelPlayerPawn;
#endif