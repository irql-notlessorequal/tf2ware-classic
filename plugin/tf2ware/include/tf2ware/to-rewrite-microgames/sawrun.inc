// SawRun module

public SawRun_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, SawRun_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, SawRun_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, SawRun_OnGameFrame);
}

public SawRun_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            if (pos[1] > 1000.0) SetStateClient(i, true, true);
        }
    }
}

public SawRun_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "scout");
        DisableClientWeapons(client);
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new column = client;
        new row = 0;
        while (column > 9) {
            column = column - 9;
            row = row + 1;
        }
        
        decl Float:pos[3];
        pos[0] = 1780.0 + float(column*55);
        pos[1] = 290.0 - float(row*55);
        pos[2] = 130.0;

        decl Float:ang[3];
        ang[0] = 27.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
        NoCollision(true);
    }
}

public SawRun_End() {
}