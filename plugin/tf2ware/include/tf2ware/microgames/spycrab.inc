// Spycrab module

#define SPRITE_SPYCRAB_SKY "materials/tf2ware/spycrab.vmt"

static int SPYCRAB_SKY_SPRITE = -1;

methodmap Spycrab < Microgame
{
	public Spycrab()
	{
		return view_as<Spycrab>(MG_SPYCRAB);
	}

	public void OnMicrogameStart()
	{
		SetStateAll(true);

		/**
		 * Let's put a cute little spycrab in the spy.
		 */	
		float vOrigin[3] = { -5.0, -5.0, 1000.0 };
		float vAngle[3] = { 90.0, 0.0, 0.0 };
	
		int ent = CreateEntityByName("env_sprite");
		if (IsValidEntity(ent))
		{
			DispatchKeyValueVector(ent, "origin", vOrigin);
			DispatchKeyValueVector(ent, "angles", vAngle);

			DispatchKeyValueFloat(ent, "scale", 1.0);

			DispatchKeyValue(ent, "classname", "env_sprite");
			DispatchKeyValue(ent, "spawnflags", "1");
			DispatchKeyValue(ent, "rendermode", "1");
			DispatchKeyValue(ent, "rendercolor", "255 255 255");
			DispatchKeyValue(ent, "model", SPRITE_SPYCRAB_SKY);
			
			if (DispatchSpawn(ent))
			{
				SPYCRAB_SKY_SPRITE = ent;
			}
		}
		else
		{
			SPYCRAB_SKY_SPRITE = -1;
		}
	}

	public void OnMicrogameEnd()
	{
		if ((SPYCRAB_SKY_SPRITE > 0) && IsValidEntity(SPYCRAB_SKY_SPRITE))
		{
			AcceptEntityInput(SPYCRAB_SKY_SPRITE, "Kill");
			SPYCRAB_SKY_SPRITE = -1;
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "spy");
			GiveClientWeapon(client, "tf_weapon_pda_spy", 3, true);
		}
	}

	public void OnMicrogameTimer(int timeLeft)
	{
		if (timeLeft == 3)
		{
			float min;
			float ang[3];
			int duck;
		
			for (int client = 1; client <= MaxClients; client++)
			{
				min = -35.0;
				if (IsValidClient(client) && IsPlayerAlive(client) && IsClientParticipating(client))
				{
					if (GetEntProp(client, Prop_Send, "m_nPlayerCond") & 16) 
					{
						SetEntProp(client, Prop_Send, "m_nPlayerCond", 0);
					}

					SetEntPropFloat(client, Prop_Send, "m_flCloakMeter", 0.0);
					TF2_RemovePlayerDisguise(client);
				
					duck = (GetEntData(client, FindSendPropInfo("CTFPlayer", "m_fFlags")) & 2);
			  
					GetClientEyeAngles(client, ang);
					
					if (ang[0] > min || duck == 0)
					{
						ForcePlayerSuicide(client);
						SetStateClient(client, false, true);

						/**
						 * Explain to the client what he should've done so he isn't left clueless.
						 */
						if (ang[0] > min && duck == 0)
						{
							CPrintToChat(client, "%T", "SpyCrab_NoAction", client);
						}
						else if (ang[0] > min)
						{
							CPrintToChat(client, "%T", "SpyCrab_NotLookingUp", client);
						}
						else
						{
							CPrintToChat(client, "%T", "SpyCrab_NotCrouching", client);
						}
					}
				}
			}
		}
	}
};