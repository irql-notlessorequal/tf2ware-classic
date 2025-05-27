/**
 * TF2Ware Classic
 * Copyright (C) 2025 IRQL_NOT_LESS_OR_EQUAL
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
#pragma newdecls required

#include <sdkhooks>

#if defined(ENABLE_MALLET)
#include "mallet.inc"
#else
#include "mimallet/mimallet_weapons.inc"
#include "mimallet/mimallet_weapons_db.inc"
#include "mimallet/mimallet_weapons_misc.inc"
#include "mimallet/mimallet_weapons_wearable.inc"
#endif

#if defined(ENABLE_ATTACHMENTS)
#include "attachments.inc"
#endif

int g_PlayerVisibleWeapon[MAXPLAYERS+1] = -1;

stock bool IsValidClient(int iClient)
{
	if (iClient <= 0)
	{
		return false;
	}
	if (iClient > MaxClients)
	{
		return false;
	}

	if (!IsClientConnected(iClient))
	{
		return false;
	}

	return IsClientInGame(iClient);
}

stock void NoCollision(bool input)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidEntity(i) && IsValidClient(i))
		{
			if (input)
			{
				SetEntData(i, g_offsCollisionGroup, 2, 4, true); // NO COLLISION
			}
			else
			{
				SetEntData(i, g_offsCollisionGroup, 5, 4, true); // BRING BACK COLLISION
			}
		}
	}
}

void PyroExplode(float vec1[3])
{
	static int color[4] = {188, 220, 255, 200};

	EmitSoundFromOrigin("ambient/explosions/explode_8.wav", vec1);
	TE_SetupExplosion(vec1, g_ExplosionSprite, 10.0, 1, 0, 0, 750); // 600
	TE_SendToAll();
	TE_SetupBeamRingPoint(vec1, 10.0, 500.0, white, g_HaloSprite, 0, 10, 0.6, 10.0, 0.5, color, 10, 0);
	TE_SendToAll();
}

void ExplosionDamage(float origin[3])
{    
	float PlayerVec[3];
	float distance;

	for (int i = 1; i <= MaxClients; i++)
	{
		if (!IsClientInGame(i) || !IsPlayerAlive(i) )
		{
			continue;
		}

		GetClientAbsOrigin(i, PlayerVec);
		
		distance = GetVectorDistance(origin, PlayerVec, true);
		if (distance > 100000.0)
		{
			continue;
		}
		
		int dmg = RoundFloat(100000.0 - distance) / 50;
		int health = GetClientHealth(i) - dmg;
		
		if (health > 0)
		{
			SetEntityHealth(i, health);
		}
		else if (health <= 0)
		{
			ForcePlayerSuicide(i);
		}
	}
}

void EmitSoundFromOrigin(const char[] sound, const float orig[3])
{
	EmitSoundToAll(
		.sample = sound,
		.entity = SOUND_FROM_WORLD,
		.channel = SNDCHAN_AUTO,
		.level = SNDLEVEL_NORMAL,
		.flags = SND_NOFLAGS,
		.volume = SNDVOL_NORMAL,
		.pitch = SNDPITCH_NORMAL,
		.speakerentity = -1,
		.origin = orig,
		.dir = NULL_VECTOR,
		.updatePos = true,
		.soundtime = 0.0
	);
}

void SetClientClass(int i, char tfclass[128])
{
	TFClassType input;
	if (StrEqual(tfclass, "soldier"))
	{
		input = TFClass_Soldier;
	}
	else if (StrEqual(tfclass, "spy"))
	{
		input = TFClass_Spy;
	}
	else if (StrEqual(tfclass, "demoman"))
	{
		input = TFClass_DemoMan;
	}
	else if (StrEqual(tfclass, "sniper"))
	{
		input = TFClass_Sniper;
	}
	else if (StrEqual(tfclass, "medic"))
	{
		input = TFClass_Medic;
	}
	else if (StrEqual(tfclass, "engineer"))
	{
		input = TFClass_Engineer;
	}
	else if (StrEqual(tfclass, "heavy"))
	{
		input = TFClass_Heavy;
	}
	else if (StrEqual(tfclass, "pyro"))
	{
		input = TFClass_Pyro;
	}
	else
	{
		input = TFClass_Scout;
	}

	if (IsValidClient(i))
	{
		if (TF2_GetPlayerClass(i) != input)
		{
			TF2_SetPlayerClass(i, input);
			SetVariantString("");
			AcceptEntityInput(i, "SetCustomModel");
		}

		HealClient(i);
	}
}

stock void HealClient(int entity)
{
	TFClassType class = TF2_GetPlayerClass(entity);
	int health = 125;

	/**
	 * Set defaults for per-class health.
	 */
	switch (class)
	{
		case TFClass_Soldier:
		{
			health = 200;
		}

		case TFClass_Pyro, TFClass_DemoMan:
		{
			health = 175;
		}

		case TFClass_Heavy:
		{
			health = 300;
		}

		case TFClass_Medic:
		{
			health = 150;
		}
	}
	
	SetEntityHealth(entity, health);
}

stock void ClientParticle(int client, char effect[128], float time)
{
	float strflVec[3];
	GetClientEyePosition(client, strflVec);
	
	int strIParticle = CreateEntityByName("info_particle_system");
	char strName[128];

	if (IsValidEdict(strIParticle))
	{
		float strflPos[3];
		GetEntPropVector(client, Prop_Send, "m_vecOrigin", strflPos);
		TeleportEntity(strIParticle, strflPos, NULL_VECTOR, NULL_VECTOR);
		
		Format(strName, sizeof(strName), "target%i", client);
		DispatchKeyValue(client, "targetname", strName);
		
		DispatchKeyValue(strIParticle, "targetname", "tf2particle");
		DispatchKeyValue(strIParticle, "parentname", strName);
		DispatchKeyValue(strIParticle, "effect_name", effect);
		DispatchSpawn(strIParticle);
		SetVariantString(strName);
		AcceptEntityInput(strIParticle, "SetParent", strIParticle, strIParticle, 0);
		//SetVariantString("head");
		//AcceptEntityInput(strIParticle, "SetParentAttachment", strIParticle, strIParticle, 0);
		ActivateEntity(strIParticle);
		AcceptEntityInput(strIParticle, "start");
		
		CreateTimer(time, KillProp_Timer, strIParticle);
	}
}

public Action KillProp_Timer(Handle hTimer, any prop)
{
	if (IsValidEntity(prop))
	{
		AcceptEntityInput(prop, "Kill" );
	}
	
	return Plugin_Stop;
}

stock int GetActivePlayers(int team = 0, bool mustBeAlive = false)
{
	int output = 0;
	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && g_Spawned[client])
		{
			if (((team == 0 && GetClientTeam(client) >= 2)
				|| (team > 0 && GetClientTeam(client) == team))
				&& (mustBeAlive == false || IsPlayerAlive(client))
				&& g_Participating[client] == true)
			{
				output += 1;
			}
		}
	}

	return output;
}

stock bool IsPlayerTaunting(int client)
{
	int playerCond = GetEntProp(client, Prop_Send, "m_nPlayerCond");
	return (playerCond & TF2_PLAYER_TAUNTING);
}  

public void RemoveClientWeapons(int client)
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::RemoveClientWeapons] Removing all client weapons");
#endif

#if defined(ENABLE_ATTACHMENTS)
	RemoveFakeWeapon(client);
#endif

	if (IsValidClient(client) && (IsPlayerAlive(client)) && (g_Winner[client] == 0))
	{
		SetClientSlot(client, 0);

		for (int j = 0; j <= 5; j++)
		{
			TF2_RemoveWeaponSlot(client, j);
		}

		SetWeaponState(client, false);
	}
}

stock void SetWeaponAmmo(int client, int slot1, int slot2)
{
	int ActiveWeapon = GetEntDataEnt2(client, FindSendPropOffs("CTFPlayer", "m_hActiveWeapon"));
	if (IsValidEntity(ActiveWeapon))
	{
		SetEntData(ActiveWeapon, FindSendPropOffs("CBaseCombatWeapon", "m_iClip1"), slot1, 4);
		SetEntData(client, FindSendPropOffs("CTFPlayer", "m_iAmmo") + 4, slot2, 4);
		SetEntData(client, FindSendPropOffs("CTFPlayer", "m_iAmmo") + 8, slot2, 4);
	}
}

public void SetWeaponState(int client, bool input)
{
	int ActiveWeapon = GetEntDataEnt2(client,FindSendPropOffs("CTFPlayer", "m_hActiveWeapon"));
	int iEntity = g_PlayerVisibleWeapon[client];

	if (IsValidEntity(ActiveWeapon))
	{
		if (input == true)
		{
			SetEntityRenderColor(ActiveWeapon, 255, 255, 255, 255);
			SetEntityRenderMode(ActiveWeapon, RENDER_NORMAL);
			SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 1);
		}
		else
		{
			SetEntityRenderColor(ActiveWeapon, 255, 255, 255, 0);
			SetEntityRenderMode(ActiveWeapon, RENDER_TRANSCOLOR);
			SetWeaponAmmo(client, 0, 0);
			SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 0);
		}
	}

	if (iEntity > 0 && IsValidEntity(iEntity))
	{
		if (input == true)
		{
			SetEntityRenderColor(iEntity, 255, 255, 255, 255);
			SetEntityRenderMode(iEntity, RENDER_NORMAL);
		}
		else
		{
			SetEntityRenderColor(iEntity, 255, 255, 255, 0);
			SetEntityRenderMode(iEntity, RENDER_TRANSCOLOR);
		}
	}
}

public void GiveClientWeapon(int client, char weapon[128], int slot, bool input)
{
	if (IsValidClient(client) && IsPlayerAlive(client))
	{
		int weaponIdx = -1;
		if (!MalletGetWeaponIndex(sizeof (weapon), weapon, weaponIdx))
		{
			PrintToServer("[tf2ware::GiveClientWeapon] [WARNING] MalletGetWeaponIndex(%s) returned FALSE!", weapon);
			return;
		}

		int wpn = MalletCreateWeapon(client, weaponIdx, weapon, 0, 0, slot);
		if (wpn <= 0)
		{
			PrintToServer("[tf2ware::GiveClientWeapon] [WARNING] MalletCreateWeapon(%i, %i, %s) returned %i!", client, weaponIdx, weapon, wpn);
			return;
		}

		if (!MalletSwapWeaponAndPurge(client, wpn, slot))
		{
			PrintToServer("[tf2ware::GiveClientWeapon] [WARNING] MalletSwapWeaponAndPurge returned FALSE!");
			return;
		}
		
		if (input)
		{
			if (StrEqual(weapon, "tf_weapon_revolver")) SetWeaponAmmo(client, 6, 24);
			if (StrEqual(weapon, "tf_weapon_shotgun_primary")) SetWeaponAmmo(client, 6, 32);
			if (StrEqual(weapon, "tf_weapon_flamethrower")) SetWeaponAmmo(client, 200, 200);
			if (StrEqual(weapon, "tf_weapon_grenadelauncher")) SetWeaponAmmo(client, 4, 16);
			if (StrEqual(weapon, "tf_weapon_syringegun_medic")) SetWeaponAmmo(client, 40, 150);
			if (StrEqual(weapon, "tf_weapon_sniperrifle")) SetWeaponAmmo(client, 25, 25);
			if (StrEqual(weapon, "tf_weapon_rocketlauncher")) SetWeaponAmmo(client, 4, 20);
			if (StrEqual(weapon, "tf_weapon_pipebomblauncher")) SetWeaponAmmo(client, 8, 24);
		}

#if defined ENABLE_ATTACHMENTS
		// Create a model everyone else can oodle at
		RemoveFakeWeapon(client);
		
		decl String:strModel[PLATFORM_MAX_PATH];
		Format(strModel, sizeof(strModel), "");
		
		if (StrEqual(weapon, "tf_weapon_bat")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_bat.mdl");
		if (StrEqual(weapon, "tf_weapon_minigun")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_minigun.mdl");
		if (StrEqual(weapon, "tf_weapon_bonesaw")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_bonesaw.mdl");
		if (StrEqual(weapon, "tf_weapon_wrench")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_wrench.mdl");
		if (StrEqual(weapon, "tf_weapon_bottle")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_bottle.mdl");
		if (StrEqual(weapon, "tf_weapon_club")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_club.mdl");
		if (StrEqual(weapon, "tf_weapon_fireaxe")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_fireaxe.mdl");
		if (StrEqual(weapon, "tf_weapon_shovel")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_shovel.mdl");
		if (StrEqual(weapon, "tf_weapon_revolver")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_revolver.mdl");
		if (StrEqual(weapon, "tf_weapon_shotgun_primary")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_shotgun.mdl");
		if (StrEqual(weapon, "tf_weapon_flamethrower")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_flamethrower.mdl");
		if (StrEqual(weapon, "tf_weapon_grenadelauncher")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_grenadelauncher.mdl");
		if (StrEqual(weapon, "tf_weapon_syringegun_medic")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_syringegun.mdl");
		if (StrEqual(weapon, "tf_weapon_sniperrifle")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_sniperrifle.mdl");
		if (StrEqual(weapon, "tf_weapon_rocketlauncher")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_rocketlauncher.mdl");
		if (StrEqual(weapon, "tf_weapon_pipebomblauncher")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_stickybomb_launcher.mdl");
		if (StrEqual(weapon, "tf_weapon_medigun")) Format(strModel, sizeof(strModel), "models/weapons/w_models/w_medigun.mdl");
		
		if (!StrEqual(strModel, "")) {
			new iEntity = Attachable_CreateAttachable(client);
			if (iEntity > 0 && IsValidEdict(iEntity)) {
				
				SetEntityModel(iEntity, strModel);
				g_PlayerVisibleWeapon[client] = iEntity;
			}
		}
#endif
		
		SetWeaponState(client, input);
	}
}

#if defined ENABLE_ATTACHMENTS
void RemoveFakeWeapon(&iClient) {
	new iEntity = g_PlayerVisibleWeapon[iClient];
	g_PlayerVisibleWeapon[iClient] = -1;
	
	if (iEntity > 0 && IsValidEdict(iEntity)) {
		if (Attachable_IsHooked(iEntity)) Attachable_UnhookEntity(iEntity);
		RemoveEdict(iEntity);
	}
}
#endif

public void RemoveAllWeapons()
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::RemoveAllWeapons] Removing all weapons");
#endif

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && (IsPlayerAlive(i)))
		{
			RemoveClientWeapons(i);
		}
	}
}

stock void DisableClientWeapons(int client)
{
	if (IsValidClient(client) && (IsPlayerAlive(client)))
	{
		RemoveClientWeapons(client);
		
		if (SpecialRound == 6 && (!(StrEqual(minigame, "SimonSays") && simon_action == 0)))
		{
			GiveClientWeapon(client, "tf_weapon_bat", 2, true);
			SetClientSlot(client, 2);
		}
		else
		{
			TFClassType class = TF2_GetPlayerClass(client);
			/**
			 * For some reason the Spy and Heavy got their primary weapons...wut.
			 */
			switch (class)
			{
				case TFClass_Scout:
				{
					GiveClientWeapon(client, "tf_weapon_bat", 2, false);
				}
				case TFClass_Soldier:
				{
					GiveClientWeapon(client, "tf_weapon_shovel", 2, false);
				}
				case TFClass_Pyro:
				{
					GiveClientWeapon(client, "tf_weapon_fireaxe", 2, false);
				}
				case TFClass_DemoMan:
				{
					GiveClientWeapon(client, "tf_weapon_bottle", 2, false);
				}
				case TFClass_Heavy:
				{
					GiveClientWeapon(client, "tf_weapon_fists", 2, false);
				}
				case TFClass_Engineer:
				{
					GiveClientWeapon(client, "tf_weapon_wrench", 2, false);
				}
				case TFClass_Medic:
				{
					GiveClientWeapon(client, "tf_weapon_bonesaw", 2, false);
				}
				case TFClass_Sniper:
				{
					GiveClientWeapon(client, "tf_weapon_club", 2, false);
				}
				case TFClass_Spy:
				{
					GiveClientWeapon(client, "tf_weapon_knife", 2, false);
				}
			}

			SetWeaponState(client, false);
		}
	}
}

// This function is meant to handle player wearables.
// Ideally it would call some kind of way to re-add a player's wearables, but until we find a way to do that, removing them will do.

public Action HandlePlayerItems(int iClient)
{
	if (!IsValidClient(iClient)) return;
	if (!IsPlayerAlive(iClient)) return;

	MalletRemoveAllWearables(iClient);

	SetEntProp(iClient, Prop_Send, "m_nBody", 0);
}

Handle GameTextHandle = INVALID_HANDLE;

void ShowGameText(const char[] strMessage, const char[] strIcon = "leaderboard_dominated", float life = 35.0)
{
	if (GameTextHandle != INVALID_HANDLE)
	{
		TriggerTimer(GameTextHandle);
	}

	int iEntity = CreateEntityByName("game_text_tf");
	DispatchKeyValue(iEntity, "message", strMessage);
	DispatchKeyValue(iEntity, "display_to_team", "0");
	DispatchKeyValue(iEntity, "icon", strIcon);
	DispatchKeyValue(iEntity, "targetname", "game_text1");
	DispatchKeyValue(iEntity, "background", "0");
	DispatchSpawn(iEntity);
	AcceptEntityInput(iEntity, "Display", iEntity, iEntity);
	
	GameTextHandle = CreateTimer(life, KillGameText, iEntity);
}

public Action KillGameText(Handle hTimer, any iEntityRef)
{
	int iEntity = _:EntRefToEntIndex(iEntityRef);
	if ((iEntity > 0) && IsValidEntity(iEntity))
	{
		AcceptEntityInput(iEntity, "kill");
	}
	
	GameTextHandle = INVALID_HANDLE;
	
	return Plugin_Stop;
}

stock void ChangeClientTeamAlive(int client, int team)
{
	SetEntProp(client, Prop_Send, "m_lifeState", 2);
	ChangeClientTeam(client, team);
	SetEntProp(client, Prop_Send, "m_lifeState", 0);
}

stock bool HasMapEnded()
{
	int timeLeft = -1;
	GetMapTimeLeft(timeLeft);	
	return timeLeft >= 0 && timeLeft < 300;
}

void ClearPlayerFreeze()
{
	SetConVarInt(FindConVar("tf_player_movement_restart_freeze"), 0);
}

void RestorePlayerFreeze()
{
	SetConVarInt(FindConVar("tf_player_movement_restart_freeze"), 1);
}

void ClearAllTaunts(int iClient)
{
	TF2_RemoveCondition(iClient, TFCond_Taunting);
}

public int GetHealTarget(int client)
{
	if (!IsValidClient(client) || TF2_GetPlayerClass(client) != TFClass_Medic)
	{
		return 0;
	}
	
	int weapon = GetPlayerWeaponSlot(client, 1);
	if (weapon == -1)
	{
		return 0;
	}
		
	char classname[64];
	GetEdictClassname(weapon, classname, 64);

	if (!StrEqual(classname, "tf_weapon_medigun"))
	{
		return 0;
	}
	
	return GetEntDataEnt2(weapon, FindSendPropInfo("CWeaponMedigun", "m_hHealingTarget"));
}

int GetClientMaxHealth(int iClient)
{
	return TF2_GetPlayerResourceData(iClient, TFResource_MaxHealth);
}

stock int FindEntityByClassname2(int startEnt, const char[] classname)
{
	/* If startEnt isn't valid shifting it back to the nearest valid one */
	while (startEnt > -1 && !IsValidEntity(startEnt)) startEnt--;
	return FindEntityByClassname(startEnt, classname);
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

stock int Entity_GetTargetName(int entity, char[] buffer, int size)
{
	return GetEntPropString(entity, Prop_Data, "m_target", buffer, size);
}

stock bool Entity_SetClassName(int entity, const char[] className)
{
	return DispatchKeyValue(entity, "classname", className);
}

stock bool Entity_SetName(int entity, const char[] name, any ...)
{
	char format[128];
	VFormat(format, sizeof(format), name, 3);
	
	return DispatchKeyValue(entity, "targetname", format);
}

static void Entity_PointHurtAtTarget(int entity, int target, const char[] name = "")
{
	char targetName[128];
	Entity_GetTargetName(entity, targetName, sizeof(targetName));
	
	if (name[0] == '\0')
	{
		if (targetName[0] == '\0')
		{
			// Let's generate our own name
			Format(
				targetName,
				sizeof(targetName),
				"_smlib_Entity_PointHurtAtTarget:%d",
				target
			);
		}
	}
	else
	{
		strcopy(targetName, sizeof(targetName), name);
	}
	
	DispatchKeyValue(entity, "DamageTarget", targetName);

	Entity_SetName(target, targetName);
}

stock bool HurtEntity(int entity, float damage, int attacker=0, int damageType=DMG_GENERIC, const char[] fakeClassName="")
{
	static int point_hurt = INVALID_ENT_REFERENCE;
	
	if (point_hurt == INVALID_ENT_REFERENCE || !IsValidEntity(point_hurt))
	{
		point_hurt = EntIndexToEntRef(CreateEntityByName("point_hurt"));
		
		if (point_hurt == INVALID_ENT_REFERENCE)
		{
			return false;
		}
		
		DispatchSpawn(point_hurt);
	}
	
	AcceptEntityInput(point_hurt, "TurnOn");
	SetEntProp(point_hurt, Prop_Data, "m_nDamage", damage);
	SetEntProp(point_hurt, Prop_Data, "m_bitsDamageType", damageType);

	Entity_PointHurtAtTarget(point_hurt, entity);
	
	if (fakeClassName[0] != '\0')
	{
		Entity_SetClassName(point_hurt, fakeClassName);
	}
	
	AcceptEntityInput(point_hurt, "Hurt", attacker);
	AcceptEntityInput(point_hurt, "TurnOff");
	
	if (fakeClassName[0] != '\0')
	{
		Entity_SetClassName(point_hurt, "point_hurt");
	}
	
	return true;
}