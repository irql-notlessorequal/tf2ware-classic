#define MAX_MINIGAMES		 40

#define PLUGIN_VERSION		 "0.0.20"

#define MUSIC2_START		 "imgay/tf2ware/tf2ware_intro.mp3"
#define MUSIC2_START_LEN	 2.18
#define MUSIC2_WIN			 "imgay/tf2ware/tf2ware_win.mp3"
#define MUSIC2_FAIL			 "imgay/tf2ware/tf2ware_fail.mp3"
#define MUSIC2_END_LEN		 2.2
#define MUSIC2_SPEEDUP		 "imgay/tf2ware/tf2ware_speedup.mp3"
#define MUSIC2_SPEEDUP_LEN	 3.29
#define MUSIC2_BOSS			 "imgay/tf2ware/boss.mp3"
#define MUSIC2_BOSS_LEN		 3.9
#define MUSIC2_GAMEOVER		 "imgay/tf2ware/warioman_gameover.mp3"
#define MUSIC2_GAMEOVER_LEN	 8.17

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