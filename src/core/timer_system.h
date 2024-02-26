#pragma once

#define TIMER_FLAG_REPEAT (1 << 0)		 /**< Timer will repeat until stopped */
#define TIMER_FLAG_NO_MAPCHANGE (1 << 1) /**< Timer will not carry over mapchanges */

class Timer;

using TimerCallback = void (*)(Timer*); // TODO: add user data

class Timer
{
	friend class TimerSystem;

public:
	Timer(float interval, float execTime, TimerCallback callback, int flags);
	~Timer();

private:
	TimerCallback m_callback;
	float m_interval;
	float m_execTime;
	int m_flags;
	bool m_inExec{};
	bool m_killMe{};
};

class TimerSystem
{
public:
	TimerSystem();
	~TimerSystem();

	void OnLevelShutdown();
	void OnGameFrame(bool simulating);

	void RunFrame();
	void RemoveMapChangeTimers();
	static double CalculateNextThink(double lastThinkTime, float interval);
	static double GetTickedTime();
	static float GetTickedInterval();

	Timer* CreateTimer(float interval, TimerCallback callback, int flags);
	void KillTimer(Timer* timer);

private:
	bool m_hasMapTicked{};
	bool m_hasMapSimulated{};
	float m_lastTickedTime{};
	std::vector<Timer*> m_onceOffTimers;
	std::vector<Timer*> m_repeatTimers;
};

extern TimerSystem g_TimerSystem;