#include "../microgame.inc"

static int g_Barrels[MAXPLAYERS] = -1;
methodmap Barrel < Microgame
{
	public Barrel()
	{
		return view_as<Barrel>(MG_BARREL);
	}

	public void OnMicrogameStart()
	{
		g_attack = true;
	
		float pos[3];
		float vel[3];

		DestroyAllBarrels();
	
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsClientParticipating(i))
			{
				GetClientAbsOrigin(i, pos);
				g_Barrels[i] = CreateEntityByName("prop_physics");
			
				if (IsValidEdict(g_Barrels[i]))
				{
					DispatchKeyValue(g_Barrels[i], "model", "models/props_farm/wooden_barrel.mdl");
					DispatchSpawn(g_Barrels[i]);

					vel[0] = GetRandomFloat(-300.0, 300.0);
					vel[1] = GetRandomFloat(-300.0, 300.0);
					vel[2] = GetRandomFloat(100.0, 300.0);
					pos[2] += 150;

					TeleportEntity(g_Barrels[i], pos, NULL_VECTOR, vel);
					SDKHook(g_Barrels[i], SDKHook_OnTakeDamage, Barrel_OnTakeDamage);
				}    
			}
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "scout");
			GiveClientWeapon(client, "tf_weapon_bat", 2, true);
		}
	}

	public void OnMicrogameEnd()
	{
		DestroyAllBarrels();
	}
}

void DestroyAllBarrels()
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i]))
		{
				DestroyBarrel(g_Barrels[i]);
		}
		g_Barrels[i] = -1;
	}    
}

public Action Barrel_OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i]) && (g_Barrels[i] == victim))
		{
			DestroyBarrel(victim);
			g_Barrels[i] = -1;
			
			if (IsValidClient(attacker))
			{
				SetStateClient(attacker, true, true);
			}
		}    
	}
	
	return Plugin_Continue;
}

void DestroyBarrel(int ent)
{
	if (ent > 0 && IsValidEdict(ent))
	{
		char strClassname[64];
		GetEdictClassname(ent, strClassname, sizeof(strClassname));
		if (StrEqual(strClassname, "prop_physics", false))
		{
			AcceptEntityInput(ent, "Kill" );
		}
	}
}