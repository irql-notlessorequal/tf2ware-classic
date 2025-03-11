/*
	1:      Speed Up        Everything is faster
	2:      Don't Touch     Touch other players and fail
	3:      x2 Bosses       Two bosses
	4:      Singleplayer    Everyone is invisible
	5:      Worst is Best   The person with the least points win
	6:      Bonk            Everyone gets a bat to push people with    
	7:      Thirdperson     Forced into thirdperson!    

*/

#define SPECIAL_TOTAL 7

bool Special_TwoBosses = false;

char var_special_name[][] = {
	"SUPER SPEED", "NO TOUCHING", "x2 BOSS BATTLE", "SINGLEPLAYER", "LEAST IS BEST", "BONK", "THIRDPERSON"
};

char var_special_desc[][] = {
	"The Game is Faster Than Usual!!", "Don't Touch Any Enemy Players or You'll Lose!!", "Two Boss Battles!!",
	"You're Playing Alone...?", "The Player With The Least Points Win!!",
	"Push Players With Your Bat!!","Everything Is At A New Perspective!!"
};

public Special_NoTouch(entity, other) {
	if (SpecialRound != NO_TOUCHING) return;
	if (StrEqual(minigame, "Hugging")) return;

	new String:classname[64];
	new String:classname2[64];
	GetEdictClassname(entity, classname, sizeof(classname));
	GetEdictClassname(other, classname2, sizeof(classname2));
	if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && GetClientTeam(entity) != GetClientTeam(other)) {
		SetStateClient(entity, false, true);
		SetStateClient(other, false, true);
		ForcePlayerSuicide(entity);
		ForcePlayerSuicide(other);
		CPrintToChatEx(entity, other, "You touched: {teamcolor}%N{default}", other);
		CPrintToChat(entity, "In this SPECIAL ROUND, you are {red}not allowed to touch anyone{default}!");
		CPrintToChatEx(other, entity, "You touched: {teamcolor}%N{default}", entity);
		CPrintToChat(other, "In this SPECIAL ROUND, you are {red}not allowed to touch anyone{default}!");
	}
}


public Action:Special_DamagePush(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
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
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i))
				{
					SDKUnhook(i, SDKHook_Touch, Special_NoTouch);
				}
			}
		}

		case SINGLEPLAYER:
		{
			NoCollision(false);
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i))
				{
					SetEntityRenderColor(i, 255, 255, 255, 255);
					SetEntityRenderMode(i, RENDER_NORMAL);
				}
			}			
		}

		case BONK:
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i))
				{
					SDKUnhook(i, SDKHook_OnTakeDamage, Special_DamagePush);
				}
			}			
		}

		case THIRDPERSON:
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && !IsFakeClient(i))
				{
					ClientCommand(i, "wait; firstperson");
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
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i))
				{
					SDKHook(i, SDKHook_Touch, Special_NoTouch);
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
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i))
				{
					SetEntityRenderColor(i, 255, 255, 255, 0);
					SetEntityRenderMode(i, RENDER_NONE);
				}
			}			
		}

		case BONK:
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i))
				{
					SDKHook(i, SDKHook_OnTakeDamage, Special_DamagePush);
				}
			}			
		}

		case THIRDPERSON:
		{
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && !IsFakeClient(i))
				{
					ClientCommand(i, "wait; thirdperson");
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