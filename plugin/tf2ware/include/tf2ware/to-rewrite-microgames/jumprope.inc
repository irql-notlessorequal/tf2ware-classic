// JumpRope module

new jumprope_speed = 0;
new jumprope_drainhp = 8;

public JumpRope_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, JumpRope_justEntered);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, JumpRope_PlayerDeath);
    
    SetStateAll(true);
    
    jumprope_speed = 0;
    CreateTimer(5.0, JumpRope_NewRound);
    CreateTimer(0.5, JumpRope_Drain);
}

public JumpRope_justEntered(client) {
    if (IsValidClient(client)) {    
        SetClientClass(client, "pyro");
        DisableClientWeapons(client);
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new column = client;
        new row = 0;
        while (column > 17) {
            column = column - 17;
            row = row + 1;
        }
        
        decl Float:pos[3];
        pos[0] = -44.0  - float(column*55);
        pos[1] = 2651.0 - float(row*55);
        pos[2] = -300.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}

public Action:JumpRope_NewRound(Handle:timer) {
    if (!StrEqual(minigame, "JumpRope")) return;
    if (status != 2) return;
    jumprope_speed += 1;
    
    new Float:timescale = (GetHostMultiplier(1.0) + float(jumprope_speed)*0.05);
    ServerCommand("host_timescale %f", timescale);
    CreateTimer((5.0 / timescale), JumpRope_NewRound);
    
    new activeplayers = 0;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && g_Complete[i] && IsClientParticipating(i)) {
            activeplayers += 1;
        }
    }
    
    if (activeplayers <= 1) TriggerTimer(microgametimer);
    
}


public Action:JumpRope_Drain(Handle:timer) {
    if (!StrEqual(minigame, "JumpRope")) return;
    if (status != 2) return;
    for(new i = 1; i <= MaxClients; i++) {
        if(IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
            new health = GetClientHealth(i);
            if (health > jumprope_drainhp) SetEntityHealth(i, health - jumprope_drainhp);
            else ForcePlayerSuicide(i);
        }
    }
    
    new Float:timescale = (GetHostMultiplier(1.0) + float(jumprope_speed)*0.05);
    CreateTimer(1.0 / timescale, JumpRope_Drain);
}

public JumpRope_PlayerDeath(client) {
        SetStateClient(client, false);
}