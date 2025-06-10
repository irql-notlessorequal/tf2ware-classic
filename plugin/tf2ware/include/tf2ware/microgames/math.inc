#include "../microgame.inc"

char MathQuestion[24];

methodmap Math < Microgame
{
	public Math()
	{
		return view_as<Math>(MG_MATH);
	}

	public void OnMicrogameStart()
	{
		int random1 = GetRandomInt(2, 16);
		int random2 = GetRandomInt(2, 16);
		int form = GetRandomInt(1, 3);
		char formString[12];
	
		if (form == 1)
		{
			/**
			 * A guy one day kept saying 9001 to the math problems and was sad that it never was it.
			 * Take this as a 1/50 chance joke :P
			 */
			if (GetRandomInt(1, 50) == 1)
			{
				random1 = GetRandomInt(1, 9) * 1000;
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

	public bool OnPlayerChatMessage(int client, const char[] message)
	{
		if (!IsValidClient(client))
		{
			return false;
		}

		if (!IsClientParticipating(client))
		{
			return false;
		}

		if (IsPlayerAlive(client) && (status == 2) && (g_Complete[client] == false))
		{		
			if (!Math_IsStringANumber(message))
			{
				return false;
			}
		
			int guess = StringToInt(message);
		
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
				ShowHudText(client, 5, "%s = %d", MathQuestion, guess);            
				SetStateClient(client, true, true);

				if (!g_first)
				{
					for (int i = 1; i <= MaxClients; i++)
					{
						if (IsValidClient(i))
						{
							CPrintToChatEx(i, client, "{teamcolor}%N{green} guessed the answer first!", client);
						}
					}

					g_first = true;
				}

				return true;
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
				ShowHudText(client, 5, "%s = %d", MathQuestion, guess);
				ForcePlayerSuicide(client);
				return false;
			}        
		}

		return false;
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