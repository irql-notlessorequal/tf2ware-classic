#include "../microgame.inc"

methodmap Movement < Microgame
{
	public Movement()
	{
		return view_as<Movement>(MG_MOVEMENT);
	}

	public void OnMicrogameStart()
	{
		SetStateAll(true);

		int mission = GetRandomInt(0,1);

		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsClientParticipating(client))
			{
				g_Mission[client] = mission;
			}
		}
	}

	public void OnMicrogameFrame()
	{
		if (g_TimeLeft <= 5)
		{
			float fVelocity[3];
			float speed;
			float limit;

			speed = 0.0;
			limit = 0.0;

			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && IsPlayerAlive(client) && IsClientParticipating(client))
				{
					limit = GetEntPropFloat(client, Prop_Send, "m_flMaxspeed") - 100.0;
					GetEntPropVector(client, Prop_Data, "m_vecVelocity", fVelocity);
					speed = GetVectorLength(fVelocity);

					if ((g_Mission[client] == 0) && (speed < limit) && (g_Complete[client]) && (IsPlayerAlive(client)))
					{
						SetStateClient(client, false, true);
						ForcePlayerSuicide(client);
					}

					if ((g_Mission[client] >= 1) && (speed > 100.0) && (g_Complete[client]) && (IsPlayerAlive(client)))
					{
						SetStateClient(client, false, true);
						ForcePlayerSuicide(client);
					}
				}
			}
		}
	}
}