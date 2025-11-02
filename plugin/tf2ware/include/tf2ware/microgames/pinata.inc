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

		if (g_Mission[client])
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "heavy");

			/* TODO: Give heavy the Eviction Notice with increased attack speed and one-hit kill. */
		}
		else
		{
			SetClientClass(client, "scout");
			DisableClientWeapons(client);
		}
	}

	public void OnMicrogameEnd()
	{
		/* Award the heavy the win, if they got at least two scouts. */
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && g_Mission[client] && g_result >= REQUIRED_SCOUTS)
			{
				SetStateClient(client, true);
				break;
			}
		}

		/* TODO: Award the scouts near the heavy */
	}
}