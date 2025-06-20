# 1.0.0-rc1

- Complete modernization and re-write of the plugin.
- Remove requirement for `gamedata.txt`
- Support for 64-bit SourceMod
- Added basic translations support.

	Currently only chat text is supported.

	Overlays and game text continue to be English only.

- Fixed missing sounds in the Hugging and Ghostbusters microgame
- Added a new score style system.

	This is controlled with the ConVar ww_score_style.

	By default this is set to one which uses an
	updated style introduced by TF2Ware Classic.

- The chosen special round is now printed into chat.
- Re-introduced overhead scores as an option.

	This is controlled by the ConVar ww_overhead_scores.

- sv_cheats is no longer used by the plugin.
- Made the gamemode use mimallet's RNG library.
- Removed Meet your Match spawn outlines.
- Added support for a map timer being actually set.
- Added logic to end the gamemode when the map timer runs out.
- Wipeout is now a special round instead of a random gamemode variant.
- Wipeout: Added player scaling from 2 up to 12 players at once.

	Having 12 players requires a _very_ large amount of players.

- Special rounds are now rolled differently.

	They are more likely to show up and with a 3 round cooldown after.

- Hopscotch: Rocket Jumping now spreads out players in a circle instead of columns and rows.
- Hopscotch: Added a demoman variant that is similar to Rocket Jumping.
- Hugging: Added player scaling.
- Hugging: Bots can now be chosen as the cuddly heavies.
- Airblast: Use pre-Jungle Inferno globally.
- Airblast: Clients are now correctly spawned in a circle again.
- Air Raid: Re-implemented the gamemode, making it function again.
- Kamikaze: Added back the old style bomb carrier.

	This is controlled by the ConVar ww_kamikaze_style.

- Spycrab: Fixed the overhead spycrab sprite.
- Spycrab: Display the overhead spycrab sprite.
- Flood: Fix players being killed twice at higher speeds.
- Sniper Target: Added targets of all classes so there is some variance.
- Ghostbusters: Added player scaling.
- Ghostbusters: Player sprites work once again.

	This requires overhead scores to be enabled.

- Math: Lowered the bounds for the math questions.