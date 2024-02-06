#pragma once

#include <concepts>
#include <plugin_export.h>
#include <utlvector.h>

enum class ResultType : uint8_t {
	Continue = 0,
	Changed = 1,
	Handled = 3,
	Stop = 4,
};

template <class = void>
class CListenerManager;

template<class Ret, class... Args>
class CListenerManager<Ret(*)(Args...)> {
public:
	using Func = Ret(*)(Args...);

	template<typename Callable>
	bool Register(Callable&& callable) requires std::invocable<Callable, Args...> {
		// Is the callable already in the vector?
		if (IsRegistered(callable)) {
			Log_Error(LOG_GENERAL, "Callback already registered.");
			return false;
		} else {
			m_Callables.emplace_back(callable);
			return true;
		}
	}

	template<typename Callable>
	bool Unregister(Callable&& callable) requires std::invocable<Callable, Args...> {
		int index = Find(callable);
		if (index == -1) {
			Log_Error(LOG_GENERAL, "Callback not registered.");
			return false;
		} else {
			m_Callables.erase(m_Callables.begin() + index);
			return true;
		}
	}

	template <typename Callable>
	bool IsRegistered(Callable&& callable) {
		return Find(callable) != -1;
	}

	template <typename Callable>
	int Find(Callable&& callable) {
		for (int i = 0; i < m_Callables.size(); ++i) {
			if (callable == m_Callables[i]) {
				return i;
			}
		}
		return -1;
	}

	void Notify(Args... args) const {
		for (int i = 0; i < m_Callables.size(); ++i) {
			m_Callables[i](std::forward<Args>(args)...);
		}
	}

	Ret operator()(size_t index, Args... args) const {
		return m_Callables[index](std::forward<Args>(args)...);
	}

	void Clear() {
		m_Callables.clear();
	}

	int GetCount() {
		return m_Callables.size();
	}

private:
	std::vector<Func> m_Callables;
};