#pragma once

#define TIMER_FLAG_REPEAT (1 << 0)		 /**< Timer will repeat until stopped */
#define TIMER_FLAG_NO_MAPCHANGE (1 << 1) /**< Timer will not carry over mapchanges */

class CTimer;

using TimerCallback = void (*)(CTimer*); // TODO: add user data

class CTimer
{
	friend class CTimerSystem;

public:
	CTimer(float interval, float execTime, TimerCallback callback, int flags);
	~CTimer();

private:
	TimerCallback m_callback;
	float m_interval;
	float m_execTime;
	int m_flags;
	bool m_inExec{};
	bool m_killMe{};
};

class CTimerSystem
{
public:
	CTimerSystem();
	~CTimerSystem();

	void OnLevelShutdown();
	void OnGameFrame(bool simulating);

	void RunFrame();
	void RemoveMapChangeTimers();
	static double CalculateNextThink(double lastThinkTime, float interval);
	static double GetTickedTime();
	static float GetTickedInterval();

	CTimer* CreateTimer(float interval, TimerCallback callback, int flags);
	void KillTimer(CTimer* timer);

private:
	bool m_hasMapTicked{};
	bool m_hasMapSimulated{};
	float m_lastTickedTime{};
	std::vector<CTimer*> m_onceOffTimers;
	std::vector<CTimer*> m_repeatTimers;
};

extern CTimerSystem g_TimerSystem;