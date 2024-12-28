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

#include <core/sdk/schema.h>

class CInButtonState {
	DECLARE_SCHEMA_CLASS(CInButtonState);

public:
	SCHEMA_FIELD_POINTER(uint64, m_pButtonStates);

	void GetButtons(uint64 buttons[3]) {
		for (int i = 0; i < 3; ++i) {
			buttons[i] = this->m_pButtonStates[i];
		}
	}

	static bool IsButtonPressed(uint64 buttons[3], uint64 button, bool onlyDown = false) {
		if (onlyDown) {
			return buttons[0] & button;
		} else {
			bool multipleKeys = (button & (button - 1));
			if (multipleKeys) {
				uint64 currentButton = button;
				uint64 key = 0;
				if (button) {
					while (true) {
						if (currentButton & 1) {
							uint64 keyMask = 1ull << key;
							EInButtonState keyState = (EInButtonState) (keyMask && buttons[0] + (keyMask && buttons[1]) * 2 + (keyMask && buttons[2]) * 4);
							if (keyState > IN_BUTTON_DOWN_UP) {
								return true;
							}
						}
						key++;
						currentButton >>= 1;
						if (!currentButton) {
							return !!(buttons[0] & button);
						}
					}
				}
				return false;
			} else {
				EInButtonState keyState = (EInButtonState) (!!(button & buttons[0]) + !!(button & buttons[1]) * 2 + !!(button & buttons[2]) * 4);
				if (keyState > IN_BUTTON_DOWN_UP) {
					return true;
				}
				return !!(buttons[0] & button);
			}
		}
	}

	bool IsButtonPressed(uint64 button, bool onlyDown) { return CInButtonState::IsButtonPressed(this->m_pButtonStates, button, onlyDown); }
};
