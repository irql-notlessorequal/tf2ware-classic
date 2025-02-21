#include <tf2>
#include <tf2_stocks>

#include "mimallet_defines.inc"

bool GetCompatibleSaxxyWeapon(const TFClassType type, char weaponName[24])
{
	if (type == TFClass_Scout)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_bat");
		return true;
	}
	else if (type == TFClass_Sniper)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_club");
		return true;
	}
	else if (type == TFClass_Soldier)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_shovel");
		return true;
	}
	else if (type == TFClass_DemoMan)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_bottle");
		return true;
	}
	else if (type == TFClass_Engineer)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_wrench");
		return true;
	}
	else if (type == TFClass_Pyro)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_fireaxe");
		return true;
	}
	else if (type == TFClass_Heavy)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_fists");
		return true;
	}
	else if (type == TFClass_Spy)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_knife");
		return true;
	}
	else if (type == TFClass_Medic)
	{
		strcopy(weaponName, sizeof weaponName, "tf_weapon_bonesaw");
		return true;
	}
	else
	{
		return false;
	}
}

public int MalletCreateWeapon(int client, int weaponId, const char[] weaponName, int level, int quality, int weaponSlot)
{
	if (TypeIsWearable(weaponName))
	{
		ThrowError("MalletCreateWeapon: mimallet doesn't support Wearables.");
		return -1;
	}

	if (StrEqual(weaponName, "saxxy", false))
	{
		/* "saxxy" doesn't exist, convert to an actual weapon. */
		TFClassType classType = TF2_GetPlayerClass(client);

		char newWeaponName[24];
		GetCompatibleSaxxyWeapon(classType, newWeaponName);

		return MalletCreateWeapon(client, weaponId, newWeaponName, level, quality, weaponSlot);
	}

	int weapon = CreateEntityByName(weaponName);
	if (!IsValidEntity(weapon))
	{
		return MALLET_ERROR_GENERAL_ERROR;
	}

	char entityClass[64];
	GetEntityNetClass(weapon, entityClass, sizeof(entityClass));

	/* Set up the entity. */
	SetEntProp(weapon, Prop_Send, "m_iItemDefinitionIndex", weaponId);	
	SetEntProp(weapon, Prop_Send, "m_bInitialized", true);
	SetEntProp(weapon, Prop_Send, "m_bValidatedAttachedEntity", true);

	/* Apply misc. data */
	int internal_level = 1;
	int internal_quality = 6;

	if (level)
	{
		internal_level = level;
	}

	if (quality)
	{
		internal_quality = quality;
	}

	/* Apply levels. */
	SetEntProp(weapon, Prop_Send, "m_iEntityLevel", internal_level);
	SetEntProp(weapon, Prop_Send, "m_iEntityQuality", internal_quality);

	switch (weaponId)
	{
		/* The Vaccinator */
		case 998:
		{
			SetEntData(weapon, FindSendPropInfo(entityClass, "m_nChargeResistType"), 0);
		}

		/* Dragon's Fury */
		case 1178:
		{
			int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1) * 4;
			int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
			SetEntData(client, iAmmoTable + iOffset, 40, 4);
		}

		/* The Flare Gun */
		/* The Detonator */
		/* The Scorch Shot */
		case 39, 351, 740:
		{
			int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1) * 4;
			int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
			SetEntData(client, iAmmoTable + iOffset, 16, 4);	
		}

		/* PDA (ed: Toolbox) */
		/* Sapper */
		case 28, 735, 736, 810, 831, 933, 1080, 1102:
		{
			SetEntProp(weapon, Prop_Send, "m_iObjectType", 3);
			if (weaponSlot == TFWeaponSlot_Secondary)
			{
				SetEntProp(weapon, Prop_Data, "m_iSubType", 3);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 0, _, 0);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 0, _, 1);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 0, _, 2);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 1, _, 3);
			}
			else
			{
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 1, _, 0);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 1, _, 1);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 1, _, 2);
				SetEntProp(weapon, Prop_Send, "m_aBuildableObjectTypes", 0, _, 3);
			}
		}

		/* Gas Passer */
		case 1180:
		{
			/* Some items don't work if they are spawned with a full charge. */
			SetEntPropFloat(client, Prop_Send, "m_flItemChargeMeter", 99.5, 1);
		}
	}

	if (!DispatchSpawn(weapon))
	{
		return MALLET_ERROR_FAILED_TO_SPAWN;
	}
	else
	{
		return weapon;
	}
}