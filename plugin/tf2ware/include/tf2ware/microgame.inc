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

	PrintToServer("[AddMiniGame] Adding a microgame with the identifier: %d", identifier);
	if (!MICROGAMES.SetValue(identifier, mg, false))
	{
		ThrowError("Failed to add microgame!");
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
	idx = MICROGAME_SNAPSHOT.GetKey(GetRandomInt(0, MICROGAME_SNAPSHOT.Length - 1));
	Microgame mg;

	PrintToServer("[GetRandomMicrogame] Requesting microgame %d", idx);

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
		case MG_BBALL, MG_HUGGING, MG_RED_FLOOR, MG_JUMP_ROPE, MG_FROGGER,
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

			case MG_GOOMBA:
			{
				view_as<Goomba>(key).OnMicrogameSetup();
			}

			case MG_HIT_ENEMY:
			{
				view_as<HitEnemy>(key).OnMicrogameSetup();
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