#include "timer_system.hpp"

#include <edict.h>

double universalTime = 0.0f;
double timerNextThink = 0.0f;
const float engineFixedTickInterval = 0.015625f;

CTimer::CTimer(float interval, float execTime, TimerCallback callback, int flags, const plg::vector<plg::any>& userData)
	: m_callback(callback)
	, m_userData(userData)
	, m_interval(interval)
	, m_execTime(execTime)
	, m_flags(flags)
{
}

CTimer::~CTimer() = default;

CTimerSystem::CTimerSystem() = default;

CTimerSystem::~CTimerSystem()
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

void CTimerSystem::OnLevelShutdown()
{
	RemoveMapChangeTimers();

	m_hasMapSimulated = false;
	m_hasMapTicked = false;
}

void CTimerSystem::OnGameFrame(bool simulating)
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

double CTimerSystem::CalculateNextThink(double lastThinkTime, float interval)
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

void CTimerSystem::RunFrame()
{
	for (int i = static_cast<int>(m_onceOffTimers.size()) - 1; i >= 0; i--)
	{
		auto timer = m_onceOffTimers[i];
		if (universalTime >= timer->m_execTime)
		{
			timer->m_inExec = true;
			timer->m_callback(timer, timer->m_userData);

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
			timer->m_callback(timer, timer->m_userData);

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

void CTimerSystem::RemoveMapChangeTimers()
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

CTimer* CTimerSystem::CreateTimer(float interval, TimerCallback callback, int flags, const plg::vector<plg::any>& userData)
{
	float execTime = static_cast<float>(universalTime) + interval;

	auto timer = new CTimer(interval, execTime, callback, flags, userData);

	std::lock_guard<std::mutex> lock(m_createTimerLock);

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

void CTimerSystem::KillTimer(CTimer* timer)
{
	if (!timer)
		return;

	auto it1 = std::find(m_repeatTimers.begin(), m_repeatTimers.end(), timer);
	auto it2 = std::find(m_onceOffTimers.begin(), m_onceOffTimers.end(), timer);
	if (it1 == m_repeatTimers.end() && it2 == m_onceOffTimers.end())
		return;

	if (timer->m_killMe)
		return;

	std::lock_guard<std::mutex> lock(m_createTimerLock);

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

double CTimerSystem::GetTickedTime()
{
	return universalTime;
}

float CTimerSystem::GetTickedInterval()
{
	return engineFixedTickInterval;
}

CTimerSystem g_TimerSystem;