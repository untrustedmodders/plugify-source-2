#pragma once

#define protected public
#define private public
#include <tier1/convar.h>
#undef protected
#undef private

#include <eiface.h>
#include <iserver.h>
#include <igameeventsystem.h>
#include <igameevents.h>
#include <entitysystem.h>

#include <ISmmPlugin.h>