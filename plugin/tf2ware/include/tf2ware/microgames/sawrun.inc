#include "../microgame.inc"

methodmap Sawrun < Microgame
{
	public Sawrun()
	{
		return view_as<Sawrun>(MG_SAW_RUN);
	}

	public void OnMicrogameStart()
	{
		/**
		 * The arena is tiny and I'm not going to make
		 * it any larger. Leaving us with this option only.
		 */
		NoCollision((GetClientCount() >= 24));
	}

	public void OnMicrogameFrame()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client) && IsPlayerAlive(client) && IsClientParticipating(client))
			{
				float pos[3];
				GetClientAbsOrigin(client, pos);

				if (pos[1] > 1000.0)
				{
					SetStateClient(client, true, true);
				}
			}
		}
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			SetClientClass(client, "scout");
			DisableClientWeapons(client);
		
			float vel[3];
			vel[0] = 0.0;
			vel[1] = 0.0;
			vel[2] = 0.0;
		
			/**
			 * TODO(irql):
			 * 
			 * Past 24 players will cause them to spawn in a wall...
			 */

			int column = client;
			int row = 0;
			
			while (column > 9)
			{
				column = column - 9;
				row = row + 1;
			}
		
			float pos[3];
			pos[0] = 1780.0 + float(column * 55);
			pos[1] = 290.0 - float(row * 55);
			pos[2] = 130.0;

			float ang[3];
			ang[0] = 27.0;
			ang[1] = 90.0;
			ang[2] = 0.0;

			TeleportEntity(client, pos, ang, vel);
			NoCollision(true);
		}
	}
}