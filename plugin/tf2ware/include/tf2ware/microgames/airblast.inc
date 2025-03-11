#include "../microgame.inc"
methodmap Airblast < Microgame
{
	public Airblast()
	{
		return view_as<Airblast>(MG_AIRBLAST);
	}

	public bool IsMicrogamePlayable(int players)
	{
		return players >= 2;
	}

	public void OnMicrogameStart()
	{
		SetStateAll(true);
		g_attack = true;
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SDKHook(client, SDKHook_PreThink, Airblast_RemoveLeftClick);
	
			RemoveClientWeapons(client);
			SetClientClass(client, "pyro");
			GiveClientWeapon(client, "tf_weapon_flamethrower", 0, true);
			SetClientClass(client, "pyro");
		
			float vel[3];
			vel[0] = 0.0;
			vel[1] = 0.0;
			vel[2] = 0.0;
		
			int total = GetActivePlayers();
			float posa = 360 / total * float(g_Id[client] - 1);
		
			float pos[3];
			pos[0] = 3544.661133 + (Cosine(DegToRad(posa)) * 220.0);
			pos[1] = -160.203690 - (Sine(DegToRad(posa)) * 220.0);
			pos[2] = -91.027405;

			float ang[3];
			ang[0] = 0.0;
			ang[1] = (180.0 - posa);
			ang[2] = 0.0;

			TeleportEntity(client, pos, ang, vel);
		}
	}

	public void OnMicrogameEnd()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && (!(IsPlayerAlive(i))) && IsClientParticipating(i))
			{
				SetStateClient(i, false, true);
			}
		}
	}

	public void OnMicrogamePostEnd()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			SDKUnhook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
		}
	}
}

void Airblast_RemoveLeftClick(int client)
{
	int iButtons = GetClientButtons(client);
	if ((status == 2))
	{
		if ((iButtons & IN_ATTACK))
		{
			iButtons &= ~IN_ATTACK;
			SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
		}
	}
}