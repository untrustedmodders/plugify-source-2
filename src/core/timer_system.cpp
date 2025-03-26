#include "timer_system.hpp"

#include <edict.h>

double universalTime = 0.0f;
double timerNextThink = 0.0f;
const double engineFixedTickInterval = 0.015625;

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

double CTimerSystem::CalculateNextThink(double lastThinkTime, double delay) {
	if (universalTime - lastThinkTime - delay <= 0.1) {
		return lastThinkTime + delay;
	} else {
		return universalTime + delay;
	}
}

void CTimerSystem::RunFrame() {
	std::lock_guard<std::mutex> lock(m_createTimerLock);

	while (!m_timers.empty()) {
		auto it = m_timers.begin();

		if (universalTime >= it->executeTime) {
			it->callback(it->id, it->userData);

			if (it->flags & TimerFlag::Repeat) {
				auto node = m_timers.extract(it);
				node.value().executeTime = universalTime + node.value().delay;
				m_timers.insert(std::move(node));
			} else {
				m_timers.erase(it); // Only erase non-repeating tasks
			}
		} else {
			break;
		}
	}
}

void CTimerSystem::RemoveMapChangeTimers() {
	for (auto it = m_timers.begin(); it != m_timers.end();) {
		if (it->flags & TimerFlag::NoMapChange) {
			it = m_timers.erase(it);
		} else {
			++it;
		}
	}
}

uint32_t CTimerSystem::CreateTimer(double delay, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>& userData) {
	std::lock_guard<std::mutex> lock(m_createTimerLock);
	
	uint32_t id = ++s_nextId;
	m_timers.emplace(id, flags, universalTime + delay, delay, callback, userData);
	return id;
}

void CTimerSystem::KillTimer(uint32_t id) {
	std::lock_guard<std::mutex> lock(m_createTimerLock);

	auto it = std::find_if(m_timers.begin(), m_timers.end(), [id](const Timer& timer) {
		return timer.id == id;
	});

	if (it != m_timers.end()) {
		m_timers.erase(it);
	}
}

void CTimerSystem::RescheduleTimer(uint32_t id, double newDelay) {
	std::lock_guard<std::mutex> lock(m_createTimerLock);

	auto it = std::find_if(m_timers.begin(), m_timers.end(), [id](const Timer& timer) {
		return timer.id == id;
	});

	if (it != m_timers.end()) {
		auto node = m_timers.extract(it);
		node.value().delay = newDelay;
		node.value().executeTime = universalTime + newDelay;
		m_timers.insert(std::move(node));
	}
}


double CTimerSystem::GetTickedTime() {
	return universalTime;
}

double CTimerSystem::GetTickedInterval() {
	return engineFixedTickInterval;
}

CTimerSystem g_TimerSystem;