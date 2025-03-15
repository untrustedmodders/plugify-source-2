#include "server_manager.hpp"

#include <edict.h>

void CServerManager::OnGameFrame() {
	if (m_nextTasks.empty())
		return;

	std::lock_guard<std::mutex> lock(m_nextTasksLock);

	//S2_LOGF(LS_DEBUG, "Executing queued tasks of size: %zu on tick number %f\n", m_nextTasks.size(), gpGlobals->tickcount);

	for (const auto& [nextTask, userData]: m_nextTasks) {
		nextTask(userData);
	}

	m_nextTasks.clear();
}

void CServerManager::OnPreWorldUpdate() {
	if (m_nextWorldUpdateTasks.empty())
		return;

	std::lock_guard<std::mutex> lock(m_nextWorldUpdateTasksLock);

	//S2_LOGF(LS_DEBUG, "Executing queued tasks of size: %zu at time %f\n", m_nextWorldUpdateTasks.size(), gpGlobals->curtime);

	for (const auto& [nextTask, userData]: m_nextWorldUpdateTasks) {
		nextTask(userData);
	}

	m_nextWorldUpdateTasks.clear();
}

void CServerManager::AddTaskForNextFrame(TaskCallback task, const plg::vector<plg::any>& userData) {
	std::lock_guard<std::mutex> lock(m_nextTasksLock);
	m_nextTasks.emplace_back(task, userData);
}

void CServerManager::AddTaskForNextWorldUpdate(TaskCallback task, const plg::vector<plg::any>& userData) {
	std::lock_guard<std::mutex> lock(m_nextWorldUpdateTasksLock);
	m_nextWorldUpdateTasks.emplace_back(task, userData);
}

CServerManager g_ServerManager;