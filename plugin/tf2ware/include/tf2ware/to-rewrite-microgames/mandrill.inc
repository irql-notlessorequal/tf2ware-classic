// Mandrill module

public Mandrill_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Mandrill_justEntered);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Mandrill_OnGameFrame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Mandrill_End);

public Mandrill_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "sniper");
        DisableClientWeapons(client);
        
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
        
        decl Float:pos[3];
        pos[0] = -2453.0  - float(column*55);
        pos[1] = -1833.0 - float(row*55);
        pos[2] = 17.6;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 270.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}

public Mandrill_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            if (g_Mandrill_arena == 0 && pos[1] > 2235.0  && g_Complete[i] == false ) SetStateClient(i, true, true);
            if (g_Mandrill_arena == 1 && pos[2] > -80.0 && pos[1] > 4270.0 && g_Complete[i] == false ) SetStateClient(i, true, true);
            if (g_Mandrill_arena == 1) SetEntityHealth(i, 200);
        }
    }
}

public Mandrill_End() {
    for (new i = 1; i <= MaxClients; i++) {
    }
}