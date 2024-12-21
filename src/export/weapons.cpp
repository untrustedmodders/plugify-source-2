#include <core/sdk/entity/cbaseentity.h>
#include <core/sdk/entity/ccsweaponbase.h>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Retrieves the weapon VData for a given weapon.
 *
 * @param entityHandle The handle of the entity from which to retrieve the weapon VData.
 * @return A pointer to the `CCSWeaponBaseVData` if the entity handle is valid and
 *         represents a player weapon; otherwise, nullptr.
 */
extern "C" PLUGIN_API CCSWeaponBaseVData* GetWeaponVData(int entityHandle)
{
	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot 'GetWeaponVData' on invalid entity handle: %d\n", entityHandle);
		return nullptr;
	}

	return pWeapon->GetWeaponVData();
}

/**
 * @brief Retrieves the weapon definition index for a given entity handle.
 *
 * @param entityHandle The handle of the entity from which to retrieve the weapon def index.
 * @return The weapon definition index as a `uint16_t`, or 0 if the entity handle is invalid.
 */
extern "C" PLUGIN_API uint16_t GetWeaponDefIndex(int entityHandle)
{
	CBasePlayerWeapon* pWeapon = static_cast<CBasePlayerWeapon*>(g_pGameEntitySystem->GetEntityInstance(CEntityHandle((uint32)entityHandle)));
	if (!pWeapon)
	{
		g_Logger.LogFormat(LS_WARNING, "Cannot 'GetWeaponDefIndex' on invalid entity handle: %d\n", entityHandle);
		return 0;
	}

	return pWeapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex;
}

PLUGIFY_WARN_POP()
