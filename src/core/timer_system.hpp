#pragma once

enum TimerFlag : int {
	Default = 0,
	Repeat = (1 << 0),
	NoMapChange = (1 << 1)
};

class CTimer;

using TimerCallback = void (*)(uint32_t, const plg::vector<plg::any>&);

/*class CTimer {
	friend class CTimerSystem;

public:
	CTimer() = delete;
	CTimer(double interval, double execTime, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>& userData);
	~CTimer();

private:
	bool KillMe(TimerFlag flag);

	double m_interval;
	double m_execTime;
	TimerCallback m_callback;
	TimerFlag m_flags;
	bool m_inExec{};
	bool m_killMe{};
	plg::vector<plg::any> m_userData;
};*/

struct Timer {
	uint32_t id;
	TimerFlag flags;
	double executeTime;
	double delay;
	TimerCallback callback;
	plg::vector<plg::any> userData;

	bool operator<(const Timer& other) const {
		return executeTime < other.executeTime ||
			   (executeTime == other.executeTime && id < other.id);
	}
};

class CTimerSystem {
public:
	void OnLevelShutdown();
	void OnGameFrame(bool simulating);

	void RunFrame();
	void RemoveMapChangeTimers();
	static double CalculateNextThink(double lastThinkTime, double delay);
	static double GetTickedTime();
	static double GetTickedInterval();

	uint32_t CreateTimer(double delay, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>&);
	void KillTimer(uint32_t id);
	void RescheduleTimer(uint32_t id, double newDelay);

private:
	bool m_hasMapTicked{};
	bool m_hasMapSimulated{};
	float m_lastTickedTime{};
	std::set<Timer> m_timers;
	std::mutex m_createTimerLock;
	static inline uint32_t s_nextId = static_cast<uint32_t>(-1);
};

extern CTimerSystem g_TimerSystem;