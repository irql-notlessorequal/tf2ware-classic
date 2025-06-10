#include "../microgame.inc"

#define SNIPER_BASEPOS 160

#define SNIPER_TARGET_SCOUT "models/props_training/target_scout.mdl"
#define SNIPER_TARGET_SOLDIER "models/props_training/target_soldier.mdl"
#define SNIPER_TARGET_PYRO "models/props_training/target_pyro.mdl"
#define SNIPER_TARGET_DEMO "models/props_training/target_demoman.mdl"
#define SNIPER_TARGET_HEAVY "models/props_training/target_heavy.mdl"
#define SNIPER_TARGET_ENGINEER "models/props_training/target_engineer.mdl"
#define SNIPER_TARGET_MEDIC "models/props_training/target_medic.mdl"
#define SNIPER_TARGET_SNIPER "models/props_training/target_sniper.mdl"
#define SNIPER_TARGET_SPY "models/props_training/target_spy.mdl"

static int var_sniper_target = -1;
static int g_Target_Pos = 0;
static int g_Target_Random = 0;
static bool g_Target_Ready = false;

methodmap SniperTarget < Microgame
{
	public SniperTarget()
	{
		return view_as<SniperTarget>(MG_SNIPER_TARGET);
	}

	public bool IsMicrogamePlayable(int players)
	{
		/**
		 * Not available on Alpine.
		 */
		return !IsAlpineVariant();
	}

	public void OnMicrogameSetup()
	{
		PrecacheModel(SNIPER_TARGET_SCOUT, true);
		PrecacheModel(SNIPER_TARGET_SOLDIER, true);
		PrecacheModel(SNIPER_TARGET_PYRO, true);
		PrecacheModel(SNIPER_TARGET_DEMO, true);
		PrecacheModel(SNIPER_TARGET_HEAVY, true);
		PrecacheModel(SNIPER_TARGET_ENGINEER, true);
		PrecacheModel(SNIPER_TARGET_MEDIC, true);
		PrecacheModel(SNIPER_TARGET_SNIPER, true);
		PrecacheModel(SNIPER_TARGET_SPY, true);
	}

	public void OnMicrogameStart()
	{
		g_attack = true;
	
		g_Target_Random = GetRandomInt(0,4);
		g_Target_Pos = 0;
		g_Target_Ready = false;
	}

	public void OnMicrogameEnd()
	{
		if (var_sniper_target >= 0 && IsValidEntity(var_sniper_target))
		{
			SDKUnhook(var_sniper_target, SDKHook_OnTakeDamage, SniperTarget_Hurt2);
			AcceptEntityInput(var_sniper_target, "Kill");
			var_sniper_target = -1;
		}

		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_OnTakeDamage, SniperTarget_Hurt);
		}
	}

	public void OnMicrogameTimer(int timeLeft)
	{
		if (timeLeft == 6)
		{
			var_sniper_target = CreateEntityByName("prop_physics");
		
			if (IsValidEdict(var_sniper_target))
			{                      
				char model[128];
				switch (GetRandomInt(1, 9))
				{
					case 1:
					{
						Format(model, sizeof(model), SNIPER_TARGET_SCOUT);
					}

					case 2:
					{
						Format(model, sizeof(model), SNIPER_TARGET_SOLDIER);
					}

					case 3:
					{
						Format(model, sizeof(model), SNIPER_TARGET_PYRO);
					}

					case 4:
					{
						Format(model, sizeof(model), SNIPER_TARGET_DEMO);
					}

					case 5:
					{
						Format(model, sizeof(model), SNIPER_TARGET_HEAVY);
					}

					case 6:
					{
						Format(model, sizeof(model), SNIPER_TARGET_ENGINEER);
					}

					case 7:
					{
						Format(model, sizeof(model), SNIPER_TARGET_MEDIC);
					}

					case 8:
					{
						Format(model, sizeof(model), SNIPER_TARGET_SNIPER);
					}

					case 9:
					{
						Format(model, sizeof(model), SNIPER_TARGET_SPY);
					}
				}
			
				DispatchKeyValue(var_sniper_target, "model", model);
				DispatchSpawn(var_sniper_target);
				SetEntityMoveType(var_sniper_target, MOVETYPE_NONE);
			
				SDKHook(var_sniper_target, SDKHook_OnTakeDamage, SniperTarget_Hurt2);
			
				g_Target_Ready = true;
			
				Sniper_SetPos();
			}
		}

		if (timeLeft == 2)
		{
			g_Target_Ready = false;
		}		
	}

	public void OnMicrogameFrame()
	{
		if (g_Target_Ready && g_Target_Pos < 100)
		{
			g_Target_Pos += 6;
			Sniper_SetPos();
		}

		if (g_Target_Ready == false && g_Target_Pos > 0)
		{
			g_Target_Pos -= 6;
			Sniper_SetPos();
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "sniper");
			GiveClientWeapon(client, "tf_weapon_sniperrifle", 0, true);
			SetClientClass(client, "sniper");
		
			float vel[3];
			vel[0] = 0.0;
			vel[1] = 0.0;
			vel[2] = 0.0;
		
			int column = client;
			int row = 0;
			while (column > 24)
			{
				column = column - 24;
				row = row + 1;
			}
		
			float pos[3];
			pos[0] = 2958.0  + float(column*55);
			pos[1] = 806.0 - float(row*55);
			pos[2] = 270.0;

			float ang[3];
			ang[0] = 0.0;
			ang[1] = 90.0;
			ang[2] = 0.0;
			TeleportEntity(client, pos, ang, vel);
		
			// No damage
			SDKHook(client, SDKHook_OnTakeDamage, SniperTarget_Hurt);
		}		
	}
}

Action SniperTarget_Hurt(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	damage = 0.0;
	return Plugin_Changed;
}

Action SniperTarget_Hurt2(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	damage = 0.0;
	if (IsValidClient(attacker))
	{
		SetStateClient(attacker, true, true);
	}

	return Plugin_Changed;
}

void Sniper_SetPos()
{
	float pos[3];
	float ang[3];
	ang[0] = 0.0;
	ang[1] = 270.0;
	ang[2] = 0.0;
	
	pos[2] = float(SNIPER_BASEPOS + g_Target_Pos);
   
	if (g_Target_Random == 0)
	{
		pos[0] = 3338.0;
		pos[1] = 2223.0;
	}
	else if (g_Target_Random == 1)
	{
		pos[0] = 4119.0;
		pos[1] = 2240.0;
	}
	else if (g_Target_Random == 2)
	{
		pos[0] = 3690.0;
		pos[1] = 2326.0;
	}
	else if (g_Target_Random == 3)
	{
		pos[0] = 3226.0;
		pos[1] = 2501.0;
	}
	else if (g_Target_Random == 4)
	{
		pos[0] = 4191.0;
		pos[1] = 2558.0;
	}
	
	if (IsValidEdict(var_sniper_target))
	{
		TeleportEntity(var_sniper_target, pos, ang, NULL_VECTOR);
	}
}