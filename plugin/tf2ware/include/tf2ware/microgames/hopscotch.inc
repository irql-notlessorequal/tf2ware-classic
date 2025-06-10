#include "../microgame.inc"

#define HOPSCOTCH_ARENA_HEAVY_PLATFORM 0
#define HOPSCOTCH_ARENA_ROCKET_JUMPING 1
#define HOPSCOTCH_ARENA_STICKY_JUMPING 2

int g_Hopscotch_Arena = 0;
bool g_Hopscotch_Weapon = false;

methodmap Hopscotch < Microgame
{
	public Hopscotch()
	{
		return view_as<Hopscotch>(MG_HOPSCOTCH);
	}

	public void OnMicrogameStart()
	{
		g_Hopscotch_Arena = GetRandomInt(0, 2);
		g_Hopscotch_Weapon = false;
	
		if (g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING
			|| g_Hopscotch_Arena == HOPSCOTCH_ARENA_STICKY_JUMPING)
		{
			g_attack = true;
			CreateTimer(4.9, Hopscotch_Weapon);
		}
	}

	public void OnMicrogameEnd()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_OnTakeDamage, Hopscotch_OnTakeDamageClient);
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_OnTakeDamage, Hopscotch_OnTakeDamageClient);
	
			if (g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
			{
				SetClientClass(client, "soldier");
			}
			else if (g_Hopscotch_Arena == HOPSCOTCH_ARENA_STICKY_JUMPING)
			{
				SetClientClass(client, "demoman");
			}
			else
			{
				SetClientClass(client, "heavy");
			}

			DisableClientWeapons(client);

			if (g_Hopscotch_Weapon && g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
			{
				RemoveClientWeapons(client);
				GiveClientWeapon(client, "tf_weapon_rocketlauncher", 0, true);
			}
			else if (g_Hopscotch_Weapon && g_Hopscotch_Arena == HOPSCOTCH_ARENA_STICKY_JUMPING)
			{
				RemoveClientWeapons(client);
				GiveClientWeapon2(client, TF2_WEAPON_STICKY_JUMPER, "tf_weapon_pipebomblauncher", 0, true);
			}
		
			/**
			 * TODO(irql):
			 * 
			 * Verify that the change worked and also make sure
			 * that Alpine also works correctly.
			 * 
			 * "setpos 2800 3800 -2400"
			 */

			float pos[3];
		
			if (g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING
			 || g_Hopscotch_Arena == HOPSCOTCH_ARENA_STICKY_JUMPING)
			{
				/**
				 * Spawn players in a circle instead since we have the room.
				 */
				int total = GetActivePlayers();
				int posa = 360 / total * client;

				if (IsAlpineVariant())
				{
					pos[0] = 2800.0 + (Cosine(DegToRad(float(posa))) * 220.0);
					pos[1] = 2706.0 - (Sine(DegToRad(float(posa))) * 220.0);
					pos[2] = -1000.0;
				}
				else
				{
					pos[0] = 2460.0 + (Cosine(DegToRad(float(posa))) * 220.0);
					pos[1] = 3395.0 - (Sine(DegToRad(float(posa))) * 220.0);
					pos[2] = -2688.0;
				}
			}
			else
			{
				int column = client;
				int row = 0;		

				while (column > 10)
				{
					column = column - 10;
					row = row + 1;
				}

				pos[0] = -2453.0  - float(column * 55);
				pos[1] = -1833.0 - float(row * 55);
				pos[2] = 17.6;
			}

			float ang[3];
			ang[0] = 0.0;
			ang[1] = 270.0;
			ang[2] = 0.0;

			TeleportEntity(client, pos, ang, NULL_VECTOR);
			NoCollision(true);
		}	
	}

	public void OnMicrogameFrame()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsPlayerAlive(client) && IsClientParticipating(client))
			{
				float pos[3];
				GetClientAbsOrigin(client, pos);

				if (g_Hopscotch_Arena == HOPSCOTCH_ARENA_HEAVY_PLATFORM && pos[1] > 2235.0 && g_Complete[client] == false)
				{
					SetStateClient(client, true, true);
				}
				else if ((g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING
					|| g_Hopscotch_Arena == HOPSCOTCH_ARENA_STICKY_JUMPING)
					&& pos[2] > -80.0 && pos[1] > 4270.0 && g_Complete[client] == false)
				{
					SetStateClient(client, true, true);
				}

				if (g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
				{
					SetEntityHealth(client, 200);
				}
			}
		}
	}
}

public Action Hopscotch_Weapon(Handle hTimer)
{
	if (status == 2 && g_Hopscotch_Arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
	{
		g_Hopscotch_Weapon = true;

		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsClientParticipating(client))
			{
				RemoveClientWeapons(client);
				GiveClientWeapon(client, "tf_weapon_rocketlauncher", 0, true);
			}
		}
	}

	if (status == 2 && g_Hopscotch_Arena == HOPSCOTCH_ARENA_STICKY_JUMPING)
	{
		g_Hopscotch_Weapon = true;
		
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsClientParticipating(client))
			{
				RemoveClientWeapons(client);
				GiveClientWeapon2(client, TF2_WEAPON_STICKY_JUMPER, "tf_weapon_pipebomblauncher", 0, true);
			}
		}
	}

	return Plugin_Stop;
}

public Action Hopscotch_OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (IsValidClient(attacker) && IsValidClient(victim) && victim != attacker && status == 2)
	{
		damage = 0.0;
	}
	
	return Plugin_Changed;
}