// Hugging module

new String:var_heavy_love[][] = {"imgay/tf2ware/heavy_ilu.wav", "vo/heavy_specialcompleted08.wav", "vo/heavy_award04.wav"};

public Hugging_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Hugging_OnMapStart);
}   

public Hugging_OnMapStart() {
    decl String:input[512];
    for (new i = 1; i <= sizeof(var_heavy_love); i++) {
        Format(input, sizeof(input), "sound/%s", var_heavy_love[i-1]);
        AddFileToDownloadsTable(input);
        precacheSound(var_heavy_love[i-1]);
    }
}

public Hugging_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Hugging_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Hugging_End);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Hugging_PlayerDeath);

    new AllPlayers = GetClientCount(true);
    new target = 0;
    new attempts = 0;
    new heavies = GetActivePlayers() / 4;
    if (heavies <= 1) heavies = 2;
    if (heavies >= 4) heavies = 3;
    new heavyclients[MAXPLAYERS+1];
    new scoutclients[MAXPLAYERS+1];
    for (new i = 1; i <= heavies; i++) {
        attempts = 0;
        target = GetRandomInt(1, AllPlayers);
        while (((!IsValidClient(target)) || IsClientParticipating(target) == false || g_Spawned[target] == false || IsFakeClient(target) || g_Mission[target] > 0) && attempts <= 50) {
            target = GetRandomInt(1, AllPlayers);
            attempts += 1;
        }
        if (attempts <= 50) {
            g_Mission[target] = 1;
        }
    }
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i)) {
            if (g_Mission[i] == 0) {
                SetStateClient(i, true);
                scoutclients[i-1] = i;
            }
            else {
                SetStateClient(i, false);
                heavyclients[i-1] = i;
            }
        }
    }
    
    AddRandVocalization("heaviestaunt", heavyclients, 4.0, 8.0);
    AddRandVocalization("scoutdisgust", scoutclients, 1.5, 5.0);
    
    NoCollision(false);
    
}

public Action:Hugging_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (status == 2) {
        if (damage > 0) damage = 1.0;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public Hugging_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_Touch, Hugging_OnPlayerTouch);
    }
}

public Hugging_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_Touch, Hugging_OnPlayerTouch);
    
        decl Float:pos[3];
        decl Float:ang[3];
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        new column = client;
        new row = 0;
        while (column > 10) {
            column = column - 10;
            row = row + 1;
        }
        if (g_Mission[client] == 0) {
            if (GetClientTeam(client) == 2) ChangeClientTeamAlive(client, 3);
            SetClientClass(client, "scout");
            DisableClientWeapons(client);
        
            pos[0] = 142.0 - float(column*80);
            pos[1] = -2040.0 + float(row*80);
            pos[2] = -700.0;
            ang[0] = 0.0;
            ang[1] = -90.0;
            ang[2] = 0.0;
        }//m_iTotalScore
        if (g_Mission[client] == 1) {
            if (GetClientTeam(client) == 3) ChangeClientTeamAlive(client, 2);
            SetClientClass(client, "heavy");
            DisableClientWeapons(client);
            pos[0] = 142.0 - float(column*80);
            pos[1] = -2965.0 - float(row*80);
            pos[2] = -700.0;
            ang[0] = 0.0;
            ang[1] = 90.0;
            ang[2] = 0.0;
        }
        TeleportEntity(client, pos, ang, vel);
    }
}

public Hugging_OnPlayerTouch(entity, other) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Hugging")) return;

    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && (g_Mission[other] > 0 || g_Mission[entity] > 0)) {
        if (g_Mission[entity] > 0 && g_Mission[other] > 0) return;
        new attacker = entity;
        new victim = other;
        if (g_Mission[other] > 0) {
            attacker = other;
            victim = entity;
        }
        ForcePlayerSuicide(victim);
        EmitSoundToAll(SOUND_HEAVY_KISS, attacker);
        EmitSoundToClient(victim, SOUND_HEAVY_KISS);
    }
}

public Hugging_PlayerDeath(client) {
    SetStateClient(client, false, true);
    
    CreateTimer(0.1, Hugging_Check);
}

public Action:Hugging_Check(Handle:timer) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Ghostbusters")) return;

    new bool:complete = true;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (g_Mission[i] == 0) && IsClientParticipating(i) && IsPlayerAlive(i)) complete = false;
    }
    if (complete) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (g_Mission[i] == 1) && IsClientParticipating(i)) SetStateClient(i, true, true);
        }
    }
}