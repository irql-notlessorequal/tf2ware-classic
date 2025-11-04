#include "../microgame.inc"

#define REQUIRED_SCOUTS          2
#define HEAVY_DISTANCE_THRESHOLD 100

/**
 * Originally Mecha wanted it to be like a real Pinata
 * meaning that Scouts would have to kill a Heavy
 * representing a Pinata (nightmare fuel)
 * 
 * Instead take the "Stand near the Heavy" microgame from
 * TF2Ware2 and implement that instead since it makes more
 * sense over whatever Mecha was cooking.
 */
methodmap Pinata < Microgame
{
	public Pinata()
	{
		return view_as<Pinata>(MG_PINATA);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 4;
	}

	public void OnMicrogameStart()
	{
		int AllPlayers = GetClientCount(true);
		int target;

		do
		{
			target = MalletGetRandomInt(1, AllPlayers);

			if (!IsValidClient(target))
				continue;

			if (!IsPlayerAlive(target))
				continue;

			if (!IsClientParticipating(target))
				continue;

			/* We've found a valid candidate, break. */
			break;
		} while (true);

		g_attack = true;
		g_Mission[target] = 1;
		g_result = 0;
		SetStateAll(false);
	}

	public void OnClientDeath(int client)
	{
		if (!g_Mission[client])
		{
			g_result += 1;
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (!IsValidClient(client))
		{
			return;
		}

		SDKHook(client, SDKHook_OnTakeDamage, Pinata_OnTakeDamageClient);

		if (g_Mission[client])
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "heavy");
			GiveClientWeapon2(client, view_as<int>(TF2WARE_WEAPON_EVICTION_NOTICE), "tf_weapon_fists", 0, true);
		}
		else
		{
			SetClientClass(client, "scout");
			DisableClientWeapons(client);
		}
	}

	public void OnMicrogameEnd()
	{
		int heavy = -1;

		/* Award the heavy the win, if they got at least two scouts. */
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && g_Mission[client])
			{
				heavy = client;
				SetStateClient(client, (g_result >= REQUIRED_SCOUTS));
				break;
			}
		}

		/* Make sure our heavy is still alive... */
		if (!heavy)
		{
			CPrintToChatAll("%T", "Pianta_HeavyDead", LANG_SERVER);
			SetStateAll(true);
		}
		else
		{
			float heavyPos[3];
			float scoutPos[3];
			GetClientAbsOrigin(heavy, heavyPos);

			for (int client = 1; client <= MaxClients; client++)
			{
				if (!IsValidClient(client))
					continue;

				if (!IsClientParticipating(client))
					continue;

				if (!IsPlayerAlive(client))
					continue;

				if (client == heavy)
					continue;

				GetClientAbsOrigin(client, scoutPos);
				
				/* Only award the player if they are close enough. */
				if (FloatAbs(heavyPos[0] - scoutPos[0]) <= HEAVY_DISTANCE_THRESHOLD &&
					FloatAbs(heavyPos[1] - scoutPos[1]) <= HEAVY_DISTANCE_THRESHOLD)
				{
					SetStateClient(client, true);
				}
			}
		}
	}

	public void OnMicrogamePostEnd()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_OnTakeDamage, Pinata_OnTakeDamageClient);
		}
	}
}

Action Pinata_OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (!IsValidClient(attacker))
	{
		return Plugin_Continue;
	}

	/* Only allow the Heavy to defend themselves. */
	if (g_Mission[attacker])
	{
		damage = 450.0;
	}
	else
	{
		damage = 0.0;
	}

	return Plugin_Changed;
}