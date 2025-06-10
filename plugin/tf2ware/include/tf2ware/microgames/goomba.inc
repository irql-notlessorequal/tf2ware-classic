#include "../microgame.inc"

int g_JumpManTeam = 2;

methodmap Goomba < Microgame
{
	public Goomba()
	{
		return view_as<Goomba>(MG_GOOMBA);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 2;
	}

	public void OnMicrogameStart()
	{
		g_JumpManTeam = GetRandomInt(2, 3);
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_Touch, Goomba_Touch);
		
			if (g_JumpManTeam == GetClientTeam(client))
			{
				SetClientClass(client, "scout");
				g_Mission[client] = 0;
			}
			else
			{
				SetClientClass(client, "heavy");
				g_Mission[client] = 1;
				SDKHook(client, SDKHook_PreThink, Goomba_RemoveJump);
				SetStateClient(client, true);
			}
		
			DisableClientWeapons(client);
		}
	}

	public void OnMicrogameEnd()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client))
			{
				SDKUnhook(client, SDKHook_Touch, Goomba_Touch);
				SDKUnhook(client, SDKHook_PreThink, Goomba_RemoveJump);
			}
		}
	}
}

void Goomba_Touch(int client, int other)
{
	if (status == 2)
	{
		char edictName[32];
		GetEdictClassname(other, edictName, sizeof(edictName));

		/**
		 * What the fuck Mecha.
		 */
		if (StrEqual(edictName, "player") && IsValidClient(client) && IsValidClient(other) 
			&& IsPlayerAlive(client) && IsPlayerAlive(other) && g_Mission[client] == 0 && g_Mission[other] == 1
			&& GetClientTeam(client) != GetClientTeam(other))
		{
			float ClientPos[3];
			float VictimPos[3];
			GetClientAbsOrigin(client, ClientPos);
			GetClientAbsOrigin(other, VictimPos);

			float HeightDiff = ClientPos[2] - VictimPos[2];
			
			if (HeightDiff >= 81 && HeightDiff <= 84) {
				float vec[3];
				GetEntPropVector(client, Prop_Data, "m_vecAbsVelocity", vec);

				if (vec[2] < -10.0)
				{
					SetStateClient(other, false, true);
					ForcePlayerSuicide(other);
					SetStateClient(client, true, true);
					
					float vecAng[3];
					float vecVel[3];

					GetClientEyeAngles(client, vecAng);
					vecAng[0] = DegToRad(vecAng[0]);
					vecAng[1] = DegToRad(vecAng[1]);
					vecVel[0] = 300.0 * Cosine(vecAng[0]) * Cosine(vecAng[1]);
					vecVel[1] = 300.0 * Cosine(vecAng[0]) * Sine(vecAng[1]);
					vecVel[2] = 300.0 + 100.0;
					
					TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecVel);
				}
			}
		} 
	}
}

void Goomba_RemoveJump(int client)
{
	if ((status == 2) && GetConVarBool(ww_enable) && g_enabled)
	{
		int iButtons = GetClientButtons(client);
		if ((iButtons & IN_JUMP))
		{
			iButtons &= ~IN_JUMP;
			SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
		}
	}
}