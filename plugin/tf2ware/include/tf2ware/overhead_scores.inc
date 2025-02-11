/////////////////////
// OVERHEAD SCORES //
/////////////////////

#include <sdktools_entinput>
#include <sdktools_functions>

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
	if (IsValidClient(client) && (SpecialRound != 4))
	{
		DestroySprite(client);

		/* We can allow zero points since there is an asset for it. */
		if (IsPlayerAlive(client))
		{
			// Sprite name
			decl String:sprite[256];
			decl String:scale[12];   
			Format(scale, sizeof(scale), "0.2");
			
			if (status == 2 && StrEqual(minigame, "Ghostbusters")) {
				if (g_Mission[client] == 0)
				{
					Format(sprite, sizeof(sprite), "materials/tf2ware/medicsign2.vmt");
				}

				if (g_Mission[client] == 1)
				{
					Format(sprite, sizeof(sprite), "materials/tf2ware/ghost2.vmt");
					Format(scale, sizeof(scale), "0.5");
				}

				if (g_Mission[client] == 2)
				{
					Format(sprite, sizeof(sprite), "materials/tf2ware/frankenstein.vmt");
					Format(scale, sizeof(scale), "0.3");
				}
			}
			else
			{
				new points = g_Points[client];
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

			new Float:vOrigin[3];
			GetClientAbsOrigin(client, vOrigin);
			
			/* Put above player. */
			vOrigin[2] += 90.0;
			
			new ent = CreateEntityByName("env_sprite");
			if (IsValidEntity(ent))
			{
				DispatchKeyValueVector(ent, "origin", vOrigin);
				DispatchKeyValueVector(ent, "angles", NULL_VECTOR);

				DispatchKeyValue(ent, "model", sprite);
				DispatchKeyValue(ent, "rendermode", "1");
				DispatchKeyValue(ent, "rendercolor", "255 255 255");
				DispatchKeyValue(ent, "classname", "env_sprite");
				DispatchKeyValue(ent, "spawnflags", "1");
				DispatchKeyValue(ent, "scale", scale);

				if (!DispatchSpawn(ent))
				{
					PrintToServer("[TF2Ware] [CreateSprite] Failed to dispatch spawn on a sprite!");
					return;
				}

				/* Explodes here. */
				ParentEntity(client, ent, "head", true);

				g_Sprites[client] = ent;
				PrintToServer("[TF2Ware] (DEBUG) [CreateSprite] Created env_sprite (%s) %i for %i at {%f, %f, %f}!", sprite, ent, client,
					vOrigin[0], vOrigin[1], vOrigin[2]);
			}
		}
	}
}

stock bool ParentEntity(int parent, int attachment, const char[] attachPoint = "", bool maintainOffset = false)
{
	SetVariantString("!activator");
	if (!AcceptEntityInput(attachment, "SetParent", parent, attachment, 0))
	{
		return false;
	}
	
	if (strlen(attachPoint) > 0)
	{
		SetVariantString(attachPoint);
		
		if (!AcceptEntityInput(attachment,
			maintainOffset ? "SetParentAttachmentMaintainOffset" : "SetParentAttachment",
			parent, parent))
		{
			return false;
		}
	}

	return true;
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