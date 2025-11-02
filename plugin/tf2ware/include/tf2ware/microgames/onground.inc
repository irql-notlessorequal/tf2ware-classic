#include "../microgame.inc"

#define HEIGHT_THRESHOLD 	200
#define MUSIC_ON_GROUND_ALT "imgay/tf2ware/minigame_12_alt.mp3"

static int MicrogameVariation = 0;

/** Filler microgame, taken from TF2Ware2.  */ 
methodmap OnGround < Microgame
{
	public OnGround()
	{
		return view_as<OnGround>(MG_ON_GROUND);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players;
	}

	public void OnMicrogameSetup()
	{
		/* The regular track is pre-cached by the gamemode. */
		precacheSound(MUSIC_ON_GROUND_ALT);
	}

	public void OnMicrogameStart()
	{
		MicrogameVariation = MalletGetRandomInt(0, 1);
		g_attack = true;

		SetConVarInt(ConVar_SVGravity, 0);
		SetStateAll(true);
	}

	public void OnMicrogameEnd()
	{
		ResetConVar(ConVar_SVGravity);

		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_OnTakeDamage, OnGround_OnTakeDamageClient);
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_OnTakeDamage, OnGround_OnTakeDamageClient);

			RemoveClientWeapons(client);
			SetClientClass(client, "soldier");

			if (MicrogameVariation)
			{
				GiveClientWeapon2(client, view_as<int>(TF2WARE_WEAPON_DIRECT_HIT), "tf_weapon_rocketlauncher_directhit", 0, true);
			}
			else
			{
				GiveClientWeapon(client, "tf_weapon_rocketlauncher", 0, true);
			}
		}
	}

	public void OnMicrogameFrame()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsClientParticipating(client))
			{
				float selfPos[3];
				GetEntPropVector(client, Prop_Send, "m_vecOrigin", selfPos);

				if (selfPos[2] > HEIGHT_THRESHOLD)
				{
					ForcePlayerSuicide(client);
					SetStateClient(client, false, true);
				}
			}
		}
	}
}

Action OnGround_OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (IsValidClient(attacker))
	{
		damage = 0.0;
	}

	return Plugin_Changed;
}