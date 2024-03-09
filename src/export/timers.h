#pragma once

extern "C" PLUGIN_API CTimer* CreateTimer(float interval, TimerCallback callback, int flags)
{
	return g_TimerSystem.CreateTimer(interval, callback, flags);
}

extern "C" PLUGIN_API void KillsTimer(CTimer* timer)
{
	g_TimerSystem.KillTimer(timer);
}