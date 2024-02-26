#pragma once

extern "C" PLUGIN_API Timer* CreateTimer(float interval, TimerCallback callback, int flags)
{
	return g_TimerSystem.CreateTimer(interval, callback, flags);
}

extern "C" PLUGIN_API void KillsTimer(Timer* timer)
{
	g_TimerSystem.KillTimer(timer);
}