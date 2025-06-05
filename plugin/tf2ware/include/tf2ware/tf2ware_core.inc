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
#pragma newdecls required

#define MAX_MINIGAMES		 40

#define PLUGIN_VERSION		 "0.0.54"

#define MUSIC_SPECIAL_START			"imgay/tf2ware/special_intro.mp3"
#define MUSIC_SPECIAL_START_LEN 	2.111429
#define MUSIC_SPECIAL_WIN			"imgay/tf2ware/special_win.mp3"
#define MUSIC_SPECIAL_FAIL			"imgay/tf2ware/special_fail.mp3"
#define MUSIC_SPECIAL_END_LEN		2.111429
#define MUSIC_SPECIAL_SPEEDUP		"imgay/tf2ware/special_speedup.mp3"
#define MUSIC_SPECIAL_SPEEDUP_LEN	3.835510
#define MUSIC_SPECIAL_GAMEOVER		"imgay/tf2ware/special_gameover.mp3"
#define MUSIC_SPECIAL_GAMEOVER_LEN	8.17

#define MUSIC_WIPEOUT_START			"imgay/tf2ware/tf2ware_intro.mp3"
#define MUSIC_WIPEOUT_START_LEN		2.18
#define MUSIC_WIPEOUT_WIN			"imgay/tf2ware/tf2ware_win.mp3"
#define MUSIC_WIPEOUT_FAIL			"imgay/tf2ware/tf2ware_fail.mp3"
#define MUSIC_WIPEOUT_END_LEN		2.2
#define MUSIC_WIPEOUT_SPEEDUP		"imgay/tf2ware/tf2ware_speedup.mp3"
#define MUSIC_WIPEOUT_SPEEDUP_LEN	3.29
#define MUSIC_WIPEOUT_BOSS			"imgay/tf2ware/boss.mp3"
#define MUSIC_WIPEOUT_BOSS_LEN		3.9
#define MUSIC_WIPEOUT_GAMEOVER		"imgay/tf2ware/warioman_gameover.mp3"
#define MUSIC_WIPEOUT_GAMEOVER_LEN	8.17

#define MUSIC_START			 "imgay/tf2ware/warioman_intro.mp3"
#define MUSIC_START_LEN		 2.17
#define MUSIC_WIN			 "imgay/tf2ware/warioman_win.mp3"
#define MUSIC_FAIL			 "imgay/tf2ware/warioman_fail.mp3"
#define MUSIC_END_LEN		 2.19
#define MUSIC_SPEEDUP		 "imgay/tf2ware/warioman_speedup.mp3"
#define MUSIC_SPEEDUP_LEN	 3.9
#define MUSIC_BOSS			 "imgay/tf2ware/warioman_boss.mp3"
#define MUSIC_BOSS_LEN		 4.2
#define MUSIC_GAMEOVER		 "imgay/tf2ware/warioman_gameover.mp3"
#define MUSIC_GAMEOVER_LEN	 8.17

#define SOUND_COMPLETE		 "imgay/tf2ware/complete_me.mp3"
#define SOUND_COMPLETE_YOU	 "imgay/tf2ware/complete_you.mp3"
#define SOUND_MINISCORE		 "items/pumpkin_drop.wav"
#define SOUND_HEAVY_KISS	 "vo/heavy_generic01.mp3"
#define MUSIC_WAITING		 "imgay/tf2ware/waitingforplayers.mp3"
#define MUSIC_SPECIAL		 "imgay/tf2ware/specialround.mp3"
#define MUSIC_SPECIAL_LEN	 13.5
#define SOUND_SELECT		 "imgay/tf2ware/select.mp3"

#define SND_CHANNEL_SPECIFIC	32

/**
 * Wipeout now scales between two and twelve players.
 * 
 * With one additional player being added
 * for every seven players.
 */
#define MAX_WIPEOUT_PLAYERS		12

#define PARTICLE_WIN_BLUE		"teleportedin_blue"
#define PARTICLE_WIN_RED		"teleportedin_red"

#define TF2_PLAYER_TAUNTING		(1 << 7)	 // 128 = Taunting

#define GAMEMODE_NORMAL			0
#define GAMEMODE_WIPEOUT		1
#define GAMEMODE_WIPEOUT_HEIGHT 1190.0

#define WW_BOMB				"pl_hoodoo/alarm_clock_ticking_3.wav"
#define WW_BOMB_MODEL		"models/custom/dirty_bomb_cart.mdl"
#define WW_BOMB_OLD_MODEL	"models/tf2ware/bobomb.mdl"
#define PARTICLE_BOMB		"cinefx_goldrush_embers"
#define PARTICLE_EXPLODE	"cinefx_goldrush_initial_smoke"

bool InternalIsAlpine = false;

/**
 * Returns "TRUE" if we are running
 * the Alpine version of TF2Ware Classic.
 */
stock bool IsAlpineVariant()
{
	return InternalIsAlpine;
}

stock void InternalSetAlpineVariant(bool value)
{
	InternalIsAlpine = value;
}

enum TF2WareMapType
{
	CLASSIC_DEFAULT = 0,
	CLASSIC_ALPINE = 1,

	LEGACY_DEFAULT = 2,
	LEGACY_ALPINE = 3
}

enum TF2WareStatus
{
	INACTIVE = 0,
	MICROGAME_IN_PROGRESS = 1,
	/* */
	SPEEDUP_IN_PROGRESS = 3,
	END_OF_ROUND = 4,
	_UNKNOWN = 5,
	SPECIAL_ROUND_ROLL = 6,
}

enum SpecialRounds
{
	NONE = 0,

	/* */

	SUPER_SPEED,
	NO_TOUCHING,
	DOUBLE_BOSS_BATTLE,
	SINGLEPLAYER,
	LEAST_IS_BEST,
	BONK,
	THIRDPERSON,
#if 0
	WIPEOUT
#endif
};

enum Microgames
{
	MG_NONE = 0,

	/* */

	MG_HIT_ENEMY = 1,
	MG_KAMIKAZE = 2,
	MG_BARREL = 3,
	MG_SAW_RUN = 4,
	MG_NEEDLE_JUMP = 5,
	MG_HOPSCOTCH = 6,
	/* */
	MG_AIRBLAST = 8,
	MG_MATH = 9,
	MG_MOVEMENT = 10,
	MG_FLOOD = 11,
	/* */
	MG_BBALL = 13,
	MG_HUGGING = 14,
	MG_SIMON_SAYS = 15,
	MG_RED_FLOOR = 16,
	MG_SPYCRAB = 17,
	MG_SNIPER_TARGET = 18,
	MG_AIR_RAID = 19,
	MG_JUMP_ROPE = 20,
	MG_COLOR_TEXT = 21,
	MG_FROGGER = 22,
	MG_GOOMBA = 23,
	MG_PINATA = 24,
	MG_GHOSTBUSTERS = 25,
}