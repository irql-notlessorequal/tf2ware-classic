methodmap Microgame
{
	public native Microgame();
	/* Core Functions */
	
	public native void OnMicrogameSetup();
	public native void OnMicrogameStart();
	public native void OnMicrogameEnd();

	public native int GetMicrogameIdentifier();

	/* Players */

	public native void OnClientJustEntered(int client);

	/* Game */

	public native void OnMicrogameTimer(int timeLeft);

	public native void OnMicrogameFrame();
}