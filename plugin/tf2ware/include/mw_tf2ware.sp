/**
 * TF2Ware Classic
 *
 * Copyright (C) 2025		IRQL_NOT_LESS_OR_EQUAL
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
#pragma semicolon 1
#pragma newdecls required

// Includes:
#include <sourcemod>
#include <sdktools>
#include <sdktools_sound>
#include <tf2>
#include <tf2_stocks>
#include <sdkhooks>
#include <geoip>

#if defined(ENABLE_MALLET)
#include <mallet>
#else
#include "tf2ware/mimallet/mimallet_weapons_wearable.inc"
#include "tf2ware/mimallet/mimallet_random.inc"
#endif

// Fixes
#include <colors>

#if defined(ENABLE_ATTACHMENTS)
#include <attachables>
#endif

/**
 * Provides defines and core enums.
 */
#include "tf2ware/tf2ware_core.inc"

/**
 * Gamemode names.
 * 
 * Planned for future removal.
 */
char g_name[MAX_MINIGAMES][24];

/**
 * Gamemode ConVar handles
 */
Handle ww_enable = INVALID_HANDLE;
Handle ww_speed = INVALID_HANDLE;
Handle ww_music = INVALID_HANDLE;
Handle ww_force = INVALID_HANDLE;
Handle ww_special = INVALID_HANDLE;
Handle ww_gamemode = INVALID_HANDLE;
Handle ww_force_special = INVALID_HANDLE;
Handle ww_overhead_scores = INVALID_HANDLE;
Handle ww_kamikaze_style = INVALID_HANDLE;
Handle ww_score_style = INVALID_HANDLE;

/**
 * TF2 ConVar handles
 */
Handle ConVar_MPFriendlyFire = INVALID_HANDLE;
Handle ConVar_TFPlayerMovementRestartFreeze = INVALID_HANDLE;
Handle ConVar_TFTournamentHideDominationIcons = INVALID_HANDLE;
Handle ConVar_TFAirblastCray = INVALID_HANDLE;
Handle ConVar_TFBotDifficulty = INVALID_HANDLE;

/**
 * Misc. handles
 */
Handle hudScore = INVALID_HANDLE;
// REPLACE WEAPON
Handle MicrogameTimer = INVALID_HANDLE;
// Keyvalues configuration handle
Handle MinigameConf = INVALID_HANDLE;

// Bools
bool g_Complete[MAXPLAYERS + 1];
bool g_Spawned[MAXPLAYERS + 1];
bool g_ModifiedOverlay[MAXPLAYERS + 1];
bool g_attack	 = false;
bool g_enabled = false;
bool g_first	 = false;
bool g_waiting = true;

// Ints
int g_Mission[MAXPLAYERS + 1];
int g_NeedleDelay[MAXPLAYERS + 1];
int g_Points[MAXPLAYERS + 1];
int g_Winner[MAXPLAYERS + 1];
int g_Minipoints[MAXPLAYERS + 1];
int g_Sprites[MAXPLAYERS+1];
float currentSpeed;
int iMinigame;
int status;
int randommini;
int g_offsCollisionGroup;
int g_TimeLeft = 8;
int white;
int g_HaloSprite;
int g_ExplosionSprite;
int g_result = 0;
int g_bomb								= 0;
int RoundStarts							= 0;
static int SpecialRoundCooldown			= 1;
int g_LastBoss							= 0;
int g_MinigamesTotal					= 0;
int bossBattle							= 0;
bool g_Participating[MAXPLAYERS + 1]	= false;
int gVelocityOffset = -1;

// Strings
char materialpath[512]			   = "tf2ware/";
/**
 * Name of current minigame being played
 * 
 * TODO:
 * I want to remove this but there's some config
 * related things preventing me from removing it.
 */
char minigame[24];
// VALID iMinigame FORWARD HANDLERS //////////////

/** We need to define it hear since we only just have imported the enum. */
SpecialRounds SpecialRound = NONE;
Microgame currentMicrogame;
/////////////////////////////////////////

#include "tf2ware/microgame.inc"

/////////////////////////////////////////

#include "tf2ware/microgames/hitenemy.inc"
#include "tf2ware/microgames/airblast.inc"
#include "tf2ware/microgames/colortext.inc"
#include "tf2ware/microgames/spycrab.inc"
#include "tf2ware/microgames/barrel.inc"
#include "tf2ware/microgames/kamikaze.inc"
#include "tf2ware/microgames/flood.inc"
#include "tf2ware/microgames/needlejump.inc"
#include "tf2ware/microgames/math.inc"
#include "tf2ware/microgames/hopscotch.inc"
#include "tf2ware/microgames/sawrun.inc"
#include "tf2ware/microgames/simonsays.inc"
#include "tf2ware/microgames/movement.inc"
#include "tf2ware/microgames/snipertarget.inc"
#include "tf2ware/microgames/bball.inc"
#include "tf2ware/microgames/airraid.inc"
#include "tf2ware/microgames/goomba.inc"
#include "tf2ware/microgames/hugging.inc"
#include "tf2ware/microgames/jumprope.inc"
#include "tf2ware/microgames/ghostbusters.inc"
#include "tf2ware/microgames/frogger.inc"

#if 0
#include "tf2ware/microgames/redfloor.inc"
#endif

#include "tf2ware/mw_tf2ware_features.inc"
#include "tf2ware/overhead_scores.inc"
#include "tf2ware/special.inc"
#include "tf2ware/vocalize.inc"

public Plugin myinfo =
{
	name		= "TF2Ware Classic",
	author		= "Mecha the Slag, NuclearWatermelon, gavintlgold, IRQL_NOT_LESS_OR_EQUAL",
	description = "Wario Ware in Team Fortress 2!",
	version		= PLUGIN_VERSION,
	url			= "https://github.com/irql-notlessorequal/tf2ware-classic"
};

static const char ALLOWED_CHEAT_COMMANDS[][] =
{
	"host_timescale",
	"r_screenoverlay",
	"sv_cheats"
};

public void OnPluginStart()
{
	// G A M E  C H E C K //
	char game[32];
	GetGameFolderName(game, sizeof(game));
	if (!StrEqual(game, "tf"))
	{
		SetFailState("This plugin is only for Team Fortress 2, not %s", game);
	}

	// Check for SDKHooks
	if (GetExtensionFileStatus("sdkhooks.ext") < 1)
	{
		SetFailState("SDK Hooks is not loaded.");
	}

#if !defined(ENABLE_MALLET)
	MimalletInitRand();

	if (!MimalletInitWearables())
	{
		SetFailState("MimalletInitWearables returned FALSE.");
	}
#endif

	LoadTranslations("tf2ware_classic.phrases");

	// Find collision group offsets
	g_offsCollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");
	if (g_offsCollisionGroup == -1)
	{
		PrintToServer("* FATAL ERROR: Failed to get offset for CBaseEntity::m_CollisionGroup");
	}

	gVelocityOffset = FindSendPropInfo("CBasePlayer", "m_vecVelocity[0]");
	if (gVelocityOffset == -1)
	{
		PrintToServer("* FATAL ERROR: Failed to get offset for CBaseEntity::m_vecVelocity[0]");
	}

	ConVar_MPFriendlyFire = FindConVar("mp_friendlyfire");
	ConVar_TFPlayerMovementRestartFreeze = FindConVar("tf_player_movement_restart_freeze");
	ConVar_TFTournamentHideDominationIcons = FindConVar("tf_tournament_hide_domination_icons");
	ConVar_TFAirblastCray = FindConVar("tf_airblast_cray");
	ConVar_TFBotDifficulty = FindConVar("tf_bot_difficulty");

	// ConVars
	ww_enable			= CreateConVar("ww_enable", "0", "Enables/Disables TF2Ware.", FCVAR_PLUGIN);
	ww_force			= CreateConVar("ww_force", "0", "Force a certain minigame (0 to not force).", FCVAR_PLUGIN);
	ww_speed			= CreateConVar("ww_speed", "1.0", "Speed level.", FCVAR_PLUGIN);
	ww_music			= CreateConVar("ww_music_fix", "0", "Apply music fix? Should only be on for localhost during testing", FCVAR_PLUGIN);
	ww_special			= CreateConVar("ww_special", "0", "Next round is Special Round?", FCVAR_PLUGIN);
	ww_gamemode			= CreateConVar("ww_gamemode", "-1", "Gamemode", FCVAR_PLUGIN);
	ww_force_special 	= CreateConVar("ww_force_special", "0", "Forces a specific Special Round on Special Round", FCVAR_PLUGIN);
	/**
	 * TODO(irql):
	 * 
	 * Switch this to a tri-state which is the following:
	 * 0 = Disabled
	 * 1 = Always enabled
	 * 2 = Only display between microgames and at the end (legacy behaviour)
	 */
	ww_overhead_scores	= CreateConVar("ww_overhead_scores", "0", "Re-enables overhead scores, a feature that was long removed.", FCVAR_PLUGIN, true, 0.0, true, 1.0);
	ww_kamikaze_style	= CreateConVar("ww_kamikaze_style", "0", "Picks the bomb model logic for Kamikaze. (0 = Use the Payload cart [default], 1 = Use the old Bo-Bomb model)", FCVAR_PLUGIN, true, 0.0, true, 1.0);
	ww_score_style		= CreateConVar("ww_score_style", "1", "Picks the player score HUD style. (0 = original, 1 = TF2Ware Classic [default])", FCVAR_PLUGIN, true, 0.0, true, 1.0);

	/**
	 * MINIGAME REGISTRATION
	 */
	AddMiniGame(MG_AIRBLAST, new Airblast());
	AddMiniGame(MG_AIR_RAID, new AirRaid());
	AddMiniGame(MG_BBALL, new BBall());
	AddMiniGame(MG_BARREL, new Barrel());
	AddMiniGame(MG_COLOR_TEXT, new ColorText());
	AddMiniGame(MG_FLOOD, new Flood());
	AddMiniGame(MG_FROGGER, new Frogger());
	AddMiniGame(MG_GHOSTBUSTERS, new Ghostbusters());
	AddMiniGame(MG_GOOMBA, new Goomba());
	AddMiniGame(MG_HIT_ENEMY, new HitEnemy());
	AddMiniGame(MG_HOPSCOTCH, new Hopscotch());
	AddMiniGame(MG_HUGGING, new Hugging());
	AddMiniGame(MG_JUMP_ROPE, new JumpRope());
	AddMiniGame(MG_KAMIKAZE, new Kamikaze());
	AddMiniGame(MG_MATH, new Math());
	AddMiniGame(MG_MOVEMENT, new Movement());
	AddMiniGame(MG_NEEDLE_JUMP, new NeedleJump());
	AddMiniGame(MG_SAW_RUN, new Sawrun());
	AddMiniGame(MG_SIMON_SAYS, new SimonSays());
	AddMiniGame(MG_SNIPER_TARGET, new SniperTarget());
	AddMiniGame(MG_SPYCRAB, new Spycrab());
}

public void OnMapStart()
{
	// Check if the map has tf2ware at the beginning, otherwise tf2ware should be disabled
	// (A bit hacky I suppose)
	char map[128];
	GetCurrentMap(map, 8);

	/**
	 * TODO(irql):
	 * 
	 * Migrate to the TF2WareMapType enum.
	 */
	bool isRegularMap = StrEqual(map, "tf2ware");
	bool isAlternateMap = StrEqual(map, "tf2ware_alpine_v4", false);

	if (isRegularMap || isAlternateMap)
	{
		/**
		 * Required to be set otherwise things
		 * _will_ break.
		 */
		InternalSetAlpineVariant(isAlternateMap);

		g_enabled = true;

		// Add server tag
		AddServerTag("TF2Ware");

		// Load minigames
		char imFile[PLATFORM_MAX_PATH];
		BuildPath(Path_SM, imFile, sizeof(imFile), "configs/minigames.cfg");

		MinigameConf = CreateKeyValues("Minigames");
		
		if (FileToKeyValues(MinigameConf, imFile))
		{
			PrintToServer("Loaded minigames from minigames.cfg");

			KvGotoFirstSubKey(MinigameConf);
			int i = 0;
			do
			{
				KvGetSectionName(MinigameConf, g_name[KvGetNum(MinigameConf, "id") - 1], 32);
				i++;
			}
			while (KvGotoNextKey(MinigameConf));

			KvRewind(MinigameConf);
		}
		else
		{
			PrintToServer("Failed to load minigames.cfg!");
		}

		// Hooks
		HookConVarChange(ww_enable, StartMinigame_cvar);
		HookConVarChange(ww_overhead_scores, OverheadScoresChanged);
		HookEvent("post_inventory_application", EventInventoryApplication, EventHookMode_Post);
		HookEvent("player_say", Player_Say, EventHookMode_Pre);
		HookEvent("player_spawn", Player_Spawn);
		HookEvent("player_death", Player_Death, EventHookMode_Post);
		HookEvent("player_team", Player_Team, EventHookMode_Post);
		HookEvent("teamplay_round_start", Event_RoundStart, EventHookMode_PostNoCopy);
		HookEvent("teamplay_game_over", Event_RoundEnd, EventHookMode_PostNoCopy);
		HookEvent("teamplay_round_stalemate", Event_RoundEnd, EventHookMode_PostNoCopy);
		HookEvent("teamplay_round_win", Event_RoundEnd, EventHookMode_PostNoCopy);

		RegAdminCmd("ww_list", Command_List, ADMFLAG_GENERIC, "Lists all the registered, enabled plugins and their ids");
		RegAdminCmd("ww_give", Command_Points, ADMFLAG_GENERIC, "Gives you 20 points - You're a winner! (testing feature)");
		RegAdminCmd("ww_event", Command_Event, ADMFLAG_GENERIC, "Starts a debugging event");

		// Vars
		currentSpeed = GetConVarFloat(ww_speed);
		iMinigame	 = 1;
		status		 = 0;
		randommini	 = 0;
		RoundStarts	 = 0;
		SetStateAll(false);
		ResetWinners();
		SetMissionAll(0);

		// CHEATS
		HookConVarChange(FindConVar("sv_cheats"), OnConVarChanged_SvCheats);

		UpdateClientCheatValue();
		HookAllCheatCommands();

#if 0
		DestroyAllBarrels();
#endif

		// HUD
		hudScore = CreateHudSynchronizer();
		ResetScores();

		// Remove Notification Flags
		RemoveNotifyFlag("sv_tags");
		RemoveNotifyFlag("mp_respawnwavetime");
		RemoveNotifyFlag("mp_friendlyfire");
		RemoveNotifyFlag("tf_tournament_hide_domination_icons");
		RemoveNotifyFlag("tf_airblast_cray");

		SetConVarInt(ConVar_TFTournamentHideDominationIcons, 0, true);
		SetConVarInt(ConVar_MPFriendlyFire, 1);
		SetConVarInt(ConVar_TFBotDifficulty, 0);

		/**
		 * Revert to pre-JI airblast.
		 */
		SetConVarInt(ConVar_TFAirblastCray, 0);

		DispatchOnMicrogameSetup();

		/* Regular */

		precacheSound(MUSIC_START);
		precacheSound(MUSIC_WIN);
		precacheSound(MUSIC_FAIL);
		precacheSound(MUSIC_SPEEDUP);
		precacheSound(MUSIC_BOSS);
		precacheSound(MUSIC_GAMEOVER);

		/* Special Rounds */

		precacheSound(MUSIC_SPECIAL_START);
		precacheSound(MUSIC_SPECIAL_WIN);
		precacheSound(MUSIC_SPECIAL_FAIL);
		precacheSound(MUSIC_SPECIAL_SPEEDUP);
		precacheSound(MUSIC_SPECIAL_GAMEOVER);

		/* Wipeout */

		precacheSound(MUSIC_WIPEOUT_START);
		precacheSound(MUSIC_WIPEOUT_WIN);
		precacheSound(MUSIC_WIPEOUT_FAIL);
		precacheSound(MUSIC_WIPEOUT_SPEEDUP);
		precacheSound(MUSIC_WIPEOUT_BOSS);
		precacheSound(MUSIC_WIPEOUT_GAMEOVER);

		/* Misc. */

		precacheSound(MUSIC_WAITING);
		precacheSound(MUSIC_SPECIAL);

		precacheSound(SOUND_COMPLETE);
		precacheSound(SOUND_COMPLETE_YOU);
		precacheSound(SOUND_MINISCORE);
		precacheSound(SOUND_SELECT);
		PrecacheModel("models/props_farm/wooden_barrel.mdl", true);
		PrecacheModel("models/props_farm/gibs/wooden_barrel_break02.mdl", true);
		PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk02.mdl", true);
		PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk04.mdl", true);
		PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk03.mdl", true);
		PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk01.mdl", true);

		PrecacheModel("models/weapons/w_models/w_bat.mdl", true);
		PrecacheModel("models/weapons/w_models/w_minigun.mdl", true);
		PrecacheModel("models/weapons/w_models/w_bonesaw.mdl", true);
		PrecacheModel("models/weapons/w_models/w_wrench.mdl", true);
		PrecacheModel("models/weapons/w_models/w_bottle.mdl", true);
		PrecacheModel("models/weapons/w_models/w_club.mdl", true);
		PrecacheModel("models/weapons/w_models/w_fireaxe.mdl", true);
		PrecacheModel("models/weapons/w_models/w_shovel.mdl", true);
		PrecacheModel("models/weapons/w_models/w_revolver.mdl", true);
		PrecacheModel("models/weapons/w_models/w_shotgun.mdl", true);
		PrecacheModel("models/weapons/w_models/w_flamethrower.mdl", true);
		PrecacheModel("models/weapons/w_models/w_grenadelauncher.mdl", true);
		PrecacheModel("models/weapons/w_models/w_syringegun.mdl", true);
		PrecacheModel("models/weapons/w_models/w_sniperrifle.mdl", true);
		PrecacheModel("models/weapons/w_models/w_rocketlauncher.mdl", true);
		PrecacheModel("models/weapons/w_models/w_stickybomb_launcher.mdl", true);
		PrecacheModel("models/weapons/w_models/w_medigun.mdl", true);

		char input[512];

		for (int i = 0; i <= 20; i++)
		{
            Format(input, sizeof(input), "materials/tf2ware/tf2ware_points%d.vmt", i);
            PrecacheModel(input, true);
            Format(input, sizeof(input), "materials/tf2ware/tf2ware_points%d.vtf", i);
            PrecacheModel(input, true);
        }

		{
			Format(input, sizeof(input), "materials/tf2ware/tf2ware_points99.vmt");
			PrecacheModel(input, true);
			Format(input, sizeof(input), "materials/tf2ware/tf2ware_points99.vtf");
			PrecacheModel(input, true);
		}

		KvGotoFirstSubKey(MinigameConf);
		int id;
		int enable;
		int i = 1;

#if defined(DEBUG)
		LogMessage("--Adding the following to downloads table from information in minigames.cfg:", input);
#endif

		do
		{
			id	   = KvGetNum(MinigameConf, "id");
			enable = KvGetNum(MinigameConf, "enable", 1);

			if (enable >= 1)
			{
				Format(input, sizeof(input), "imgay/tf2ware/minigame_%d.mp3", id);

#if defined(DEBUG)
				LogMessage("%s", input);
#endif

				precacheSound(input);
			}

			i++;
		} while (KvGotoNextKey(MinigameConf));

		KvRewind(MinigameConf);

		white			  = PrecacheModel("materials/sprites/white.vmt");
		g_HaloSprite	  = PrecacheModel("materials/sprites/halo01.vmt");
		g_ExplosionSprite = PrecacheModel("sprites/sprite_fire01.vmt");

		PrecacheSound("ambient/explosions/explode_8.wav", true);
		SetConVarFloat(ww_speed, 1.0);
		ResetScores();
		bossBattle	= 0;
		RoundStarts = 0;

		SpecialPrecache();

#if defined(DEBUG)
		LogMessage("Map started");
#endif
	}
	else
	{
		InternalSetAlpineVariant(false);
		g_enabled = false;
	}
}

/////////////////////////////////////////

/**
 * I cannot believe this fucking works, I honestly prefer to have to make
 * this unholy abomination than have to deal with the previous Mecha code.
 * 
 * My eyes weep at this.
 * Have fun trying to add custom microgames to this.
 * 
 * tl;dr I should really start working on "source.js" or wait till the
 * C# based replacement for SourcePawn actually matures and becomes public.
 */
void DispatchOnClientJustEntered(int client)
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnClientJustEntered(client);
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnClientJustEntered] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}
}

void DispatchOnMicrogameStart()
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnMicrogameStart();
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnMicrogameStart();
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnMicrogameStart();
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnMicrogameStart();
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnMicrogameStart();
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnMicrogameStart();
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnMicrogameStart();
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnMicrogameStart();
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnMicrogameStart();
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameStart();
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnMicrogameStart();
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnMicrogameStart();
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnMicrogameStart();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameStart();
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnMicrogameStart();
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnMicrogameStart();
		}

		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnMicrogameStart();
		}
		
		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnMicrogameStart();
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnMicrogameStart();
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnMicrogameStart();
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnMicrogameStart();
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnMicrogameStart] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}
}

void DispatchOnMicrogameTimer(int timeLeft)
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnMicrogameTimer] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}
}

void DispatchOnMicrogameEnd()
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnMicrogameEnd();
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnMicrogameEnd] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}	
}

void DispatchOnMicrogamePostEnd()
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnMicrogamePostEnd();
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnMicrogamePostEnd] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}
}

void DispatchOnMicrogameFrame()
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnMicrogameFrame();
		}
		
		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnMicrogameFrame();
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnMicrogameFrame] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}
}

void DispatchOnClientDeath(int client)
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_AIRBLAST:
		{
			view_as<Airblast>(currentMicrogame).OnClientDeath(client);
		}

		case MG_AIR_RAID:
		{
			view_as<AirRaid>(currentMicrogame).OnClientDeath(client);
		}

		case MG_BARREL:
		{
			view_as<Barrel>(currentMicrogame).OnClientDeath(client);
		}

		case MG_BBALL:
		{
			view_as<BBall>(currentMicrogame).OnClientDeath(client);
		}

		case MG_COLOR_TEXT:
		{
			view_as<ColorText>(currentMicrogame).OnClientDeath(client);
		}

		case MG_FLOOD:
		{
			view_as<Flood>(currentMicrogame).OnClientDeath(client);
		}

		case MG_FROGGER:
		{
			view_as<Frogger>(currentMicrogame).OnClientDeath(client);
		}

		case MG_GHOSTBUSTERS:
		{
			view_as<Ghostbusters>(currentMicrogame).OnClientDeath(client);
		}

		case MG_GOOMBA:
		{
			view_as<Goomba>(currentMicrogame).OnClientDeath(client);
		}

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnClientDeath(client);
		}

		case MG_HOPSCOTCH:
		{
			view_as<Hopscotch>(currentMicrogame).OnClientDeath(client);
		}

		case MG_HUGGING:
		{
			view_as<Hugging>(currentMicrogame).OnClientDeath(client);
		}

		case MG_JUMP_ROPE:
		{
			view_as<JumpRope>(currentMicrogame).OnClientDeath(client);
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnClientDeath(client);
		}

		case MG_MATH:
		{
			view_as<Math>(currentMicrogame).OnClientDeath(client);
		}

		case MG_MOVEMENT:
		{
			view_as<Movement>(currentMicrogame).OnClientDeath(client);
		}

		case MG_NEEDLE_JUMP:
		{
			view_as<NeedleJump>(currentMicrogame).OnClientDeath(client);
		}

		case MG_SAW_RUN:
		{
			view_as<Sawrun>(currentMicrogame).OnClientDeath(client);
		}

		case MG_SIMON_SAYS:
		{
			view_as<SimonSays>(currentMicrogame).OnClientDeath(client);
		}

		case MG_SNIPER_TARGET:
		{
			view_as<SniperTarget>(currentMicrogame).OnClientDeath(client);
		}

		case MG_SPYCRAB:
		{
			view_as<Spycrab>(currentMicrogame).OnClientDeath(client);
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchOnClientDeath] Ignoring dispatch for unknown microgame %d.", currentMicrogame);
		}
	}
}

bool DispatchIsMicrogamePlayable(Microgame mg, int players)
{
	switch (view_as<Microgames>(mg))
	{
		case MG_AIRBLAST:
		{
			return view_as<Airblast>(mg).IsMicrogamePlayable(players);
		}

		case MG_AIR_RAID:
		{
			return view_as<AirRaid>(mg).IsMicrogamePlayable(players);
		}

		case MG_BARREL:
		{
			return view_as<Barrel>(mg).IsMicrogamePlayable(players);
		}

		case MG_BBALL:
		{
			return view_as<BBall>(mg).IsMicrogamePlayable(players);
		}

		case MG_COLOR_TEXT:
		{
			return view_as<ColorText>(mg).IsMicrogamePlayable(players);
		}

		case MG_FLOOD:
		{
			return view_as<Flood>(mg).IsMicrogamePlayable(players);
		}

		case MG_FROGGER:
		{
			return view_as<Frogger>(mg).IsMicrogamePlayable(players);
		}

		case MG_GHOSTBUSTERS:
		{
			return view_as<Ghostbusters>(mg).IsMicrogamePlayable(players);
		}

		case MG_GOOMBA:
		{
			return view_as<Goomba>(mg).IsMicrogamePlayable(players);
		}

		case MG_HIT_ENEMY:
		{
			return view_as<HitEnemy>(mg).IsMicrogamePlayable(players);
		}

		case MG_HOPSCOTCH:
		{
			return view_as<Hopscotch>(mg).IsMicrogamePlayable(players);
		}

		case MG_HUGGING:
		{
			return view_as<Hugging>(mg).IsMicrogamePlayable(players);
		}

		case MG_JUMP_ROPE:
		{
			return view_as<JumpRope>(mg).IsMicrogamePlayable(players);
		}

		case MG_KAMIKAZE:
		{
			return view_as<Kamikaze>(mg).IsMicrogamePlayable(players);
		}

		case MG_MATH:
		{
			return view_as<Math>(mg).IsMicrogamePlayable(players);
		}

		case MG_MOVEMENT:
		{
			return view_as<Movement>(mg).IsMicrogamePlayable(players);
		}

		case MG_NEEDLE_JUMP:
		{
			return view_as<NeedleJump>(mg).IsMicrogamePlayable(players);
		}

		case MG_SAW_RUN:
		{
			return view_as<Sawrun>(mg).IsMicrogamePlayable(players);
		}

		case MG_SIMON_SAYS:
		{
			return view_as<SimonSays>(mg).IsMicrogamePlayable(players);
		}

		case MG_SNIPER_TARGET:
		{
			return view_as<SniperTarget>(mg).IsMicrogamePlayable(players);
		}

		case MG_SPYCRAB:
		{
			return view_as<Spycrab>(mg).IsMicrogamePlayable(players);
		}

		default:
		{
			PrintToServer("[TF2Ware] [DispatchIsMicrogamePlayable] Ignoring dispatch for unknown microgame %d.", mg);
			return true;
		}
	}
}

public Action DispatchOnPlayerChatSay(int client, const char message[256])
{
	switch (view_as<Microgames>(currentMicrogame))
	{
		case MG_COLOR_TEXT:
		{
			bool ret = view_as<ColorText>(currentMicrogame).OnPlayerChatMessage(client, message);
			return ret ? Plugin_Handled : Plugin_Continue;			
		}

		case MG_MATH:
		{
			bool ret = view_as<Math>(currentMicrogame).OnPlayerChatMessage(client, message);
			return ret ? Plugin_Handled : Plugin_Continue;
		}

		default:
		{
			return Plugin_Continue;
		}
	}
}

public Microgame GetCurrentMicrogame()
{
	return currentMicrogame;
}

/////////////////////////////////////////

public Action OnGetGameDescription(char gameDesc[64])
{
	if (g_enabled)
	{
		Format(gameDesc, sizeof(gameDesc), "TF2Ware Classic");
		return Plugin_Changed;
	}
	else
	{
		return Plugin_Continue;
	}
}

public Action Timer_DisplayVersion(Handle timer, any client)
{
	if (IsValidClient(client))
	{
		SetHudTextParams(0.63, 0.73, 25.0, 255, 255, 255, 255, 1, 3.0, 0.0, 3.0);
		ShowHudText(client, 1, "c%s", PLUGIN_VERSION);
	}
	return Plugin_Handled;
}

public Action Event_RoundStart(Handle event, const char[] name, bool dontBroadcast)
{
	if (g_enabled && GetConVarBool(ww_enable))
	{
		if (RoundStarts == 0)
		{
			g_waiting = true;
			SetGameMode();
			RemoveAllParticipants();
		}

		if (RoundStarts == 1)
		{
			g_waiting = false;

			SetConVarInt(ConVar_TFPlayerMovementRestartFreeze, 0);

			SetGameMode();
			ResetScores();
			StartMinigame();

			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && g_Spawned[client])
				{
					if (!IsFakeClient(client))
					{
						StopSound(client, SND_CHANNEL_SPECIFIC, MUSIC_WAITING);
						SetOverlay(client, "");
					}

					if (SpecialRound == WIPEOUT)
					{
						SetWipeoutPosition(client, true);
					}
				}
			}

#if defined(DEBUG)
			LogMessage("[TF2Ware::Event_RoundStart] Waiting-for-players period has ended");
#endif
		}

		SpecialRoundCooldown--;		
	}

	RoundStarts++;

	return Plugin_Continue;
}

public Action Event_RoundEnd(Handle event, const char[] name, bool dontBroadcast)
{
	if (g_enabled && GetConVarBool(ww_enable))
	{
		g_enabled = false;

#if defined(DEBUG)
		LogMessage("== ROUND ENDED SUCCESSFULLY == ");
#endif
	}

	return Plugin_Stop;
}

public void OnClientPostAdminCheck(int client)
{
	if (!g_enabled) return;

	UpdateClientCheatValue();

	if (SpecialRound == WIPEOUT)
	{
		g_Points[client] = -1;
	}
	else
	{
		g_Points[client] = GetAverageScore();
	}

#if defined(DEBUG)
	LogMessage("[TF2Ware::OnClientPostAdminCheck] Client (%d) post admin check", client);
#endif
}

public void OnClientPutInServer(int client)
{
	if (!g_enabled) return;

#if defined(DEBUG)
	LogMessage("[TF2Ware::OnClientPutInServer] Client (%d) put in server and hooked", client);
#endif

	SDKHook(client, SDKHook_PreThink, OnPreThink);
	SDKHook(client, SDKHook_OnTakeDamage, OnTakeDamageClient);
	SDKHook(client, SDKHook_Touch, Special_NoTouch);
	SDKHook(client, SDKHook_OnTakeDamage, Special_DamagePush);
}

public void OnClientDisconnect(int client)
{
	if (!g_enabled) return;

#if defined(DEBUG)
	LogMessage("[TF2Ware::OnClientDisconnect] Client (%d) disconnected", client);
#endif

	if (GetConVarBool(ww_overhead_scores))
	{
		DestroySprite(client);
	}

	g_Points[client] = 0;
	g_Participating[client] = false;
	g_Spawned[client] = false;
}

public Action OnTakeDamageClient(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if ((g_Winner[victim] >= 1) && (status != 2))
	{
		damage = 0.0;
		return Plugin_Changed;
	}

	if (IsValidClient(attacker) && (g_Winner[attacker] == 1) && (g_Winner[victim] == 0) && IsValidClient(victim) && IsPlayerAlive(victim))
	{
		damage = 450.0;
		return Plugin_Changed;
	}

	return Plugin_Continue;
}

public void OnPreThink(int client)
{
	int iButtons = GetClientButtons(client);
	if ((status != 2) && GetConVarBool(ww_enable) && g_enabled && (g_Winner[client] == 0) && !(SpecialRound == BONK && status != 5))
	{
		if ((iButtons & IN_ATTACK2) || (iButtons & IN_ATTACK))
		{
			iButtons &= ~IN_ATTACK;
			iButtons &= ~IN_ATTACK2;
			SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
		}
	}

	if ((status == 2) && (g_attack == false || !IsClientParticipating(client)) && GetConVarBool(ww_enable) && g_enabled)
	{
		if ((iButtons & IN_ATTACK2) || (iButtons & IN_ATTACK))
		{
			iButtons &= ~IN_ATTACK;
			iButtons &= ~IN_ATTACK2;
			SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
		}
	}
}

public Action EventInventoryApplication(Handle event, const char[] name, bool dontBroadcast)
{
	int client = GetClientOfUserId(GetEventInt(event, "userid"));

#if defined(DEBUG)
	LogMessage("[TF2Ware::EventInventoryApplication] Client (%d) post inventory", client);
#endif

	if (g_Spawned[client] == false && g_waiting && GetConVarBool(ww_enable) && g_enabled && !IsFakeClient(client))
	{
		EmitSoundToClient(client, MUSIC_WAITING, SOUND_FROM_PLAYER, SND_CHANNEL_SPECIFIC);
		SetOverlay(client, "tf2ware_welcome");
		CreateTimer(0.25, Timer_DisplayVersion, client);
	}

	g_Spawned[client] = true;

	if (GetConVarBool(ww_enable) && g_enabled)
	{
		SetEntityRenderMode(client, RENDER_NORMAL);
		SetEntityRenderColor(client, 255, 255, 255, 255);
		SetEntProp(client, Prop_Send, "m_bGlowEnabled", 0);

		if ((status != 2) && (g_Winner[client] == 0))
		{
			DisableClientWeapons(client);

			if (status != 5 && GetConVarBool(ww_overhead_scores)) 
			{
				CreateSprite(client);
			}
		}

		if (status == 2 && IsClientParticipating(client))
		{
			DispatchOnClientJustEntered(client);

			if (GetConVarBool(ww_overhead_scores))
			{
				CreateSprite(client);
			}
		}

		if (status == 5 && g_Winner[client] > 0 && GetConVarBool(ww_overhead_scores)) 
		{
			CreateSprite(client);
		}

		if ((status == 2 && g_attack) || (g_Winner[client] > 0) || (SpecialRound == BONK))
		{
			SetWeaponState(client, true);
		}
		else
		{
			SetWeaponState(client, false);
		}

		HandlePlayerItems(client);

		if (SpecialRound == SINGLEPLAYER)
		{
			SetEntityRenderMode(client, RENDER_TRANSCOLOR);
			SetEntityRenderColor(client, 255, 255, 255, 0);
		}

		if (SpecialRound == WIPEOUT && g_waiting == false)
		{
			if (status == 2 && IsClientParticipating(client))
			{
				// do nothing
			}
			else
			{
				SetWipeoutPosition(client, true);
			}

			HandleWipeoutLives(client);
		}
	}

	return Plugin_Continue;
}

stock void precacheSound(const char[] var0)
{
	char buffer[128];
	PrecacheSound(var0, true);
	Format(buffer, sizeof(buffer), "sound/%s", var0);
	AddFileToDownloadsTable(buffer);
}

public void StartMinigame_cvar(Handle cvar, const char[] oldVal, const char[] newVal)
{
	if (GetConVarBool(ww_enable) && g_enabled)
	{
		StartMinigame();
		SetConVarInt(FindConVar("mp_respawnwavetime"), 9999);
		SetConVarInt(FindConVar("mp_forcecamera"), 0);
	}
	else
	{
		ServerCommand("host_timescale %f", 1.0);
		ServerCommand("phys_timescale %f", 1.0);
		ResetConVar(FindConVar("mp_respawnwavetime"));
		ResetConVar(FindConVar("mp_forcecamera"));
		status = 0;
	}
}

public void OverheadScoresChanged(Handle cvar, const char[] oldVal, const char[] newVal)
{
	if (GetConVarBool(ww_overhead_scores) && g_enabled)
	{
		CreateAllSprites();
	}
	else
	{
		DestroyAllSprites();
	}
}

public void OnGameFrame()
{
	if (!GetConVarBool(ww_enable))
	{
		return;
	}

	if (status == 2)
	{
		DispatchOnMicrogameFrame();

		if (SpecialRound == WIPEOUT && status == 1)
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i))
				{
					float pos[3];
					GetClientAbsOrigin(i, pos);
					pos[2] -= 25.0;

					if (pos[2] < GAMEMODE_WIPEOUT_HEIGHT)
					{
						pos[2] = GAMEMODE_WIPEOUT_HEIGHT;
					}

					TeleportEntity(i, pos, NULL_VECTOR, NULL_VECTOR);
				}
			}
		}
	}

	if (GetConVarBool(ww_overhead_scores))
	{
		/**
		 * Enjoy the tickrate dip.
		 */
		OverheadScoresUpdate();
	}
}

public Action StartMinigame_timer(Handle hTimer)
{
	if (status == 0)
	{
		StartMinigame();
	}
	return Plugin_Stop;
}

public Action StartMinigame_timer2(Handle hTimer)
{
	if (status == 10)
	{
		status = 0;
		StartMinigame();
	}

	return Plugin_Stop;
}

int RollMinigame()
{
	/**
	 * Rewrite the entire boss logic since it's a clusterfuck at the moment.
	 * 
	 * Remaining logic TODO:
	 * - Handle disablement of microgames (probably won't be a feature anymore)
	 * - Do we still handle the "chance" value?
	 * - Use "g_LastBoss" to prevent repeating the boss during double boss battle.
	 */
	Microgame candidate;
	int candidateIndex = GetConVarInt(ww_force);

	/**
	 * Allow overriding the microgame, but be warned:
	 * Invalid values will crash the plugin!
	 */
	if (candidateIndex)
	{
		currentMicrogame = GetMicrogame(candidateIndex);
		return candidateIndex;
	}

	int players = GetClientCount();
	static int lastPlayedIndex = -1;

	do
	{
		candidate = GetRandomMicrogame(candidateIndex);

		/**
		 * Don't play the same thing.
		 */
		if (lastPlayedIndex == candidateIndex)
		{
			continue;
		}
		
		/**
		 * Don't play boss microgames if we're not at the boss.
		 */
		if (bossBattle == 0 && IsBossMicrogame(candidate))
		{
			continue;
		}

		/**
		 * However, if we are at a boss then we need only
		 * boss microgames.
		 */
		if (bossBattle != 0 && !IsBossMicrogame(candidate))
		{
			continue;
		}

		/**
		 * If we don't have enough players, try another microgame.
		 */
		if (!DispatchIsMicrogamePlayable(candidate, players))
		{
			continue;
		}

		currentMicrogame = candidate;
		lastPlayedIndex = candidateIndex;
		break;
	} while (true);

	return candidateIndex;
}

public Action Player_Team(Handle event, const char[] name, bool dontBroadcast)
{
	int client	= GetClientOfUserId(GetEventInt(event, "userid"));
	int oldTeam = GetEventInt(event, "oldteam");
	int newTeam = GetEventInt(event, "team");

#if defined(DEBUG)
	LogMessage("[TF2Ware::Player_Team] Client (%N) changed team", client);
#endif

	if (GetConVarBool(ww_enable) && g_enabled)
	{
		CreateTimer(0.1, StartMinigame_timer);

		if (oldTeam < 2 && newTeam >= 2)
		{
			GiveSpecialRoundInfo(client);
		}
	}

	return Plugin_Continue;
}

void HandOutPoints()
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::HandOutPoints] Handing out points");
#endif

	for (int client = 1; client <= MaxClients; client++)
	{
		int points = 1;
		if (bossBattle == 1)
		{
			points = 5;
		}

		if ((IsValidClient(client)) && IsClientParticipating(client))
		{
			if (g_Complete[client])
			{
				if (SpecialRound != WIPEOUT)
				{
					g_Points[client] += points;
				}
			}
			else
			{
				if (SpecialRound == WIPEOUT && g_Points[client] > 0)
				{
					g_Points[client] -= points;

					if (g_Points[client] < 0)
					{
						g_Points[client] = 0;
					}

					HandleWipeoutLives(client, true);
				}
			}
		}

		g_Complete[client] = false;
	}
}

stock void PrintWipeoutMessage(int candidatePlayers[MAX_WIPEOUT_PLAYERS], int populated)
{
	char formatString[512];

	for (int i = 0; i < populated; i++)
	{
		StrCat(formatString, sizeof (formatString), "%N");
	}

	PrintCenterTextAll(formatString, candidatePlayers);
}

stock int GetWipeoutLimit()
{
	/**
	 * TODO(irql): Add a dynamic limit.
	 */

	return MAX_WIPEOUT_PLAYERS;
}

int PopulateWipeoutPlayers(int candidatePlayers[MAX_WIPEOUT_PLAYERS])
{
	int playersAdded = 0;
	int dynamicLimit = GetWipeoutLimit();

	for (int client = MaxClients; client >= 1; client--)
	{
		if (!IsValidClient(client))
		{
			continue;
		}

		if (!IsClientParticipating(client))
		{
			continue;
		}

		if (GetClientTeam(client) < 2)
		{
			continue;
		}

		/**
		 * We use points as lives, so...
		 */
		if (g_Points[client] <= 0)
		{
			continue;
		}

		if (playersAdded + 1 > dynamicLimit)
		{
			break;
		}

		candidatePlayers[playersAdded++] = client;
		g_Participating[client] = true;
	}

	return playersAdded;
}

void StartMinigame()
{
	if (GetConVarBool(ww_enable) && g_enabled && (status == 0) && g_waiting == false)
	{
#if defined(DEBUG)
		LogMessage("[TF2Ware::StartMinigame] Starting microgame %s! (status=0)", minigame);
#endif
		
		SetConVarInt(FindConVar("mp_respawnwavetime"), 9999);
		SetConVarInt(ConVar_MPFriendlyFire, 1);

		float MUSIC_INFO_LEN;
		char MUSIC_INFO[PLATFORM_MAX_PATH];
		
		if (SpecialRound == WIPEOUT)
		{
			MUSIC_INFO_LEN = MUSIC_WIPEOUT_START_LEN;
			Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_WIPEOUT_START);
		}
		else if (SpecialRound != NONE)
		{
			MUSIC_INFO_LEN = MUSIC_SPECIAL_START_LEN;
			Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_SPECIAL_START);			
		}
		else
		{
			MUSIC_INFO_LEN = MUSIC_START_LEN;
			Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_START);
		}

		RespawnAll();
		RemoveAllParticipants();
		UpdateHud(GetSpeedMultiplier(MUSIC_INFO_LEN));

		if (SpecialRound == SINGLEPLAYER)
		{
			NoCollision(true);
		}

		currentSpeed = GetConVarFloat(ww_speed);
		
		ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
		ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));

		if (SpecialRound == WIPEOUT)
		{
			int candidatePlayers[MAX_WIPEOUT_PLAYERS];
			int populated = PopulateWipeoutPlayers(candidatePlayers);

			/**
			 * Everyone lost, lol.
			 */
			if (populated == 0)
			{
				status	   = 4;
				bossBattle = 2;
				CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Victory_Timer);
				return;
			}

			PrintWipeoutMessage(candidatePlayers, populated);
		}
		else
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && GetClientTeam(client) >= 2 && g_Spawned[client])
				{
					g_Participating[client] = true;
				}
			}
		}

		if (GetConVarBool(ww_music))
		{
			EmitSoundToClient(1, MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		}
		else
		{
			EmitSoundToAll(MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		}

		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && (!(IsFakeClient(i))))
			{
				SetOverlay(i, "");
				g_Minipoints[i] = 0;
			}
		}

		status	  = 1;
		iMinigame = RollMinigame();
		minigame  = g_name[iMinigame - 1];

		if (bossBattle == 1)
		{
			g_LastBoss = iMinigame;
		}

		CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Game_Start);

		g_attack = (SpecialRound == BONK);

		if (GetConVarBool(ww_overhead_scores))
		{
			CreateAllSprites();
		}
	}
}

public Action Game_Start(Handle hTimer)
{
	if (status == 1)
	{
#if defined(DEBUG)
		LogMessage("[TF2Ware::Game_Start] Microgame %s started! (status=1)", minigame);
#endif

		// Spawn everyone so they can participate
		RespawnAll();

		if (SpecialRound == SINGLEPLAYER) 
		{
			NoCollision(true);
		}
		else if (SpecialRound == NO_TOUCHING)
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && !IsFakeClient(client)) 
				{
					ToggleThirdperson(client, true);
				}
			}
		}
		else if (SpecialRound == WIPEOUT)
		{
			for (int i2 = 1; i2 <= MaxClients; i2++)
			{
				if (IsValidClient(i2) && IsPlayerAlive(i2) && IsClientParticipating(i2))
				{
					SetEntityMoveType(i2, MOVETYPE_WALK);
					SetWipeoutPosition(i2, false);
				}
			}
		}

		// Play the microgame's music
		char sound[512];
		Format(sound, sizeof(sound), "imgay/tf2ware/minigame_%d.mp3", iMinigame);

		if (view_as<Microgames>(currentMicrogame) == MG_GHOSTBUSTERS && GetRandomInt(1, 3) == 1)
		{
			Format(sound, sizeof(sound), "imgay/tf2ware/minigame_%d_alt.mp3", iMinigame);
		}
		
		int channel = SNDCHAN_AUTO;
		if (GetMinigameConfNum(minigame, "dynamic", 0))
		{
			channel = SND_CHANNEL_SPECIFIC;
		}

		if (GetConVarBool(ww_music))
		{
			EmitSoundToClient(1, sound, SOUND_FROM_PLAYER, channel, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		}
		else
		{
			EmitSoundToAll(sound, SOUND_FROM_PLAYER, channel, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		}

		// Set everyone's state to fail
		SetStateAll(false);

		// The 'x did y first' is untriggered
		g_first = false;

		// current proccess
		status	= 2;

		// Reset everyone's mission
		SetMissionAll(0);

		// noone can attack
		g_attack = (SpecialRound == BONK);

		// initiate mission
		InitMinigame();

		// show the mission text
		PrintMissionText();

		// g_TimeLeft counter. Let it stay longer on boss battles.
		g_TimeLeft = 8;

		if (bossBattle == 1)
		{
			CreateTimer(GetSpeedMultiplier(3.0), CountDown_Timer);
		}
		else
		{
			CreateTimer(GetSpeedMultiplier(1.0), CountDown_Timer);
		}

		// get the lasting time from the cfg
		MicrogameTimer = CreateTimer(GetSpeedMultiplier(GetMinigameConfFloat(minigame, "duration")), EndGame);

#if defined(DEBUG)
		LogMessage("[TF2Ware::Game_Start] Microgame started post");
#endif
	}

	return Plugin_Stop;
}

void PrintMissionText()
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::PrintMissionText] Printing mission text");
#endif

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i))
		{
			char input[512];
			Format(input, sizeof(input), "tf2ware_minigame_%d_%d", iMinigame, g_Mission[i] + 1);
			SetOverlay(i, input);
			g_ModifiedOverlay[i] = false;
		}
	}
}

public Action CountDown_Timer(Handle hTimer)
{
	if (status == 2 && g_TimeLeft > 0)
	{
		g_TimeLeft = g_TimeLeft - 1;
		CreateTimer(GetSpeedMultiplier(0.4), CountDown_Timer);

		if (bossBattle != 1)
		{
			DispatchOnMicrogameTimer(g_TimeLeft);
		}

		if (g_TimeLeft == 2)
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && !IsFakeClient(i) && g_ModifiedOverlay[i] == false)
				{
					SetOverlay(i, "");
				}
			}
		}
	}

	return Plugin_Continue;
}

public Action EndGame(Handle hTimer)
{
	MicrogameTimer = INVALID_HANDLE;

	if (status == 2)
	{
#if defined(DEBUG)
		LogMessage("[TF2Ware::EndGame] Microgame '%s' (id:%d) ended! (status=2)", minigame, iMinigame);
#endif
		
		DispatchOnMicrogameEnd();

		status = 0;

		float MUSIC_INFO_LEN;
		char MUSIC_INFO_WIN[PLATFORM_MAX_PATH];
		char MUSIC_INFO_FAIL[PLATFORM_MAX_PATH];

		if (SpecialRound == WIPEOUT)
		{
			MUSIC_INFO_LEN = MUSIC_WIPEOUT_END_LEN;
			Format(MUSIC_INFO_WIN, sizeof(MUSIC_INFO_WIN), MUSIC_WIPEOUT_WIN);
			Format(MUSIC_INFO_FAIL, sizeof(MUSIC_INFO_FAIL), MUSIC_WIPEOUT_FAIL);
		}
		else if (SpecialRound != NONE)
		{
			MUSIC_INFO_LEN = MUSIC_SPECIAL_END_LEN;
			Format(MUSIC_INFO_WIN, sizeof(MUSIC_INFO_WIN), MUSIC_SPECIAL_WIN);
			Format(MUSIC_INFO_FAIL, sizeof(MUSIC_INFO_FAIL), MUSIC_SPECIAL_FAIL);
		}
		else
		{
			MUSIC_INFO_LEN = MUSIC_END_LEN;
			Format(MUSIC_INFO_WIN, sizeof(MUSIC_INFO_WIN), MUSIC_WIN);
			Format(MUSIC_INFO_FAIL, sizeof(MUSIC_INFO_FAIL), MUSIC_FAIL);
		}

		g_attack = (SpecialRound == BONK);

		/**
		 * Send a late end event here to maintain compatibility.
		 */
		DispatchOnMicrogamePostEnd();

		CleanupAllVocalizations();

		currentSpeed = GetConVarFloat(ww_speed);
		ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
		ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));

		char sound[512];
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i))
			{
				if (IsClientParticipating(i))
				{
					// heal everyone
					// TF2_RegeneratePlayer(i);

					// Kill their weapons
					DisableClientWeapons(i);
					HealClient(i);

					// Cancel taunts.
					ClearAllTaunts(i);

					// if client won
					if (g_Complete[i])
					{
						Format(sound, sizeof(sound), MUSIC_INFO_WIN);
					}

					// if client lost
					if (g_Complete[i] == false)
					{
						Format(sound, sizeof(sound), MUSIC_INFO_FAIL);
					}
				}
				else
				{
					Format(sound, sizeof(sound), MUSIC_INFO_WIN);
				}

				char oldSound[512];
				Format(oldSound, sizeof(oldSound), "imgay/tf2ware/minigame_%d.mp3", iMinigame);

				if (GetMinigameConfNum(minigame, "dynamic", 0))
				{
					StopSound(i, SND_CHANNEL_SPECIFIC, oldSound);
				}
				
				EmitSoundToClient(i, sound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			}
		}

		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && !IsFakeClient(i) && IsClientParticipating(i))
			{
				if (g_Complete[i])
				{
					SetOverlay(i, "tf2ware_minigame_win");
				}
				if (g_Complete[i] == false)
				{
					SetOverlay(i, "tf2ware_minigame_fail");
				}
			}
		}
		
		UpdateHud(GetSpeedMultiplier(MUSIC_INFO_LEN));

		bool bHandlePoints = true;

		if (SpecialRound == WIPEOUT)
		{
			bool bSomeoneWon = false;

			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && IsClientParticipating(i) && g_Complete[i] == true)
				{
					bSomeoneWon = true;
				}
			}

			if (bSomeoneWon == false && bossBattle == 1 && GetLeftWipeoutPlayers() == 2)
			{
				bHandlePoints = false;
				CPrintToChatAll("{red}DRAW{default}... playing new boss!");
			}
		}

		if (bHandlePoints)
		{
			HandOutPoints();
		}

		// RESPAWN START
		if (GetMinigameConfNum(minigame, "endrespawn", 0) > 0)
		{
			RespawnAll(true, false);
		}
		else
		{
			RespawnAll();
		}
		
		if (SpecialRound == WIPEOUT)
		{
			for (int i2 = 1; i2 <= MaxClients; i2++)
			{
				if (IsValidClient(i2) && IsClientParticipating(i2))
				{
					SetWipeoutPosition(i2, true);
				}
			}
		}

		NoCollision(SpecialRound == BONK);

		if (SpecialRound == THIRDPERSON)
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsValidClient(client) && !IsFakeClient(client))
				{
					ToggleThirdperson(client, true);
				}
			}
		}

		// RESPAWN END

		bool speedup = false;
		g_MinigamesTotal += 1;

		if (bossBattle == 1) bossBattle = 2;

		if (SpecialRound == WIPEOUT)
		{
			if ((GetAverageScoreFloat() <= 2.80) && (bossBattle == 0) && currentSpeed <= 1.0) speedup = true;
			if ((GetAverageScoreFloat() <= 2.50) && (bossBattle == 0) && currentSpeed <= 2.0) speedup = true;
			if ((GetAverageScoreFloat() <= 2.20) && (bossBattle == 0) && currentSpeed <= 3.0) speedup = true;
			if ((GetAverageScoreFloat() <= 1.80) && (bossBattle == 0) && currentSpeed <= 4.0) speedup = true;
			if ((GetAverageScoreFloat() <= 1.40) && (bossBattle == 0) && currentSpeed <= 5.0) speedup = true;
			if ((GetAverageScoreFloat() <= 1.0) && (bossBattle == 0) && currentSpeed <= 6.0) speedup = true;
			if ((GetLeftWipeoutPlayers() == 2) && (bossBattle != 1))
			{
				speedup	   = true;
				bossBattle = 1;
			}
		}
		else
		{
			if ((g_MinigamesTotal == 4) && (bossBattle == 0)) speedup = true;
			if ((g_MinigamesTotal == 8) && (bossBattle == 0)) speedup = true;
			if ((g_MinigamesTotal == 12) && (bossBattle == 0)) speedup = true;
			if ((g_MinigamesTotal == 16) && (bossBattle == 0)) speedup = true;
			if ((g_MinigamesTotal == 19) && (bossBattle == 0))
			{
				speedup	   = true;
				bossBattle = 1;
			}

			if ((g_MinigamesTotal >= 19) && bossBattle == 2 && SpecialRound == DOUBLE_BOSS_BATTLE && Special_TwoBosses == false)
			{
				speedup			  = true;
				bossBattle		  = 1;
				Special_TwoBosses = true;
			}
		}

		if (SpecialRound == WIPEOUT && GetLeftWipeoutPlayers() <= 1)
		{
			status = 4;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Victory_Timer);
		}

		/**
		 * TODO(irql):
		 * 
		 * Mecha....WHY
		 */
		if (speedup == false)
		{
			status = 10;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), StartMinigame_timer2);
		}

		if (speedup == true)
		{
			status = 3;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), SpeedUp_Timer);
		}
		
		if (bossBattle == 2 && speedup == false)
		{
			status = 4;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Victory_Timer);
		}
	}

	return Plugin_Stop;
}

public Action SpeedUp_Timer(Handle hTimer)
{
	if (status == 3)
	{
		RemoveAllParticipants();

		if (bossBattle == 1)
		{
#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] GETTING READY TO START SOME BOSS");
#endif
			
			float MUSIC_INFO_LEN;
			char MUSIC_INFO[PLATFORM_MAX_PATH];
			
			if (SpecialRound == WIPEOUT)
			{
				MUSIC_INFO_LEN = MUSIC_WIPEOUT_BOSS_LEN;
				Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_WIPEOUT_BOSS);
			}
			else if (SpecialRound != NONE)
			{
				/**
				 * We can reuse the wipeout boss sound, since it's what we need.
				 */
				MUSIC_INFO_LEN = MUSIC_WIPEOUT_BOSS_LEN;
				Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_WIPEOUT_BOSS);
			}
			else
			{
				MUSIC_INFO_LEN = MUSIC_BOSS_LEN;
				Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_BOSS);
			}

#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] Played music to clients, setting global speed");
#endif

			// Set the Speed. If special round, we want it to be a tad faster ;)
			if (SpecialRound == SUPER_SPEED)
			{
				SetConVarFloat(ww_speed, 3.0);
			}
			else
			{
				SetConVarFloat(ww_speed, 1.0);
			}

#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] Resetting timescales.");
#endif

			currentSpeed = GetConVarFloat(ww_speed);
			ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
			ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));

#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] Creating timer to start minigame.");
#endif

			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), StartMinigame_timer2);

#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] Playing minigame start.");
#endif

			if (GetConVarBool(ww_music))
			{
				EmitSoundToClient(1, MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			}
			else
			{
				EmitSoundToAll(MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			}

			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && (!(IsFakeClient(i))))
				{
					SetOverlay(i, "tf2ware_minigame_boss");
				}
			}

#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] Calling UpdateHUD().");
#endif

			UpdateHud(GetSpeedMultiplier(MUSIC_INFO_LEN));
		}

#if defined(DEBUG)
			LogMessage("[TF2Ware::SpeedUp_Timer] Boss battle check.");
#endif

		if (bossBattle != 1)
		{
			float MUSIC_INFO_LEN;
			char MUSIC_INFO[PLATFORM_MAX_PATH];

			if (SpecialRound == WIPEOUT)
			{
				MUSIC_INFO_LEN = MUSIC_WIPEOUT_SPEEDUP_LEN;
				Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_WIPEOUT_SPEEDUP);
			}
			else if (SpecialRound != NONE)
			{
				MUSIC_INFO_LEN = MUSIC_SPECIAL_SPEEDUP_LEN;
				Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_SPECIAL_SPEEDUP);
			}
			else
			{
				MUSIC_INFO_LEN = MUSIC_SPEEDUP_LEN;
				Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_SPEEDUP);
			}

			if (GetConVarBool(ww_music))
			{
				EmitSoundToClient(1, MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			}
			else
			{
				EmitSoundToAll(MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			}

			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && (!(IsFakeClient(i))))
				{
					SetOverlay(i, "tf2ware_minigame_speed");
				}
			}

			UpdateHud(GetSpeedMultiplier(MUSIC_INFO_LEN));
			SetConVarFloat(ww_speed, GetConVarFloat(ww_speed) + 1.0);
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), StartMinigame_timer2);
		}

#if defined(DEBUG)
		LogMessage("[TF2Ware::SpeedUp_Timer] Setting (status=10)");
#endif

		status = 10;
	}

	return Plugin_Stop;
}

public Action Victory_Timer(Handle hTimer)
{
	if ((status == 4) && (bossBattle > 0))
	{
		bossBattle = 0;
		SetConVarFloat(ww_speed, 1.0);
		currentSpeed = GetConVarFloat(ww_speed);

		float MUSIC_INFO_LEN;
		char MUSIC_INFO[PLATFORM_MAX_PATH];

		if (SpecialRound == WIPEOUT)
		{
			MUSIC_INFO_LEN = MUSIC_WIPEOUT_GAMEOVER_LEN;
			Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_WIPEOUT_GAMEOVER);
		}
		else if (SpecialRound != NONE)
		{
			MUSIC_INFO_LEN = MUSIC_SPECIAL_GAMEOVER_LEN;
			Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_SPECIAL_GAMEOVER);			
		}
		else
		{
			MUSIC_INFO_LEN = MUSIC_GAMEOVER_LEN;
			Format(MUSIC_INFO, sizeof(MUSIC_INFO), MUSIC_GAMEOVER);
		}

		status = 5;
		if (HasMapEnded())
		{
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Classic_EndMap);
		}
		else
		{
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), RestartAll_Timer);
		}

		if (GetConVarBool(ww_music))
		{
			EmitSoundToClient(1, MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		}
		else
		{
			EmitSoundToAll(MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
		}

		DestroyAllSprites();
		ResetWinners();

		int targetscore;
		if (SpecialRound == LEAST_IS_BEST)
		{
			targetscore = GetLowestScore();
		}
		else
		{
			targetscore = GetHighestScore();
		}

		int winnernumber = 0;
		Handle ArrayWinners = CreateArray();

		char winnerstring_prefix[128];
		char winnerstring_names[512];
		char pointsname[512];

		if (SpecialRound == WIPEOUT)
		{
			Format(pointsname, sizeof(pointsname), "lives");
		}
		else
		{
			Format(pointsname, sizeof(pointsname), "points");
		}

		bool bAccepted = false;
		for (int i = 1; i <= MaxClients; i++)
		{
			SetOverlay(i, "");

			if (IsValidClient(i) && (GetClientTeam(i) == 2 || GetClientTeam(i) == 3))
			{
				bAccepted = false;

				if (SpecialRound == WIPEOUT)
				{
					if (g_Points[i] > 0)
					{
						bAccepted = true;
					}
				}
				else
				{
					if (SpecialRound != LEAST_IS_BEST && g_Points[i] >= targetscore)
					{
						bAccepted = true;
					}

					if (SpecialRound == LEAST_IS_BEST && g_Points[i] <= targetscore)
					{
						bAccepted = true;
					}
				}

				if (bAccepted)
				{
					g_Winner[i] = 1;

					if (GetConVarBool(ww_overhead_scores))
					{
						CreateSprite(i);
					}

					RespawnClient(i, true, true);
					SetWeaponState(i, true);
					winnernumber += 1;
					PushArrayCell(ArrayWinners, i);

#if defined(ENABLE_SHILLINGS)
					if (SlagShillingsGive(i, 3))
					{
						CPrintToChat(i, "You were rewarded {green}3 Slag Shillings{default}!");
					}
#endif
				}
			}
		}
		
		for (int i = 0; i < GetArraySize(ArrayWinners); i++)
		{
			int client = GetArrayCell(ArrayWinners, i);

			if (winnernumber > 1)
			{
				if (i >= (GetArraySize(ArrayWinners) - 1))
				{
					Format(winnerstring_names, sizeof(winnerstring_names), "%s and {olive}%N{green}", winnerstring_names, client);
				}
				else
				{
					Format(winnerstring_names, sizeof(winnerstring_names), "%s, {olive}%N{green}", winnerstring_names, client);
				}
			}
			else
			{
				Format(winnerstring_names, sizeof(winnerstring_names), "{olive}%N{green}", client);
			}
		}

		if (winnernumber > 1)
		{
			ReplaceStringEx(winnerstring_names, sizeof(winnerstring_names), ", ", "");
		}

		/**
		 * TODO(irql):
		 * 
		 * - Migrate to the localized version
		 * - Reword the string to "The winners [is/are] %s with %i %s!"
		 */
		if (winnernumber == 1)
		{
			Format(winnerstring_prefix, sizeof(winnerstring_prefix), "{green}The winner is");
		}
		else
		{
			Format(winnerstring_prefix, sizeof(winnerstring_prefix), "{green}The winners are");
		}

		CPrintToChatAll("%s %s (%i %s)!", winnerstring_prefix, winnerstring_names, targetscore, pointsname);
		CloseHandle(ArrayWinners);

		if (SpecialRound != NONE)
		{
			CPrintToChatAll("The {lightgreen}Special Round{default} is over!");
			ResetSpecialRoundEffect(SpecialRound);
			SpecialRound = NONE;
			ShowGameText("Special Round is over!");
		}

		UpdateHud(GetSpeedMultiplier(MUSIC_INFO_LEN));
	}

	return Plugin_Stop;
}

public Action Classic_EndMap(Handle hTimer)
{
	SetConVarFloat(ww_speed, 1.0);

	status = 0;
	currentSpeed = GetConVarFloat(ww_speed);
	ResetScores();
	SetStateAll(false);
	ResetWinners();
	g_waiting = true;
	RoundStarts = 0;
	g_MinigamesTotal = 0;

	ServerCommand("host_timescale %f", 1.0);
	ServerCommand("phys_timescale %f", 1.0);

	ResetConVar(FindConVar("mp_respawnwavetime"));
	ResetConVar(FindConVar("mp_forcecamera"));

	ResetConVar(ConVar_MPFriendlyFire);
	ResetConVar(ConVar_TFAirblastCray);
	ResetConVar(ConVar_TFBotDifficulty);
	ResetConVar(ConVar_TFPlayerMovementRestartFreeze);
	ResetConVar(ConVar_TFTournamentHideDominationIcons);

	int entity = FindEntityByClassname(-1, "game_end");
	if (entity == -1 && (entity = CreateEntityByName("game_end")) == -1)
	{
		char map[PLATFORM_MAX_PATH];
		if (!GetNextMap(map, PLATFORM_MAX_PATH))
		{
			PrintToServer("[Classic_EndMap] GetNextMap returned false, cannot switch map!");
			return Plugin_Stop;
		}

		ForceChangeLevel(map, "TF2Ware Classic has ended due to map timer.");
	}
	else
	{
		AcceptEntityInput(entity, "EndGame");
	}

	return Plugin_Stop;
}

stock bool ShouldDoSpecialRound()
{
	if (SpecialRound != NONE)
	{
		return false;
	}

	if (GetConVarBool(ww_special))
	{
		/**
		 * Forced by an admin.
		 */
		return true;
	}

	if (SpecialRoundCooldown > 0)
	{
		/**
		 * Don't roll a special round at the start.
		 * 
		 * Or if we've played a special round already.
		 */
		return false;
	}

	int rand = MalletGetRandomInt(0, 100);
	return 33 <= rand <= 66;
}

public Action RestartAll_Timer(Handle hTimer)
{
	if (status == 5)
	{
		bossBattle = 0;

		// Set the game speed
		if (SpecialRound == SUPER_SPEED)
		{
			SetConVarFloat(ww_speed, 3.0);
		}
		else
		{
			SetConVarFloat(ww_speed, 1.0);
		}

		if (SpecialRound != NONE)
		{
			AddSpecialRoundEffect(SpecialRound);
		}

		currentSpeed = GetConVarFloat(ww_speed);
		ResetScores();
		SetStateAll(false);
		ResetWinners();
		g_MinigamesTotal = 0;

		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i))
			{
				DisableClientWeapons(i);
			}
		}

		if (ShouldDoSpecialRound())
		{
			SpecialRoundCooldown = 3;
			status = 6;
			StartSpecialRound();
		}
		else
		{
			status = 0;
			SetGameMode();
			ResetScores();
			StartMinigame();
		}
	}

	return Plugin_Stop;
}

int var_SpecialRoundRoll  = 0;
int var_SpecialRoundCount = 0;

public void StartSpecialRound()
{
	if (status == 6)
	{
		RespawnAll();
		SetConVarBool(ww_special, false);
		DestroyAllSprites();
		
		if (GetConVarInt(ww_force_special) <= 0)
		{
			do
			{
				SpecialRound = view_as<SpecialRounds>(GetRandomInt(1, SPECIAL_TOTAL));

				/**
				 * Require at least six players to enable Wipeout
				 * as a valid special round.
				 */
				if (SpecialRound == WIPEOUT && GetClientCount() < 6)
				{
					continue;
				}
				else
				{
					break;
				}
			}
			while (true);
		}
		else
		{
			SpecialRound = view_as<SpecialRounds>(GetConVarInt(ww_force_special));
		}

		if (GetConVarBool(ww_music))
		{
			EmitSoundToClient(1, MUSIC_SPECIAL);
		}
		else
		{
			EmitSoundToAll(MUSIC_SPECIAL);
		}

		status = 5;
		CreateTimer(0.1, SpecialRound_Timer);

		var_SpecialRoundCount = 130;

		CreateTimer(GetSpeedMultiplier(MUSIC_SPECIAL_LEN), RestartAll_Timer);

		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && (!(IsFakeClient(i))))
			{
				SetOverlay(i, "");
			}
		}
	}
}

public Action SpecialRound_Timer(Handle hTimer)
{
	if (status == 5 && var_SpecialRoundCount > 0)
	{
		CreateTimer(0.0, SpecialRound_Timer);

		var_SpecialRoundCount -= 1;
		var_SpecialRoundRoll += 1;

		if (var_SpecialRoundRoll > sizeof(var_special_name) + 1)
		{
			var_SpecialRoundRoll = 0;
		}
		
		char Name[128];
		if (var_SpecialRoundRoll < sizeof(var_special_name))
		{
			Format(Name, sizeof(Name), var_special_name[var_SpecialRoundRoll]);
		}
		else
		{
			static char var_funny_names[][] = { "FAT LARD RUN", "MOUSTACHIO", "LOVE STORY", "SIZE MATTERS", "ENGINERD", "IDLE FOR HATS", "TF2 BROS: BRAWL", "HOT SPY ON ICE" };
			Format(Name, sizeof(Name), var_funny_names[GetRandomInt(0, sizeof(var_funny_names) - 1)]);
		}

		if (var_SpecialRoundCount > 0)
		{
			char Text[128];
			Format(Text, sizeof(Text), "SPECIAL ROUND: %s?\nSpecial Round adds a new condition to the next round!", Name);
			ShowGameText(Text, "leaderboard_dominated", 1.0);
		}

		if (var_SpecialRoundCount == 0)
		{
			if (GetConVarBool(ww_music))
			{
				EmitSoundToClient(1, SOUND_SELECT);
			}
			else
			{
				EmitSoundToAll(SOUND_SELECT);
			}

			GiveSpecialRoundInfo(-1);
		}
	}

	return Plugin_Stop;
}

void GiveSpecialRoundInfo(int client)
{
	if (SpecialRound != NONE)
	{
		if (client == -1)
		{
			/**
			 * I don't think we can use localization on game_text_tf,
			 * for now use the English text.
			 */
			char Desc[128];
			Format(Desc, sizeof (Desc), "%T", var_special_phrases[view_as<int>(SpecialRound) - 1], LANG_SERVER);

			char Text[128];
			Format(Text, sizeof(Text), "SPECIAL ROUND: %s!\n%s",
				var_special_name[view_as<int>(SpecialRound) - 1], Desc);
			ShowGameText(Text, "leaderboard_dominated");

			/**
			 * Also print to the players chat in case they have a very special hud.
			 * 
			 * This variant now supports translations meaning that the description
			 * and "SPECIAL ROUND: " text can be translated, while keeping the special
			 * round name in English.
			 */
			for (int i = 1; i <= MaxClients; i++)
			{
			if (IsClientInGame(i))
				{
					char desc[64];
					Format(desc, sizeof (desc), "%T", var_special_phrases[view_as<int>(SpecialRound) - 1], i);

					CPrintToChat(i, "{olive}%T{default}", "SpecialRound", i, var_special_name[view_as<int>(SpecialRound) - 1], desc);
				}
			}
		}
		else
		{
			/**
			 * Player likely just joined or came out of spectator,
			 * show them what they are playing.
			 */
			char desc[64];
			Format(desc, sizeof (desc), "%T", var_special_phrases[view_as<int>(SpecialRound) - 1], client);

			CPrintToChat(client, "{olive}%T{default}", "SpecialRound", client, var_special_name[view_as<int>(SpecialRound) - 1], desc);
		}
	}
}

public Action Command_Event(int client, int args)
{
	status = 6;
	StartSpecialRound();
	SetConVarBool(ww_enable, true);

	return Plugin_Handled;
}

void SetStateAll(bool value)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		g_Complete[i] = value;
	}
}

void SetMissionAll(int value)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		g_Mission[i] = value;
	}
}

void SetClientSlot(int client, int slot)
{
	if (!IsValidClient(client) || !IsPlayerAlive(client))
	{
#if defined(DEBUG)
		LogMessage("Rejecting client slot for %i", client);
#endif

		return;
	}

#if defined(DEBUG)
	LogMessage("[TF2Ware::SetClientSlot] Setting client slot for %i", client);
#endif

	int weapon = GetPlayerWeaponSlot(client, slot);
	SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon", weapon);

#if defined(DEBUG)
	LogMessage("[TF2Ware::SetClientSlot] Set client slot");
#endif
}

void RespawnAll(bool force = false, bool savepos = true)
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::RespawnAll] Respawning everyone");
#endif

	for (int i = 1; i <= MaxClients; i++)
	{
		RespawnClient(i, force, savepos);
	}
}

void RespawnClient(int i, bool force = false, bool savepos = true)
{
	float pos[3];
	float vel[3];
	float ang[3];

	bool alive = false;
	if (IsValidClient(i) && IsValidTeam(i) && (g_Spawned[i] == true))
	{
		bool force2 = false;
		if (!IsPlayerAlive(i))
		{
			force2 = true;
		}

		if (force && IsClientParticipating(i))
		{
			force2 = true;
		}

		if (SpecialRound == WIPEOUT && g_Points[i] <= 0)
		{
			force2 = false;
		}

		if (force2)
		{
			alive = false;
			if (savepos)
			{
				GetClientAbsOrigin(i, pos);
				GetClientEyeAngles(i, ang);
				GetEntPropVector(i, Prop_Data, "m_vecVelocity", vel);
				if (IsPlayerAlive(i)) alive = true;
			}

			TF2_RespawnPlayer(i);

			if (savepos && alive)
			{
				TeleportEntity(i, pos, ang, vel);
			}
		}

		TF2_RemovePlayerDisguise(i);
	}
}

#if 0
int ScoreGreaterThan(int left, int right, const int[] playerids, Handle data)
{
	if (g_Points[left] < g_Points[right])
	{
		return -1;
	}
	else if (g_Points[left] == g_Points[right])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void DrawScoresheet()
{
	int players[MAXPLAYERS];

	for (int i = 1; i <= GetClientCount(); i++)
	{
		players[i] = i;
	}

	SortCustom1D(players, GetClientCount(), ScoreGreaterThan);
	
	char cName[128];
	char Lines[10][128];
	char Sheet[512];
	int count = 0;

	for (int i = GetClientCount(); i > 0; i--)
	{
		if (count >= 10)
		{
			break;
		}

		if (IsValidClient(i) && !IsClientObserver(i))
		{
			GetClientName(players[i], cName, sizeof(cName));
			Format(Lines[count], 128, "%2d - %s", g_Points[players[i]], cName);
			count++;
		}
	}

	ImplodeStrings(Lines, 10, "\n", Sheet, 512);
	SetHudTextParams(0.30, 0.30, 5.0, 255, 255, 255, 0);

	for (int i = 1; i <= GetClientCount(); i++)
	{
		if (IsValidClient(i))
		{
			ShowHudText(i, 7, Sheet);
		}
	}
}
#endif

void SetStateClient(int client, bool value, bool complete = false)
{
	if (IsValidClient(client) && IsClientParticipating(client))
	{
		if (complete && g_Complete[client] != value)
		{
			if (value)
			{
				EmitSoundToClient(client, SOUND_COMPLETE);

				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsValidClient(i) && !IsFakeClient(i))
					{
						EmitSoundToClient(i, SOUND_COMPLETE_YOU, client);

						if (IsClientParticipating(i) && IsPlayerAlive(i) && SpecialRound == WIPEOUT && i != client)
						{
							SetStateClient(i, false, true);
							ForcePlayerSuicide(i);
							CPrintToChatEx(i, client, "{green}You were beaten by {teamcolor}%N{green}!", client);
						}
					}
				}

				char effect[128];

				if (GetClientTeam(client) == 2)
				{
					effect = PARTICLE_WIN_RED;
				}
				else
				{
					effect = PARTICLE_WIN_BLUE;
				}

				ClientParticle(client, effect, 8.0);
			}
		}
		
		g_Complete[client] = value;
	}
}

stock float GetSpeedMultiplier(float count)
{
	float divide = ((currentSpeed - 1.0) / 7.5) + 1.0;
	float speed  = count / divide;
	return speed;
}

stock float GetHostMultiplier(float count)
{
	float divide = ((currentSpeed - 1.0) / 7.5) + 1.0;
	float speed  = count* divide;
	return speed;
}

int GetSoundMultiplier()
{
	int speed = SNDPITCH_NORMAL + RoundFloat((currentSpeed - 1.0) * 10.0);
	return speed;
}

void HookAllCheatCommands()
{
	char name[64];
	Handle cvar;
	bool isCommand;
	int flags;

	cvar = FindFirstConCommand(name, sizeof(name), isCommand, flags);
	if (cvar == INVALID_HANDLE)
	{
		SetFailState("Could not load cvar list");
	}

	do
	{
		if (!isCommand || !(flags & FCVAR_CHEAT))
		{
			continue;
		}

		RegConsoleCmd(name, OnCheatCommand);
	}
	while (FindNextConCommand(cvar, name, sizeof(name), isCommand, flags));

	CloseHandle(cvar);
}

void UpdateClientCheatValue()
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::UpdateClientCheatValue] Updating client cheat value");
#endif

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && !IsFakeClient(client))
		{
			SendConVarValue(client, FindConVar("sv_cheats"), "1");
		}
	}
}

public void OnConVarChanged_SvCheats(Handle convar, const char[] oldValue, const char[] newValue)
{
	UpdateClientCheatValue();
}

public Action OnCheatCommand(int client, int args)
{
#if defined(DEBUG)
	LogMessage("[TF2Ware::OnCheatCommand] Client (%d) issued cheat command", client);
#endif

	if (GetConVarBool(ww_enable) && g_enabled)
	{
		char command[32];
		GetCmdArg(0, command, sizeof(command));

		char buf[64];

		for (int i = 0; i < sizeof(ALLOWED_CHEAT_COMMANDS); ++i)
		{
			strcopy(buf, 0, ALLOWED_CHEAT_COMMANDS[i]);

			if (StrEqual(buf, command, false) || GetConVarInt(FindConVar("sv_cheats")) == 1)
			{
				return Plugin_Continue;
			}
		}

		KickClient(client, "%T", "Kicked_CheatCommand");

		return Plugin_Handled;
	}

	return Plugin_Continue;
}

void SetOverlay(int client, char overlay[512])
{
	if (IsValidClient(client) && (!(IsFakeClient(client))))
	{
		char language[512];
		char input[512];
		// TRANSLATION
		Format(language, sizeof(language), "");

		if (StrEqual(overlay, ""))
		{
			Format(input, sizeof(input), "r_screenoverlay \"\"");
		}
		if (!(StrEqual(overlay, "")))
		{
			Format(input, sizeof(input), "r_screenoverlay \"%s%s%s\"", materialpath, language, overlay);
		}

		ClientCommand(client, input);
		g_ModifiedOverlay[client] = true;
	}
}

void UpdateHud(float time)
{
	bool newStyle = GetConVarBool(ww_score_style);

	char output[512];
	char add[5];
	char scorename[26];

	int colorR = 255;
	int colorG = 255;
	int colorB = 0;

	if (newStyle)
	{
		if (SpecialRound == WIPEOUT)
		{
			Format(scorename, sizeof(scorename), "lives");
		}
		else
		{
			Format(scorename, sizeof(scorename), "points");
		}
	}
	else
	{
		if (SpecialRound == WIPEOUT)
		{
			Format(scorename, sizeof(scorename), "Lives:");
		}
		else
		{
			Format(scorename, sizeof(scorename), "Points:");
		}
	}

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i))
		{
			Format(add, sizeof(add), "");

			if (SpecialRound == WIPEOUT)
			{
				if (!g_Complete[i] && IsClientParticipating(i) && bossBattle != 1)
				{
					Format(add, sizeof(add), newStyle ? "(-1)" : "-1");
				}

				if (!g_Complete[i] && IsClientParticipating(i) && bossBattle == 1)
				{
					Format(add, sizeof(add), newStyle ? "(-5)" : "-5");
				}
			}
			else
			{
				if (g_Complete[i] && IsClientParticipating(i) && bossBattle != 1 && SpecialRound != THIRDPERSON)
				{
					Format(add, sizeof(add), newStyle ? "(+1)" : "+1");
				}

				if (g_Complete[i] && IsClientParticipating(i) && bossBattle == 1 && SpecialRound != THIRDPERSON)
				{
					Format(add, sizeof(add), newStyle ? "(+5)" : "+5");
				}
			}

			if (newStyle)
			{
				Format(output, sizeof(output), "%i %s %s", g_Points[i], scorename, add);
				SetHudTextParams(-1.0, 0.95, time, colorR, colorG, colorB, 0);
			}
			else
			{
				Format(output, sizeof(output), "%s %i %s", scorename, g_Points[i], add);
				SetHudTextParams(0.3, 0.70, time, colorR, colorG, colorB, 0);
			}

			ShowSyncHudText(i, hudScore, output);
		}
	}
}

void ResetScores()
{
	for (int client = 1; client <= MaxClients; client++)
	{
		if (SpecialRound == WIPEOUT)
		{
			g_Points[client] = 3;
		}
		else
		{
			g_Points[client] = 0;
		}
	}
}

int GetHighestScore()
{
	int out = 0;

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && GetClientTeam(client) >= 2 && g_Points[client] > out)
		{
			out = g_Points[client];
		}
	}

	return out;
}

int GetLowestScore()
{
	int out = 99;

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && GetClientTeam(client) >= 2 && g_Points[client] < out)
		{
			out = g_Points[client];
		}
	}

	return out;
}

int GetAverageScore()
{
	int out	  = 0;
	int total = 0;

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && GetClientTeam(client) >= 2 && (g_Points[client] > 0))
		{
			out += g_Points[client];
			total += 1;
		}
	}

	if ((total > 0) && (out > 0))
	{
		out = out / total;
	}

	return out;
}

stock float GetAverageScoreFloat()
{
	int out		= 0;
	float out2	= 0.0;
	int total	= 0;

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && GetClientTeam(client) >= 2 && (g_Points[client] > 0))
		{
			out += g_Points[client];
			total += 1;
		}
	}

	if ((total > 0) && (out > 0))
	{
		out2 = float(out) / float(total);
	}

	return out2;
}

void ResetWinners()
{
	for (int client = 1; client <= MaxClients; client++)
	{
		g_Winner[client] = 0;
	}
}

public Action Command_Points(int client, int args)
{
	char clientName[128];
	GetClientName(client, clientName, sizeof (clientName));

	CPrintToChatAll("%T", "CheatCommandGive", LANG_SERVER, clientName);

	g_Points[client] += 20;
	g_Points[0] += 20;
	g_Points[1] += 20;

	return Plugin_Handled;
}

public Action Command_List(int client, int args)
{
	PrintToConsole(client, "Listing all registered minigames...");

	char output[128];
	
	for (int i = 0; i < sizeof(g_name); i++)
	{
		if (StrEqual(g_name[i], ""))
		{
			continue;
		}

		if (GetMinigameConfNum(g_name[i], "enable", 1))
		{
			Format(output, sizeof(output), " %2d - %s", GetMinigameConfNum(g_name[i], "id"), g_name[i]);
		}
		else
		{
			Format(output, sizeof(output), " %2d - %s (disabled)", GetMinigameConfNum(g_name[i], "id"), g_name[i]);
		}

		PrintToConsole(client, output);
	}

	return Plugin_Handled;
}

stock void RemoveNotifyFlag(char name[128])
{
	Handle cv1	= FindConVar(name);
	int flags	= GetConVarFlags(cv1);
	flags &= ~FCVAR_REPLICATED;
	flags &= ~FCVAR_NOTIFY;
	SetConVarFlags(cv1, flags);
}

void InitMinigame()
{
	DispatchOnMicrogameStart();

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && IsClientParticipating(client))
		{
			DispatchOnClientJustEntered(client);
		}
	}
}

public Action Player_Say(Handle event, const char[] name, bool dontBroadcast)
{
	if (!GetConVarBool(ww_enable))
	{
		return Plugin_Continue;
	}

	char message[256];
	int client = GetClientOfUserId(GetEventInt(event, "userid"));
	GetEventString(event, "text", message, sizeof (message));

	return DispatchOnPlayerChatSay(client, message);
}

public void Player_Spawn(Handle event, const char[] name, bool dontBroadcast)
{
	if (!GetConVarBool(ww_enable))
		return;

	int client = GetClientOfUserId(GetEventInt(event, "userid"));

	/**
	 * Not needed in this gamemode.
	 */
	TF2_RemoveCondition(client, TFCond_SpawnOutline);
}

public void Player_Death(Handle event, const char[] name, bool dontBroadcast)
{
	if (!GetConVarBool(ww_enable))
		return;

	int client = GetClientOfUserId(GetEventInt(event, "userid"));

	DestroySprite(client);

	if (status == 2)
	{
		if (IsValidClient(client) && IsClientParticipating(client))
		{
			DispatchOnClientDeath(client);
		}
	}

#if defined(ENABLE_ATTACHMENTS)
	RemoveFakeWeapon(client);
#endif
}

// Some convenience functions for parsing the configuration file more simply.
void GotoGameConf(char[] game)
{
	if (!KvJumpToKey(MinigameConf, game))
	{
		PrintToServer("ERROR: Couldn't find requested iMinigame %s in configuration file!", game);
		KvRewind(MinigameConf);
	}
}

#if 0
// This is never used... yet. No need for it for now.
void GetMinigameConfStr(char[] game, const char[] key, char[] buffer, int size)
{
	GotoGameConf(game);
	KvGetString(MinigameConf, key, buffer, size);
	KvGoBack(MinigameConf);
}
#endif

float GetMinigameConfFloat(char[] game, const char[] key, float def = 4.0)
{
	GotoGameConf(game);
	float value = KvGetFloat(MinigameConf, key, def);
	KvGoBack(MinigameConf);

	return value;
}

int GetMinigameConfNum(char[] game, const char[] key, int def = 0)
{
	GotoGameConf(game);
	int value = KvGetNum(MinigameConf, key, def);
	KvGoBack(MinigameConf);

	return value;
}

stock bool IsClientParticipating(int iClient)
{
	return g_Participating[iClient];
}

void SetGameMode()
{
	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && IsPlayerAlive(client))
		{
			SetWipeoutPosition(client, (SpecialRound == WIPEOUT));
		}
	}
}

void RemoveAllParticipants()
{
	for (int client = 1; client <= MaxClients; client++)
	{
		g_Participating[client] = false;
	}
}

int GetLeftWipeoutPlayers()
{
	int out = 0;

	for (int client = 1; client <= MaxClients; client++)
	{
		if (IsValidClient(client) && GetClientTeam(client) >= 2 && g_Points[client] > 0)
		{
			out++;
		}
	}

	return out;
}

void SetWipeoutPosition(int iClient, bool bState = false)
{
	float fPos[3];
	GetClientAbsOrigin(iClient, fPos);

	if (bState)
	{
		fPos[2] = GAMEMODE_WIPEOUT_HEIGHT;
	}
	else
	{
		fPos[2] = -70.0;
	}

	TeleportEntity(iClient, fPos, NULL_VECTOR, NULL_VECTOR);
}

public Action Timer_HandleWOLives(Handle hTimer, any iClient)
{
	HandleWipeoutLives(iClient);
	return Plugin_Stop;
}

void HandleWipeoutLives(int iClient, bool bMessage = false)
{
	if (SpecialRound == WIPEOUT && IsValidClient(iClient) && IsPlayerAlive(iClient) && g_Points[iClient] <= 0)
	{
		if (bMessage)
		{
			if (g_Points[iClient] == 0)
			{
				CPrintToChatAllEx(iClient, "{teamcolor}%N{olive} has been {green}wiped out!", iClient);
			}

			if (g_Points[iClient] < 0)
			{
				CPrintToChat(iClient, "{default}Please wait, the current {olive}Wipeout round{default} needs to finish before you can join.");
			}
		}

		ForcePlayerSuicide(iClient);
		CreateTimer(0.2, Timer_HandleWOLives, iClient);
	}
}

public Action TF2_CalcIsAttackCritical(int iClient, int iWeapon, char[] StrWeapon, bool &bCrit)
{
	if (g_enabled && GetConVarBool(ww_enable))
	{
		bCrit = false;
		return Plugin_Changed;
	}
	else
	{
		return Plugin_Continue;
	}
}

stock bool IsValidTeam(int iClient)
{
	int iTeam = GetClientTeam(iClient);
	return view_as<TFTeam>(iTeam) == TFTeam_Blue || view_as<TFTeam>(iTeam) == TFTeam_Red;
}