/////////////////////
// OVERHEAD SCORES //
/////////////////////

#include <sdktools_entinput>
#include <sdktools_functions>

#define OVERHEAD_SCORE_OFFSET 90.0

stock CreateAllSprites() {
	for (new client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client))
		{
			CreateSprite(client);
		}
	}
}

stock CreateSprite(client)
{
	if (IsValidClient(client) && (SpecialRound != SINGLEPLAYER))
	{
		DestroySprite(client);

		/* We can allow zero points since there is an asset for it. */
		if (IsPlayerAlive(client))
		{
			// Sprite name
			char sprite[256];
			float scale = 0.2;
			
			if (status == 2 && StrEqual(minigame, "Ghostbusters")) {
				if (g_Mission[client] == 0)
				{
					Format(sprite, sizeof(sprite), "materials/tf2ware/medicsign2.vmt");
				}

				if (g_Mission[client] == 1)
				{
					Format(sprite, sizeof(sprite), "materials/tf2ware/ghost2.vmt");
					scale = 0.5;
				}

				if (g_Mission[client] == 2)
				{
					Format(sprite, sizeof(sprite), "materials/tf2ware/frankenstein.vmt");
					scale = 0.3;
				}
			}
			else
			{
				int points = g_Points[client];
				if (points > 20)
				{
					points = 20;
				}

				if (g_Winner[client] > 0)
				{
					points = 99;
				}

				Format(sprite, sizeof(sprite), "materials/tf2ware/tf2ware_points%d.vmt", points);
			}

			float vOrigin[3];
			GetClientAbsOrigin(client, vOrigin);
			
			/* Put above player. */
			vOrigin[2] += OVERHEAD_SCORE_OFFSET;
			
			new ent = CreateEntityByName("env_sprite");
			if (IsValidEntity(ent))
			{
				DispatchKeyValueVector(ent, "origin", vOrigin);
				DispatchKeyValueVector(ent, "angles", NULL_VECTOR);

				DispatchKeyValue(ent, "classname", "env_sprite");
				DispatchKeyValue(ent, "spawnflags", "1");

				DispatchKeyValueFloat(ent, "scale", scale);

				DispatchKeyValue(ent, "rendermode", "1");
				DispatchKeyValue(ent, "rendercolor", "255 255 255");
				DispatchKeyValue(ent, "model", sprite);

				if (!DispatchSpawn(ent))
				{
					PrintToServer("[TF2Ware] [CreateSprite] Failed to dispatch spawn on a sprite!");
					return;
				}

				/**
				 * Apparently we can't parent models to entities, unless the
				 * documentation that I am reading is incorrect.
				 * 
				 * Meaning we need to update the entities manually ourselves.
				 * 
				 * Which is fucking horrible.
				 * 
				 * This is why overhead scores are disabled by default.
				 */
				SetEntityMoveType(ent, MOVETYPE_NOCLIP);

				g_Sprites[client] = ent;
			}
		}
	}
}

stock DestroyAllSprites()
{
	for (new client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client))
		{
			DestroySprite(client);
		}
	}
}

stock DestroySprite(client)
{
	if (IsValidClient(client))
	{
		new ent = g_Sprites[client];
		if (ent > 0 && IsValidEdict(ent))
		{
			new String:classname[64];
			GetEdictClassname(ent, classname, sizeof(classname));
			if(StrEqual(classname, "env_sprite", false))
			{
				AcceptEntityInput(ent, "kill");
			}
		}
	}
	g_Sprites[client] = 0;
}

stock OverheadScoresUpdate()
{
	for (int i = 1; i <= MaxClients; i++)
	{
		int ent = g_Sprites[i];
		if (ent > 0 && IsClientInGame(i))
		{
			float vOrigin[3];
			GetClientAbsOrigin(i, vOrigin);

			vOrigin[2] += OVERHEAD_SCORE_OFFSET;

			float vVelocity[3];
			GetEntDataVector(i, gVelocityOffset, vVelocity);

			TeleportEntity(ent, vOrigin, NULL_VECTOR, vVelocity);
		}
	}
}