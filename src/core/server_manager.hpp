#pragma once

#include <mutex>
#include <thread>

using TaskCallback = void (*)(const plg::vector<plg::any>&);

class ServerManager {
public:
	void AddTaskForNextWorldUpdate(TaskCallback task, const plg::vector<plg::any>& userData);
	void AddTaskForNextFrame(TaskCallback task, const plg::vector<plg::any>& userData);

	void OnGameFrame();
	void OnPreWorldUpdate();

private:
	struct Task {
		TaskCallback callback;
		plg::vector<plg::any> userData;
	};

	std::vector<Task> m_nextWorldUpdateTasks;
	std::vector<Task> m_nextTasks;
	std::mutex m_nextWorldUpdateTasksLock;
	std::mutex m_nextTasksLock;
};

extern ServerManager g_ServerManager;