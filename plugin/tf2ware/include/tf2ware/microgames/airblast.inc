// Airblast module

public Airblast_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Airblast_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Airblast_End);
    // Hopscotch game has the same exact check
    AddToForward(g_OnAlmostEnd, INVALID_HANDLE, Airblast_CheckDeadPlayers);
    
    SetStateAll(true);
    g_attack = true;
}

public Airblast_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_PreThink, Airblast_RemoveLeftClick);
    
        RemoveClientWeapons(client);
        SetClientClass(client, "pyro");
        GiveClientWeapon(client, "tf_weapon_flamethrower", 0, true);
        SetClientClass(client, "pyro");
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new total = GetActivePlayers();
        new posa = 360 / total * (g_Id[client]-1);
        
        decl Float:pos[3];
        pos[0] = 3544.661133   + (Cosine(DegToRad(float(posa)))*220.0);
        pos[1] = -160.203690  - (Sine(DegToRad(float(posa)))*220.0);
        pos[2] = -91.027405;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = float(180-posa);
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}

public Airblast_RemoveLeftClick(client) {
    new iButtons = GetClientButtons(client);
    if ((status == 2) && GetConVarBool(ww_enable) && g_enabled) {
        if ((iButtons & IN_ATTACK)) {
            iButtons &= ~IN_ATTACK;
            SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
        }
    }
}

public Airblast_CheckDeadPlayers() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (!(IsPlayerAlive(i))) && IsClientParticipating(i)) SetStateClient(i, false, true);
    }
}

public Airblast_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
    }
}