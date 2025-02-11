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
			vOrigin[1] += 90.0;
			
			new ent = CreateEntityByName("env_sprite");
			if (IsValidEntity(ent))
			{
				decl String:strName[16];
				Format(strName, sizeof(strName), "client%d", client);
				DispatchKeyValue(client, "targetname", strName);

				SetEntityModel(ent, sprite);
				SetEntityRenderColor(ent);			
				SetEntityRenderMode(ent, RENDER_TRANSCOLOR);
			
				DispatchKeyValue(ent, "classname", "env_sprite");
				DispatchKeyValue(ent, "spawnflags", "1");
				DispatchKeyValue(ent, "scale", scale);

				if (!DispatchSpawn(ent))
				{
					PrintToServer("[TF2Ware] [CreateSprite] Failed to dispatch spawn on a sprite!");
					return;
				}

				TeleportEntity(ent, vOrigin, NULL_VECTOR, NULL_VECTOR);              
				SetVariantString(strName);

				ParentEntity(client, ent, "head", true);     

				AcceptEntityInput(ent, "Display", ent, ent, 0);
				g_Sprites[client] = ent;
				PrintToServer("[TF2Ware] (DEBUG) [CreateSprite] Created env_sprite (%s) %i for %i at {%f, %f, %f}!", sprite, ent, client,
					vOrigin[0], vOrigin[1], vOrigin[2]);
			}
		}
	}
}

stock void ParentEntity(int parent, int attachment, const char[] attachPoint = "", bool maintainOffset = false)
{
	SetVariantString("!activator");
	AcceptEntityInput(attachment, "SetParent", parent, attachment, 0);
	
	if (strlen(attachPoint) > 0)
	{
		SetVariantString(attachPoint);
		AcceptEntityInput(attachment,
				maintainOffset ? "SetParentAttachmentMaintainOffset" : "SetParentAttachment",
				parent, parent);
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