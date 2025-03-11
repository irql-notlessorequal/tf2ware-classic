#include "../microgame.inc"

methodmap BBall < Microgame
{
	public BBall()
	{
		return view_as<BBall>(MG_BBALL);
	}

	public void OnMicrogameStart()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i))
			{
				SDKHook(i, SDKHook_OnTakeDamage, BBall_OnTakeDamageClient);
			}
		}

		g_attack = true;
	}

	public void OnMicrogameEnd()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			SDKUnhook(i, SDKHook_OnTakeDamage, BBall_OnTakeDamageClient);
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "demoman");
			GiveClientWeapon(client, "tf_weapon_grenadelauncher", 0, true);
		
			float vel[3];
			vel[0] = 0.0;
			vel[1] = 0.0;
			vel[2] = 0.0;
		
			int column = client;
			int row = 0;
			while (column > 17)
			{
				column = column - 17;
				row = row + 1;
			}
		
			float pos[3];
			pos[0] = 3076.0  + float(column*55);
			pos[1] = -1930.0 + float(row*55);
			pos[2] = -260.0;

			float ang[3];
			ang[0] = 0.0;
			ang[1] = 90.0;
			ang[2] = 0.0;
		
			TeleportEntity(client, pos, ang, vel);
		}
	}
}

public void OnEntityCreated(int entity, const char[] classname)
{
	if (view_as<Microgames>(currentMicrogame) != MG_BBALL)
	{
		return;
	}

	if ((status == 2) && (StrEqual(classname, "tf_projectile_pipe")))
	{
		SDKHook(entity, SDKHook_StartTouch, Hook_StartTouch_Pipe);
	}
}

Action Hook_StartTouch_Pipe(int entity, int other)
{
	char classname[64];
	GetEdictClassname(other, classname, sizeof(classname));
	
	if (StrEqual(classname, "func_button") && status == 2)
	{
		int owner = GetEntPropEnt(entity, Prop_Send, "m_hThrower");
		if (g_Complete[owner] == false)
		{
			EmitSoundToClient(owner, SOUND_MINISCORE, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			g_Minipoints[owner] += 1;
			if (g_Minipoints[owner] >= 7)
			{
				SetStateClient(owner, true, true);
			}
			AcceptEntityInput(entity, "Kill");
		}
	}

	return Plugin_Continue;
}

Action BBall_GiveHealth(Handle hTimer, int iClient)
{
	if (IsValidClient(iClient) && IsPlayerAlive(iClient))
	{
		int iMaxHealth = GetClientMaxHealth(iClient);
		SetEntityHealth(iClient, iMaxHealth);  
	}

	return Plugin_Continue;
}

Action BBall_OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (status == 2 && IsValidClient(victim))
	{
		int iMaxHealth = GetClientMaxHealth(victim);
		SetEntityHealth(victim, iMaxHealth);
		
		if (damage >= float(iMaxHealth - 5))
		{
			damage = float(iMaxHealth - 10);
		}

		if (damage > 150.0)
		{
			damage = 150.0;
		}

		CreateTimer(0.0, BBall_GiveHealth, victim);
		return Plugin_Changed;
	}

	return Plugin_Continue;
}