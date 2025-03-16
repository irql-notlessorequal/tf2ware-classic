#include "../microgame.inc"

#define HOPSCOTCH_ARENA_HEAVY_PLATFORM 0
#define HOPSCOTCH_ARENA_ROCKET_JUMPING 1

int g_Hopscotch_arena = 0;
bool g_Hopscotch_weapon = false;

methodmap Hopscotch < Microgame
{
	public Hopscotch()
	{
		return view_as<Hopscotch>(MG_HOPSCOTCH);
	}

	public void OnMicrogameStart()
	{
		g_Hopscotch_arena = GetRandomInt(0, 1);
		g_Hopscotch_weapon = false;
	
		if (g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
		{
			g_attack = true;
			CreateTimer(4.9, Hopscotch_Weapon);
		}
	}

	public void OnMicrogameEnd()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			SDKUnhook(i, SDKHook_OnTakeDamage, Hopscotch_OnTakeDamageClient);
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_OnTakeDamage, Hopscotch_OnTakeDamageClient);
	
			if (g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
			{
				SetClientClass(client, "soldier");
			}
			else
			{
				SetClientClass(client, "heavy");
			}

			DisableClientWeapons(client);

			if (g_Hopscotch_weapon && g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
			{
				RemoveClientWeapons(client);
				GiveClientWeapon(client, "tf_weapon_rocketlauncher", 0, true);
			}
		
			float vel[3];
			vel[0] = 0.0;
			vel[1] = 0.0;
			vel[2] = 0.0;
		
			int column = client;
			int row = 0;
			while (column > 10)
			{
				column = column - 10;
				row = row + 1;
			}
		
			float pos[3];
			if (g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING) {
				pos[0] = 2460.0  + float(column*55);
				pos[1] = 3395.0 - float(row*55);
				pos[2] = -2688.0;
			}
			else
			{
				pos[0] = -2453.0  - float(column*55);
				pos[1] = -1833.0 - float(row*55);
				pos[2] = 17.6;
			}

			float ang[3];
			ang[0] = 0.0;
			ang[1] = 270.0;
			ang[2] = 0.0;

			TeleportEntity(client, pos, ang, vel);
			NoCollision(true);
		}	
	}

	public void OnMicrogameFrame()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i))
			{
				float pos[3];
				GetClientAbsOrigin(i, pos);
				if (g_Hopscotch_arena == HOPSCOTCH_ARENA_HEAVY_PLATFORM && pos[1] > 2235.0 && g_Complete[i] == false)
				{
					SetStateClient(i, true, true);
				}
				else if (g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING &&
						pos[2] > -80.0 && pos[1] > 4270.0 && g_Complete[i] == false)
				{
					SetStateClient(i, true, true);
				}

				if (g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
				{
					SetEntityHealth(i, 200);
				}
			}
		}
	}
}

public Action Hopscotch_Weapon(Handle hTimer)
{
	if (status == 2 && g_Hopscotch_arena == HOPSCOTCH_ARENA_ROCKET_JUMPING)
	{
		g_Hopscotch_weapon = true;
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsClientParticipating(i))
			{
				RemoveClientWeapons(i);
				GiveClientWeapon(i, "tf_weapon_rocketlauncher", 0, true);
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