#include "server_manager.h"

#include <edict.h>

void CServerManager::OnGameFrame()
{
	if (m_nextTasks.empty())
		return;

	std::lock_guard<std::mutex> lock(m_nextTasksLock);

	g_Logger.LogFormat(LS_DEBUG, "Executing queued tasks of size: %zu on tick number %f\n", m_nextTasks.size(), gpGlobals->tickcount);

	for (const auto& nextTask : m_nextTasks)
	{
		nextTask();
	}

	m_nextTasks.clear();
}

void CServerManager::OnPreWorldUpdate()
{
	if (m_nextWorldUpdateTasks.empty())
		return;

	std::lock_guard<std::mutex> lock(m_nextWorldUpdateTasksLock);

	g_Logger.LogFormat(LS_DEBUG, "Executing queued tasks of size: %zu at time %f\n", m_nextWorldUpdateTasks.size(), gpGlobals->curtime);

	for (const auto& nextWorldUpdateTask : m_nextWorldUpdateTasks)
	{
		nextWorldUpdateTask();
	}

	m_nextWorldUpdateTasks.clear();
}

void CServerManager::AddTaskForNextFrame(TaskCallback task)
{
	std::lock_guard<std::mutex> lock(m_nextTasksLock);
	m_nextTasks.push_back(task);
}

void CServerManager::AddTaskForNextWorldUpdate(TaskCallback task)
{
	std::lock_guard<std::mutex> lock(m_nextWorldUpdateTasksLock);
	m_nextWorldUpdateTasks.push_back(task);
}

CServerManager g_ServerManager;