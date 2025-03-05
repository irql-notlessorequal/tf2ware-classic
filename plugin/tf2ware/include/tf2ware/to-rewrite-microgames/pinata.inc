// Pinata module

public Pinata_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Pinata_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Pinata_End);
    //AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Pinata_OnGameFrame);

    // Should the person be able to attack?
    g_attack = true;
    
    //The below function needs to be there if you're planning on giving clients a weapon!
    RemoveAllWeapons();

    randommini = GetRandomInt(0, 2);
    
    new AllPlayers = GetClientCount(true);
    new target = GetRandomInt(1, AllPlayers);
    while ((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || !IsPlayerAlive(target))
    {
        target = GetRandomInt(1, AllPlayers);
    }

    g_Mission[target] = 1;
    g_result = 0;
    SetStateAll(true);
}


public Pinata_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i))
        {
            SDKUnhook(i, SDKHook_OnTakeDamage, Pinata_OnTakeDamageClient);
        }
    }
}


public Pinata_justEntered(client) {

    if (IsValidClient(client)) {
        if (g_Mission[client] == 0) {
            SetClientClass(client, "heavy");
            DisableClientWeapons(client);
        }
        if (g_Mission[client] == 1) {
            SetClientClass(client, "scout");
            GiveClientWeapon(client, "tf_weapon_bat", 2, true);
        }
    }


    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_OnTakeDamage, Pinata_OnTakeDamageClient);
    }
}

/*
public Pinata_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
        }
    }
}
*/

public Action:Pinata_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(attacker)) {
        SetStateClient(attacker, true, true);
        SetStateClient(victim, false, true);
        damage = 150.0;
    }
    return Plugin_Changed;
}