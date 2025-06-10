#include "../microgame.inc"

int JumpRope_Speed = 0;
const int JumpRope_DrainHP = 8;

/**
 * TODO(irql):
 * 
 * Jump Rope seems to be completely fucking broken.
 * You will die instantly for some reason.
 */

methodmap JumpRope < Microgame
{
	public JumpRope()
	{
		return view_as<JumpRope>(MG_JUMP_ROPE);
	}

	public void OnMicrogameStart()
	{
		SetStateAll(true);

		/**
		 * Disable collisions if there are too many players.
		 */
		NoCollision(GetActivePlayers() > 24);

		JumpRope_Speed = 0;
		
		CreateTimer(5.0, JumpRope_NewRound);
		CreateTimer(0.5, JumpRope_Drain);
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SetClientClass(client, "pyro");
			DisableClientWeapons(client);
		
			int column = client;
			int row = 0;
			while (column > 17)
			{
				column = column - 17;
				row = row + 1;
			}
		
			float pos[3];
			pos[0] = -44.0  - float(column * 55);
			pos[1] = 2651.0 - float(row * 55);
			pos[2] = -300.0;

			float ang[3];
			ang[0] = 0.0;
			ang[1] = 90.0;
			ang[2] = 0.0;

			TeleportEntity(client, pos, ang, NULL_VECTOR);
		}
	}

	public void OnClientDeath(int client)
	{
		SetStateClient(client, false);
	}
}

public Action JumpRope_NewRound(Handle timer)
{
	if (view_as<Microgames>(currentMicrogame) != MG_JUMP_ROPE)
	{
		return Plugin_Continue;
	}

	if (status != 2)
	{
		return Plugin_Continue;
	}

	JumpRope_Speed++;
	float timescale = (GetHostMultiplier(1.0) + float(JumpRope_Speed) * 0.05);

	ServerCommand("host_timescale %f", timescale);
	CreateTimer((5.0 / timescale), JumpRope_NewRound);

	int ActivePlayers = 0;
	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && g_Complete[client] && IsClientParticipating(client))
		{
			ActivePlayers += 1;
		}
	}
	
	if (ActivePlayers <= 1)
	{
		TriggerTimer(MicrogameTimer);
	}

	return Plugin_Stop;
}

public Action JumpRope_Drain(Handle timer)
{
	if (view_as<Microgames>(currentMicrogame) != MG_JUMP_ROPE)
	{
		return Plugin_Continue;
	}

	if (status != 2)
	{
		return Plugin_Continue;
	}

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && IsPlayerAlive(client) && IsClientParticipating(client))
		{
			/** 
			 * DMG_GENERIC:
			 * 
			 * "Does not make any kind of visual or audible indication
			 *  of incoming damage, unlike other damage types."
			 * 
			 * Leave it as is.
			 */
			HurtEntity(client, float(JumpRope_DrainHP));
		}
	}
	
	float timescale = (GetHostMultiplier(1.0) + float(JumpRope_Speed) * 0.05);
	CreateTimer(1.0 / timescale, JumpRope_Drain);

	return Plugin_Stop;
}