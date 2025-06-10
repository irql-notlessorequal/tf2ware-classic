# TF2Ware Classic

The original TF2Ware from 2011, fixed up and working!

# Roadmap

- [x] Make the gamemode function again
- [x] Fix the player freeze issue during "Waiting For Players"
- [x] Drop the `gamedata.txt` requirement, making the gamemode 64-bit ready and immune to breakages.
- [x] Bring back overhead scores
- [x] Bring back the "cute little spycrab"
- [x] Fix assorted issues with "!activator" (i.e. Kamikaze microgame)
- [ ] Rewrite the codebase, dropping the spaghetti code of the past.
- [ ] Move to a different map with additional fixes. (tf2ware_classic)
- [ ] Mitigate player collisions with more than 24 players.
- [ ] Fix the gamemode misbehaving when loading in the map.
- [x] Fix the flood microgame killing you twice at higher speeds.

# Installation.

Download a build of the gamemode.

Extract the .smx into your server's SourceMod plugins folder.

Extract the assets into your server's `tf` folder.

Install `tf2ware_classic.phrases.txt` into SourceMod's translations folder.

# Usage.

1. Load the `tf2ware` map on your server.
2. Set the convar `ww_enable` to `1`.
3. Have fun!

# Credits

## Main

- `Mecha The Slag` for creating TF2Ware

- `NuclearWatermelon` and `gavintlgold` for contributions to the original codebase

- TF2Ware Ultimate for helping me notice Mecha's stupidity in sprite handling

- SLAG Gaming for existing and then dying (rip, forever miss)

## Microgames

- `Mecha The Slag` for all microgames not explicitly listed.

- `gNatFreak` for the "Color Text" microgame

# License

```
TF2Ware Classic

Copyright (C) 2025		IRQL_NOT_LESS_OR_EQUAL

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
```

The `legacy` branch which contains the pre-rewrite code is available under the MIT license
instead.