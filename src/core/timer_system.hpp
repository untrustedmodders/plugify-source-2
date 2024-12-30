#pragma once

enum TimerFlag : int {
	Default = 0,
	Repeat = (1 << 0),
	NoMapChange = (1 << 1)
};

class CTimer;

using TimerCallback = void (*)(Handle, const plg::vector<plg::any>&);

class CTimer {
	friend class CTimerSystem;

public:
	CTimer() = delete;
	CTimer(double interval, double execTime, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>& userData);
	~CTimer();

private:
	bool KillMe(TimerFlag flag);

	TimerCallback m_callback;
	double m_execTime;
	double m_interval;
	TimerFlag m_flags;
	bool m_inExec{};
	bool m_killMe{};
	plg::vector<plg::any> m_userData;
};

class CTimerSystem {
public:
	void OnLevelShutdown();
	void OnGameFrame(bool simulating);

	void RunFrame();
	void RemoveMapChangeTimers();
	static double CalculateNextThink(double lastThinkTime, double interval);
	static double GetTickedTime();
	static double GetTickedInterval();

	Handle CreateTimer(double interval, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>&);
	void KillTimer(Handle handle);

private:
	bool m_hasMapTicked{};
	bool m_hasMapSimulated{};
	float m_lastTickedTime{};
	std::unordered_map<Handle, CTimer> m_onceOffTimers;
	std::unordered_map<Handle, CTimer> m_repeatTimers;
	std::mutex m_createTimerLock;
};

extern CTimerSystem g_TimerSystem;