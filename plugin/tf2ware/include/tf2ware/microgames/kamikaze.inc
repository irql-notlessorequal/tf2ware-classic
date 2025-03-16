#include "../microgame.inc"

methodmap Kamikaze < Microgame
{
	public Kamikaze()
	{
		return view_as<Kamikaze>(MG_KAMIKAZE);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 4;
	}

	public void OnMicrogameSetup()
	{
		PrecacheModel(WW_BOMB_MODEL, true);
		PrecacheModel(WW_BOMB_OLD_MODEL, true);
		precacheSound(WW_BOMB);		
	}

	public void OnMicrogameStart()
	{
		randommini = GetRandomInt(0, 2);
	
		int AllPlayers = GetClientCount(true);
		int target = GetRandomInt(1, AllPlayers);
		while ((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || !IsPlayerAlive(target))
		{
			target = GetRandomInt(1, AllPlayers);
		}
	
		EmitSoundToClient(target, WW_BOMB, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		ClientParticle(target, PARTICLE_BOMB, 2.0);
	
		g_Mission[target] = 1;
		g_result = 0;
		SetStateAll(true);
		SetStateClient(target, false);		
	}

	public void OnMicrogameEnd()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && (g_Mission[i] == 1) && (g_result >= 2))
			{
				SetStateClient(i, true);
			}
		}		
	}

	public void OnMicrogamePostEnd()
	{
		if ((g_bomb > 0) && IsValidEntity(g_bomb))
		{
			AcceptEntityInput(g_bomb, "Kill");
			RemoveEntity(g_bomb);
			g_bomb = 0;
		}
	}

	public void OnClientDeath(int client)
	{
		if (g_Mission[client] == 0)
		{
			SetStateClient(client, false);
			g_result += 1;
		}
	}

	public void OnMicrogameTimer(int timeLeft)
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i) && (g_Mission[i] == 1))
			{
				if (timeLeft > 2)
				{
					ClientParticle(i, PARTICLE_BOMB, 2.0);
				}
			
				if (timeLeft == 2)
				{
					ClientParticle(i, PARTICLE_EXPLODE, 8.0);

					float pos[3];
					GetClientAbsOrigin(i, pos);
						
					PyroExplode(pos);
					ExplosionDamage(pos);
					
					if ((g_bomb > 0) && IsValidEntity(g_bomb))
					{
						AcceptEntityInput(g_bomb, "Kill");
						RemoveEntity(g_bomb);
						g_bomb = 0;
					}
				}
			}
		}		
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			if (g_Mission[client] == 0)
			{
				SetClientClass(client, "heavy");
				DisableClientWeapons(client);
			}
			else if (g_Mission[client] == 1)
			{
				SetClientClass(client, "scout");
				DisableClientWeapons(client);

				if (GetConVarBool(ww_kamikaze_style))
				{
					/* Use the "bobomb" model that is otherwise unused. */
					SetVariantString(WW_BOMB_OLD_MODEL);
					AcceptEntityInput(client, "SetCustomModel");
				}
				else
				{
					if ((g_bomb > 0) && IsValidEntity(g_bomb))
					{
						AcceptEntityInput(g_bomb, "Kill");
						g_bomb = 0;
					}
			
					g_bomb = CreateEntityByName("tf_wearable");
					if (IsValidEdict(g_bomb))
					{ 
						/**
						 * Thank you TF2Ware Ultimate for reminding me that tf_wearable is very flexible.
						 * 
						 * Fuck you Valve and your restrictions.
						 */
						SetEntProp(g_bomb, Prop_Send, "m_nModelIndex", PrecacheModel(WW_BOMB_MODEL));
						SetEntProp(g_bomb, Prop_Send, "m_bValidatedAttachedEntity", true);
						SetEntProp(g_bomb, Prop_Send, "m_hOwnerEntity", client);

						if (DispatchSpawn(g_bomb))
						{
							MalletEquipWearable(client, g_bomb);
							if (!ParentEntity(client, g_bomb, "flag"))
							{
								PrintToServer("[TF2Ware Classic] [Kamikaze] Failed to parent bomb onto player!");
							}
						}
						else
						{
							g_bomb = 0;
						}
					}					
				}
			}
		}		
	}
}