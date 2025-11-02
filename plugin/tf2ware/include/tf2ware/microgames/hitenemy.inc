#include "../microgame.inc"

static int MicrogameVariation = 0;

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
		MicrogameVariation = MalletGetRandomInt(1, 5);
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

			switch (MicrogameVariation)
			{
				case 1:
				{
					SetClientClass(client, "spy");
					GiveClientWeapon(client, "tf_weapon_revolver", 0, true);
				}

				case 2:
				{
					SetClientClass(client, "demoman");
					GiveClientWeapon(client, "tf_weapon_bottle", 2, true);
				}

				case 3:
				{
					SetClientClass(client, "engineer");
					GiveClientWeapon(client, "tf_weapon_shotgun_primary", 0, true);
				}

				case 4:
				{
					SetClientClass(client, "soldier");
					GiveClientWeapon2(client, view_as<int>(TF2WARE_WEAPON_DIRECT_HIT), "tf_weapon_rocketlauncher_directhit", 0, true);
				}

				case 5:
				{
					SetClientClass(client, "sniper");
					GiveClientWeapon2(client, view_as<int>(TF2WARE_WEAPON_HUNTSMAN), "tf_weapon_compound_bow", 0, true);
				}
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