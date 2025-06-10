#include "../microgame.inc"

static const char VAR_HEAVY_LOVE[][] =
{
	"imgay/tf2ware/heavy_ilu.wav",
	"vo/heavy_specialcompleted08.mp3",
	"vo/heavy_award04.mp3"
};

static const char VAR_SCOUT_DISGUST[][] =
{
	"vo/scout_autodejectedtie01.mp3",
	"vo/scout_autodejectedtie02.mp3",
	"vo/scout_autodejectedtie03.mp3",
	"vo/scout_autodejectedtie04.mp3",
	"vo/scout_cartstaycloseoffense03.mp3",
	/** 
	 * "vo/scout_cartstopitdefense.wav" never existed, wtf Mecha
	 */
	"vo/scout_dominationhvy02.mp3",
	"vo/scout_dominationhvy06.mp3",
	"vo/scout_dominationhvy08.mp3",
	"vo/scout_domination05.mp3",
	"vo/scout_domination06.mp3",
	"vo/scout_domination09.mp3",
	"vo/scout_domination08.mp3",
	"vo/scout_domination20.mp3"
};

methodmap Hugging < Microgame
{
	public Hugging()
	{
		return view_as<Hugging>(MG_HUGGING);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 6;
	}

	public void OnMicrogameSetup()
	{
		char input[512];
		for (int i = 1; i <= sizeof(VAR_HEAVY_LOVE); i++)
		{
			Format(input, sizeof(input), "sound/%s", VAR_HEAVY_LOVE[i - 1]);
			AddFileToDownloadsTable(input);
			precacheSound(VAR_HEAVY_LOVE[i - 1]);
		}

		for (int i = 1; i <= sizeof(VAR_SCOUT_DISGUST); i++)
		{
			Format(input, sizeof(input), "sound/%s", VAR_SCOUT_DISGUST[i - 1]);
			AddFileToDownloadsTable(input);
			precacheSound(VAR_SCOUT_DISGUST[i - 1]);
		}
	}

	public void OnMicrogameStart()
	{
		int AllPlayers = GetClientCount(true);
		int target = 0;
		int activePlayers = GetActivePlayers();
		
		/**
		 * Previously Mecha would limit the heavies to:
		 * 
		 * `ACTIVE_PLAYERS / 4`
		 * 
		 * And would then bound it past four heavies.
		 * 
		 * One problem, we can have more than 24 players nowadays.
		 */
		int heavies = RoundToFloor(activePlayers / 7.0);
		
		/**
		 * If we have less than seven players,
		 * make sure we have at least one heavy.
		 */
		if (heavies == 0)
		{
			heavies = 1;
		}

		int heavyClients[MAXPLAYERS + 1];
		int scoutClients[MAXPLAYERS+ 1];

		/**
		 * Switch to a loop so that we can actually isolate players with Wipeout
		 * being active.
		 * 
		 * Also allow bots to be a cuddly heavy.
		 */
		do
		{
			/**
			 * Sanity check, don't loop if there is no-one.
			 */
			if (activePlayers == 0)
			{
				break;
			}

			target = GetRandomInt(1, AllPlayers);
			if (!IsValidClient(target))
			{
				continue;
			}

			if (!IsClientParticipating(target))
			{
				continue;
			}

			if (g_Mission[target] > 0 || g_Spawned[target] == false)
			{
				continue;
			}

			g_Mission[target] = 1;
			heavies--;
		} while (heavies != 0);

		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsClientParticipating(client))
			{
				if (g_Mission[client] == 0)
				{
					SetStateClient(client, true);
					scoutClients[client - 1] = client;
				}
				else
				{
					SetStateClient(client, false);
					heavyClients[client - 1] = client;
				}
			}
		}

		AddRandVocalization(VAR_HEAVY_LOVE, sizeof(VAR_HEAVY_LOVE), heavyClients, sizeof (heavyClients), 4.0, 8.0);
		AddRandVocalization(VAR_SCOUT_DISGUST, sizeof(VAR_SCOUT_DISGUST), scoutClients, sizeof (scoutClients), 1.5, 5.0);

		NoCollision(false);
	}

	public void OnMicrogameEnd()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_OnTakeDamage, Hugging_OnTakeDamageClient);
			SDKUnhook(client, SDKHook_Touch, Hugging_OnPlayerTouch);
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_Touch, Hugging_OnPlayerTouch);
			SDKHook(client, SDKHook_OnTakeDamage, Hugging_OnTakeDamageClient);
	
			float pos[3];
			float ang[3];
			float vel[3];

			vel[0] = 0.0;
			vel[1] = 0.0;
			vel[2] = 0.0;

			int column = client;
			int row = 0;

			while (column > 10)
			{
				column = column - 10;
				row = row + 1;
			}

			if (g_Mission[client] == 0)
			{
				if (GetClientTeam(client) == 2)
				{
					ChangeClientTeamAlive(client, 3);
				}

				SetClientClass(client, "scout");
				DisableClientWeapons(client);
		
				pos[0] = 142.0 - float(column * 80);
				pos[1] = -2040.0 + float(row * 80);
				pos[2] = -700.0;
				ang[0] = 0.0;
				ang[1] = -90.0;
				ang[2] = 0.0;
			}
			else if (g_Mission[client] == 1)
			{
				if (GetClientTeam(client) == 3)
				{
					ChangeClientTeamAlive(client, 2);
				}

				SetClientClass(client, "heavy");
				DisableClientWeapons(client);

				pos[0] = 142.0 - float(column * 80);
				pos[1] = -2965.0 - float(row * 80);
				pos[2] = -700.0;
				ang[0] = 0.0;
				ang[1] = 90.0;
				ang[2] = 0.0;
			}
			
			TeleportEntity(client, pos, ang, vel);
		}
	}

	public void OnClientDeath(int client)
	{
		SetStateClient(client, false, true);
	
		CreateTimer(0.1, Hugging_Check);
	}
}

Action Hugging_Check(Handle timer)
{
	if (status != 2)
	{
		return Plugin_Continue;
	}

	if (view_as<Microgames>(currentMicrogame) != MG_HUGGING)
	{
		return Plugin_Continue;
	}

	bool complete = true;
	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && (g_Mission[client] == 0) && IsClientParticipating(client) && IsPlayerAlive(client))
		{
			complete = false;
		}
	}

	if (complete)
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && (g_Mission[client] == 1) && IsClientParticipating(client))
			{
				SetStateClient(client, true, true);
			}
		}
	}

	return Plugin_Continue;
}

Action Hugging_OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (status == 2)
	{
		if (damage > 0)
		{
			damage = 1.0;
		}

		return Plugin_Changed;
	}

	return Plugin_Continue;
}

void Hugging_OnPlayerTouch(int entity, int other)
{
	if (status != 2)
	{
		return;
	}
	
	if (view_as<Microgames>(currentMicrogame) != MG_HUGGING)
	{
		return;
	}

	char classname[64];
	char classname2[64];
	GetEdictClassname(entity, classname, sizeof(classname));
	GetEdictClassname(other, classname2, sizeof(classname2));

	/**
	 * Once again, what the fuck Mecha.
	 */
	if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) &&
		IsValidClient(other) && status == 2 && IsPlayerAlive(entity) &&
		IsPlayerAlive(other) && (g_Mission[other] > 0 || g_Mission[entity] > 0))
	{
		if (g_Mission[entity] > 0 && g_Mission[other] > 0)
		{
			return;
		}

		int attacker = entity;
		int victim = other;
		if (g_Mission[other] > 0)
		{
			attacker = other;
			victim = entity;
		}

		ForcePlayerSuicide(victim);
		EmitSoundToAll(SOUND_HEAVY_KISS, attacker);
		EmitSoundToClient(victim, SOUND_HEAVY_KISS);
	}
}