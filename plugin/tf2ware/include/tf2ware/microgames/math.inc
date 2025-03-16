#include "../microgame.inc"

char MathQuestion[24];

methodmap Math < Microgame
{
	public Math()
	{
		return view_as<Math>(MG_MATH);
	}

	public void OnMicrogameSetup()
	{
		RegConsoleCmd("say", Math_Player_Say);
		RegConsoleCmd("say_team", Math_Player_Say);
	}

	public void OnMicrogameStart()
	{
		int random1 = GetRandomInt(3, 15);
		int random2 = GetRandomInt(3, 15);
		int form = GetRandomInt(1,3);
		char formString[12];
	
		if (form == 1)
		{
			/**
			 * A guy one day kept saying 9001 to the math problems and was sad that it never was it.
			 * Take this as a 1/50 chance joke :P
			 */
			if (GetRandomInt(1, 50) == 1)
			{
				random1 = GetRandomInt(1,9)*1000;
				random2 = 9001 - random1;
			}

			g_result = random1 + random2;
			Format(formString, sizeof(formString), "+");
		}
		else if (form == 2)
		{
			g_result = random1 - random2;
			Format(formString, sizeof(formString), "-");
		}
		else if (form == 3)
		{
			Format(formString, sizeof(formString), "*");
			random1 = GetRandomInt(2, 10);
			random2 = GetRandomInt(2, 10);
			g_result = random1 * random2;
		}
	
		// Save the question string so we can replace it in when the player answers
		Format(MathQuestion, sizeof(MathQuestion), "%d %s %d", random1, formString, random2);
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SetHudTextParams(
				.x = -1.0,
				.y = 0.4,
				.holdTime = 4.0,
				.r = 255,
				.g = 255,
				.b = 255,
				.a = 255,
				.effect = 0,
				.fxTime = 6.0,
				.fadeIn = 0.2,
				.fadeOut = 0.5
			);
			ShowHudText(client, 5, "%s = ?", MathQuestion);
		}
	}
}

stock bool Math_IsStringANumber(const char arg[64])
{
	if (StringToInt(arg) != 0)
	{
		return true;
	}
	else if (StrEqual(arg, "0"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

Action Math_Player_Say(int iClient, int iArgs)
{
	// Check to make sure this is happening during the math minigame
	if (view_as<Microgames>(currentMicrogame) != MG_MATH)
	{
		return Plugin_Continue;
	}

	if (iArgs < 1)
	{
		return Plugin_Continue;
	}

	if (!(IsValidClient(iClient)))
	{
		return Plugin_Continue;
	}

	if (!IsClientParticipating(iClient))
	{
		return Plugin_Continue;
	}
	
	if (IsPlayerAlive(iClient) && (status == 2) && (g_Complete[iClient] == false))
	{
		// Retrieve the first argument and check it's a valid trigger
		char strArgument[64];
		GetCmdArg(1, strArgument, sizeof(strArgument));
		
		if (!Math_IsStringANumber(strArgument))
		{
			return Plugin_Continue;
		}
		
		new guess = StringToInt(strArgument);
		
		if (guess == g_result)
		{
			SetHudTextParams(
				.x = -1.0,
				.y = 0.4,
				.holdTime = 3.0,
				.r = 0,
				.g = 255,
				.b = 0,
				.a = 255,
				.effect = 0,
				.fxTime = 6.0,
				.fadeIn = 0.2,
				.fadeOut = 0.5
			);

			// Replace the current display to include the guess instead of a question mark
			ShowHudText(iClient, 5, "%s = %d", MathQuestion, guess);            
			SetStateClient(iClient, true, true);

			if (!g_first)
			{
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsValidClient(i))
					{
						CPrintToChatEx(i, iClient, "{teamcolor}%N{green} guessed the answer first!", iClient);
					}
				}

				g_first = true;
			}

			return Plugin_Handled;
		}
		else
		{
			SetHudTextParams(
				.x = -1.0,
				.y = 0.4,
				.holdTime = 3.0,
				.r = 255,
				.g = 0,
				.b = 0,
				.a = 255,
				.effect = 0,
				.fxTime = 6.0,
				.fadeIn = 0.2,
				.fadeOut = 0.5
			);

			// Use a notequals sign!
			ShowHudText(iClient, 5, "%s = %d", MathQuestion, guess);
			ForcePlayerSuicide(iClient);
			return Plugin_Continue;
		}        
	}
	return Plugin_Continue;
}