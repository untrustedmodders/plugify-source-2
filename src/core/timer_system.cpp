#include "timer_system.hpp"

#include <edict.h>

double universalTime = 0.0f;
double timerNextThink = 0.0f;
const double engineFixedTickInterval = 0.015625;

CTimer::CTimer(double interval, double execTime, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>& userData)
	: m_callback(callback), m_userData(userData), m_interval(interval), m_execTime(execTime), m_flags(flags) {
}

CTimer::~CTimer() = default;

bool CTimer::KillMe(TimerFlag flag) {
	if (!flag || m_flags & flag) {
		if (m_killMe)
			return true;

		// If were executing, make sure it doesn't run again next time.
		if (m_inExec) {
			m_killMe = true;
			return true;
		}
	}

	return false;
}

void CTimerSystem::OnLevelShutdown() {
	RemoveMapChangeTimers();

	m_hasMapSimulated = false;
	m_hasMapTicked = false;
}

void CTimerSystem::OnGameFrame(bool simulating) {
	if (simulating && m_hasMapTicked) {
		universalTime += gpGlobals->curtime - m_lastTickedTime;
		m_hasMapSimulated = true;
	} else {
		universalTime += engineFixedTickInterval;
	}

	m_lastTickedTime = gpGlobals->curtime;
	m_hasMapTicked = true;

	// Handle timer tick
	if (universalTime >= timerNextThink) {
		RunFrame();
		timerNextThink = CalculateNextThink(timerNextThink, 0.1);
	}
}

double CTimerSystem::CalculateNextThink(double lastThinkTime, double interval) {
	if (universalTime - lastThinkTime - interval <= 0.1) {
		return lastThinkTime + interval;
	} else {
		return universalTime + interval;
	}
}

void CTimerSystem::RunFrame() {
	for (auto it = m_onceOffTimers.begin(); it != m_onceOffTimers.end();) {
		auto& [handle, timer] = *it;
		if (universalTime >= timer.m_execTime) {
			timer.m_inExec = true;
			timer.m_callback(handle, timer.m_userData);
			it = m_onceOffTimers.erase(it);
			continue;
		}

		++it;
	}

	for (auto it = m_repeatTimers.begin(); it != m_repeatTimers.end();) {
		auto& [handle, timer] = *it;
		if (universalTime >= timer.m_execTime) {
			timer.m_inExec = true;
			timer.m_callback(handle, timer.m_userData);

			if (timer.m_killMe) {
				it = m_repeatTimers.erase(it);
				continue;
			}

			timer.m_inExec = false;
			timer.m_execTime = CalculateNextThink(timer.m_execTime, timer.m_interval);
		}

		++it;
	}
}

void CTimerSystem::RemoveMapChangeTimers() {
	for (auto it = m_onceOffTimers.begin(); it != m_onceOffTimers.end();) {
		auto& [_, timer] = *it;
		if (timer.KillMe(TimerFlag::NoMapChange)) {
			it = m_onceOffTimers.erase(it);
		} else {
			++it;
		}
	}

	for (auto it = m_repeatTimers.begin(); it != m_repeatTimers.end();) {
		auto& [_, timer] = *it;
		if (timer.KillMe(TimerFlag::NoMapChange)) {
			it = m_repeatTimers.erase(it);
		} else {
			++it;
		}
	}
}

Handle CTimerSystem::CreateTimer(double interval, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>& userData) {
	double execTime = universalTime + interval;
	Handle handle = CreateHandle();

	std::lock_guard<std::mutex> lock(m_createTimerLock);
	if (flags & TimerFlag::Repeat) {
		m_repeatTimers.emplace(handle, CTimer(interval, execTime, callback, flags, userData));
	} else {
		m_onceOffTimers.emplace(handle, CTimer(interval, execTime, callback, flags, userData));
	}

	return handle;
}

void CTimerSystem::KillTimer(Handle handle) {
	if (auto it = m_repeatTimers.find(handle); it != m_repeatTimers.end()) {
		auto& [_, timer] = *it;
		if (!timer.KillMe(TimerFlag::Default)) {
			std::lock_guard<std::mutex> lock(m_createTimerLock);
			m_repeatTimers.erase(it);
		}
		return;
	}

	if (auto it = m_onceOffTimers.find(handle); it != m_onceOffTimers.end()) {
		auto& [_, timer] = *it;
		if (!timer.KillMe(TimerFlag::Default)) {
			std::lock_guard<std::mutex> lock(m_createTimerLock);
			m_onceOffTimers.erase(it);
		}
		return;
	}

	g_Logger.LogFormat(LS_WARNING, "KillTimer: Timer with handle %u was not found.", handle);
}

double CTimerSystem::GetTickedTime() {
	return universalTime;
}

double CTimerSystem::GetTickedInterval() {
	return engineFixedTickInterval;
}

CTimerSystem g_TimerSystem;