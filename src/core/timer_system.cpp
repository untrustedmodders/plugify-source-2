#include "timer_system.h"

#include <edict.h>

double universalTime = 0.0f;
double timerNextThink = 0.0f;
const float engineFixedTickInterval = 0.015625f;

Timer::Timer(float interval, float exec_time, TimerCallback callback, int flags) : m_callback(callback), m_interval(interval), m_execTime(exec_time), m_flags(flags)
{
}

Timer::~Timer() = default;

TimerSystem::TimerSystem() = default;

TimerSystem::~TimerSystem()
{
	for (auto timer : m_onceOffTimers)
	{
		delete timer;
	}

	for (auto timer : m_repeatTimers)
	{
		delete timer;
	}
}

void TimerSystem::OnLevelShutdown()
{
	RemoveMapChangeTimers();

	m_hasMapSimulated = false;
	m_hasMapTicked = false;
}

void TimerSystem::OnGameFrame(bool simulating)
{
	if (simulating && m_hasMapTicked)
	{
		universalTime += gpGlobals->curtime - m_lastTickedTime;
		m_hasMapSimulated = true;
	}
	else
	{
		universalTime += engineFixedTickInterval;
	}

	m_lastTickedTime = gpGlobals->curtime;
	m_hasMapTicked = true;

	// Handle timer tick
	if (universalTime >= timerNextThink)
	{
		RunFrame();
		timerNextThink = CalculateNextThink(timerNextThink, 0.1f);
	}
}

double TimerSystem::CalculateNextThink(double lastThinkTime, float interval)
{
	if (universalTime - lastThinkTime - interval <= 0.1)
	{
		return lastThinkTime + interval;
	}
	else
	{
		return universalTime + interval;
	}
}

void TimerSystem::RunFrame()
{
	for (int i = static_cast<int>(m_onceOffTimers.size()) - 1; i >= 0; i--)
	{
		auto timer = m_onceOffTimers[i];
		if (universalTime >= timer->m_execTime)
		{
			timer->m_inExec = true;
			timer->m_callback(timer);

			m_onceOffTimers.erase(m_onceOffTimers.begin() + i);
			delete timer;
		}
	}

	for (int i = static_cast<int>(m_repeatTimers.size()) - 1; i >= 0; i--)
	{
		auto timer = m_repeatTimers[i];
		if (universalTime >= timer->m_execTime)
		{
			timer->m_inExec = true;
			timer->m_callback(timer);

			if (timer->m_killMe)
			{
				m_repeatTimers.erase(m_repeatTimers.begin() + i);
				delete timer;
				continue;
			}

			timer->m_inExec = false;
			timer->m_execTime = static_cast<float>(CalculateNextThink(timer->m_execTime, timer->m_interval));
		}
	}
}

void TimerSystem::RemoveMapChangeTimers()
{
	for (int i = static_cast<int>(m_onceOffTimers.size()) - 1; i >= 0; i--)
	{
		auto timer = m_onceOffTimers[i];
		if (timer->m_flags & TIMER_FLAG_NO_MAPCHANGE)
		{
			if (timer->m_killMe)
				continue;

			// If were executing, make sure it doesn't run again next time.
			if (timer->m_inExec)
			{
				timer->m_killMe = true;
				continue;
			}

			m_onceOffTimers.erase(m_onceOffTimers.begin() + i);
			delete timer;
		}
	}

	for (int i = static_cast<int>(m_repeatTimers.size()) - 1; i >= 0; i--)
	{
		auto timer = m_repeatTimers[i];
		if (timer->m_flags & TIMER_FLAG_NO_MAPCHANGE)
		{
			if (timer->m_killMe)
				continue;

			// If were executing, make sure it doesn't run again next time.
			if (timer->m_inExec)
			{
				timer->m_killMe = true;
				continue;
			}

			m_repeatTimers.erase(m_repeatTimers.begin() + i);
			delete timer;
		}
	}
}

Timer* TimerSystem::CreateTimer(float interval, TimerCallback callback, int flags)
{
	float execTime = static_cast<float>(universalTime) + interval;

	auto timer = new Timer(interval, execTime, callback, flags);

	if (flags & TIMER_FLAG_REPEAT)
	{
		m_repeatTimers.push_back(timer);
	}
	else
	{
		m_onceOffTimers.push_back(timer);
	}

	return timer;
}

void TimerSystem::KillTimer(Timer* timer)
{
	if (!timer)
		return;

	auto it1 = std::find(m_repeatTimers.begin(), m_repeatTimers.end(), timer);
	auto it2 = std::find(m_onceOffTimers.begin(), m_onceOffTimers.end(), timer);
	if (it1 == m_repeatTimers.end() && it2 == m_onceOffTimers.end())
		return;

	if (timer->m_killMe)
		return;

	// If were executing, make sure it doesn't run again next time.
	if (timer->m_inExec)
	{
		timer->m_killMe = true;
		return;
	}

	if (timer->m_flags & TIMER_FLAG_REPEAT)
	{
		if (it1 != m_repeatTimers.end())
			m_repeatTimers.erase(it1);
	}
	else
	{
		if (it2 != m_onceOffTimers.end())
			m_onceOffTimers.erase(it2);
	}

	delete timer;
}

double TimerSystem::GetTickedTime()
{
	return universalTime;
}

float TimerSystem::GetTickedInterval()
{
	return engineFixedTickInterval;
}

TimerSystem g_TimerSystem;