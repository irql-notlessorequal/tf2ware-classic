/**
 * TF2Ware Classic
 *
 * Copyright (C) 2025		IRQL_NOT_LESS_OR_EQUAL
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
	1:      Speed Up        Everything is faster
	2:      Don't Touch     Touch other players and fail
	3:      x2 Bosses       Two bosses
	4:      Singleplayer    Everyone is invisible
	5:      Worst is Best   The person with the least points win
	6:      Bonk            Everyone gets a bat to push people with    
	7:      Thirdperson     Forced into thirdperson!

	8:      Wipeout
	9:      Random Score
*/
#pragma newdecls required

#define SPECIAL_TOTAL	9

bool Special_TwoBosses = false;

char var_special_name[][] =
{
	"SUPER SPEED",
	"NO TOUCHING",
	"x2 BOSS BATTLE",
	"SINGLEPLAYER",
	"LEAST IS BEST",
	"BONK",
	"THIRDPERSON",
	"WIPEOUT",
	"RANDOM SCORE"
};

char var_special_phrases[][] =
{
	"Special_SuperSpeed",
	"Special_NoTouching",
	"Special_DoubleBoss",
	"Special_Singleplayer",
	"Special_LeastIsBest",
	"Special_Bonk",
	"Special_ThirdPerson",
	"Special_Wipeout",
	"Special_RandomScore"
};

char FAKE_SPECIAL_ROUNDS[][] =
{ 
	"FAT LARD RUN",
	"MOUSTACHIO",
	"LOVE STORY",
	"SIZE MATTERS",
	"ENGINEERD",
	"IDLE FOR HATS",
	"TF2 BROS: BRAWL",
	"HOT SPY ON ICE",
	"SV_CHEATS 1",
	"STOP THAT SPYCRAB"
};

public void Special_NoTouch(int entity, int other)
{
	if (SpecialRound != NO_TOUCHING)
	{
		return;
	}

	if (view_as<Microgames>(currentMicrogame) == MG_HUGGING ||
	    view_as<Microgames>(currentMicrogame) == MG_GOOMBA)
	{
		return;
	}

	char classname[MAX_NAME_LENGTH];
	char classname2[MAX_NAME_LENGTH];
	GetEdictClassname(entity, classname, sizeof(classname));
	GetEdictClassname(other, classname2, sizeof(classname2));
	
	if (StrEqual(classname, "player") && StrEqual(classname2, "player")
			&& IsValidClient(entity) && IsValidClient(other) && status == 2
			&& IsPlayerAlive(entity) && IsPlayerAlive(other)
			&& GetClientTeam(entity) != GetClientTeam(other))
	{
		SetStateClient(entity, false, true);
		SetStateClient(other, false, true);

		ForcePlayerSuicide(entity);
		ForcePlayerSuicide(other);

		GetClientName(entity, classname, MAX_NAME_LENGTH);
		GetClientName(other, classname2, MAX_NAME_LENGTH);

		CPrintToChatEx(entity, other, "%t", "NoTouching_PlayerTouched", classname2);
		CPrintToChat(entity, "%T", "NoTouching_Explainer", LANG_SERVER);

		CPrintToChatEx(other, entity, "%t", "NoTouching_PlayerTouched", classname);
		CPrintToChat(other, "%T", "NoTouching_Explainer", LANG_SERVER);
	}
}


public Action Special_DamagePush(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (IsValidClient(attacker) && IsValidClient(victim) && SpecialRound == BONK)
	{
		char Weapon[128];
		GetClientWeapon(attacker, Weapon, sizeof(Weapon));

		if (StrEqual(Weapon, "tf_weapon_bat"))
		{
			float ang[3];
			float vel[3];
			GetClientEyeAngles(attacker, ang);

			vel[0] -= 300.0 * Cosine(DegToRad(ang[1])) * -1.0;
			vel[1] -= 300.0 * Sine(DegToRad(ang[1])) * -1.0;
			vel[2] += 300.0;

			TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, vel);
			damage = 0.0;
		}
	}

	return Plugin_Changed;
}

void ResetSpecialRoundEffect(int specialRound)
{
	switch (specialRound)
	{
		case NO_TOUCHING:
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client))
				{
					SDKUnhook(client, SDKHook_Touch, Special_NoTouch);
				}
			}

			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && !IsFakeClient(client))
				{
					ToggleThirdperson(client, false);
				}
			}
		}

		case SINGLEPLAYER:
		{
			NoCollision(false);
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client))
				{
					SetEntityRenderColor(client, 255, 255, 255, 255);
					SetEntityRenderMode(client, RENDER_NORMAL);
				}
			}
		}

		case BONK:
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client))
				{
					SDKUnhook(client, SDKHook_OnTakeDamage, Special_DamagePush);
				}
			}
		}

		case THIRDPERSON:
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && !IsFakeClient(client))
				{
					ToggleThirdperson(client, false);
				}
			}
		}
	}
}

void AddSpecialRoundEffect(int specialRound)
{
	switch (specialRound)
	{
		case NO_TOUCHING:
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client))
				{
					SDKHook(client, SDKHook_Touch, Special_NoTouch);
				}
			}

			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && !IsFakeClient(client))
				{
					ToggleThirdperson(client, true);
				}
			}
		}

		case DOUBLE_BOSS_BATTLE:
		{
			Special_TwoBosses = false;
		}

		case SINGLEPLAYER:
		{
			NoCollision(true);

			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client))
				{
					SetEntityRenderColor(client, 255, 255, 255, 0);
					SetEntityRenderMode(client, RENDER_NONE);
				}
			}
		}

		case BONK:
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client))
				{
					SDKHook(client, SDKHook_OnTakeDamage, Special_DamagePush);
				}
			}			
		}

		case THIRDPERSON:
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && !IsFakeClient(client))
				{
					ToggleThirdperson(client, true);
				}
			}
		}
	}
}

bool SpecialPrecache()
{
	/* Unused. */
	return true;
}