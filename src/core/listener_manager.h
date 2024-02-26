#pragma once

#include <concepts>

enum class HookMode : bool
{
	Pre,
	Post,
};

enum class ResultType : uint8_t
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
			g_Logger.Error("Callback already registered.");
			return false;
		}
		else
		{
			m_Callables.emplace_back(callable);
			return true;
		}
	}

	template <typename Callable>
	bool Unregister(Callable&& callable)
		requires std::invocable<Callable, Args...>
	{
		auto index = Find(callable);
		if (!index.has_value())
		{
			g_Logger.Error("Callback not registered.");
			return false;
		}
		else
		{
			m_Callables.erase(m_Callables.begin() + static_cast<ptrdiff_t>(*index));
			return true;
		}
	}

	template <typename Callable>
	std::optional<size_t> Find(Callable&& callable) const
	{
		for (size_t i = 0; i < m_Callables.size(); ++i)
		{
			if (callable == m_Callables[i])
			{
				return i;
			}
		}
		return {};
	}

	template <typename Callable>
	bool IsRegistered(Callable&& callable) const
	{
		return Find(callable).has_value();
	}

	void Notify(Args... args) const
	{
		for (size_t i = 0; i < m_Callables.size(); ++i)
		{
			m_Callables[i](std::forward<Args>(args)...);
		}
	}

	Ret Notify(size_t index, Args... args) const
	{
		return m_Callables[index](std::forward<Args>(args)...);
	}

	Ret operator()(size_t index, Args... args) const
	{
		return m_Callables[index](std::forward<Args>(args)...);
	}

	void Clear()
	{
		m_Callables.clear();
	}

	size_t GetCount() const
	{
		return m_Callables.size();
	}

	bool Empty() const
	{
		return m_Callables.empty();
	}

private:
	std::vector<Func> m_Callables;
};