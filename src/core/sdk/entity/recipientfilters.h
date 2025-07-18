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

#include "core/player_manager.hpp"
#include "irecipientfilter.h"

// Simple filter for when only 1 recipient is needed
class CSingleRecipientFilter : public IRecipientFilter {
public:
	CSingleRecipientFilter(CPlayerSlot iRecipient, NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false) : m_iRecipient(iRecipient), m_nBufType(nBufType), m_bInitMessage(bInitMessage) {}
	~CSingleRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType() const override { return m_nBufType; }
	bool IsInitMessage() const override { return m_bInitMessage; }
	int GetRecipientCount() const override { return 1; }
	CPlayerSlot GetRecipientIndex(int slot) const override { return m_iRecipient; }

private:
	CPlayerSlot m_iRecipient;
	NetChannelBufType_t m_nBufType;
	bool m_bInitMessage;
};


class CRecipientFilter : public IRecipientFilter {
public:
	CRecipientFilter() {
		m_nBufType = BUF_RELIABLE;
		m_bInitMessage = false;
	}

	CRecipientFilter(IRecipientFilter* source, int iExcept = -1) {
		m_nBufType = source->GetNetworkBufType();
		m_bInitMessage = source->IsInitMessage();
		m_Recipients.RemoveAll();

		for (int i = 0; i < source->GetRecipientCount(); ++i) {
			if (source->GetRecipientIndex(i).Get() != iExcept)
				m_Recipients.AddToTail(source->GetRecipientIndex(i));
		}
	}

	~CRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType() const override { return m_nBufType; }
	bool IsInitMessage() const override { return m_bInitMessage; }
	int GetRecipientCount() const override { return m_Recipients.Count(); }

	CPlayerSlot GetRecipientIndex(int slot) const override {
		if (slot < 0 || slot >= GetRecipientCount())
			return -1;

		return m_Recipients[slot];
	}

	void AddAllPlayers() {
		m_Recipients.RemoveAll();

		for (int i = 0; i < PlayerManager::MaxClients(); ++i) {
			auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(i));
			if (!pPlayer)
				continue;

			AddRecipient(i);
		}
	}

	void AddRecipient(CPlayerSlot slot) {
		// Don't add if it already exists
		if (m_Recipients.Find(slot) != m_Recipients.InvalidIndex())
			return;

		m_Recipients.AddToTail(slot);
	}

	void AddRecipientsFromMask(uint64 mask) {
		m_Recipients.RemoveAll();

		for (int i = 0; i < PlayerManager::MaxClients(); ++i) {
			if (mask & static_cast<uint64>(1 << i)) {
				AddRecipient(CPlayerSlot(i));
			}
		}
	}

private:
	CUtlVectorFixed<CPlayerSlot, MAXPLAYERS> m_Recipients;
	NetChannelBufType_t m_nBufType;
	bool m_bInitMessage;
};