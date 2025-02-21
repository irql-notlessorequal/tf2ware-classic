StringMap __GenerateWeaponIndexDB()
{
	StringMap DB = new StringMap();
	DB.SetValue("tf_weapon_bat", 0);
	DB.SetValue("tf_weapon_bottle", 1);
	DB.SetValue("tf_weapon_fireaxe", 2);
	DB.SetValue("tf_weapon_club", 3);
	DB.SetValue("tf_weapon_knife", 4);
	DB.SetValue("tf_weapon_fists", 5);
	DB.SetValue("tf_weapon_shovel", 6);
	DB.SetValue("tf_weapon_wrench", 7);
	DB.SetValue("tf_weapon_bonesaw", 8);
	DB.SetValue("tf_weapon_shotgun_primary", 9);
	DB.SetValue("tf_weapon_shotgun_soldier", 10);
	DB.SetValue("tf_weapon_shotgun_hwg", 11);
	DB.SetValue("tf_weapon_shotgun_pyro", 12);
	DB.SetValue("tf_weapon_scattergun", 13);
	DB.SetValue("tf_weapon_sniperrifle", 14);
	DB.SetValue("tf_weapon_minigun", 15);
	DB.SetValue("tf_weapon_smg", 16);
	DB.SetValue("tf_weapon_syringegun_medic", 17);
	DB.SetValue("tf_weapon_rocketlauncher", 18);
	DB.SetValue("tf_weapon_grenadelauncher", 19);
	DB.SetValue("tf_weapon_pipebomblauncher", 20);
	DB.SetValue("tf_weapon_flamethrower", 21);	
	DB.SetValue("tf_weapon_pistol", 22);
	DB.SetValue("tf_weapon_pistol", 23);
	DB.SetValue("tf_weapon_revolver", 24);
	DB.SetValue("tf_weapon_pda_engineer_build", 25);
	DB.SetValue("tf_weapon_pda_engineer_destroy", 26);
	DB.SetValue("tf_weapon_pda_spy", 27);
	DB.SetValue("tf_weapon_builder", 28);
	DB.SetValue("tf_weapon_medigun", 29);
	return DB;
}

StringMap HMMR_WEAPON_INDEX_DB;

bool InternalGetWeaponIndex(const char[] weaponName, int& value)
{
	if (HMMR_WEAPON_INDEX_DB == null)
	{
		HMMR_WEAPON_INDEX_DB = __GenerateWeaponIndexDB();
	}

	return HMMR_WEAPON_INDEX_DB.GetValue(weaponName, value);
}

public bool MalletGetWeaponIndex(int weaponNameSize, const char[] weaponName, int& value)
{
	return InternalGetWeaponIndex(weaponName, value);
}