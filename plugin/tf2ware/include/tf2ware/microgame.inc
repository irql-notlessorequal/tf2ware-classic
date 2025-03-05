#include "mimallet/mimallet_int_map.sp"
methodmap Microgame < Handle
{
	public native Microgame();
	/* Core Functions */
	
	public native void OnMicrogameSetup();
	public native void OnMicrogameStart();
	public native void OnMicrogameEnd();
	public native void OnMicrogamePostEnd();

	public native int GetMicrogameIdentifier();

	/* Players */

	public native void OnClientJustEntered(int client);

	public native void OnClientDeath(int client);

	/* Game */

	public native void OnMicrogameTimer(int timeLeft);

	public native void OnMicrogameFrame();
}

static IntMap MICROGAMES;

public void AddMiniGame(Microgame mg)
{
	if (MICROGAMES == null)
	{
		MICROGAMES = new IntMap();
	}

	if (!mg)
	{
		ThrowError("Cannot add a NULL microgame!");
	}

	MICROGAMES.SetValue(mg.GetMicrogameIdentifier(), mg);
}

public Microgame GetRandomMicrogame()
{
	int idx = GetRandomInt(0, MICROGAMES.Size - 1);
	Microgame mg;

	if (!MICROGAMES.GetValue(idx, mg))
	{
		ThrowError("Failed to fetch a random microgame!");
	}

	return mg;
}