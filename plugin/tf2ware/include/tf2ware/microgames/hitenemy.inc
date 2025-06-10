#include "../microgame.inc"

methodmap HitEnemy < Microgame
{
	public HitEnemy()
	{
		return view_as<HitEnemy>(MG_HIT_ENEMY);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 2;
	}

	public void OnMicrogameStart()
	{
		randommini = GetRandomInt(0, 2);
		g_attack = true;
	}

	public void OnMicrogameEnd()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_OnTakeDamage, HitEnemy_OnTakeDamageClient);
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_OnTakeDamage, HitEnemy_OnTakeDamageClient);

			RemoveClientWeapons(client);

			if (randommini <= 0)
			{
				SetClientClass(client, "spy");
				GiveClientWeapon(client, "tf_weapon_revolver", 0, true);
			}
			else if (randommini == 1)
			{
				SetClientClass(client, "demoman");
				GiveClientWeapon(client, "tf_weapon_bottle", 2, true);
			}
			else if (randommini >= 2)
			{
				SetClientClass(client, "engineer");
				GiveClientWeapon(client, "tf_weapon_shotgun_primary", 0, true);
			}
		}
	}
}

Action HitEnemy_OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (IsValidClient(attacker))
	{
		SetStateClient(attacker, true, true);
		damage = 450.0;
	}

	return Plugin_Changed;
}