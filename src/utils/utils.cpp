#include "utils.h"

#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/recipientfilters.h>
#include <engine/igameeventsystem.h>
#include <networkbasetypes.pb.h>
#include <public/networksystem/inetworkmessages.h>
#include <entity2/entitysystem.h>
#include <core/sdk/datatypes.h>

#include <tier0/memdbgon.h>

CBaseEntity2* utils::FindEntityByClassname(CEntityInstance* start, const char* name)
{
	if (!g_pEntitySystem)
	{
		return NULL;
	}
	EntityInstanceByClassIter_t iter(start, name);

	return static_cast<CBaseEntity2*>(iter.Next());
}

#define FCVAR_FLAGS_TO_REMOVE (FCVAR_HIDDEN | FCVAR_DEVELOPMENTONLY | FCVAR_MISSING0 | FCVAR_MISSING1 | FCVAR_MISSING2 | FCVAR_MISSING3)

CBasePlayerController* utils::GetController(CBaseEntity2* entity)
{
	CCSPlayerController* controller = nullptr;

	if (entity->IsPawn())
	{
		CBasePlayerPawn* pawn = static_cast<CBasePlayerPawn*>(entity);
		if (!pawn->m_hController().IsValid() || pawn->m_hController.Get() == nullptr)
		{
			// Seems like the pawn lost its controller, we can try looping through the controllers to find this pawn instead.
			for (int i = 0; i <= gpGlobals->maxClients; i++)
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
	if (!g_pEntitySystem || slot.Get() < 0 || slot.Get() > MAXPLAYERS)
	{
		return nullptr;
	}
	
	CBaseEntity2* ent = static_cast<CBaseEntity2*>(g_pEntitySystem->GetBaseEntity(CEntityIndex(slot.Get() + 1)));
	if (!ent)
	{
		return nullptr;
	}
	
	return ent->IsController() ? static_cast<CBasePlayerController*>(ent) : nullptr;
}

CPlayerSlot utils::GetEntityPlayerSlot(CBaseEntity2* entity)
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

	return CALL_VIRTUAL(CUtlVector<CServerSideClient *> *, g_pGameConfig->GetOffset("GetClientList"), g_pNetworkGameServer);
}*/

void utils::PlaySoundToClient(CPlayerSlot player, const char* sound, float volume)
{
	CSingleRecipientFilter filter(player.Get());
	EmitSound_t soundParams;
	soundParams.m_pSoundName = sound;
	soundParams.m_flVolume = volume;
	addresses::EmitSound(filter, player.Get() + 1, soundParams);
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

void utils::SendConVarValue(CPlayerSlot slot, CConVarBaseData* conVar, const char* value)
{
	INetworkSerializable* netmsg = g_pNetworkMessages->FindNetworkMessagePartial("SetConVar");
	CNETMsg_SetConVar msg;
	CMsg_CVars_CVar* cvar = msg.mutable_convars()->add_cvars();
	cvar->set_name(conVar->GetName());
	cvar->set_value(value);
	CSingleRecipientFilter filter(slot.Get());
	g_gameEventSystem->PostEventAbstract(0, false, &filter, netmsg, &msg, 0);
}

void utils::SendMultipleConVarValues(CPlayerSlot slot, CConVarBaseData** conVar, const char** value, uint32_t size)
{
	INetworkSerializable* netmsg = g_pNetworkMessages->FindNetworkMessagePartial("SetConVar");
	CNETMsg_SetConVar msg;
	for (uint32_t i = 0; i < size; i++)
	{
		CMsg_CVars_CVar* cvar = msg.mutable_convars()->add_cvars();
		cvar->set_name(conVar[i]->GetName());
		cvar->set_value(value[i]);
	}
	CSingleRecipientFilter filter(slot.Get());
	g_gameEventSystem->PostEventAbstract(0, false, &filter, netmsg, &msg, 0);
}

bool utils::IsSpawnValid(const Vector& origin)
{
	bbox_t bounds = {{-16.0f, -16.0f, 0.0f}, {16.0f, 16.0f, 72.0f}};
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
	return true;
}

bool utils::FindValidSpawn(Vector& origin, QAngle& angles)
{
	bool foundValidSpawn = false;
	bool searchCT = false;
	Vector spawnOrigin;
	QAngle spawnAngles;
	CBaseEntity2* spawnEntity = nullptr;
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

const std::string& utils::GameDirectory()
{
	static std::string gameDirectory(Plat_GetGameDirectory());
	return gameDirectory;
}

const std::string& utils::RootDirectory()
{ 
	static std::string rootDirectory(GameDirectory() + "/addons/plugify/");
	return rootDirectory;
}

const std::string& utils::BinDirectory()
{ 
	static std::string binDirectory(GameDirectory() + "/addons/plugify/bin/" CS2SDK_BINARY "/");
	return binDirectory;
}

const std::string& utils::ConfigsDirectory()
{ 
	static std::string configsDirectory(GameDirectory() + "/addons/plugify/configs/");
	return configsDirectory;
}

const std::string& utils::GamedataDirectory()
{ 
	static std::string gamedataDirectory(GameDirectory() + "/addons/plugify/gamedata/");
	return gamedataDirectory;
}

