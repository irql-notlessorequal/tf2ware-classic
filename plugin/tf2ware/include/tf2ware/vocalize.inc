// Support for game Vocalizations

Handle vTimers[MAXPLAYERS + 1];
int g_ivTimers = 0;

public Action Timer_Vocalize(Handle hTimer, DataPack data)
{
    // If the Vocalization is succesful make a new timer, otherwise don't
    data.Reset();

	DataPack pack = view_as<DataPack>(data);
	int game = pack.ReadCell();

	if (iMinigame == game && status == 2)
	{
		float minDelay = pack.ReadFloat();
		float maxDelay = pack.ReadFloat();
		int level = pack.ReadCell();
		
		int clients[MAXPLAYERS + 1];
		pack.ReadCellArray(clients, sizeof (clients));
		
		char buffer[128];
		ArrayList sounds = pack.ReadCell();
		
		if (sounds.Length != 0)
		{
			int idx = GetRandomInt(0, sounds.Length - 1);
			if (!sounds.GetString(idx, buffer, sizeof (buffer)))
			{
				ThrowError("Failed to get a voiceline!");
			}
	
			int client = RollFromValid(clients);
			// RollFromValid returns -1 if there are no clients to choose from
			if (client != -1)
			{
				EmitSoundToAll(buffer, client, SNDCHAN_AUTO, level);
			}

			AddRandVocalizationLoop(sounds, clients, sizeof (clients), minDelay, maxDelay, level);
		}
	}

	return Plugin_Stop;
}

int RollFromValid(int clients[MAXPLAYERS+1])
{
	Handle list = CreateArray();
	for (int i = 0; i < sizeof(clients); i++)
	{
		if (IsValidClient(clients[i]) && IsPlayerAlive(clients[i]))
		{
			PushArrayCell(list, clients[i]);
		}
	}

	int result;
	// Check to avoid "invalid index 0" if array size is <1
	if (GetArraySize(list) > 0)
	{
		result = GetArrayCell(list, GetRandomInt(0, GetArraySize(list) - 1));
	}
	else
	{
		result = -1;
	}

	CloseHandle(list);
	return result;
}

void CleanupAllVocalizations()
{
	g_ivTimers = 0;
}

// Never being used. Disabled until.
/*StopVocalization(id) {
	CloseHandle(vTimers[id]);
}*/

int AddRandVocalization(const char[][] lines, int lines_length,
	int[] clients, int clients_length,
	float minDelay=4.0, float maxDelay=8.0, int level = SNDLEVEL_NORMAL)
{
	float rand = GetRandomFloat(minDelay, maxDelay);

	DataPack voiceline;
	vTimers[g_ivTimers] = CreateDataTimer(rand, Timer_Vocalize, voiceline);

	/**
	 * I <3 SourcePawn and SourceMod
	 */
	voiceline.WriteCell(iMinigame);
	voiceline.WriteFloat(minDelay);
	voiceline.WriteFloat(maxDelay);
	voiceline.WriteCell(level);
	voiceline.WriteCellArray(clients, clients_length);

	ArrayList workaroundArray = new ArrayList(.blocksize = ByteCountToCells(PLATFORM_MAX_PATH));
	for (int i = 1; i <= lines_length; i++)
	{
		workaroundArray.PushString(lines[i - 1]);
	}
	voiceline.WriteCell(workaroundArray);

	g_ivTimers++;
	return (g_ivTimers - 1);
}

int AddRandVocalizationLoop(const ArrayList lines,
	int[] clients, int clients_length,
	float minDelay=4.0, float maxDelay=8.0, int level = SNDLEVEL_NORMAL)
{
	float rand = GetRandomFloat(minDelay, maxDelay);

	DataPack voiceline;
	vTimers[g_ivTimers] = CreateDataTimer(rand, Timer_Vocalize, voiceline);

	voiceline.WriteCell(iMinigame);
	voiceline.WriteFloat(minDelay);
	voiceline.WriteFloat(maxDelay);
	voiceline.WriteCell(level);
	voiceline.WriteCellArray(clients, clients_length);
	voiceline.WriteCell(lines);

	g_ivTimers++;
	return (g_ivTimers - 1);
}