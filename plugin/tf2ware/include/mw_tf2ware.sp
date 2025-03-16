#pragma semicolon 1

// rake: I haven't tested if the Attachments API still works.
//#define ENABLE_ATTACHMENTS  1

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
#endif

// Fixes
#include "colors.inc"

#if defined ENABLE_ATTACHMENTS
	#include <attachables>
#endif

/**
 * Provides defines and core enums.
 */
#include "tf2ware/tf2ware_core.inc"

char g_name[MAX_MINIGAMES][24];

// Language strings
char var_lang[][] = { "", "it/" };

// Handles
Handle ww_enable = INVALID_HANDLE;
Handle ww_speed = INVALID_HANDLE;
Handle ww_music = INVALID_HANDLE;
Handle ww_force = INVALID_HANDLE;
Handle ww_log = INVALID_HANDLE;
Handle ww_special = INVALID_HANDLE;
Handle ww_gamemode = INVALID_HANDLE;
Handle ww_force_special = INVALID_HANDLE;
Handle ww_overhead_scores = INVALID_HANDLE;
Handle ww_kamikaze_style = INVALID_HANDLE;
Handle ww_allowedCommands = INVALID_HANDLE;
Handle hudScore = INVALID_HANDLE;
// REPLACE WEAPON
Handle microgametimer = INVALID_HANDLE;

// Keyvalues configuration handle
new Handle:MinigameConf		  = INVALID_HANDLE;

// Bools
bool g_Complete[MAXPLAYERS + 1];
bool g_Spawned[MAXPLAYERS + 1];
bool g_ModifiedOverlay[MAXPLAYERS + 1];
bool g_attack	 = false;
bool g_enabled = false;
bool g_first	 = false;
bool g_waiting = true;
bool g_AlwaysShowPoints = false;

// Ints
int g_Mission[MAXPLAYERS + 1];
int g_NeedleDelay[MAXPLAYERS + 1];
int g_Points[MAXPLAYERS + 1];
int g_Id[MAXPLAYERS + 1];
int g_Winner[MAXPLAYERS + 1];
int g_Minipoints[MAXPLAYERS + 1];
int g_Country[MAXPLAYERS + 1];
int g_Sprites[MAXPLAYERS+1];
float currentSpeed;
int iMinigame;
int status;
int randommini;
int g_offsCollisionGroup;
int timeleft = 8;
int white;
int g_HaloSprite;
int g_ExplosionSprite;
int g_result = 0;
char g_mathquestion[24];
int g_bomb								   = 0;
int Roundstarts							   = 0;
int g_lastminigame						   = 0;
int g_lastboss							   = 0;
int g_minigamestotal					   = 0;
int bossBattle							   = 0;
bool g_Participating[MAXPLAYERS + 1] = false;
int g_Gamemode							   = 0;
int gVelocityOffset = -1;

// Strings
char materialpath[512]			   = "tf2ware/";
// Name of current minigame being played
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

#if 0
#include "tf2ware/microgames/math.inc"
#include "tf2ware/microgames/hopscotch.inc"
#endif

#include "tf2ware/microgames/sawrun.inc"
#include "tf2ware/microgames/simonsays.inc"
#include "tf2ware/microgames/movement.inc"
#include "tf2ware/microgames/snipertarget.inc"
#include "tf2ware/microgames/bball.inc"
#include "tf2ware/microgames/airraid.inc"

#if 0
#include "tf2ware/microgames/hugging.inc"
#include "tf2ware/microgames/redfloor.inc"
#include "tf2ware/microgames/jumprope.inc"
#include "tf2ware/microgames/frogger.inc"
#include "tf2ware/microgames/goomba.inc"
#include "tf2ware/microgames/ghostbusters.inc"
#endif

#include "tf2ware/mw_tf2ware_features.inc"
#include "tf2ware/overhead_scores.inc"
#include "tf2ware/special.inc"
#include "tf2ware/vocalize.inc"

public Plugin myinfo =
{
	name		= "TF2Ware Classic",
	author		= "Mecha the Slag, IRQL_NOT_LESS_OR_EQUAL",
	description = "Wario Ware in Team Fortress 2!",
	version		= PLUGIN_VERSION,
	url			= "https://github.com/irql-notlessorequal/tf2ware-classic"
};

public void OnPluginStart()
{
	// G A M E  C H E C K //
	decl String:game[32];
	GetGameFolderName(game, sizeof(game));
	if (!(StrEqual(game, "tf"))) SetFailState("This plugin is only for Team Fortress 2, not %s", game);

#if defined FIXED_IP
	new iIp = GetConVarInt(FindConVar("hostip"));
	if (FIXED_IP != iIp) SetFailState("This server does not have credidentals to run this plugin. Please contact the TF2Ware staff.");
#endif

	// Check for SDKHooks
	if (GetExtensionFileStatus("sdkhooks.ext") < 1)
		SetFailState("SDK Hooks is not loaded.");

#if !defined(ENABLE_MALLET)
	if (!MimalletInitWearables())
	{
		SetFailState("MimalletInitWearables returned FALSE.");
	}
#endif

	// Find collision group offsets
	g_offsCollisionGroup = FindSendPropOffs("CBaseEntity", "m_CollisionGroup");
	if (g_offsCollisionGroup == -1)
	{
		PrintToServer("* FATAL ERROR: Failed to get offset for CBaseEntity::m_CollisionGroup");
	}

	gVelocityOffset = FindSendPropInfo("CBasePlayer", "m_vecVelocity[0]");
	if (gVelocityOffset == -1)
	{
		PrintToServer("* FATAL ERROR: Failed to get offset for CBaseEntity::m_vecVelocity[0]");
	}

	// ConVars
	ww_enable		 = CreateConVar("ww_enable", "0", "Enables/Disables TF2Ware.", FCVAR_PLUGIN);
	ww_force		 = CreateConVar("ww_force", "0", "Force a certain minigame (0 to not force).", FCVAR_PLUGIN);
	ww_speed		 = CreateConVar("ww_speed", "1.0", "Speed level.", FCVAR_PLUGIN);
	ww_music		 = CreateConVar("ww_music_fix", "0", "Apply music fix? Should only be on for localhosts during testing", FCVAR_PLUGIN);
	ww_log			 = CreateConVar("ww_log", "0", "Log server events?", FCVAR_PLUGIN);
	ww_special		 = CreateConVar("ww_special", "0", "Next round is Special Round?", FCVAR_PLUGIN);
	ww_gamemode		 = CreateConVar("ww_gamemode", "-1", "Gamemode", FCVAR_PLUGIN);
	ww_force_special = CreateConVar("ww_force_special", "0", "Forces a specific Special Round on Special Round", FCVAR_PLUGIN);
	ww_overhead_scores = CreateConVar("ww_overhead_scores", "0", "Re-enables overhead scores, a feature that was long removed.", FCVAR_PLUGIN, true, 0.0, true, 1.0);
	ww_kamikaze_style = CreateConVar("ww_kamikaze_style", "0", "Picks the bomb model logic for Kamikaze. (0 = Use the Payload cart [default], 1 = Use the old Bo-Bomb model)", FCVAR_PLUGIN, true, 0.0, true, 1.0);

	// MINIGAME REGISTRATION
	AddMiniGame(MG_AIRBLAST, new Airblast());
	AddMiniGame(MG_AIR_RAID, new AirRaid());
	AddMiniGame(MG_BBALL, new BBall());
	AddMiniGame(MG_BARREL, new Barrel());
	AddMiniGame(MG_COLOR_TEXT, new ColorText());
	AddMiniGame(MG_FLOOD, new Flood());
	AddMiniGame(MG_HIT_ENEMY, new HitEnemy());
	AddMiniGame(MG_KAMIKAZE, new Kamikaze());
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
	if (StrEqual(map, "tf2ware"))
	{
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
			new i = 0;
			do
			{
				KvGetSectionName(MinigameConf, g_name[KvGetNum(MinigameConf, "id") - 1], 32);
				i++;
			}
			while (KvGotoNextKey(MinigameConf));

			KvRewind(MinigameConf);
		}
		else {
			PrintToServer("Failed to load minigames.cfg!");
		}

		// Add logging
		if (GetConVarBool(ww_log))
		{
			LogMessage("//////////////////////////////////////////////////////");
			LogMessage("//                     TF2WARE LOG                  //");
			LogMessage("//////////////////////////////////////////////////////");
		}

		// Hooks
		HookConVarChange(ww_enable, StartMinigame_cvar);
		HookConVarChange(ww_overhead_scores, OverheadScoresChanged);
		HookEvent("post_inventory_application", EventInventoryApplication, EventHookMode_Post);
		HookEvent("player_death", Player_Death, EventHookMode_Post);
		HookEvent("player_team", Player_Team, EventHookMode_Post);
		HookEvent("teamplay_round_start", Event_Roundstart, EventHookMode_PostNoCopy);
		HookEvent("teamplay_game_over", Event_Roundend, EventHookMode_PostNoCopy);
		HookEvent("teamplay_round_stalemate", Event_Roundend, EventHookMode_PostNoCopy);
		HookEvent("teamplay_round_win", Event_Roundend, EventHookMode_PostNoCopy);
		RegAdminCmd("ww_list", Command_list, ADMFLAG_GENERIC, "Lists all the registered, enabled plugins and their ids");
		RegAdminCmd("ww_give", Command_points, ADMFLAG_GENERIC, "Gives you 20 points - You're a winner! (testing feature)");
		RegAdminCmd("ww_event", Command_event, ADMFLAG_GENERIC, "Starts a debugging event");

		// Vars
		currentSpeed = GetConVarFloat(ww_speed);
		iMinigame	 = 1;
		status		 = 0;
		randommini	 = 0;
		Roundstarts	 = 0;
		SetStateAll(false);
		ResetWinners();
		SetMissionAll(0);

		// CHEATS
		HookConVarChange(FindConVar("sv_cheats"), OnConVarChanged_SvCheats);
		ww_allowedCommands = CreateArray(64);
		PushArrayString(ww_allowedCommands, "host_timescale");
		PushArrayString(ww_allowedCommands, "r_screenoverlay");
		PushArrayString(ww_allowedCommands, "thirdperson");
		PushArrayString(ww_allowedCommands, "firstperson");
		PushArrayString(ww_allowedCommands, "sv_cheats");
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
		SetConVarInt(FindConVar("tf_tournament_hide_domination_icons"), 0, true);
		SetConVarInt(FindConVar("mp_friendlyfire"), 1);

		if (GetConVarBool(ww_log)) LogMessage("Calling OnMapStart Forward");

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

		decl String:input[512];

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
		decl id;
		decl enable;
		new i = 1;
		if (GetConVarBool(ww_log)) LogMessage("--Adding the following to downloads table from information in minigames.cfg:", input);
		do
		{
			id	   = KvGetNum(MinigameConf, "id");
			enable = KvGetNum(MinigameConf, "enable", 1);
			if (enable >= 1)
			{
				Format(input, sizeof(input), "imgay/tf2ware/minigame_%d.mp3", id);
				if (GetConVarBool(ww_log)) LogMessage("%s", input);
				precacheSound(input);
			}
			i++;
		}
		while (KvGotoNextKey(MinigameConf));
		KvRewind(MinigameConf);

		white			  = PrecacheModel("materials/sprites/white.vmt");
		g_HaloSprite	  = PrecacheModel("materials/sprites/halo01.vmt");
		g_ExplosionSprite = PrecacheModel("sprites/sprite_fire01.vmt");

		PrecacheSound("ambient/explosions/explode_8.wav", true);
		SetConVarFloat(ww_speed, 1.0);
		ResetScores();
		bossBattle	= 0;
		Roundstarts = 0;

		SpecialPrecache();

		if (GetConVarBool(ww_log)) LogMessage("Map started");
	}
	else
	{
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
 * tl;dr I should really start working on "source.js"
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnClientJustEntered(client);
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnClientJustEntered(client);
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameStart();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameStart();
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameTimer(timeLeft);
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameTimer(timeLeft);
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameEnd();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameEnd();
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogamePostEnd();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogamePostEnd();
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnMicrogameFrame();
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnMicrogameFrame();
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

		case MG_HIT_ENEMY:
		{
			view_as<HitEnemy>(currentMicrogame).OnClientDeath(client);
		}

		case MG_KAMIKAZE:
		{
			view_as<Kamikaze>(currentMicrogame).OnClientDeath(client);
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

		case MG_HIT_ENEMY:
		{
			return view_as<HitEnemy>(mg).IsMicrogamePlayable(players);
		}

		case MG_KAMIKAZE:
		{
			return view_as<Kamikaze>(mg).IsMicrogamePlayable(players);
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

public Action:Timer_DisplayVersion(Handle:timer, any:client)
{
	if (IsValidClient(client))
	{
		SetHudTextParams(0.63, 0.73, 25.0, 255, 255, 255, 255, 1, 3.0, 0.0, 3.0);
		ShowHudText(client, 1, "c%s", PLUGIN_VERSION);
	}
	return Plugin_Handled;
}

public Action Event_Roundstart(Handle event, const char[] name, bool dontBroadcast)
{
	if (g_enabled && GetConVarBool(ww_enable))
	{
		if (Roundstarts == 0)
		{
			g_waiting = true;
			SetGameMode();
			RemoveAllParticipants();
		}

		if (Roundstarts == 1)
		{
			g_waiting = false;
			ClearPlayerFreeze();
			SetGameMode();
			ResetScores();
			StartMinigame();
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && g_Spawned[i])
				{
					if (!IsFakeClient(i))
					{
						StopSound(i, SND_CHANNEL_SPECIFIC, MUSIC_WAITING);
						SetOverlay(i, "");
					}
					if (g_Gamemode == GAMEMODE_WIPEOUT) SetWipeoutPosition(i, true);
				}
			}
			if (GetConVarBool(ww_log)) LogMessage("Waiting-for-players period has ended");
		}
	}

	Roundstarts++;
}

public Action Event_Roundend(Handle event, const char[] name, bool dontBroadcast)
{
	if (g_enabled && GetConVarBool(ww_enable))
	{
		g_enabled = false;
		if (GetConVarBool(ww_log)) LogMessage("== ROUND ENDED SUCCESSFULLY == ");
	}
}

public OnClientPostAdminCheck(client)
{
	if (!g_enabled) return;
	UpdateClientCheatValue();
	g_Points[client] = GetAverageScore();
	if (g_Gamemode == GAMEMODE_WIPEOUT) g_Points[client] = -1;

	// Country
	decl String:ip[32];
	GetClientIP(client, ip, sizeof(ip));
	decl String:country[3];
	GeoipCode2(ip, country);
	g_Country[client] = 0;

	if (GetConVarBool(ww_log)) LogMessage("Client post admin check. Country: %d", g_Country[client]);
}

public OnClientPutInServer(client)
{
	if (!g_enabled) return;
	if (GetConVarBool(ww_log)) LogMessage("Client put in server and hooked");
	SDKHook(client, SDKHook_PreThink, OnPreThink);
	SDKHook(client, SDKHook_OnTakeDamage, OnTakeDamageClient);
	SDKHook(client, SDKHook_Touch, Special_NoTouch);
	SDKHook(client, SDKHook_OnTakeDamage, Special_DamagePush);
}

public OnClientDisconnect(client)
{
	if (GetConVarBool(ww_log)) LogMessage("Client disconnected");

	g_Spawned[client] = false;
}

public Action:OnTakeDamageClient(victim, &attacker, &inflictor, &Float: damage, &damagetype)
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

public EventInventoryApplication(Handle:event, const String:name[], bool:dontBroadcast)
{
	if (GetConVarBool(ww_log)) LogMessage("Client post inventory");
	new client = GetClientOfUserId(GetEventInt(event, "userid"));
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

		if (g_Gamemode == GAMEMODE_WIPEOUT && g_waiting == false)
		{
			if (status == 2 && IsClientParticipating(client))
			{
				// do nothing
			}
			else {
				SetWipeoutPosition(client, true);
			}
			HandleWipeoutLives(client);
		}
	}
}

void precacheSound(char[] var0)
{
	char buffer[128];
	PrecacheSound(var0, true);
	Format(buffer, sizeof(buffer), "sound/%s", var0);
	AddFileToDownloadsTable(buffer);
}

public StartMinigame_cvar(Handle cvar, const char[] oldVal, const char[] newVal)
{
	if (GetConVarBool(ww_enable) && g_enabled)
	{
		StartMinigame();
		SetConVarInt(FindConVar("mp_respawnwavetime"), 199);
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

public OverheadScoresChanged(Handle:cvar, const String:oldVal[], const String:newVal[])
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
		return;


	if (status == 2)
	{
		DispatchOnMicrogameFrame();

		if (g_Gamemode == GAMEMODE_WIPEOUT && status == 1)
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
	 * - Handle `ww_force`
	 * - Do we still handle the "chance" value?
	 */
	Microgame candidate;
	int candidateIndex;

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

public Player_Team(Handle: event, const String: name[], bool: dontBroadcast)
{
	new client	= GetClientOfUserId(GetEventInt(event, "userid"));
	new oldteam = GetEventInt(event, "oldteam");
	new newteam = GetEventInt(event, "team");

	if (GetConVarBool(ww_log)) LogMessage("%N changed team", client);
	if (GetConVarBool(ww_enable) && g_enabled)
	{
		CreateTimer(0.1, StartMinigame_timer);
		if (oldteam < 2 && newteam >= 2)
		{
			GiveSpecialRoundInfo();
		}
	}
}

HandOutPoints()
{
	if (GetConVarBool(ww_log)) LogMessage("Handing out points");
	for (new i = 1; i <= MaxClients; i++)
	{
		new points = 1;
		if (bossBattle == 1) points = 5;
		if ((IsValidClient(i)) && IsClientParticipating(i))
		{
			if (g_Complete[i])
			{
				if (g_Gamemode == GAMEMODE_NORMAL) g_Points[i] += points;
			}
			else {
				if (g_Gamemode == GAMEMODE_WIPEOUT && g_Points[i] > 0)
				{
					g_Points[i] -= points;
					if (g_Points[i] < 0) g_Points[i] = 0;
					HandleWipeoutLives(i, true);
				}
			}
		}
		g_Complete[i] = false;
	}
}

StartMinigame()
{
	if (GetConVarBool(ww_enable) && g_enabled && (status == 0) && g_waiting == false)
	{
		if (GetConVarBool(ww_log)) LogMessage("Starting microgame %s! Status = 0", minigame);
		SetConVarInt(FindConVar("mp_respawnwavetime"), 199);
		SetConVarInt(FindConVar("mp_friendlyfire"), 1);

		float MUSIC_INFO_LEN;
		char MUSIC_INFO[PLATFORM_MAX_PATH];
		
		if (g_Gamemode == GAMEMODE_WIPEOUT)
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

		if (g_Gamemode == GAMEMODE_WIPEOUT)
		{
			// Get two people to fight it off
			new personA = GetRandomWipeoutPlayer();
			if (IsValidClient(personA)) g_Participating[personA] = true;
			new personB = GetRandomWipeoutPlayer();
			if (IsValidClient(personB)) g_Participating[personB] = true;

			new personC = -1;
			if (GetLeftWipeoutPlayers() > 4) personC = GetRandomWipeoutPlayer();
			if (IsValidClient(personC)) g_Participating[personC] = true;

			if (IsValidClient(personA) == false || IsValidClient(personB) == false)
			{
				status	   = 4;
				bossBattle = 2;
				CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Victory_timer);
				return;
			}
			decl String:strMessage[512];
			Format(strMessage, sizeof(strMessage), "%N\n%N", personA, personB);

			if (IsValidClient(personC)) Format(strMessage, sizeof(strMessage), "%s\n%N", strMessage, personC);
			PrintCenterTextAll(strMessage);
		}
		else
		{
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && GetClientTeam(i) >= 2 && g_Spawned[i] == true) g_Participating[i] = true;
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

		for (new i = 1; i <= MaxClients; i++)
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
		if (bossBattle == 1) g_lastboss = iMinigame;
		else g_lastminigame = iMinigame;
		CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Game_Start);

		g_attack = (SpecialRound == BONK);

		if (GetConVarBool(ww_overhead_scores))
		{
			CreateAllSprites();
		}
	}
}

public Action:Game_Start(Handle: hTimer)
{
	if (status == 1)
	{
		if (GetConVarBool(ww_log)) LogMessage("Microgame %s started! Status = 1", minigame);

		// Spawn everyone so they can participate
		RespawnAll();

		if (SpecialRound == SINGLEPLAYER) 
		{
			NoCollision(true);
		}

		if (SpecialRound == NO_TOUCHING)
		{
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && !IsFakeClient(i)) 
				{
					ClientCommand(i, "wait; thirdperson");
				}
			}
		}

		if (g_Gamemode == GAMEMODE_WIPEOUT)
		{
			for (new i2 = 1; i2 <= MaxClients; i2++)
			{
				if (IsValidClient(i2) && IsPlayerAlive(i2) && IsClientParticipating(i2))
				{
					SetEntityMoveType(i2, MoveType:MOVETYPE_WALK);
					SetWipeoutPosition(i2, false);
				}
			}
		}

		// Play the microgame's music
		char sound[512];
		Format(sound, sizeof(sound), "imgay/tf2ware/minigame_%d.mp3", iMinigame);
		if (StrEqual(minigame, "Ghostbusters") && GetRandomInt(1, 3) == 1)
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

		// Don't allow no points by default
        g_AlwaysShowPoints = false;

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

		// timeleft counter. Let it stay longer on boss battles.
		timeleft = 8;
		if (bossBattle == 1) CreateTimer(GetSpeedMultiplier(3.0), CountDown_Timer);
		else CreateTimer(GetSpeedMultiplier(1.0), CountDown_Timer);

		// get the lasting time from the cfg
		microgametimer = CreateTimer(GetSpeedMultiplier(GetMinigameConfFloat(minigame, "duration")), EndGame);

		// debug
		if (GetConVarBool(ww_log)) LogMessage("Microgame started post");
	}
	return Plugin_Stop;
}

PrintMissionText()
{
	if (GetConVarBool(ww_log)) LogMessage("Printing mission text");
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i))
		{
			new String:input[512];
			Format(input, sizeof(input), "tf2ware_minigame_%d_%d", iMinigame, g_Mission[i] + 1);
			SetOverlay(i, input);
			g_ModifiedOverlay[i] = false;
		}
	}
}

public Action CountDown_Timer(Handle hTimer)
{
	if ((status == 2) && (timeleft > 0))
	{
		timeleft = timeleft - 1;
		CreateTimer(GetSpeedMultiplier(0.4), CountDown_Timer);
		if (bossBattle != 1)
		{
			DispatchOnMicrogameTimer(timeleft);
		}
		if (timeleft == 2)
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && (!(IsFakeClient(i))) && g_ModifiedOverlay[i] == false)
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
	microgametimer = INVALID_HANDLE;
	if (status == 2)
	{
		if (GetConVarBool(ww_log)) LogMessage("Microgame %s, (id:%d) ended!", minigame, iMinigame);
		
		DispatchOnMicrogameEnd();

		g_AlwaysShowPoints = false;
		status = 0;

		float MUSIC_INFO_LEN;
		char MUSIC_INFO_WIN[PLATFORM_MAX_PATH];
		char MUSIC_INFO_FAIL[PLATFORM_MAX_PATH];

		if (g_Gamemode == GAMEMODE_WIPEOUT)
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
		 * Send a late end event here to maintain compatiblity.
		 */
		DispatchOnMicrogamePostEnd();

		CleanupAllVocalizations();

		currentSpeed = GetConVarFloat(ww_speed);
		ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
		ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));

		new String:sound[512];
		for (new i = 1; i <= MaxClients; i++)
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
				new String:oldsound[512];
				Format(oldsound, sizeof(oldsound), "imgay/tf2ware/minigame_%d.mp3", iMinigame);
				if (GetMinigameConfNum(minigame, "dynamic", 0)) StopSound(i, SND_CHANNEL_SPECIFIC, oldsound);
				EmitSoundToClient(i, sound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			}
		}

		for (new i = 1; i <= MaxClients; i++)
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
		if (g_Gamemode == GAMEMODE_WIPEOUT)
		{
			new bool:bSomeoneWon = false;
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && IsClientParticipating(i) && g_Complete[i] == true) bSomeoneWon = true;
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
		if (GetMinigameConfNum(minigame, "endrespawn", 0) > 0) RespawnAll(true, false);
		else RespawnAll();
		if (g_Gamemode == GAMEMODE_WIPEOUT)
		{
			for (new i2 = 1; i2 <= MaxClients; i2++)
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
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && !IsFakeClient(i)) ClientCommand(i, "wait; thirdperson");
			}
		}

		// RESPAWN END

		bool speedup = false;
		g_minigamestotal += 1;

		if (bossBattle == 1) bossBattle = 2;

		if (g_Gamemode == GAMEMODE_WIPEOUT)
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
		else {
			if ((g_minigamestotal == 4) && (bossBattle == 0)) speedup = true;
			if ((g_minigamestotal == 8) && (bossBattle == 0)) speedup = true;
			if ((g_minigamestotal == 12) && (bossBattle == 0)) speedup = true;
			if ((g_minigamestotal == 16) && (bossBattle == 0)) speedup = true;
			if ((g_minigamestotal == 19) && (bossBattle == 0))
			{
				speedup	   = true;
				bossBattle = 1;
			}
			if ((g_minigamestotal >= 19) && bossBattle == 2 && SpecialRound == DOUBLE_BOSS_BATTLE && Special_TwoBosses == false)
			{
				speedup			  = true;
				bossBattle		  = 1;
				Special_TwoBosses = true;
			}
		}
		if (g_Gamemode == GAMEMODE_WIPEOUT && GetLeftWipeoutPlayers() <= 1)
		{
			status = 4;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Victory_timer);
		}
		if (speedup == false)
		{
			status = 10;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), StartMinigame_timer2);
		}
		if (speedup == true)
		{
			status = 3;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Speedup_timer);
		}
		if (bossBattle == 2 && speedup == false)
		{
			status = 4;
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Victory_timer);
		}
	}
	return Plugin_Stop;
}

public Action:Speedup_timer(Handle:hTimer)
{
	if (status == 3)
	{
		RemoveAllParticipants();
		if (bossBattle == 1)
		{
			if (GetConVarBool(ww_log)) LogMessage("GETTING READY TO START SOME BOSS");
			
			float MUSIC_INFO_LEN;
			char MUSIC_INFO[PLATFORM_MAX_PATH];
			
			if (g_Gamemode == GAMEMODE_WIPEOUT)
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

			if (GetConVarBool(ww_log)) LogMessage("Boss part 2");

			// Set the Speed. If special round, we want it to be a tad faster ;)
			if (SpecialRound == SUPER_SPEED)
			{
				SetConVarFloat(ww_speed, 3.0);
			}
			else
			{
				SetConVarFloat(ww_speed, 1.0);
			}

			if (GetConVarBool(ww_log)) LogMessage("Boss part 3");

			currentSpeed = GetConVarFloat(ww_speed);
			ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
			ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));

			if (GetConVarBool(ww_log)) LogMessage("Boss part 4");

			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), StartMinigame_timer2);

			if (GetConVarBool(ww_log)) LogMessage("Boss part 5");

			if (GetConVarBool(ww_music)) EmitSoundToClient(1, MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			else EmitSoundToAll(MUSIC_INFO, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL, GetSoundMultiplier());
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsValidClient(i) && (!(IsFakeClient(i))))
				{
					SetOverlay(i, "tf2ware_minigame_boss");
				}
			}

			if (GetConVarBool(ww_log)) LogMessage("Boss part 6");

			UpdateHud(GetSpeedMultiplier(MUSIC_INFO_LEN));
		}

		if (GetConVarBool(ww_log)) LogMessage("Boss part 7");

		if (bossBattle != 1)
		{
			float MUSIC_INFO_LEN;
			char MUSIC_INFO[PLATFORM_MAX_PATH];

			if (g_Gamemode == GAMEMODE_WIPEOUT)
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

			for (new i = 1; i <= MaxClients; i++)
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

		if (GetConVarBool(ww_log)) LogMessage("Boss part 8");

		status = 10;

		if (GetConVarBool(ww_log)) LogMessage("Post boss");
	}
}

public Action:Victory_timer(Handle:hTimer)
{
	if ((status == 4) && (bossBattle > 0))
	{
		bossBattle = 0;
		SetConVarFloat(ww_speed, 1.0);
		currentSpeed = GetConVarFloat(ww_speed);

		float MUSIC_INFO_LEN;
		char MUSIC_INFO[PLATFORM_MAX_PATH];

		if (g_Gamemode == GAMEMODE_WIPEOUT)
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
			CreateTimer(GetSpeedMultiplier(MUSIC_INFO_LEN), Restartall_timer);
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

		new winnernumber		  = 0;
		new Handle:ArrayWinners = CreateArray();
		decl String:winnerstring_prefix[128];
		decl String:winnerstring_names[512];
		decl String:pointsname[512];
		Format(pointsname, sizeof(pointsname), "points");
		if (g_Gamemode == GAMEMODE_WIPEOUT) Format(pointsname, sizeof(pointsname), "lives");

		new bool:bAccepted = false;
		for (new i = 1; i <= MaxClients; i++)
		{
			SetOverlay(i, "");
			if (IsValidClient(i) && (GetClientTeam(i) == 2 || GetClientTeam(i) == 3))
			{
				bAccepted = false;
				if (g_Gamemode == GAMEMODE_WIPEOUT)
				{
					if (g_Points[i] > 0) bAccepted = true;
				}
				else {
					if (SpecialRound != LEAST_IS_BEST && g_Points[i] >= targetscore) bAccepted = true;
					if (SpecialRound == LEAST_IS_BEST && g_Points[i] <= targetscore) bAccepted = true;
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
#if defined ENABLE_SHILLINGS
					if (SlagShillingsGive(i, 3))
					{
						CPrintToChat(i, "You were rewarded {green}3 Slag Shillings{default}!");
					}
#endif
				}
			}
		}
		for (new i = 0; i < GetArraySize(ArrayWinners); i++)
		{
			new client = GetArrayCell(ArrayWinners, i);
			if (winnernumber > 1)
			{
				if (i >= (GetArraySize(ArrayWinners) - 1)) Format(winnerstring_names, sizeof(winnerstring_names), "%s and {olive}%N{green}", winnerstring_names, client);
				else Format(winnerstring_names, sizeof(winnerstring_names), "%s, {olive}%N{green}", winnerstring_names, client);
			}
			else Format(winnerstring_names, sizeof(winnerstring_names), "{olive}%N{green}", client);
		}
		if (winnernumber > 1) ReplaceStringEx(winnerstring_names, sizeof(winnerstring_names), ", ", "");

		if (winnernumber == 1) Format(winnerstring_prefix, sizeof(winnerstring_prefix), "{green}The winner is");
		else Format(winnerstring_prefix, sizeof(winnerstring_prefix), "{green}The winners are");

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
	Roundstarts = 0;
	g_minigamestotal = 0;

	RestorePlayerFreeze();

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

public Action Restartall_timer(Handle hTimer)
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
		g_minigamestotal = 0;

		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i)) DisableClientWeapons(i);
		}

		// Roll special round
		if ((GetRandomInt(0, 9) == 5 || GetConVarBool(ww_special)) && SpecialRound == NONE)
		{
			status = 6;
			StartSpecialRound();
		}
		else {
			status = 0;
			SetGameMode();
			ResetScores();
			StartMinigame();
		}
	}

	return Plugin_Stop;
}

new var_SpecialRoundRoll  = 0;
new var_SpecialRoundCount = 0;

public StartSpecialRound()
{
	if (status == 6)
	{
		RespawnAll();
		SetConVarBool(ww_special, false);
		if (GetConVarInt(ww_force_special) <= 0)
		{
			SpecialRound = view_as<SpecialRounds>(GetRandomInt(1, SPECIAL_TOTAL));
		}
		else
		{
			SpecialRound = view_as<SpecialRounds>(GetConVarInt(ww_force_special));
		}

		if (GetConVarBool(ww_music)) EmitSoundToClient(1, MUSIC_SPECIAL);
		else EmitSoundToAll(MUSIC_SPECIAL);

		status = 5;
		CreateTimer(0.1, SpecialRound_timer);

		var_SpecialRoundCount = 130;

		CreateTimer(GetSpeedMultiplier(MUSIC_SPECIAL_LEN), Restartall_timer);

		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && (!(IsFakeClient(i))))
			{
				SetOverlay(i, "");
			}
		}
	}
}

public Action:SpecialRound_timer(Handle: hTimer)
{
	if (status == 5 && var_SpecialRoundCount > 0)
	{
		CreateTimer(0.0, SpecialRound_timer);

		var_SpecialRoundCount -= 1;
		var_SpecialRoundRoll += 1;
		if (var_SpecialRoundRoll > sizeof(var_special_name) + 1) var_SpecialRoundRoll = 0;
		decl String:Name[128];
		if (var_SpecialRoundRoll < sizeof(var_special_name)) Format(Name, sizeof(Name), var_special_name[var_SpecialRoundRoll]);
		else {
			decl String:var_funny_names[][] = { "FAT LARD RUN", "MOUSTACHIO", "LOVE STORY", "SIZE MATTERS", "ENGINERD", "IDLE FOR HATS", "TF2 BROS: BRAWL", "HOT SPY ON ICE" };
			Format(Name, sizeof(Name), var_funny_names[GetRandomInt(0, sizeof(var_funny_names) - 1)]);
		}

		if (var_SpecialRoundCount > 0)
		{
			decl String:Text[128];
			Format(Text, sizeof(Text), "SPECIAL ROUND: %s?\nSpecial Round adds a new condition to the next round!", Name);
			ShowGameText(Text, "leaderboard_dominated", 1.0);
		}

		if (var_SpecialRoundCount == 0)
		{
			if (GetConVarBool(ww_music)) EmitSoundToClient(1, SOUND_SELECT);
			else EmitSoundToAll(SOUND_SELECT);
			GiveSpecialRoundInfo();
		}
	}
}

GiveSpecialRoundInfo()
{
	if (SpecialRound != NONE)
	{
		decl String:Text[128];
		Format(Text, sizeof(Text), "SPECIAL ROUND: %s!\n%s",
			var_special_name[SpecialRound - 1], var_special_desc[SpecialRound - 1]);
		ShowGameText(Text, "leaderboard_dominated");
	}
}

public Action:Command_event(client, args)
{
	status = 6;
	StartSpecialRound();
	SetConVarBool(ww_enable, true);

	return Plugin_Handled;
}

SetStateAll(bool: value)
{
	for (new i = 1; i <= MaxClients; i++)
	{
		g_Complete[i] = value;
	}
}

SetMissionAll(value)
{
	for (new i = 1; i <= MaxClients; i++)
	{
		g_Mission[i] = value;
	}
}

SetClientSlot(client, slot)
{
	if (!IsValidClient(client) || !IsPlayerAlive(client))
	{
		if (GetConVarBool(ww_log)) LogMessage("Rejecting client slot for %i", client);
		return;
	}
	if (GetConVarBool(ww_log)) LogMessage("Setting client slot for %i", client);
	new weapon = GetPlayerWeaponSlot(client, slot);
	SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon", weapon);
	if (GetConVarBool(ww_log)) LogMessage("Set client slot");
}

void RespawnAll(bool:force = false, bool:savepos = true)
{
	if (GetConVarBool(ww_log)) LogMessage("Respawning everyone");
	for (int i = 1; i <= MaxClients; i++)
	{
		RespawnClient(i, force, savepos);
	}
}

RespawnClient(any:i, bool:force	= false, bool:savepos = true)
{
	decl Float:pos[3];
	decl Float:vel[3];
	decl Float:ang[3];
	new alive = false;
	if (IsValidClient(i) && IsValidTeam(i) && (g_Spawned[i] == true))
	{
		new bool:force2 = false;
		if (!IsPlayerAlive(i)) force2 = true;
		if (force && IsClientParticipating(i)) force2 = true;
		if (g_Gamemode == GAMEMODE_WIPEOUT && g_Points[i] <= 0) force2 = false;
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
			if ((savepos) && (alive)) TeleportEntity(i, pos, ang, vel);
		}

		TF2_RemovePlayerDisguise(i);
	}
}

/* DrawScoresheet() {
	new players[GetClientCount()+1];
	for (new i=1; i<=GetClientCount(); i++) {
		players[i] = i;
	}
	SortCustom1D(players, GetClientCount(), ScoreGreaterThan);
	new String:cName[128];
	new String:Lines[10][128];
	new String:Sheet[512];
	new count = 0;
	for (new i=GetClientCount(); i>0; i--) {
		if (count >= 10) break;
		if (IsValidClient(i) && !IsClientObserver(i)) {
			GetClientName(players[i], cName, sizeof(cName));
			Format(Lines[count], 128, "%2d - %s", g_Points[players[i]], cName);
			count++;
		}
	}
	ImplodeStrings(Lines, 10, "\n", Sheet, 512);
	SetHudTextParams(0.30, 0.30, 5.0, 255, 255, 255, 0);
	for (new i=1; i<=GetClientCount(); i++) {
		if (IsValidClient(i)) {
			ShowHudText(i, 7, Sheet);
		}
	}
}

 */

SetStateClient(client, bool:value, bool:complete = false)
{
	if (IsValidClient(client) && IsClientParticipating(client))
	{
		if (complete && g_Complete[client] != value)
		{
			if (value)
			{
				EmitSoundToClient(client, SOUND_COMPLETE);
				for (new i = 1; i <= MaxClients; i++)
				{
					if (IsValidClient(i) && !IsFakeClient(i))
					{
						EmitSoundToClient(i, SOUND_COMPLETE_YOU, client);
						if (IsClientParticipating(i) && IsPlayerAlive(i) && g_Gamemode == GAMEMODE_WIPEOUT && i != client)
						{
							SetStateClient(i, false, true);
							ForcePlayerSuicide(i);
							CPrintToChatEx(i, client, "{green}You were beaten by {teamcolor}%N{green}!", client);
						}
					}
				}
				new String:effect[128] = PARTICLE_WIN_BLUE;
				if (GetClientTeam(client) == 2) effect = PARTICLE_WIN_RED;
				ClientParticle(client, effect, 8.0);
			}
		}
		g_Complete[client] = value;
	}
}

stock Float:GetSpeedMultiplier(Float:count)
{
	new Float:divide = ((currentSpeed - 1.0) / 7.5) + 1.0;
	new Float:speed  = count / divide;
	return speed;
}

stock Float:GetHostMultiplier(Float: count)
{
	new Float:divide = ((currentSpeed - 1.0) / 7.5) + 1.0;
	new Float:speed  = count* divide;
	return speed;
}

GetSoundMultiplier()
{
	new speed = SNDPITCH_NORMAL + RoundFloat((currentSpeed - 1.0) * 10.0);
	return speed;
}

HookAllCheatCommands()
{
	decl String:name[64];
	new Handle:cvar;
	new bool:isCommand;
	new flags;

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

UpdateClientCheatValue()
{
	if (GetConVarBool(ww_log)) LogMessage("Updating client cheat value");
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && (!(IsFakeClient(i))))
		{
			SendConVarValue(i, FindConVar("sv_cheats"), "1");
		}
	}
}

public OnConVarChanged_SvCheats(Handle:convar, const String:oldValue[], const String:newValue[])
{
	UpdateClientCheatValue();
}

public Action:OnCheatCommand(client, args)
{
	if (GetConVarBool(ww_log)) LogMessage("on cheat command");
	if (GetConVarBool(ww_enable) && g_enabled)
	{
		decl String:command[32];
		GetCmdArg(0, command, sizeof(command));

		decl String:buf[64];
		new size = GetArraySize(ww_allowedCommands);
		for (new i = 0; i < size; ++i)
		{
			GetArrayString(ww_allowedCommands, i, buf, sizeof(buf));

			if (StrEqual(buf, command, false) || GetConVarInt(FindConVar("sv_cheats")) == 1)
			{
				return Plugin_Continue;
			}
		}

		KickClient(client, "Attempted to use cheat command.");
		return Plugin_Handled;
	}

	return Plugin_Continue;
}

SetOverlay(i, String:overlay[512])
{
	if (IsValidClient(i) && (!(IsFakeClient(i))))
	{
		new String:language[512];
		new String:input[512];
		// TRANSLATION
		Format(language, sizeof(language), "");

		if (g_Country[i] > 0)
		{
			Format(language, sizeof(language), "/%s", var_lang[g_Country[i]]);
		}

		if (StrEqual(overlay, ""))
		{
			Format(input, sizeof(input), "r_screenoverlay \"\"");
		}
		if (!(StrEqual(overlay, "")))
		{
			Format(input, sizeof(input), "r_screenoverlay \"%s%s%s\"", materialpath, language, overlay);
		}
		ClientCommand(i, input);
		g_ModifiedOverlay[i] = true;
	}
}

UpdateHud(Float:time)
{
	decl String:output[512];
	decl String:add[5];
	decl String:scorename[26];
	new colorR = 255;
	new colorG = 255;
	new colorB = 0;
	Format(scorename, sizeof(scorename), "Points:");
	if (g_Gamemode == GAMEMODE_WIPEOUT && SpecialRound != THIRDPERSON)
	{
		Format(scorename, sizeof(scorename), "Lives:");
	}
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i))
		{
			Format(add, sizeof(add), "");
			if (g_Gamemode == GAMEMODE_WIPEOUT)
			{
				if (!g_Complete[i] && IsClientParticipating(i) && bossBattle != 1 && SpecialRound != THIRDPERSON)
				{
					Format(add, sizeof(add), "-1");
				}
				if (!g_Complete[i] && IsClientParticipating(i) && bossBattle == 1 && SpecialRound != THIRDPERSON)
				{
					Format(add, sizeof(add), "-5");
				}
			}
			else
			{
				if (g_Complete[i] && IsClientParticipating(i) && bossBattle != 1 && SpecialRound != THIRDPERSON)
				{
					Format(add, sizeof(add), "+1");
				}
				if (g_Complete[i] && IsClientParticipating(i) && bossBattle == 1 && SpecialRound != THIRDPERSON)
				{
					Format(add, sizeof(add), "+5");
				}
			}
			Format(output, sizeof(output), "%s %i %s", scorename, g_Points[i], add);
			SetHudTextParams(0.3, 0.70, time, colorR, colorG, colorB, 0);
			ShowSyncHudText(i, hudScore, output);
		}
	}
}

public SortPlayerTimes(elem1[], elem2[], const array[][], Handle: hndl)
{
	if (elem1[1] > elem2[1])
	{
		return -1;
	}
	else if (elem1[1] < elem2[1]) {
		return 1;
	}

	return 0;
}

ResetScores()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_Gamemode == GAMEMODE_WIPEOUT) g_Points[i] = 3;
		else g_Points[i] = 0;
	}
}

GetHighestScore()
{
	new out = 0;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) >= 2 && g_Points[i] > out) out = g_Points[i];
	}

	return out;
}

GetLowestScore()
{
	new out = 99;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) >= 2 && g_Points[i] < out) out = g_Points[i];
	}

	return out;
}

GetAverageScore()
{
	new out	  = 0;
	new total = 0;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) >= 2 && (g_Points[i] > 0))
		{
			out += g_Points[i];
			total += 1;
		}
	}

	if ((total > 0) && (out > 0)) out = out / total;

	return out;
}

stock Float:GetAverageScoreFloat()
{
	new out			 = 0;
	new Float:out2 = 0.0;
	new total		 = 0;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) >= 2 && (g_Points[i] > 0))
		{
			out += g_Points[i];
			total += 1;
		}
	}

	if ((total > 0) && (out > 0)) out2 = float(out) / float(total);

	return out2;
}

ResetWinners()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		g_Winner[i] = 0;
	}
}

public Action:Command_points(client, args)
{
	PrintToChatAll("Gave %N 20 points", client);
	g_Points[client] += 20;
	g_Points[0] += 20;
	g_Points[1] += 20;
	return Plugin_Handled;
}

public Action:Command_list(client, args)
{
	PrintToConsole(client, "Listing all registered minigames...");
	new String:output[128];
	for (new i = 0; i < sizeof(g_name); i++)
	{
		if (StrEqual(g_name[i], "")) continue;
		if (GetMinigameConfNum(g_name[i], "enable", 1))
			Format(output, sizeof(output), " %2d - %s", GetMinigameConfNum(g_name[i], "id"), g_name[i]);
		else
			Format(output, sizeof(output), " %2d - %s (disabled)", GetMinigameConfNum(g_name[i], "id"), g_name[i]);
		PrintToConsole(client, output);
	}
}

RemoveNotifyFlag(String:name[128])
{
	new Handle:cv1 = FindConVar(name);
	new flags		 = GetConVarFlags(cv1);
	flags &= ~FCVAR_REPLICATED;
	flags &= ~FCVAR_NOTIFY;
	SetConVarFlags(cv1, flags);
}

void InitMinigame()
{
	DispatchOnMicrogameStart();

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && IsClientParticipating(i))
		{
			DispatchOnClientJustEntered(i);
		}
	}
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

// This is never used... yet. No need for it for now.
/*GetMinigameConfStr(String:game[], String:key[], String:buffer, size) {
	GotoGameConf(game);
	KvGetString(MinigameConf, key, buffer, size);
	KvGoBack(MinigameConf);
}*/

Float:GetMinigameConfFloat(String:game[], String:key[], Float:def = 4.0)
{
	GotoGameConf(game);
	new Float:value = KvGetFloat(MinigameConf, key, def);
	KvGoBack(MinigameConf);
	return value;
}

GetMinigameConfNum(String:game[], String:key[], def = 0)
{
	GotoGameConf(game);
	new value = KvGetNum(MinigameConf, key, def);
	KvGoBack(MinigameConf);
	return value;
}

GetRandomWipeoutPlayer()
{
	new Handle:roll = CreateArray();
	new out			  = -1;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) >= 2 && IsClientParticipating(i) == false && g_Points[i] > 0)
		{
			PushArrayCell(roll, i);
		}
	}

	if (GetArraySize(roll) > 0) out = GetArrayCell(roll, GetRandomInt(0, GetArraySize(roll) - 1));
	CloseHandle(roll);

	return out;
}

stock bool:IsClientParticipating(iClient)
{
	if (g_Participating[iClient] == false) return false;
	return true;
}

SetGameMode()
{
	new iOld	  = g_Gamemode;
	new iGamemode = GetConVarInt(ww_gamemode);
	if (iGamemode >= 0) g_Gamemode = iGamemode;
	else {
		g_Gamemode = GAMEMODE_NORMAL;
		new iRoll  = GetRandomInt(0, 100);
		if (iRoll <= 5) g_Gamemode = GAMEMODE_WIPEOUT;
	}

	if (iOld == GAMEMODE_WIPEOUT && g_Gamemode != iOld)
	{
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i)) SetWipeoutPosition(i, false);
		}
	}
	if (g_Gamemode == GAMEMODE_WIPEOUT && g_Gamemode != iOld)
	{
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i) && IsPlayerAlive(i)) SetWipeoutPosition(i, true);
		}
	}
}

RemoveAllParticipants()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		g_Participating[i] = false;
	}
}

GetLeftWipeoutPlayers()
{
	new out = 0;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) >= 2 && g_Points[i] > 0) out++;
	}
	return out;
}

SetWipeoutPosition(iClient, bool:bState = false)
{
	new Float:fPos[3];
	GetClientAbsOrigin(iClient, fPos);
	if (bState) fPos[2] = GAMEMODE_WIPEOUT_HEIGHT;
	else fPos[2] = -70.0;
	TeleportEntity(iClient, fPos, NULL_VECTOR, NULL_VECTOR);
}

public Action:Timer_HandleWOLives(Handle:hTimer, any:iClient)
{
	HandleWipeoutLives(iClient);
}

HandleWipeoutLives(iClient, bMessage = false)
{
	if (g_Gamemode == GAMEMODE_WIPEOUT && IsValidClient(iClient) && IsPlayerAlive(iClient) && g_Points[iClient] <= 0)
	{
		if (bMessage)
		{
			if (g_Points[iClient] == 0) CPrintToChatAllEx(iClient, "{teamcolor}%N{olive} has been {green}wiped out!", iClient);
			if (g_Points[iClient] < 0) CPrintToChat(iClient, "{default}Please wait, the current {olive}Wipeout round{default} needs to finish before you can join.");
		}
		ForcePlayerSuicide(iClient);
		CreateTimer(0.2, Timer_HandleWOLives, iClient);
	}
}

public Action:TF2_CalcIsAttackCritical(iClient, iWeapon, String:StrWeapon[], &bool:bCrit)
{
	if (g_enabled && GetConVarBool(ww_enable))
	{
		bCrit = false;
		return Plugin_Changed;
	}

	return Plugin_Continue;
}

bool:IsValidTeam(iClient)
{
	new iTeam = GetClientTeam(iClient);
	if (iTeam == 2 || iTeam == 3) return true;
	return false;
}