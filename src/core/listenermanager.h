#pragma once

#include <concepts>
#include <plugin_export.h>
#include <utlvector.h>

namespace cs2sdk {
	template <class = void>
	class CListenerManager;

	template<class Ret, class... Args>
	class CListenerManager<Ret(*)(Args...)> {
	public:
		using Func = Ret(*)(Args...);

		template<typename Callable>
		bool Register(Callable&& callable) requires std::invocable<Callable, Args...> {
			// Is the callable already in the vector?
			if(IsRegistered(callable)) {
				Error("Callback already registered.");
				return false;
			} else {
				m_vecCallables.AddToTail(callable);
				return true;
			}
		}

		template<typename Callable>
		bool Unregister(Callable&& callable) requires std::invocable<Callable, Args...> {
			int index = Find(callable);
			if (index == -1) {
				Error("Callback not registered.");
				return false;
			} else {
				m_vecCallables.Remove(index);
				return true;
			}
		}

		template <typename Callable>
		bool IsRegistered(Callable&& callable) {
			return Find(callable) != -1;
		}

		template <typename Callable>
		int Find(Callable&& callable) {
			for (int i = 0; i < m_vecCallables.Count(); ++i) {
				if (callable == m_vecCallables[i]) {
					return i;
				}
			}
			return -1;
		}

		void Notify(Args... args) const {
			for (int i = 0; i < m_vecCallables.Count(); ++i) {
				m_vecCallables[i](std::forward<Args>(args)...);
			}
		}

		Ret operator()(size_t index, Args... args) const {
			return m_vecCallables[index](std::forward<Args>(args)...);
		}

		void Clear() {
			if (GetCount()) {
				m_vecCallables.RemoveAll();
			}
		}

		int GetCount() {
			return m_vecCallables.Count();
		}

	private:
		CUtlVector<Func> m_vecCallables;
	};
}