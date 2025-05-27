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
		
			int total = GetActivePlayers();
			int posa = 360 / total * client;
		
			float pos[3];
			if (IsAlpineVariant())
			{
				pos[0] = 3546.0 + (Cosine(DegToRad(float(posa))) * 220.0);
				pos[1] = 94.0 - (Sine(DegToRad(float(posa))) * 220.0);
				pos[2] = -520.0;
			}
			else
			{
				pos[0] = 3544.661133 + (Cosine(DegToRad(float(posa))) * 220.0);
				pos[1] = -160.203690 - (Sine(DegToRad(float(posa))) * 220.0);
				pos[2] = -91.027405;
			}


			float ang[3];
			ang[0] = 0.0;
			ang[1] = float(180 - posa);
			ang[2] = 0.0;

			TeleportEntity(client, pos, ang, NULL_VECTOR);

			/**
			 * Thank me later.
			 */
			NoCollision(true);
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