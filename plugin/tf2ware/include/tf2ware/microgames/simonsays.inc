// SimonSays module

#define A_TAUNT 0
#define A_JUMP 1
#define A_CROUCH 2
#define A_LOOKUP 3
#define A_LOOKDOWN 4

// simon_action == 0 | Taunt
// simon_action == 1 | Jump
// simon_action == 2 | Crouch
// simon_action == 3 | Look Up
// simon_action == 4 | Look Down

// NEED TO ALLOW BUILDINGS

int simon_action;
int simon_or_someone;

methodmap SimonSays < Microgame
{
	public SimonSays()
	{
		return view_as<SimonSays>(MG_SIMON_SAYS);
	}

	public void OnMicrogameStart()
	{
		simon_action = GetRandomInt(0,2);
		simon_or_someone = GetRandomInt(0,1);

		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client))
			{
				g_Mission[client] = simon_action * 2 + simon_or_someone;
			}
		}

		/**
		 * If simon says it (simon_or_someone == 0) set state to false until proven true
		 * Otherwise, set to true until proven false
		 */
		SetStateAll(simon_or_someone == 1);
	}

	public void OnMicrogameFrame()
	{
		if (g_TimeLeft <= 6)
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				/**
				 * If the player is valid and taunting
				 */
				if (IsValidClient(client) && IsPlayerAlive(client) && IsClientParticipating(client))
				{
					if (SimonSays_IsPlayerDoing(client))
					{
						if (simon_or_someone == 0)
						{
							/**
							 * If simon said it
							 */
							SetStateClient(client, true, true);
						}
						else if (g_Complete[client])
						{
							/**
							 * Otherwise, simon didn't say it and if we still haven't lost
							 */
							char input[512];
							/**
							 * Display "Simon didn't say it!"
							 */
							Format(input, sizeof(input), "tf2ware_minigame_%d_11", GetMinigameConfNum("SimonSays", "id"));
							SetOverlay(client, input);
							SetStateClient(client, false, true);
						}
					}
				}
			}
		}
	}
};

/**
 * Methodmaps don't let us easily embed in functions, leave them local to the include.
 */
bool SimonSays_IsPlayerDoing(int client)
{
	switch (simon_action)
	{
		case A_TAUNT:
		{
			if (TF2_IsPlayerInCondition(client, TFCond_Taunting))
			{
				return true;
			}
		}
		case A_JUMP:
		{
			if (GetClientButtons(client) & IN_JUMP)
			{
				return true;
			}
		}
		case A_CROUCH:
		{
			if (GetClientButtons(client) & IN_DUCK)
			{
				return true;
			}
		}
	}
	
	return false;
}