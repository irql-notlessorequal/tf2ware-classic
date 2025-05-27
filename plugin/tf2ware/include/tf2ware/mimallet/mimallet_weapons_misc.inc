#include <sdktools>
#include <tf2>

#include "mimallet_defines.inc"

int InternalSwapWeapon(int client, int weaponEntity, int slot)
{
#if defined MALLET_DEBUG
	PrintToServer("[Mallet] [SwapWeapon] client=%d weaponEntity=%d slot=%d", client, weaponEntity, slot);
#endif

	if (!IsValidEntity(weaponEntity))
	{
		return MALLET_ERROR_GENERAL_ERROR;
	}

	int oldEntity = GetEntProp(client, Prop_Send, "m_hMyWeapons", _, slot);

	EquipPlayerWeapon(client, weaponEntity);
	SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon", weaponEntity);

	if (oldEntity == -1)
	{
		/**
		 * Technically this is the worldspawn entity,
		 * but you should be checking if the entity index is above zero anyways.
		 */
		return 0;
	}
	else
	{
		return oldEntity;
	}
}

bool InternalSwapWeaponAndPurge(int client, int weaponEntity, int slot)
{
	int oldWeapon = InternalSwapWeapon(client, weaponEntity, slot);

	/* Sometimes we can *not* have a weapon, don't return false. */
	if (oldWeapon && IsValidEntity(oldWeapon))
	{
		RemoveEntity(oldWeapon);
	}

	return true;
}

public int MalletSwapWeaponAndPurge(int client, int weaponEntity, int slot)
{
	return InternalSwapWeaponAndPurge(client, weaponEntity, slot);
}