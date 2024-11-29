/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2024 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <entity2/entitysystem.h>
#include <interfaces/interfaces.h>
#include <irecipientfilter.h>
#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/utils.h>

class CBroadcastRecipientFilter : public IRecipientFilter
{
public:
	CBroadcastRecipientFilter(bool bInitMessage = false) : m_bInitMessage(bInitMessage)
	{
		m_Recipients.RemoveAll();
		if (!g_pEntitySystem)
		{
			return;
		}

		for (int i = 0; i <= gpGlobals->maxClients; ++i)
		{
			CBaseEntity* pEntity = static_cast<CBaseEntity*>(g_pEntitySystem->GetEntityInstance(CEntityIndex(i)));
			if (pEntity)
			{
				m_Recipients.AddToTail(i);
			}
		}
	}

	~CBroadcastRecipientFilter() override {}

	bool IsInitMessage() const override { return m_bInitMessage; }

	int GetRecipientCount() const override { return m_Recipients.Count(); }

	CPlayerSlot GetRecipientIndex(int slot) const override { return m_Recipients[slot]; }

	NetChannelBufType_t GetNetworkBufType() const override { return BUF_DEFAULT; }

private:
	bool m_bInitMessage;
	CUtlVector<CPlayerSlot> m_Recipients;
};

class CSingleRecipientFilter : public IRecipientFilter
{
public:
	CSingleRecipientFilter(CPlayerSlot iRecipient, bool bInitMessage = false) : m_bInitMessage(bInitMessage), m_iRecipient(iRecipient) {}

	~CSingleRecipientFilter() override {}

	bool IsInitMessage() const override { return m_bInitMessage; }

	int GetRecipientCount() const override { return 1; }

	CPlayerSlot GetRecipientIndex(int slot) const override { return m_iRecipient; }

	NetChannelBufType_t GetNetworkBufType() const override { return BUF_DEFAULT; }

private:
	bool m_bInitMessage;
	CPlayerSlot m_iRecipient;
};

class CCopyRecipientFilter : public IRecipientFilter
{
public:
	CCopyRecipientFilter(IRecipientFilter* source, int iExcept)
	{
		m_bInitMessage = source->IsInitMessage();
		m_Recipients.RemoveAll();

		for (int i = 0; i < source->GetRecipientCount(); ++i)
		{
			if (source->GetRecipientIndex(i) != iExcept)
				m_Recipients.AddToTail(source->GetRecipientIndex(i));
		}
	}

	~CCopyRecipientFilter() override {}

	bool IsInitMessage() const override { return m_bInitMessage; }

	int GetRecipientCount() const override { return m_Recipients.Count(); }

	CPlayerSlot GetRecipientIndex(int slot) const override
	{
		if (slot < 0 || slot >= GetRecipientCount())
			return -1;

		return m_Recipients[slot];
	}

private:
	bool m_bInitMessage;
	CUtlVectorFixed<CPlayerSlot, 64> m_Recipients;
};
