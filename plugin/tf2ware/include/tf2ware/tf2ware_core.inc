#define MAX_MINIGAMES		 40

#define PLUGIN_VERSION		 "0.0.30"

#define MUSIC_SPECIAL_START		"imgay/tf2ware/special_intro.mp3"
#define MUSIC_SPECIAL_START_LEN  4.167
#define MUSIC_SPECIAL_WIN		"imgay/tf2ware/special_win.mp3"
#define MUSIC_SPECIAL_FAIL		"imgay/tf2ware/special_fail.mp3"
#define MUSIC_SPECIAL_END_LEN	2.039
#define MUSIC_SPECIAL_SPEEDUP	"imgay/tf2ware/special_speedup.mp3"
#define MUSIC_SPECIAL_SPEEDUP_LEN	4.2
#define MUSIC_SPECIAL_GAMEOVER	"imgay/tf2ware/special_gameover.mp3"
#define MUSIC_SPECIAL_GAMEOVER_LEN	2.04

#define MUSIC_WIPEOUT_START		"imgay/tf2ware/tf2ware_intro.mp3"
#define MUSIC_WIPEOUT_START_LEN	 2.18
#define MUSIC_WIPEOUT_WIN		"imgay/tf2ware/tf2ware_win.mp3"
#define MUSIC_WIPEOUT_FAIL		"imgay/tf2ware/tf2ware_fail.mp3"
#define MUSIC_WIPEOUT_END_LEN	2.2
#define MUSIC_WIPEOUT_SPEEDUP	"imgay/tf2ware/tf2ware_speedup.mp3"
#define MUSIC_WIPEOUT_SPEEDUP_LEN	 3.29
#define MUSIC_WIPEOUT_BOSS		"imgay/tf2ware/boss.mp3"
#define MUSIC_WIPEOUT_BOSS_LEN	3.9
#define MUSIC_WIPEOUT_GAMEOVER	"imgay/tf2ware/warioman_gameover.mp3"
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
#define SOUND_HEAVY_KISS	 "vo/heavy_generic01.wav"
#define MUSIC_WAITING		 "imgay/tf2ware/waitingforplayers.mp3"
#define MUSIC_SPECIAL		 "imgay/tf2ware/specialround.mp3"
#define MUSIC_SPECIAL_LEN	 13.5
#define SOUND_SELECT		 "imgay/tf2ware/select.mp3"

#define SND_CHANNEL_SPECIFIC 32

#define PARTICLE_WIN_BLUE	 "teleportedin_blue"
#define PARTICLE_WIN_RED	 "teleportedin_red"

#define TF2_PLAYER_TAUNTING	 (1 << 7)	 // 128        Taunting

#define GAMEMODE_NORMAL			0
#define GAMEMODE_WIPEOUT		1
#define GAMEMODE_WIPEOUT_HEIGHT 1190.0

#define WW_BOMB "pl_hoodoo/alarm_clock_ticking_3.wav"
#define WW_BOMB_MODEL "models/custom/dirty_bomb_cart.mdl"
#define PARTICLE_BOMB "cinefx_goldrush_embers"
#define PARTICLE_EXPLODE "cinefx_goldrush_initial_smoke"

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

	/* */

	_MAX = THIRDPERSON
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
	MG_MANDRILL = 26
}

enum TF2WareStatus
{
	PRE_MICROGAME = 0
}