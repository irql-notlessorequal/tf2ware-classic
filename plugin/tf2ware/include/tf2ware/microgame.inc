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

#include "mimallet/mimallet_int_map.inc"

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
#pragma unused players
		return true;
	}

	/* Players */

	public void OnClientJustEntered(int client)
	{
#pragma unused client
	}

	public void OnClientDeath(int client)
	{
#pragma unused client
	}

	public bool OnPlayerChatMessage(int client, const char[] message)
	{
#pragma unused client
#pragma unused message
		return false;
	}

	/* Game */

	public void OnMicrogameTimer(int timeLeft)
	{
#pragma unused timeLeft
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
			 MG_GHOSTBUSTERS, MG_MANDRILL:
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

#define HANDLE_MICROGAMES(%1) \
	%1(MG_AIRBLAST, Airblast) \
	%1(MG_AIR_RAID, AirRaid) \
	%1(MG_BARREL, Barrel) \
	%1(MG_BBALL, BBall) \
	%1(MG_COLOR_TEXT, ColorText) \
	%1(MG_FLOOD, Flood) \
	%1(MG_ON_GROUND, OnGround) \
	%1(MG_FROGGER, Frogger) \
	%1(MG_GHOSTBUSTERS, Ghostbusters) \
	%1(MG_GOOMBA, Goomba) \
	%1(MG_HIT_ENEMY, HitEnemy) \
	%1(MG_HOPSCOTCH, Hopscotch) \
	%1(MG_HUGGING, Hugging) \
	%1(MG_JUMP_ROPE, JumpRope) \
	%1(MG_KAMIKAZE, Kamikaze) \
	%1(MG_MATH, Math) \
	%1(MG_MOVEMENT, Movement) \
	%1(MG_NEEDLE_JUMP, NeedleJump) \
	%1(MG_PINATA, Pinata) \
	%1(MG_SAW_RUN, Sawrun) \
	%1(MG_SIMON_SAYS, SimonSays) \
	%1(MG_SNIPER_TARGET, SniperTarget) \
	%1(MG_SPYCRAB, Spycrab)

void DispatchOnClientJustEntered(int client)
{
	switch (view_as<Microgames>(currentMicrogame))
	{
#define ON_CLIENT_ENTERED(%1,%2) case %1: { view_as<%2>(currentMicrogame).OnClientJustEntered(client); }
		HANDLE_MICROGAMES(ON_CLIENT_ENTERED)
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
		HANDLE_MICROGAMES(ON_MICROGAME_START)
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
		HANDLE_MICROGAMES(ON_MICROGAME_TIMER)
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
		HANDLE_MICROGAMES(ON_MICROGAME_END)
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
		HANDLE_MICROGAMES(ON_MICROGAME_POST_END)
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
		HANDLE_MICROGAMES(ON_MICROGAME_FRAME)
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
		HANDLE_MICROGAMES(ON_MICROGAME_CLIENT_DEATH)
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
		HANDLE_MICROGAMES(IS_MICROGAME_PLAYABLE)
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
		HANDLE_MICROGAMES(ON_MICROGAME_CHAT_SAY)
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
		HANDLE_MICROGAMES(ON_MICROGAME_SETUP)
#undef  ON_MICROGAME_SETUP

			default:
			{
				PrintToServer("[TF2Ware] [DispatchOnMicrogameSetup] Ignoring dispatch for unknown microgame %d.", key);
			}
		}
	}
}