#include "utils.h"

#include <engine/igameeventsystem.h>
#include <networkbasetypes.pb.h>
#include <igameevents.h>
#include <networksystem/inetworkmessages.h>
#include <entity2/entitysystem.h>

#include "entity/globaltypes.h"
#include "entity/cbaseplayerpawn.h"
#include "entity/ccsplayercontroller.h"
#include "entity/recipientfilters.h"

#include <tier0/memdbgon.h>
#undef CreateEvent

CBaseEntity* utils::FindEntityByClassname(CEntityInstance* start, const char* name)
{
	if (!g_pGameEntitySystem)
	{
		return NULL;
	}
	EntityInstanceByClassIter_t iter(start, name);

	return static_cast<CBaseEntity*>(iter.Next());
}

#define FCVAR_FLAGS_TO_REMOVE (FCVAR_HIDDEN | FCVAR_DEVELOPMENTONLY | FCVAR_MISSING0 | FCVAR_MISSING1 | FCVAR_MISSING2 | FCVAR_MISSING3)

CBasePlayerController* utils::GetController(CBaseEntity* entity)
{
	CCSPlayerController* controller = nullptr;

	if (entity->IsPawn())
	{
		CBasePlayerPawn* pawn = static_cast<CBasePlayerPawn*>(entity);
		if (!pawn->m_hController().IsValid() || pawn->m_hController.Get() == nullptr)
		{
			if (!gpGlobals)
				return nullptr;

			// Seems like the pawn lost its controller, we can try looping through the controllers to find this pawn instead.
			for (int i = 0; i <= gpGlobals->maxClients; ++i)
			{
				controller = static_cast<CCSPlayerController*>(utils::GetController(CPlayerSlot(i)));
				if (controller && controller->m_hPlayerPawn() && controller->m_hPlayerPawn().Get() == entity)
				{
					return controller;
				}
			}
			return nullptr;
		}
		return pawn->m_hController.Get();
	}
	else if (entity->IsController())
	{
		return static_cast<CBasePlayerController*>(entity);
	}
	else
	{
		return nullptr;
	}
}

CBasePlayerController* utils::GetController(CPlayerSlot slot)
{
	if (!g_pGameEntitySystem || slot.Get() < 0 || slot.Get() > MAXPLAYERS)
	{
		return nullptr;
	}
	
	CBaseEntity* ent = static_cast<CBaseEntity*>(g_pGameEntitySystem->GetEntityInstance(CEntityIndex(slot.Get() + 1)));
	if (!ent)
	{
		return nullptr;
	}
	
	return ent->IsController() ? static_cast<CBasePlayerController*>(ent) : nullptr;
}

CPlayerSlot utils::GetEntityPlayerSlot(CBaseEntity* entity)
{
	CBasePlayerController* controller = utils::GetController(entity);
	if (!controller)
	{
		return -1;
	}
	else
	{
		return controller->m_pEntity->m_EHandle.GetEntryIndex() - 1;
	}
}

/*CUtlVector<CServerSideClient *>* GetClientList()
{
	if (!g_pNetworkGameServer)
		return nullptr;

	static int offset = g_pGameConfig->GetOffset("GetClientList");
	return CALL_VIRTUAL(CUtlVector<CServerSideClient *> *, offset, g_pNetworkGameServer);
}*/


void utils::PlaySoundToClient(CPlayerSlot player, int channel, const char* soundName, float volume, soundlevel_t soundLevel, int flags, int pitch, const Vector& origin, float soundTime)
{
	CSingleRecipientFilter filter(player.Get());
	EmitSound_t soundParams;
	soundParams.m_nChannel = channel;
	soundParams.m_pSoundName = soundName;
	soundParams.m_flVolume = volume;
	soundParams.m_SoundLevel = soundLevel;
	soundParams.m_nFlags = flags;
	soundParams.m_nPitch = pitch;
	soundParams.m_pOrigin = &origin;
	soundParams.m_flSoundTime = soundTime;
	addresses::CBaseEntity_EmitSoundFilter(filter, player.Get() + 1, soundParams);
}

float utils::NormalizeDeg(float a)
{
	a = fmod(a, 360.0f);
	if (a >= 180.0f)
		a -= 360.0f;
	else if (a < -180.0f)
		a += 360.0f;
	return a;
}

float utils::GetAngleDifference(float source, float target, float c, bool relative)
{
	if (relative)
		return fmod((fmod(target - source, 2 * c) + 3 * c), 2 * c) - c;
	return fmod(fabs(target - source) + c, 2 * c) - c;
}

void utils::NotifyConVar(BaseConVar* conVar, const char* value)
{
	IGameEvent* pEvent = g_pGameEventManager->CreateEvent("server_cvar");
	if (pEvent == nullptr)
	{
		return;
	}

	pEvent->SetString("cvarname", conVar->GetName());
	if (conVar->IsFlagSet(FCVAR_PROTECTED))
	{
		pEvent->SetString("cvarvalue", "***PROTECTED***");
	}
	else
	{
		pEvent->SetString("cvarvalue", value);
	}

	g_pGameEventManager->FireEvent(pEvent);
}

void utils::ReplicateConVar(BaseConVar* conVar, const char* value)
{
	if (!gpGlobals)
		return;

	for (int i = 0; i <= gpGlobals->maxClients; ++i)
	{
		utils::SendConVarValue(CPlayerSlot(i), conVar, value);
	}
}

void utils::SendConVarValue(CPlayerSlot slot, BaseConVar* conVar, const char* value)
{
	INetworkMessageInternal* pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("CNETMsg_SetConVar");
	auto msg = pNetMsg->AllocateMessage()->ToPB<CNETMsg_SetConVar>();
	CMsg_CVars_CVar* cvar = msg->mutable_convars()->add_cvars();
	cvar->set_name(conVar->GetName());
	cvar->set_value(value);

	CSingleRecipientFilter filter(slot);
	g_pGameEventSystem->PostEventAbstract(-1, false, &filter, pNetMsg, msg, 0);

#ifndef _WIN32
	delete msg;
#endif
}

void utils::SendMultipleConVarValues(CPlayerSlot slot, BaseConVar** conVar, const char** value, uint32_t size)
{
	INetworkMessageInternal* pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("CNETMsg_SetConVar");
	auto msg = pNetMsg->AllocateMessage()->ToPB<CNETMsg_SetConVar>();
	for (uint32_t i = 0; i < size; ++i)
	{
		CMsg_CVars_CVar* cvar = msg->mutable_convars()->add_cvars();
		cvar->set_name(conVar[i]->GetName());
		cvar->set_value(value[i]);
	}
	CSingleRecipientFilter filter(slot);
	g_pGameEventSystem->PostEventAbstract(-1, false, &filter, pNetMsg, reinterpret_cast<const CNetMessage*>(&msg), 0);

#ifndef _WIN32
	delete msg;
#endif
}

bool utils::IsSpawnValid(const Vector& origin)
{
	/*bbox_t bounds = {{-16.0f, -16.0f, 0.0f}, {16.0f, 16.0f, 72.0f}};
	CTraceFilterS2 filter;
	filter.attr.m_bHitSolid = true;
	filter.attr.m_bHitSolidRequiresGenerateContacts = true;
	filter.attr.m_bShouldIgnoreDisabledPairs = true;
	filter.attr.m_nCollisionGroup = COLLISION_GROUP_PLAYER_MOVEMENT;
	filter.attr.m_nInteractsWith = 0x2c3011;
	filter.attr.m_bUnkFlag3 = true;
	filter.attr.m_nObjectSetMask = RNQUERY_OBJECTS_ALL;
	filter.attr.m_nInteractsAs = 0x40000;
	trace_t_s2 tr;
	addresses::TracePlayerBBox(origin, origin, bounds, &filter, tr);
	if (tr.fraction != 1.0 || tr.startsolid)
	{
		return false;
	}
	return true;*/
	return false;
}

bool utils::FindValidSpawn(Vector& origin, QAngle& angles)
{
	bool foundValidSpawn = false;
	bool searchCT = false;
	Vector spawnOrigin;
	QAngle spawnAngles;
	CBaseEntity* spawnEntity = nullptr;
	while (!foundValidSpawn)
	{
		if (searchCT)
		{
			spawnEntity = FindEntityByClassname(spawnEntity, "info_player_counterterrorist");
		}
		else
		{
			spawnEntity = FindEntityByClassname(spawnEntity, "info_player_terrorist");
		}

		if (spawnEntity != nullptr)
		{
			spawnOrigin = spawnEntity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin;
			spawnAngles = spawnEntity->m_CBodyComponent->m_pSceneNode->m_angRotation;
			if (utils::IsSpawnValid(spawnOrigin))
			{
				origin = spawnOrigin;
				angles = spawnAngles;
				foundValidSpawn = true;
			}
		}
		else if (!searchCT)
		{
			searchCT = true;
		}
		else
		{
			break;
		}
	}
	return foundValidSpawn;
}

const plg::string& utils::GameDirectory()
{
	static plg::string gameDirectory(Plat_GetGameDirectory());
	return gameDirectory;
}

std::vector<plg::string> utils::Split(std::string_view strv, std::string_view delims)
{
	std::vector<plg::string> output;
	size_t first = 0;

	while (first < strv.size()) {
		const size_t second = strv.find_first_of(delims, first);

		if (first != second)
			output.emplace_back(strv.substr(first, second-first));

		if (second == std::string_view::npos)
			break;

		first = second + 1;
	}

	return output;
}

#if S2SDK_PLATFORM_WINDOWS

#include <Windows.h>

bool utils::ConvertUtf8ToWide(plg::wstring& dest, std::string_view str) {
	int wlen = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), nullptr, 0);
	if (wlen < 0)
		return false;

	dest.resize(static_cast<size_t>(wlen));
	if (wlen > 0 && MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), dest.data(), wlen) < 0)
		return false;

	return true;
}

plg::wstring utils::ConvertUtf8ToWide(std::string_view str){
	plg::wstring ret;
	if (!ConvertUtf8ToWide(ret, str))
		return {};
	return ret;
}

bool utils::ConvertWideToUtf8(plg::string& dest, std::wstring_view str) {
	int mblen = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), nullptr, 0, nullptr, nullptr);
	if (mblen < 0)
		return false;

	dest.resize(static_cast<size_t>(mblen));
	if (mblen > 0 && WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), dest.data(), mblen, nullptr, nullptr) < 0)
		return false;

	return true;
}
plg::string utils::ConvertWideToUtf8(std::wstring_view str) {
	plg::string ret;
	if (!ConvertWideToUtf8(ret, str))
		return {};
	return ret;
}

#endif
