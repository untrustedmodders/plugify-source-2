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

#include "ccsplayerpawn.h"
#include "services.h"

class CCSObserver_ObserverServices : public CPlayer_ObserverServices {};

class CCSObserver_MovementServices : public CPlayer_MovementServices {};

class CCSObserver_UseServices : public CPlayer_UseServices {};

class CCSObserver_ViewModelServices : public CPlayer_ViewModelServices {};

class CCSObserver_CameraServices : public CCSPlayerBase_CameraServices {};

class CCSObserverPawn : public CCSPlayerPawnBase {
public:
	DECLARE_SCHEMA_CLASS(CCSObserverPawn);

	SCHEMA_FIELD(CCSObserver_ObserverServices*, m_pObserverServices)
	SCHEMA_FIELD(CCSObserver_MovementServices*, m_pMovementServices)
	SCHEMA_FIELD(CCSObserver_UseServices*, m_pUseServices)
	SCHEMA_FIELD(CCSObserver_ViewModelServices*, m_pViewModelServices)
	SCHEMA_FIELD(CCSObserver_CameraServices*, m_pCameraServices)
};