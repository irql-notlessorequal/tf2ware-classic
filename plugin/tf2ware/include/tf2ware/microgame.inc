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
#include "tf2ware_core.inc"

#include "mimallet/mimallet_int_map.sp"

/**
 * This is still better than the previous call-based solution Mecha used since
 * even if I have duplicate 200 lines of code, it's still much nicer and easier to read.
 * 
 * Which is what matters here, since the TF2Ware codebase is a fever dreem.
 */

methodmap Microgame < Handle
{
	public native Microgame();

	/* Core Functions */
	
	public void OnMicrogameSetup()
	{
	}

	public void OnMicrogameStart()
	{
	}

	public void OnMicrogameEnd()
	{
	}

	public void OnMicrogamePostEnd()
	{
	}

	/* Queries */

	public bool IsMicrogamePlayable(int players)
	{
		return true;
	}

	/* Players */

	public void OnClientJustEntered(int client)
	{
	}

	public void OnClientDeath(int client)
	{
	}

	public bool OnPlayerChatMessage(int client, const char[] message)
	{
		return false;
	}

	/* Game */

	public void OnMicrogameTimer(int timeLeft)
	{
	}

	public void OnMicrogameFrame()
	{
	}
}

static IntMap MICROGAMES;
static IntMapSnapshot MICROGAME_SNAPSHOT;

public void AddMiniGame(int identifier, Microgame mg)
{
	if (MICROGAMES == null)
	{
		MICROGAMES = new IntMap();
	}

	if (!mg)
	{
		ThrowError("Cannot add a NULL microgame!");
	}

	/**
	 * Check if the microgame was disabled by the server.
	 */
	if (!GetMinigameConfNum(g_name[identifier - 1], "enable", 1))
	{
		PrintToServer("[AddMiniGame] Not adding microgame with identifier %d as the user has disabled it.", identifier);
		return;
	}
	else
	{
#if defined DEBUG
		PrintToServer("[AddMiniGame] Adding a microgame with the identifier: %d", identifier);
#endif
		if (!MICROGAMES.SetValue(identifier, mg, false))
		{
			ThrowError("Failed to add microgame!");
		}		
	}
}

public Microgame GetMicrogame(int idx)
{
	Microgame mg;
	if (!MICROGAMES.GetValue(idx, mg))
	{
		ThrowError("Failed to fetch a random microgame!");
	}

	return mg;
}

public Microgame GetRandomMicrogame(int &idx)
{
	if (MICROGAME_SNAPSHOT == null)
	{
		MICROGAME_SNAPSHOT = MICROGAMES.Snapshot();
	}

	/**
	 * Epic RNG here.
	 * I love SourcePawn so much.
	 */
	idx = MICROGAME_SNAPSHOT.GetKey(MalletGetRandomInt(0, MICROGAME_SNAPSHOT.Length - 1));
	Microgame mg;
#if defined DEBUG
	PrintToServer("[GetRandomMicrogame] Requesting microgame %d", idx);
#endif
	if (!MICROGAMES.GetValue(idx, mg))
	{
		ThrowError("Failed to fetch a random microgame!");
	}

	return mg;
}

public bool IsBossMicrogame(Microgame mg)
{
	switch (view_as<Microgames>(mg))
	{
		case MG_BBALL, MG_HUGGING, MG_HOPSCOTCH,
			 MG_RED_FLOOR, MG_JUMP_ROPE, MG_FROGGER,
			 MG_GHOSTBUSTERS:
		{
			return true;
		}

		default:
		{
			return false;
		}
	}
}

stock bool MicrogameHasAltSound(Microgame mg)
{
	return view_as<Microgames>(mg) == MG_GHOSTBUSTERS ||
	       view_as<Microgames>(mg) == MG_ON_GROUND;
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

/////////////////////////////////////////

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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnClientJustEntered(client);
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnClientJustEntered(client);
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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnMicrogameStart();
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnMicrogameStart();
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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnMicrogameTimer(timeLeft);
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnMicrogameTimer(timeLeft);
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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnMicrogameEnd();
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnMicrogameEnd();
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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnMicrogamePostEnd();
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnMicrogamePostEnd();
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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnMicrogameFrame();
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnMicrogameFrame();
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

		case MG_ON_GROUND:
		{
			view_as<OnGround>(currentMicrogame).OnClientDeath(client);
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

		case MG_PINATA:
		{
			view_as<Pinata>(currentMicrogame).OnClientDeath(client);
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

		case MG_ON_GROUND:
		{
			return view_as<OnGround>(mg).IsMicrogamePlayable(players);
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

		case MG_PINATA:
		{
			return view_as<Pinata>(mg).IsMicrogamePlayable(players);
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

public Action DispatchOnPlayerChatSay(int client, const char message[255])
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

public void DispatchOnMicrogameSetup()
{
	if (MICROGAME_SNAPSHOT == null)
	{
		MICROGAME_SNAPSHOT = MICROGAMES.Snapshot();
	}

	for (int idx = 0; idx < MICROGAME_SNAPSHOT.Length; idx++)
	{
		int key = MICROGAME_SNAPSHOT.GetKey(idx);
		switch (view_as<Microgames>(key))
		{
			case MG_AIRBLAST:
			{
				view_as<Airblast>(key).OnMicrogameSetup();
			}

			case MG_AIR_RAID:
			{
				view_as<AirRaid>(key).OnMicrogameSetup();
			}

			case MG_BARREL:
			{
				view_as<Barrel>(key).OnMicrogameSetup();
			}

			case MG_BBALL:
			{
				view_as<BBall>(key).OnMicrogameSetup();
			}

			case MG_COLOR_TEXT:
			{
				view_as<ColorText>(key).OnMicrogameSetup();
			}

			case MG_FLOOD:
			{
				view_as<Flood>(key).OnMicrogameSetup();
			}

			case MG_ON_GROUND:
			{
				view_as<OnGround>(key).OnMicrogameSetup();
			}

			case MG_FROGGER:
			{
				view_as<Frogger>(key).OnMicrogameSetup();
			}

			case MG_GHOSTBUSTERS:
			{
				view_as<Ghostbusters>(key).OnMicrogameSetup();
			}

			case MG_GOOMBA:
			{
				view_as<Goomba>(key).OnMicrogameSetup();
			}

			case MG_HIT_ENEMY:
			{
				view_as<HitEnemy>(key).OnMicrogameSetup();
			}

			case MG_HOPSCOTCH:
			{
				view_as<Hopscotch>(key).OnMicrogameSetup();
			}

			case MG_HUGGING:
			{
				view_as<Hugging>(key).OnMicrogameSetup();
			}

			case MG_JUMP_ROPE:
			{
				view_as<JumpRope>(key).OnMicrogameSetup();
			}

			case MG_KAMIKAZE:
			{
				view_as<Kamikaze>(key).OnMicrogameSetup();
			}

			case MG_MATH:
			{
				view_as<Math>(key).OnMicrogameSetup();
			}

			case MG_MOVEMENT:
			{
				view_as<Movement>(key).OnMicrogameSetup();
			}

			case MG_NEEDLE_JUMP:
			{
				view_as<NeedleJump>(key).OnMicrogameSetup();
			}

			case MG_PINATA:
			{
				view_as<Pinata>(key).OnMicrogameSetup();
			}

			case MG_SAW_RUN:
			{
				view_as<Sawrun>(key).OnMicrogameSetup();
			}

			case MG_SIMON_SAYS:
			{
				view_as<SimonSays>(key).OnMicrogameSetup();
			}

			case MG_SNIPER_TARGET:
			{
				view_as<SniperTarget>(key).OnMicrogameSetup();
			}

			case MG_SPYCRAB:
			{
				view_as<Spycrab>(key).OnMicrogameSetup();
			}

			default:
			{
				PrintToServer("[TF2Ware] [DispatchOnMicrogameSetup] Ignoring dispatch for unknown microgame %d.", key);
			}
		}
	}
}