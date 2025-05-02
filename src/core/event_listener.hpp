#pragma once

#include <igameevents.h>

typedef void (*FnEventListenerCallback)(IGameEvent* event);

class CGameEventListener;

extern std::vector<CGameEventListener*> g_vecEventListeners;

class CGameEventListener : public IGameEventListener2
{
public:
	CGameEventListener(FnEventListenerCallback callback, const char* pszEventName) :
		m_callback(callback), m_eventName(pszEventName)
	{
		g_vecEventListeners.emplace_back(this);
	}

	~CGameEventListener() override
	{}

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	void FireGameEvent(IGameEvent* event) override {
		m_callback(event);
	}

	const char* GetEventName() { return m_eventName; }

private:
	FnEventListenerCallback m_callback;
	const char* m_eventName;
};

void RegisterEventListeners();
void UnregisterEventListeners();

#define GAME_EVENT_F(_event)                                          \
	void _event##_callback(IGameEvent*);                              \
	CGameEventListener _event##_listener(_event##_callback, #_event); \
	void _event##_callback(IGameEvent* event)
