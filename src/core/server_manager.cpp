#include "server_manager.h"

#include <edict.h>

void ServerManager::OnGameFrame()
{
	if (m_nextTasks.empty())
		return;

	std::lock_guard<std::mutex> lock(m_nextTasksLock);

	g_Logger.MessageFormat("Executing queued tasks of size: %zu on tick number %f\n", m_nextTasks.size(), gpGlobals->tickcount);

	for (const auto& nextTask : m_nextTasks) {
		nextTask();
	}

	m_nextTasks.clear();
}

void ServerManager::OnPreWorldUpdate()
{
	if (m_nextWorldUpdateTasks.empty())
		return;

	std::lock_guard<std::mutex> lock(m_nextWorldUpdateTasksLock);

	g_Logger.MessageFormat("Executing queued tasks of size: %zu at time %f\n", m_nextWorldUpdateTasks.size(), gpGlobals->curtime);

	for (const auto& nextWorldUpdateTask : m_nextWorldUpdateTasks)
	{
		nextWorldUpdateTask();
	}

	m_nextWorldUpdateTasks.clear();
}

void ServerManager::AddTaskForNextFrame(TaskCallback task)
{
	std::lock_guard<std::mutex> lock(m_nextTasksLock);
	m_nextTasks.push_back(task);
}

void ServerManager::AddTaskForNextWorldUpdate(TaskCallback task)
{
	std::lock_guard<std::mutex> lock(m_nextWorldUpdateTasksLock);
	m_nextWorldUpdateTasks.push_back(task);
}

ServerManager g_ServerManager;