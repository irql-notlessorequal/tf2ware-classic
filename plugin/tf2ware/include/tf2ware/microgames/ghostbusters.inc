#include "../microgame.inc"

#define GHOSTBUSTERS_MEDIC 0
#define GHOSTBUSTERS_SPY 1
#define GHOSTBUSTERS_HEAVY 2

#define MUSIC_GHOSTB "imgay/tf2ware/minigame_25_alt.mp3"

bool Ghostbusters_pp[MAXPLAYERS + 1] = false;
int Ghostbusters_target[MAXPLAYERS + 1] = 0;
int Ghostbusters_hurt[MAXPLAYERS+  1] = 0;
int Ghostbusters_hp[MAXPLAYERS + 1] = 0;
bool Ghostbusters_ended = false;

static const char VAR_GHOSTS_LAUGH[][] =
{
	"vo/spy_laughshort06.mp3",
	"vo/spy_laughshort05.mp3",
	"vo/spy_laughshort04.mp3",
	"vo/spy_laughshort03.mp3",
	"vo/spy_laughshort02.mp3",
	"vo/spy_laughshort01.mp3",
	"vo/spy_laughlong01.mp3",
	"vo/spy_laughhappy03.mp3",
	"vo/spy_laughhappy02.mp3",
	"vo/spy_laughhappy01.mp3",
	"vo/spy_laughevil02.mp3",
	"vo/spy_laughevil01.mp3"
}

static const int Ghostbusters_Pos[][] =
{
	{-3051, 8439, -20, 0},
	{-2615, 8446, 10, 90},
	{-3028, 8548, 551, 90},
	{-1654, 9570, -180, -90},
	{-72, 9141, 234, -117},
	{-2863, 8525, 427, 90},
	{-2860, 9083, 11, -90},
	{-43, 9147, 11, -90},
	{-2865, 8747, 11, 0},
	{-2911, 8078, 235, 90},
	{-1040, 8563, 427, 180},
	{-2260, 8464, 235, 0},
	{-3203, 8823, 234, 0},
	{-841, 9064, 123, -44},
	{-420, 9178, 11, -90},
	{-771, 8759, 10, -180},
	{-1924, 9034, 426, -138},
	{127, 8604, 234, -180},
	{-619, 8552, 234, 90},
	{-859, 8904, 298, 32}
};

methodmap Ghostbusters < Microgame
{
	public Ghostbusters()
	{
		return view_as<Ghostbusters>(MG_GHOSTBUSTERS);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 5;
	}

	public void OnMicrogameSetup()
	{
		precacheSound(MUSIC_GHOSTB);
	}

	public void OnMicrogameStart()
	{
		SetConVarInt(FindConVar("mp_teams_unbalance_limit"), 0);
		SetConVarInt(FindConVar("mp_autoteambalance"), 0);
	
		for (int i = 1; i <= MaxClients; i++)
		{
			g_Mission[i] = GHOSTBUSTERS_SPY;
		}

		int AllPlayers = GetClientCount(true);
		int target = 0;
		int attempts = 0;

		/**
		 * TODO(irql): Make this scale properly since it wasn't
		 * fun with 40 players at all.
		 */
		float medicCountRaw = float(GetActivePlayers()) / 3.5;
		int medics = RoundToCeil(medicCountRaw);
		int heavies = GetActivePlayers() / 6;

		if (medics <= 1) medics = 2;
		if (medics >= 8) medics = 7;
		if (heavies <= 0) heavies = 1;
		if (heavies >= 4) heavies = 3;

		int SpyClients[MAXPLAYERS + 1];
	
		/**
		 * Get the medics!
		 */
		for (int i = 1; i <= medics; i++)
		{
			attempts = 0;
			target = GetRandomInt(1, AllPlayers);

			while (((!IsValidClient(target)) || IsClientParticipating(target) == false ||
					IsFakeClient(target) || g_Mission[target] != GHOSTBUSTERS_SPY) && attempts <= 50)
			{
				target = GetRandomInt(1, AllPlayers);
				attempts += 1;
			}

			if (attempts <= 50)
			{
				g_Mission[target] = GHOSTBUSTERS_MEDIC;
			}
		}
	
		/**
		 * Get the heavies!
		 */
		for (int i = 1; i <= heavies; i++)
		{
			attempts = 0;
			target = GetRandomInt(1, AllPlayers);

			while (((!IsValidClient(target)) || IsClientParticipating(target) == false ||
					g_Mission[target] != GHOSTBUSTERS_SPY) && attempts <= 50)
			{
				target = GetRandomInt(1, AllPlayers);
				attempts += 1;
			}

			if (attempts <= 50)
			{
				g_Mission[target] = GHOSTBUSTERS_HEAVY;
			}
		}
	
		/**
		 * We only need the Spy clients so we can
		 * have voicelines generated, discard any other
		 * type of player.
		 */
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsClientParticipating(i))
			{
				/**
				 * Medics need to kill all ghosts to win,
				 * undead and spies win by default.
				 */
				SetStateClient(i, (g_Mission[i] != GHOSTBUSTERS_MEDIC));

				if (g_Mission[i] == GHOSTBUSTERS_SPY)
				{
					SpyClients[i - 1] = i;
				}    
			}
		}
	
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i))
			{
				SetEntProp(i, Prop_Send, "m_bGlowEnabled", 1);
			}
		}
	
		AddRandVocalization(VAR_GHOSTS_LAUGH, sizeof (VAR_GHOSTS_LAUGH),
				SpyClients, sizeof (SpyClients), 2.0, 4.0, SNDLEVEL_AIRCRAFT);
	
		g_attack = true;
		Ghostbusters_ended = false;
	}

	public void OnMicrogameEnd()
	{
		SetConVarInt(FindConVar("mp_teams_unbalance_limit"), 1);
		SetConVarInt(FindConVar("mp_autoteambalance"), 1);
	
		for (int i = 1; i <= MaxClients; i++)
		{
			SDKUnhook(i, SDKHook_OnTakeDamage, Ghostbusters_Damage);

			if (IsValidClient(i) && IsClientParticipating(i))
			{
				StopSound(i, SND_CHANNEL_SPECIFIC, MUSIC_GHOSTB);

				if (IsPlayerAlive(i))
				{
					Ghostbusters_FixSlap(i);

					SetEntityRenderMode(i, RENDER_NORMAL);
					SetEntityRenderColor(i, 255, 255, 255, 255);
				
					int default_fov = GetEntProp(i, Prop_Send, "m_iDefaultFOV"); 
					SetEntProp(i, Prop_Send, "m_iFOV", default_fov);
				}
			}
		}
	
		/**
		 * TODO(irql): Is this even needed?
		 */
		RespawnAll(true, false);
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{   
			float pos[3];
			float ang[3];
		
			Ghostbusters_hp[client] = GetClientHealth(client);

			if (g_Mission[client] == GHOSTBUSTERS_SPY)
			{
				if (GetClientTeam(client) == 2)
				{
					ChangeClientTeamAlive(client, 3);
				}

				SetClientClass(client, "spy");
				DisableClientWeapons(client);
			
				int id = Ghostbusters_GetGroupId(client, g_Mission[client]) -1;
				if (id >= sizeof(Ghostbusters_Pos))
				{
					id = 0;
				}
		
				pos[0] = float(Ghostbusters_Pos[id][0]);
				pos[1] = float(Ghostbusters_Pos[id][1]);
				pos[2] = float(Ghostbusters_Pos[id][2]);

				ang[0] = 0.0;
				ang[1] = float(Ghostbusters_Pos[id][3]);
				ang[2] = 0.0;
			
				SetEntityRenderMode(client, RENDER_TRANSCOLOR);
				SetEntityRenderColor(client, 255, 255, 255, 60);
				SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 280.0);
			}
			else if (g_Mission[client] == GHOSTBUSTERS_MEDIC)
			{
				if (GetClientTeam(client) == 3)
				{
					ChangeClientTeamAlive(client, 3);
				}

				SetClientClass(client, "medic");
				RemoveClientWeapons(client);
				GiveClientWeapon(client, "tf_weapon_medigun", 1, true);
				SetClientClass(client, "medic");

				pos[0] = -1787.0;
				pos[1] = 8389.0;
				pos[2] = 10.0;

				ang[0] = 0.0;
				ang[1] = 90.0;
				ang[2] = 0.0;

				SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 300.0);
			}
			else if (g_Mission[client] == GHOSTBUSTERS_HEAVY)
			{
				if (GetClientTeam(client) == 3)
				{
					ChangeClientTeamAlive(client, 2);
				}

				SetClientClass(client, "heavy");
				RemoveClientWeapons(client);
				GiveClientWeapon(client, "tf_weapon_fists", 2, true);
				SetClientClass(client, "heavy");

				pos[0] = -1713.0;
				pos[1] = 8738.0;
				pos[2] = 666.0;

				ang[0] = 0.0;
				ang[1] = 180.0;
				ang[2] = 0.0;

				SetEntityRenderColor(client, 0, 255, 0, 255);
				SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 220.0);
			}

			TeleportEntity(client, pos, ang, NULL_VECTOR);
			
			Ghostbusters_pp[client] = false;
			Ghostbusters_target[client] = 0;

			SDKHook(client, SDKHook_OnTakeDamage, Ghostbusters_Damage);

			/**
			 * We need to update player sprites.
			 */
			CreateSprite(client);
		}
	}

	public void OnClientDeath(int client)
	{
		SetStateClient(client, false, true);
		Ghostbusters_SetPP(client, false);
	
		CreateTimer(0.1, Ghostbusters_Check);
	}

	public void OnMicrogameFrame()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i))
			{

				if (GetClientTeam(i) == 2 && g_Mission[i] != GHOSTBUSTERS_HEAVY)
				{
					ChangeClientTeamAlive(i, 3);
				}

				if (GetClientTeam(i) == 3 && g_Mission[i] == GHOSTBUSTERS_HEAVY)
				{
					ChangeClientTeamAlive(i, 2);
				}
		
				if (g_Mission[i] == GHOSTBUSTERS_MEDIC)
				{
					int target = GetHealTarget(i);
					if (IsValidClient(target) && IsPlayerAlive(target) && g_Mission[target] != GHOSTBUSTERS_MEDIC)
					{
						Ghostbusters_SetPP(i, true);
						Ghostbusters_SetPP(target, true);
						Ghostbusters_target[i] = target;
					
						if (Ghostbusters_hurt[target] <= 0)
						{
							Ghostbusters_Slap(i);

							int default_fov = GetEntProp(target, Prop_Send, "m_iDefaultFOV"); 
							int health = Ghostbusters_hp[target];
							int new_fov = default_fov + RoundFloat(float(125 - health) * 0.75);

							SetEntProp(target, Prop_Send, "m_iFOV", new_fov);
					
							int newHealth = health - 3;
							if (newHealth > 0)
							{
								SetEntityHealth(target, newHealth);
								Ghostbusters_hp[target] = newHealth;
							}
							else
							{
								Ghostbusters_SetPP(target, false);
								ForcePlayerSuicide(target);
							}

							Ghostbusters_hurt[target] = 6;
						}
						else
						{
							Ghostbusters_hurt[target] -= 1;
						}
					}
					else
					{
						Ghostbusters_SetPP(i, false);

						int target2 = Ghostbusters_target[i];
						if (target2 != 0)
						{
							if (IsPlayerAlive(target2))
							{
								Ghostbusters_SetPP(target2, false);
							}

							Ghostbusters_target[i] = 0;
						}
					}
				}
			}
		}
	}
}

int Ghostbusters_GetGroupId(int client, int id)
{
	int out = 0;

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && IsClientParticipating(i))
		{
			if (g_Mission[i] == id)
			{
				out += 1;
			}

			if (i == client)
			{
				return out;
			}
		}
	}

	return 1;
}

Action Ghostbusters_Check(Handle timer)
{
	if (status != 2)
	{
		return Plugin_Continue;
	}

	if (view_as<Microgames>(currentMicrogame) != MG_GHOSTBUSTERS)
	{
		return Plugin_Continue;
	}

	if (Ghostbusters_ended == true)
	{
		return Plugin_Continue;
	}

	// Did the medics win?
	bool complete_medic = true;
	bool complete_spy = true;

	int winner = 0;

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_SPY) && IsPlayerAlive(i))
		{
			complete_medic = false;
		}

		if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_MEDIC) && IsPlayerAlive(i))
		{
			complete_spy = false;
		}
	}
	
	if (complete_spy)
	{
		winner = 2;
	}

	if (complete_medic)
	{
		winner = 1;
	}

	if (winner > 0)
	{
		Ghostbusters_ended = true;

		if (winner == 1)
		{
			CPrintToChatAll("{lightgreen}All ghost spies are dead... {green}surviving medics win!");
		}

		if (winner == 2)
		{
			CPrintToChatAll("{lightgreen}All medics are dead... {green}surviving undead win!");
		}

		for (int i = 1; i <= MaxClients; i++)
		{
			if (winner == 1)
			{
				if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_MEDIC) && IsPlayerAlive(i))
				{
					SetStateClient(i, true, true);
				}

				if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] != GHOSTBUSTERS_MEDIC))
				{
					SetStateClient(i, false, true);

					if (IsPlayerAlive(i))
					{
						ForcePlayerSuicide(i);
					}
				}
			}

			if (winner == 2)
			{
				if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] != GHOSTBUSTERS_MEDIC) && IsPlayerAlive(i))
				{
					SetStateClient(i, true, true);
				}

				if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_MEDIC))
				{
					SetStateClient(i, false, true);
					
					if (IsPlayerAlive(i))
					{
						ForcePlayerSuicide(i);
					}
				}
			}
		}

		CreateTimer(3.0, Ghostbusters_TEnd);
	}

	return Plugin_Continue;
}

Action Ghostbusters_TEnd(Handle timer)
{
	if (status != 2)
	{
		return Plugin_Continue;
	}

	if (view_as<Microgames>(currentMicrogame) != MG_GHOSTBUSTERS)
	{
		return Plugin_Continue;
	}
	
	TriggerTimer(MicrogameTimer);
	
	return Plugin_Stop;
}

void Ghostbusters_Slap(int client)
{
	float fEye[3];
	GetClientEyeAngles(client, fEye);

	fEye[0] += GetRandomFloat(-10.0, 10.0) * 0.1;
	fEye[1] += GetRandomFloat(-10.0, 10.0) * 0.1;
	fEye[2] += GetRandomFloat(-10.0, 10.0) * 0.1;

	TeleportEntity(client, NULL_VECTOR, fEye, NULL_VECTOR);
}

void Ghostbusters_FixSlap(int client)
{
	float fEye[3];
	GetClientEyeAngles(client, fEye);

	fEye[2] = 0.0;

	TeleportEntity(client, NULL_VECTOR, fEye, NULL_VECTOR);
}

void Ghostbusters_SetPP(int client, bool input)
{
	if (IsValidClient(client))
	{
		if (input == true && Ghostbusters_pp[client] == false)
		{
			TF2_SetPlayerPowerPlay(client, true);
			Ghostbusters_pp[client] = true;
			SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 500.0);
		}

		if (input == false && Ghostbusters_pp[client] == true)
		{
			TF2_SetPlayerPowerPlay(client, false);
			Ghostbusters_pp[client] = false;

			if (g_Mission[client] == GHOSTBUSTERS_SPY)
			{
				SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 280.0);
			}
			else
			{
				SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 300.0);
			}

			Ghostbusters_FixSlap(client);
		}
	}
}

Action Ghostbusters_Damage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (IsValidClient(victim) && IsValidClient(attacker)
			&& g_Mission[victim] == GHOSTBUSTERS_SPY
			&& g_Mission[attacker] == GHOSTBUSTERS_HEAVY)
	{
		damage = 0.0;
		return Plugin_Changed;
	}

	if (IsValidClient(victim) && IsValidClient(attacker) && g_Mission[victim] == GHOSTBUSTERS_HEAVY)
	{
		damage = 0.0;
		return Plugin_Changed;
	}

	return Plugin_Continue;
}