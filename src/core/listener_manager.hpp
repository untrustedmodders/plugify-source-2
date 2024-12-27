#pragma once

#include <concepts>

enum class HookMode : bool
{
	Pre,
	Post,
};

enum class ResultType : int
{
	Continue = 0,
	Changed = 1,
	Handled = 3,
	Stop = 4,
};

template <class = void>
class CListenerManager;

template <class Ret, class... Args>
class CListenerManager<Ret (*)(Args...)>
{
public:
	using Func = Ret (*)(Args...);

	template <typename Callable>
	bool Register(Callable&& callable)
		requires std::invocable<Callable, Args...>
	{
		// Is the callable already in the vector?
		if (IsRegistered(callable))
		{
			g_Logger.Log(LS_WARNING, "Callback already registered.\n");
			return false;
		}
		else
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callables.emplace_back(callable);
			return true;
		}
	}

	template <typename Callable>
	bool Unregister(Callable&& callable)
		requires std::invocable<Callable, Args...>
	{
		auto index = Find(callable);
		if (index == -1)
		{
			g_Logger.Log(LS_WARNING, "Callback not registered.\n");
			return false;
		}
		else
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callables.erase(m_callables.begin() + index);
			return true;
		}
	}

	template <typename Callable>
	ptrdiff_t Find(Callable&& callable) const
	{
		for (size_t i = 0; i < m_callables.size(); ++i)
		{
			if (callable == m_callables[i])
			{
				return static_cast<ptrdiff_t>(i);
			}
		}
		return -1;
	}

	template <typename Callable>
	bool IsRegistered(Callable&& callable) const
	{
		return Find(callable) != -1;
	}

	void Notify(Args... args) const
	{
		for (size_t i = 0; i < m_callables.size(); ++i)
		{
			m_callables[i](std::forward<Args>(args)...);
		}
	}

	Ret Notify(size_t index, Args... args) const
	{
		return m_callables[index](std::forward<Args>(args)...);
	}

	Ret operator()(size_t index, Args... args) const
	{
		return m_callables[index](std::forward<Args>(args)...);
	}

	void Clear()
	{
		m_callables.clear();
	}

	size_t GetCount() const
	{
		return m_callables.size();
	}

	bool Empty() const
	{
		return m_callables.empty();
	}

private:
	std::vector<Func> m_callables;
	std::mutex m_mutex;
};