#pragma once

#include <mutex>
#include <thread>

using TaskCallback = void (*)();

class ServerManager
{
public:
	ServerManager() = default;
	~ServerManager() = default;

	void AddTaskForNextWorldUpdate(TaskCallback task);
	void AddTaskForNextFrame(TaskCallback task);

	void OnGameFrame();
	void OnPreWorldUpdate();

private:
	std::vector<TaskCallback> m_nextWorldUpdateTasks;
	std::vector<TaskCallback> m_nextTasks;
	std::mutex m_nextWorldUpdateTasksLock;
	std::mutex m_nextTasksLock;
};

extern ServerManager g_ServerManager;