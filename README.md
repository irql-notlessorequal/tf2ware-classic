# TF2Ware Classic

The original TF2Ware from 2011, fixed up and working!

# Roadmap

- [x] Make the gamemode function again
- [x] Fix the player freeze issue during "Waiting For Players"
- [x] Drop the `gamedata.txt` requirement, making the gamemode 64-bit ready and immune to breakages.
- [ ] Bring back overhead scores
- [ ] Fix assorted issues with "!activator" (i.e. Kamikaze microgame)
- [ ] Rewrite the codebase, dropping the spaghetti code of the past.
- [ ] Fix the gamemode misbehaving when loading in the map.
- [ ] Fix the flood microgame killing you twice at higher speeds.

> Most of the progress is taking place on the `code-rewrite` branch, while this
> branch is kept as the legacy branch sticking mostly to the original Mecha code
> plus some additional bug fixes.

# Installation.

Download a build of the gamemode.

Extract the .smx into your server's SourceMod plugins folder.

Extract the assets into your server's `tf` folder.

Copy the `minigames.cfg` file into SourceMod configs folder.

# Usage.

1. Load the `tf2ware` map on your server.
2. Set the convar `ww_enable` to `1`.
3. Have fun!

# Credits

## Main

- Mecha The Slag for creating TF2Ware

- gavintlgold for additional contributions to the original codebase.

- SLAG Gaming for existing and then dying (rip, forever miss)

## Microgames

- Mecha The Slag for all microgames not explicitly listed.

- gNatFreak for the "Color Text" microgame