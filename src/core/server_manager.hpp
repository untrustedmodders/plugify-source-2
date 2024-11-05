#pragma once

#include <mutex>
#include <thread>

using TaskCallback = void (*)();

class CServerManager
{
public:
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

extern CServerManager g_ServerManager;