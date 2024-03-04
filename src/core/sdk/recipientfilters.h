#pragma once

#include <entity2/entitysystem.h>
#include <irecipientfilter.h>
#include <interfaces/interfaces.h>
#include <utils/utils.h>

class CBroadcastRecipientFilter : public IRecipientFilter
{
public:
	CBroadcastRecipientFilter(bool bReliable = true, bool bInitMessage = false) :
		m_bReliable(bReliable), m_bInitMessage(bInitMessage)
	{
		m_Recipients.RemoveAll();
		if (!g_pEntitySystem)
		{
			return;
		}

		for (int i = 0; i <= gpGlobals->maxClients; i++)
		{
			CBaseEntity* ent = g_pEntitySystem->GetBaseEntity(CEntityIndex(i));
			if (ent)
			{
				m_Recipients.AddToTail(i);
			}
		}
	}

	~CBroadcastRecipientFilter() override {}

	bool IsReliable(void) const override { return m_bReliable; }

	bool IsInitMessage(void) const override { return m_bInitMessage; }

	int GetRecipientCount(void) const override { return m_Recipients.Count(); }

	int GetRecipientIndex(int slot) const override { return m_Recipients[slot]; }

private:
	bool m_bReliable;
	bool m_bInitMessage;
	CUtlVector<int> m_Recipients;
};

class CSingleRecipientFilter : public IRecipientFilter
{
public:
	CSingleRecipientFilter(int iRecipient, bool bReliable = true, bool bInitMessage = false) :
		m_bReliable(bReliable), m_bInitMessage(bInitMessage), m_iRecipient(iRecipient) {}

	~CSingleRecipientFilter() override {}

	bool IsReliable(void) const override { return m_bReliable; }

	bool IsInitMessage(void) const override { return m_bInitMessage; }

	int GetRecipientCount(void) const override { return 1; }

	int GetRecipientIndex(int slot) const override { return m_iRecipient; }

private:
	bool m_bReliable;
	bool m_bInitMessage;
	int m_iRecipient;
};

class CCopyRecipientFilter : public IRecipientFilter
{
public:
	CCopyRecipientFilter(IRecipientFilter *source, int iExcept)
	{
		m_bReliable = source->IsReliable();
		m_bInitMessage = source->IsInitMessage();
		m_Recipients.RemoveAll();

		for (int i = 0; i < source->GetRecipientCount(); i++)
		{
			if (source->GetRecipientIndex(i) != iExcept)
				m_Recipients.AddToTail(source->GetRecipientIndex(i));
		}
	}

	~CCopyRecipientFilter() override {}

	bool IsReliable(void) const override { return m_bReliable; }

	bool IsInitMessage(void) const override { return m_bInitMessage; }

	int GetRecipientCount(void) const override { return m_Recipients.Count(); }

	int GetRecipientIndex(int slot) const override
	{
		if (slot < 0 || slot >= GetRecipientCount())
			return -1;

		return m_Recipients[slot];
	}

private:
	bool m_bReliable;
	bool m_bInitMessage;
	CUtlVectorFixed<int, 64> m_Recipients;
};
