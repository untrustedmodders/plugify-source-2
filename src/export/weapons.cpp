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

PLUGIFY_WARN_POP()
