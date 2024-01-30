#pragma once

#include <igameevents.h>

namespace cs2sdk {
	typedef void (*FnEventListenerCallback)(IGameEvent* event);

	class CGameEventListener : public IGameEventListener2 {
	public:
		CGameEventListener(FnEventListenerCallback callback, const char* pszEventName) : m_Callback{callback}, m_pszEventName{pszEventName} {
		}

		~CGameEventListener() override {
		}

		// FireEvent is called by EventManager if event just occured
		// KeyValue memory will be freed by manager if not needed anymore
		void FireGameEvent(IGameEvent* event) override {
			m_Callback(event);
		}

		const char* GetEventName() { return m_pszEventName; }

	private:
		FnEventListenerCallback m_Callback;
		const char* m_pszEventName;
	};
}

