#include "../microgame.inc"

#define FROGGER_MODEL "models/props_2fort/frog.mdl"

static int g_Frogger_Timer = 0;
static bool g_Frogger_Goal = false;
static int g_Frogger_Pos[MAXPLAYERS + 1] = 0;

methodmap Frogger < Microgame
{
	public Frogger()
	{
		return view_as<Frogger>(MG_FROGGER);
	}

	public void OnMicrogameSetup()
	{
		PrecacheModel(FROGGER_MODEL, true);

		precacheSound("vo/announcer_ends_5sec.wav");
		precacheSound("vo/announcer_ends_4sec.wav");
		precacheSound("vo/announcer_ends_3sec.wav");
		precacheSound("vo/announcer_ends_2sec.wav");
		precacheSound("vo/announcer_ends_1sec.wav");
	}

	public void OnMicrogameStart()
	{
		NoCollision(true);

		g_Frogger_Timer = 11;
		g_Frogger_Goal = false;
	
		for (int client = 1; client <= MaxClients; client++)
		{
			g_Frogger_Pos[client] = 0;
		}
	}

	public void OnMicrogameEnd()
	{
		/**
		 * Let's turn them back
		 */
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client))
			{
				SetVariantString("");
				AcceptEntityInput(client, "SetCustomModel");
				SetVariantInt(0);
				AcceptEntityInput(client, "SetCustomModelRotates");

				ToggleThirdperson(client, false);
			}
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			/**
			 * Change the client's class?
			 */
			SetClientClass(client, "medic");
			DisableClientWeapons(client);
		
			/**
			 * Turn him into a frog!
			 */
			SetVariantString(FROGGER_MODEL);
			AcceptEntityInput(client, "SetCustomModel");
			SetVariantInt(1);
			AcceptEntityInput(client, "SetCustomModelRotates");
		
			Frogger_Position(client);
			NoCollision(true);
		
			CreateTimer(0.1, Frogger_Ragdoll, client);
		}
	}

	public void OnClientDeath(int client)
	{
		if (IsValidClient(client) && status == 2)
		{
			SetVariantString("");
			AcceptEntityInput(client, "SetCustomModel");
			SetVariantInt(0);
			AcceptEntityInput(client, "SetCustomModelRotates");
			SetEntProp(client, Prop_Send, "m_iObserverMode", 0);

			CreateTimer(1.0, Frogger_Respawn, client);
		
			CreateTimer(0.0, Frogger_Ragdoll, client);
		}
	}

	public void OnMicrogameFrame()
	{
		if (status != 2)
		{
			return;
		}
		
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsPlayerAlive(client))
			{
				float pos[3];
				GetClientAbsOrigin(client, pos);
			
				// checkpoints
				if (pos[1] >= -1566.0 && g_Frogger_Pos[client] < 1)
				{
					g_Frogger_Pos[client] = 1;
				}
				else if (pos[1] >= -209.0 && g_Frogger_Pos[client] < 2)
				{
					g_Frogger_Pos[client] = 2;
				}
				else if (pos[1] >= 603.0 && g_Frogger_Pos[client] < 3)
				{
					g_Frogger_Pos[client] = 3;
				}
				else if (pos[1] >= 1322.0 && g_Frogger_Pos[client] < 4)
				{
					g_Frogger_Pos[client] = 4;
				}
				else if (pos[1] >= 1873.0 && g_Frogger_Pos[client] < 5)
				{
					g_Frogger_Pos[client] = 5;
				}
				else if (pos[1] >= 2831.0 && pos[0] >= 8020.0 && g_Frogger_Pos[client] < 6)
				{
					g_Frogger_Pos[client] = 6;
				}
				else if (pos[1] >= 2831.0 && pos[0] >= 8932.0 && g_Frogger_Pos[client] < 7)
				{
					g_Frogger_Pos[client] = 7;
				}
				else if (pos[1] >= 2935.0 && pos[0] >= 9468.0 && g_Frogger_Pos[client] < 8)
				{
					g_Frogger_Pos[client] = 8;
				}
			
				// goal
				if (pos[1] > 3751.0 && g_Complete[client] == false )
				{
					bool bWin = true;

					if (SpecialRound == WIPEOUT && g_Frogger_Goal)
					{
						bWin = false;
					}
				
					if (bWin)
					{
						SetStateClient(client, true, true);
					
						CPrintToChatAllEx(client, "{teamcolor}%N{default} made it to the goal first!", client);
						g_Frogger_Pos[client] = 99;

						if (!g_Frogger_Goal)
						{
							if (SpecialRound == WIPEOUT)
							{
								CreateTimer(0.1, Frogger_Timer);
								g_Frogger_Timer = 0;
							}
							else
							{
								CreateTimer(0.0, Frogger_Timer);
								CPrintToChatAll("The boss will end in {red}10 seconds{default}!");
							}
						}

						g_Frogger_Goal = true;
					}
				}
			}
		}
	}
}

public Action Frogger_Respawn(Handle hTimer, any client)
{
	if (status == 2)
	{
		RespawnClient(client, true, false);
	}

	return Plugin_Stop;
}


public Action Frogger_Ragdoll(Handle timer, any client)
{
	if (IsValidClient(client))
	{
		int rag = GetEntPropEnt(client, Prop_Send, "m_hRagdoll");
		if (rag > MaxClients && IsValidEntity(rag))
		{
			AcceptEntityInput(rag, "Kill");
		}
	}

	return Plugin_Stop;
}

public Action Frogger_Timer(Handle hTimer)
{
	if (status == 2)
	{
		g_Frogger_Timer -= 1;
		
		if (g_Frogger_Timer > 0)
		{
			char sound[512];
			Format(sound, sizeof(sound), "vo/announcer_ends_%dsec.wav", g_Frogger_Timer);

			if (GetConVarBool(ww_music) == true)
			{
				EmitSoundToClient(1, sound);
			}
			else
			{
				EmitSoundToAll(sound);
			}

			if (g_Frogger_Timer < 5)
			{
				CPrintToChatAll("{red}%d{default}!", g_Frogger_Timer);
			}

			CreateTimer(1.0, Frogger_Timer);
		}
		else
		{
			if (MicrogameTimer != INVALID_HANDLE)
			{
				TriggerTimer(MicrogameTimer);
			}
		}
	}

	return Plugin_Handled;
}

void Frogger_Position(int client)
{
	float vel[3];
	vel[0] = 0.0;
	vel[1] = 0.0;
	vel[2] = 0.0;
	
	float ang[3];
	ang[0] = 0.0;
	ang[1] = 90.0;
	ang[2] = 0.0;
	
	float pos[3]; 
	int checkpoint = g_Frogger_Pos[client];

	if (checkpoint == 99)
	{
		pos[0] = 8726.0;
		pos[1] = 3947.0;
		pos[2] = 455.0;
	}
	else if (checkpoint == 8)
	{
		pos[0] = 9468.0;
		pos[1] = 2935.0;
		pos[2] = 293.0;
		ang[1] = 0.0;
	}
	else if (checkpoint == 7)
	{
		pos[0] = 8932.0;
		pos[1] = 2831.0;
		pos[2] = 293.0;
		ang[1] = 0.0;
	}
	else if (checkpoint == 6)
	{
		pos[0] = 8020.0;
		pos[1] = 2873.0;
		pos[2] = 293.0;
		ang[1] = 0.0;
	}
	else if (checkpoint == 5)
	{
		pos[0] = 8654.0;
		pos[1] = 1873.0;
		pos[2] = 195.0;
	}
	else if (checkpoint == 4)
	{
		pos[0] = 8654.0;
		pos[1] = 1322.0;
		pos[2] = 195.0;
	}
	else if (checkpoint == 3)
	{
		pos[0] = 8615.0;
		pos[1] = 603.0;
		pos[2] = 191.0;
	}
	else if (checkpoint == 2)
	{
		pos[0] = 8615.0;
		pos[1] = -209.0;
		pos[2] = 191.0;
	}
	else if (checkpoint == 1)
	{
		pos[0] = 8654.0;
		pos[1] = -1566.0;
		pos[2] = 223.0;
	}
	else if (checkpoint == 0)
	{
		int column = client;
		int row = 0;

		while (column > 7)
		{
			column = column - 7;
			row = row + 1;
		}
		
		pos[0] = 8497.0  + float(column*55);
		pos[1] = -3438.0 + float(row*55);
		pos[2] = 216.0;
	}

	TeleportEntity(client, pos, ang, vel);

	ToggleThirdperson(client, true);
}