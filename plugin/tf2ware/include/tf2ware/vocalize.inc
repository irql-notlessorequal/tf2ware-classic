// Support for game Vocalizations

new Handle:vTimers[MAXPLAYERS+1] = INVALID_HANDLE;
new g_ivTimers = 0;

public Action:Timer_Vocalize(Handle:hTimer, any:pack) {
    // If the Vocalization is succesful make a new timer, otherwise don't
    ResetPack(pack);
    new String:id[24];
    new game = ReadPackCell(pack);
    if (iMinigame == game && status == 2) {
        ReadPackString(pack, id, sizeof(id));
        new Float:mindelay = ReadPackFloat(pack);
        new Float:maxdelay = ReadPackFloat(pack);
        new level = ReadPackCell(pack);
        
        new clients[MAXPLAYERS+1];
        new i=0;
        while(IsPackReadable(pack, 1)) {
            clients[i] = ReadPackCell(pack);
            i++;
        }
        
        new Handle:sounds = CreateArray(128);
        KvJumpToKey(MinigameConf, minigame);
        KvJumpToKey(MinigameConf, id);
        KvGotoFirstSubKey(MinigameConf);
        new String:buffer[128];
        new String:number[4];
        i=1;
        IntToString(i, number, sizeof(number));
        while (KvJumpToKey(MinigameConf, number)) {
            KvGoBack(MinigameConf);
            KvGetString(MinigameConf, number, buffer, sizeof(buffer));
            PushArrayString(sounds, buffer);
            i++;
            IntToString(i, number, sizeof(number));
        }
        KvRewind(MinigameConf);
        
        if (GetArraySize(sounds) > 0) {
            GetArrayString(sounds, GetRandomInt(0, GetArraySize(sounds)-1), buffer, sizeof(buffer));
            CloseHandle(sounds);
            
            new client = RollFromValid(clients);
            // RollFromValid returns -1 if there are no clients to choose from
            if (client != -1) {
                EmitSoundToAll(buffer, client, SNDCHAN_AUTO, level);
            }        
            AddRandVocalization(id, clients, mindelay, maxdelay, level);
        }
    }
    return Plugin_Stop;
}

RollFromValid(clients[MAXPLAYERS+1]) {
    new Handle:list = CreateArray();
    for (new i=0; i<sizeof(clients); i++) {
        if (IsValidClient(clients[i]) && IsPlayerAlive(clients[i])) {
            PushArrayCell(list, clients[i]);
        }
    }
    new result;
    // Check to avoid "invalid index 0" if array size is <1
    if (GetArraySize(list) > 0) {
        result = GetArrayCell(list, GetRandomInt(0, GetArraySize(list)-1));
    }
    else result = -1;
    CloseHandle(list);
    return result;
}

CleanupAllVocalizations() {
    g_ivTimers = 0;
}

// Never being used. Disabled until.
/*StopVocalization(id) {
    CloseHandle(vTimers[id]);
}*/

AddRandVocalization(String:strid[], clients[MAXPLAYERS+1], Float:mindelay=4.0, Float:maxdelay=8.0, level = SNDLEVEL_NORMAL) {
    new Float:rand = GetRandomFloat(mindelay, maxdelay);
    new Handle:pack = CreateDataPack();
    vTimers[g_ivTimers] = CreateDataTimer(rand, Timer_Vocalize, pack);
    WritePackCell(pack, iMinigame);
    WritePackString(pack, strid);
    WritePackFloat(pack, mindelay);
    WritePackFloat(pack, maxdelay);
    WritePackCell(pack, level);
    for (new i=0; i<sizeof(clients); i++) {
        if (IsValidClient(clients[i])) {
            WritePackCell(pack, clients[i]);
        }
    }    
    g_ivTimers++;
    return g_ivTimers-1;
}