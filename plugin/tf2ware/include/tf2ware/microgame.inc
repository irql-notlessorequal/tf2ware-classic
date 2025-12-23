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
 * even if I have to duplicate 200 lines of code, it's still much nicer and easier to read.
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
 * this unholy abomination than have to deal with the previous Mecha code
 * that used the forwards method in SourcePawn.
 * 
 * My eyes weep at this.
 * Have fun trying to add custom microgames to this.
 */

/////////////////////////////////////////

void DispatchOnClientJustEntered(int client)
{
	switch (view_as<Microgames>(currentMicrogame))
	{
#define ON_CLIENT_ENTERED(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnClientJustEntered(client); }

		ON_CLIENT_ENTERED(MG_AIRBLAST, Airblast)
		ON_CLIENT_ENTERED(MG_AIR_RAID, AirRaid)
		ON_CLIENT_ENTERED(MG_BARREL, Barrel)
		ON_CLIENT_ENTERED(MG_BBALL, BBall)
		ON_CLIENT_ENTERED(MG_COLOR_TEXT, ColorText)
		ON_CLIENT_ENTERED(MG_FLOOD, Flood)
		ON_CLIENT_ENTERED(MG_ON_GROUND, OnGround)
		ON_CLIENT_ENTERED(MG_FROGGER, Frogger)
		ON_CLIENT_ENTERED(MG_GHOSTBUSTERS, Ghostbusters)
		ON_CLIENT_ENTERED(MG_GOOMBA, Goomba)
		ON_CLIENT_ENTERED(MG_HIT_ENEMY, HitEnemy)
		ON_CLIENT_ENTERED(MG_HOPSCOTCH, Hopscotch)
		ON_CLIENT_ENTERED(MG_HUGGING, Hugging)
		ON_CLIENT_ENTERED(MG_JUMP_ROPE, JumpRope)
		ON_CLIENT_ENTERED(MG_KAMIKAZE, Kamikaze)
		ON_CLIENT_ENTERED(MG_MATH, Math)
		ON_CLIENT_ENTERED(MG_MOVEMENT, Movement)
		ON_CLIENT_ENTERED(MG_NEEDLE_JUMP, NeedleJump)
		ON_CLIENT_ENTERED(MG_PINATA, Pinata)
		ON_CLIENT_ENTERED(MG_SAW_RUN, Sawrun)
		ON_CLIENT_ENTERED(MG_SIMON_SAYS, SimonSays)
		ON_CLIENT_ENTERED(MG_SNIPER_TARGET, SniperTarget)
		ON_CLIENT_ENTERED(MG_SPYCRAB, Spycrab)

#undef  ON_CLIENT_ENTERED

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
#define ON_MICROGAME_START(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnMicrogameStart(); }

		ON_MICROGAME_START(MG_AIRBLAST, Airblast)
		ON_MICROGAME_START(MG_AIR_RAID, AirRaid)
		ON_MICROGAME_START(MG_BARREL, Barrel)
		ON_MICROGAME_START(MG_BBALL, BBall)
		ON_MICROGAME_START(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_START(MG_FLOOD, Flood)
		ON_MICROGAME_START(MG_ON_GROUND, OnGround)
		ON_MICROGAME_START(MG_FROGGER, Frogger)
		ON_MICROGAME_START(MG_GHOSTBUSTERS, Ghostbusters)
		ON_MICROGAME_START(MG_GOOMBA, Goomba)
		ON_MICROGAME_START(MG_HIT_ENEMY, HitEnemy)
		ON_MICROGAME_START(MG_HOPSCOTCH, Hopscotch)
		ON_MICROGAME_START(MG_HUGGING, Hugging)
		ON_MICROGAME_START(MG_JUMP_ROPE, JumpRope)
		ON_MICROGAME_START(MG_KAMIKAZE, Kamikaze)
		ON_MICROGAME_START(MG_MATH, Math)
		ON_MICROGAME_START(MG_MOVEMENT, Movement)
		ON_MICROGAME_START(MG_NEEDLE_JUMP, NeedleJump)
		ON_MICROGAME_START(MG_PINATA, Pinata)
		ON_MICROGAME_START(MG_SAW_RUN, Sawrun)
		ON_MICROGAME_START(MG_SIMON_SAYS, SimonSays)
		ON_MICROGAME_START(MG_SNIPER_TARGET, SniperTarget)
		ON_MICROGAME_START(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_START

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
#define ON_MICROGAME_TIMER(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnMicrogameTimer(timeLeft); }

		ON_MICROGAME_TIMER(MG_AIRBLAST, Airblast)
		ON_MICROGAME_TIMER(MG_AIR_RAID, AirRaid)
		ON_MICROGAME_TIMER(MG_BARREL, Barrel)
		ON_MICROGAME_TIMER(MG_BBALL, BBall)
		ON_MICROGAME_TIMER(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_TIMER(MG_FLOOD, Flood)
		ON_MICROGAME_TIMER(MG_ON_GROUND, OnGround)
		ON_MICROGAME_TIMER(MG_FROGGER, Frogger)
		ON_MICROGAME_TIMER(MG_GHOSTBUSTERS, Ghostbusters)
		ON_MICROGAME_TIMER(MG_GOOMBA, Goomba)
		ON_MICROGAME_TIMER(MG_HIT_ENEMY, HitEnemy)
		ON_MICROGAME_TIMER(MG_HOPSCOTCH, Hopscotch)
		ON_MICROGAME_TIMER(MG_HUGGING, Hugging)
		ON_MICROGAME_TIMER(MG_JUMP_ROPE, JumpRope)
		ON_MICROGAME_TIMER(MG_KAMIKAZE, Kamikaze)
		ON_MICROGAME_TIMER(MG_MATH, Math)
		ON_MICROGAME_TIMER(MG_MOVEMENT, Movement)
		ON_MICROGAME_TIMER(MG_NEEDLE_JUMP, NeedleJump)
		ON_MICROGAME_TIMER(MG_PINATA, Pinata)
		ON_MICROGAME_TIMER(MG_SAW_RUN, Sawrun)
		ON_MICROGAME_TIMER(MG_SIMON_SAYS, SimonSays)
		ON_MICROGAME_TIMER(MG_SNIPER_TARGET, SniperTarget)
		ON_MICROGAME_TIMER(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_TIMER

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
#define ON_MICROGAME_END(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnMicrogameEnd(); }

		ON_MICROGAME_END(MG_AIRBLAST, Airblast)
		ON_MICROGAME_END(MG_AIR_RAID, AirRaid)
		ON_MICROGAME_END(MG_BARREL, Barrel)
		ON_MICROGAME_END(MG_BBALL, BBall)
		ON_MICROGAME_END(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_END(MG_FLOOD, Flood)
		ON_MICROGAME_END(MG_ON_GROUND, OnGround)
		ON_MICROGAME_END(MG_FROGGER, Frogger)
		ON_MICROGAME_END(MG_GHOSTBUSTERS, Ghostbusters)
		ON_MICROGAME_END(MG_GOOMBA, Goomba)
		ON_MICROGAME_END(MG_HIT_ENEMY, HitEnemy)
		ON_MICROGAME_END(MG_HOPSCOTCH, Hopscotch)
		ON_MICROGAME_END(MG_HUGGING, Hugging)
		ON_MICROGAME_END(MG_JUMP_ROPE, JumpRope)
		ON_MICROGAME_END(MG_KAMIKAZE, Kamikaze)
		ON_MICROGAME_END(MG_MATH, Math)
		ON_MICROGAME_END(MG_MOVEMENT, Movement)
		ON_MICROGAME_END(MG_NEEDLE_JUMP, NeedleJump)
		ON_MICROGAME_END(MG_PINATA, Pinata)
		ON_MICROGAME_END(MG_SAW_RUN, Sawrun)
		ON_MICROGAME_END(MG_SIMON_SAYS, SimonSays)
		ON_MICROGAME_END(MG_SNIPER_TARGET, SniperTarget)
		ON_MICROGAME_END(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_END

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
#define ON_MICROGAME_POST_END(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnMicrogamePostEnd(); }
	
		ON_MICROGAME_POST_END(MG_AIRBLAST, Airblast)
		ON_MICROGAME_POST_END(MG_AIR_RAID, AirRaid)
		ON_MICROGAME_POST_END(MG_BARREL, Barrel)
		ON_MICROGAME_POST_END(MG_BBALL, BBall)
		ON_MICROGAME_POST_END(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_POST_END(MG_FLOOD, Flood)
		ON_MICROGAME_POST_END(MG_ON_GROUND, OnGround)
		ON_MICROGAME_POST_END(MG_FROGGER, Frogger)
		ON_MICROGAME_POST_END(MG_GHOSTBUSTERS, Ghostbusters)
		ON_MICROGAME_POST_END(MG_GOOMBA, Goomba)
		ON_MICROGAME_POST_END(MG_HIT_ENEMY, HitEnemy)
		ON_MICROGAME_POST_END(MG_HOPSCOTCH, Hopscotch)
		ON_MICROGAME_POST_END(MG_HUGGING, Hugging)
		ON_MICROGAME_POST_END(MG_JUMP_ROPE, JumpRope)
		ON_MICROGAME_POST_END(MG_KAMIKAZE, Kamikaze)
		ON_MICROGAME_POST_END(MG_MATH, Math)
		ON_MICROGAME_POST_END(MG_MOVEMENT, Movement)
		ON_MICROGAME_POST_END(MG_NEEDLE_JUMP, NeedleJump)
		ON_MICROGAME_POST_END(MG_PINATA, Pinata)
		ON_MICROGAME_POST_END(MG_SAW_RUN, Sawrun)
		ON_MICROGAME_POST_END(MG_SIMON_SAYS, SimonSays)
		ON_MICROGAME_POST_END(MG_SNIPER_TARGET, SniperTarget)
		ON_MICROGAME_POST_END(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_POST_END

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
#define ON_MICROGAME_FRAME(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnMicrogameFrame(); }

		ON_MICROGAME_FRAME(MG_AIRBLAST, Airblast)
		ON_MICROGAME_FRAME(MG_AIR_RAID, AirRaid)
		ON_MICROGAME_FRAME(MG_BARREL, Barrel)
		ON_MICROGAME_FRAME(MG_BBALL, BBall)
		ON_MICROGAME_FRAME(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_FRAME(MG_FLOOD, Flood)
		ON_MICROGAME_FRAME(MG_ON_GROUND, OnGround)
		ON_MICROGAME_FRAME(MG_FROGGER, Frogger)
		ON_MICROGAME_FRAME(MG_GHOSTBUSTERS, Ghostbusters)
		ON_MICROGAME_FRAME(MG_GOOMBA, Goomba)
		ON_MICROGAME_FRAME(MG_HIT_ENEMY, HitEnemy)
		ON_MICROGAME_FRAME(MG_HOPSCOTCH, Hopscotch)
		ON_MICROGAME_FRAME(MG_HUGGING, Hugging)
		ON_MICROGAME_FRAME(MG_JUMP_ROPE, JumpRope)
		ON_MICROGAME_FRAME(MG_KAMIKAZE, Kamikaze)
		ON_MICROGAME_FRAME(MG_MATH, Math)
		ON_MICROGAME_FRAME(MG_MOVEMENT, Movement)
		ON_MICROGAME_FRAME(MG_NEEDLE_JUMP, NeedleJump)
		ON_MICROGAME_FRAME(MG_PINATA, Pinata)
		ON_MICROGAME_FRAME(MG_SAW_RUN, Sawrun)
		ON_MICROGAME_FRAME(MG_SIMON_SAYS, SimonSays)
		ON_MICROGAME_FRAME(MG_SNIPER_TARGET, SniperTarget)
		ON_MICROGAME_FRAME(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_FRAME

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
#define ON_MICROGAME_CLIENT_DEATH(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnClientDeath(client); }

		ON_MICROGAME_CLIENT_DEATH(MG_AIRBLAST, Airblast)
		ON_MICROGAME_CLIENT_DEATH(MG_AIR_RAID, AirRaid)
		ON_MICROGAME_CLIENT_DEATH(MG_BARREL, Barrel)
		ON_MICROGAME_CLIENT_DEATH(MG_BBALL, BBall)
		ON_MICROGAME_CLIENT_DEATH(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_CLIENT_DEATH(MG_FLOOD, Flood)
		ON_MICROGAME_CLIENT_DEATH(MG_ON_GROUND, OnGround)
		ON_MICROGAME_CLIENT_DEATH(MG_FROGGER, Frogger)
		ON_MICROGAME_CLIENT_DEATH(MG_GHOSTBUSTERS, Ghostbusters)
		ON_MICROGAME_CLIENT_DEATH(MG_GOOMBA, Goomba)
		ON_MICROGAME_CLIENT_DEATH(MG_HIT_ENEMY, HitEnemy)
		ON_MICROGAME_CLIENT_DEATH(MG_HOPSCOTCH, Hopscotch)
		ON_MICROGAME_CLIENT_DEATH(MG_HUGGING, Hugging)
		ON_MICROGAME_CLIENT_DEATH(MG_JUMP_ROPE, JumpRope)
		ON_MICROGAME_CLIENT_DEATH(MG_KAMIKAZE, Kamikaze)
		ON_MICROGAME_CLIENT_DEATH(MG_MATH, Math)
		ON_MICROGAME_CLIENT_DEATH(MG_MOVEMENT, Movement)
		ON_MICROGAME_CLIENT_DEATH(MG_NEEDLE_JUMP, NeedleJump)
		ON_MICROGAME_CLIENT_DEATH(MG_PINATA, Pinata)
		ON_MICROGAME_CLIENT_DEATH(MG_SAW_RUN, Sawrun)
		ON_MICROGAME_CLIENT_DEATH(MG_SIMON_SAYS, SimonSays)
		ON_MICROGAME_CLIENT_DEATH(MG_SNIPER_TARGET, SniperTarget)
		ON_MICROGAME_CLIENT_DEATH(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_CLIENT_DEATH

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
#define IS_MICROGAME_PLAYABLE(%1,%2) case %1: { return view_as<%2>(currentMicrogame).IsMicrogamePlayable(players); }

		IS_MICROGAME_PLAYABLE(MG_AIRBLAST, Airblast)
		IS_MICROGAME_PLAYABLE(MG_AIR_RAID, AirRaid)
		IS_MICROGAME_PLAYABLE(MG_BARREL, Barrel)
		IS_MICROGAME_PLAYABLE(MG_BBALL, BBall)
		IS_MICROGAME_PLAYABLE(MG_COLOR_TEXT, ColorText)
		IS_MICROGAME_PLAYABLE(MG_FLOOD, Flood)
		IS_MICROGAME_PLAYABLE(MG_ON_GROUND, OnGround)
		IS_MICROGAME_PLAYABLE(MG_FROGGER, Frogger)
		IS_MICROGAME_PLAYABLE(MG_GHOSTBUSTERS, Ghostbusters)
		IS_MICROGAME_PLAYABLE(MG_GOOMBA, Goomba)
		IS_MICROGAME_PLAYABLE(MG_HIT_ENEMY, HitEnemy)
		IS_MICROGAME_PLAYABLE(MG_HOPSCOTCH, Hopscotch)
		IS_MICROGAME_PLAYABLE(MG_HUGGING, Hugging)
		IS_MICROGAME_PLAYABLE(MG_JUMP_ROPE, JumpRope)
		IS_MICROGAME_PLAYABLE(MG_KAMIKAZE, Kamikaze)
		IS_MICROGAME_PLAYABLE(MG_MATH, Math)
		IS_MICROGAME_PLAYABLE(MG_MOVEMENT, Movement)
		IS_MICROGAME_PLAYABLE(MG_NEEDLE_JUMP, NeedleJump)
		IS_MICROGAME_PLAYABLE(MG_PINATA, Pinata)
		IS_MICROGAME_PLAYABLE(MG_SAW_RUN, Sawrun)
		IS_MICROGAME_PLAYABLE(MG_SIMON_SAYS, SimonSays)
		IS_MICROGAME_PLAYABLE(MG_SNIPER_TARGET, SniperTarget)
		IS_MICROGAME_PLAYABLE(MG_SPYCRAB, Spycrab)

#undef  IS_MICROGAME_PLAYABLE

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
#define ON_MICROGAME_CHAT_SAY(%1,%2) case %1: \
 { bool ret = view_as<%2>(currentMicrogame).OnPlayerChatMessage(client, message); return ret ? Plugin_Handled : Plugin_Continue; }

		ON_MICROGAME_CHAT_SAY(MG_COLOR_TEXT, ColorText)
		ON_MICROGAME_CHAT_SAY(MG_MATH, Math)

#undef  ON_MICROGAME_CHAT_SAY

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
#define ON_MICROGAME_SETUP(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnMicrogameSetup(); }

			ON_MICROGAME_SETUP(MG_AIRBLAST, Airblast)
			ON_MICROGAME_SETUP(MG_AIR_RAID, AirRaid)
			ON_MICROGAME_SETUP(MG_BARREL, Barrel)
			ON_MICROGAME_SETUP(MG_BBALL, BBall)
			ON_MICROGAME_SETUP(MG_COLOR_TEXT, ColorText)
			ON_MICROGAME_SETUP(MG_FLOOD, Flood)
			ON_MICROGAME_SETUP(MG_ON_GROUND, OnGround)
			ON_MICROGAME_SETUP(MG_FROGGER, Frogger)
			ON_MICROGAME_SETUP(MG_GHOSTBUSTERS, Ghostbusters)
			ON_MICROGAME_SETUP(MG_GOOMBA, Goomba)
			ON_MICROGAME_SETUP(MG_HIT_ENEMY, HitEnemy)
			ON_MICROGAME_SETUP(MG_HOPSCOTCH, Hopscotch)
			ON_MICROGAME_SETUP(MG_HUGGING, Hugging)
			ON_MICROGAME_SETUP(MG_JUMP_ROPE, JumpRope)
			ON_MICROGAME_SETUP(MG_KAMIKAZE, Kamikaze)
			ON_MICROGAME_SETUP(MG_MATH, Math)
			ON_MICROGAME_SETUP(MG_MOVEMENT, Movement)
			ON_MICROGAME_SETUP(MG_NEEDLE_JUMP, NeedleJump)
			ON_MICROGAME_SETUP(MG_PINATA, Pinata)
			ON_MICROGAME_SETUP(MG_SAW_RUN, Sawrun)
			ON_MICROGAME_SETUP(MG_SIMON_SAYS, SimonSays)
			ON_MICROGAME_SETUP(MG_SNIPER_TARGET, SniperTarget)
			ON_MICROGAME_SETUP(MG_SPYCRAB, Spycrab)

#undef  ON_MICROGAME_SETUP

			default:
			{
				PrintToServer("[TF2Ware] [DispatchOnMicrogameSetup] Ignoring dispatch for unknown microgame %d.", key);
			}
		}
	}
}