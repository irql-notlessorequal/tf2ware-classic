// TEMPLATE module

#define FROGGER_MODEL "models/props_2fort/frog.mdl"

new g_Frogger_Timer = 0;
new bool:g_Frogger_Goal = false;
new g_Frogger_Pos[MAXPLAYERS+1] = 0;

public Frogger_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Frogger_OnMapStart);
}

public Frogger_OnMinigame() {
    // These are forwards if you need a specific function!
    AddToForward(g_justEntered, INVALID_HANDLE, Frogger_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Frogger_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Frogger_OnGameFrame);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Frogger_PlayerDeath);
    
    //SetCameraState(true);
    NoCollision(true);
    
    g_Frogger_Timer = 11;
    g_Frogger_Goal = false;
    
    for (new i = 1; i <= MaxClients; i++) {
        g_Frogger_Pos[i] = 0;
    }
}

public Frogger_OnMapStart() {
    PrecacheModel(FROGGER_MODEL, true);
    precacheSound("vo/announcer_ends_5sec.wav");
    precacheSound("vo/announcer_ends_4sec.wav");
    precacheSound("vo/announcer_ends_3sec.wav");
    precacheSound("vo/announcer_ends_2sec.wav");
    precacheSound("vo/announcer_ends_1sec.wav");
}

public Action:Frogger_Timer(Handle:hTimer) {
    if (status == 2) {
        g_Frogger_Timer -= 1;
        
        if (g_Frogger_Timer > 0) {
            decl String:sound[512];
            Format(sound, sizeof(sound), "vo/announcer_ends_%dsec.wav", g_Frogger_Timer);
            if (GetConVarBool(ww_music) == true) EmitSoundToClient(1, sound);
            else EmitSoundToAll(sound);
            if (g_Frogger_Timer < 5) CPrintToChatAll("{red}%d{default}!", g_Frogger_Timer);
            CreateTimer(1.0, Frogger_Timer);
        }
        else {
            if (microgametimer != INVALID_HANDLE) TriggerTimer(microgametimer);
        }
    }
    return Plugin_Handled;
}


public Frogger_End() {
    // Let's turn them back
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SetVariantString("");
            AcceptEntityInput(i, "SetCustomModel");
            SetVariantInt(0);
            AcceptEntityInput(i, "SetCustomModelRotates");
            ClientCommand(i, "wait; firstperson");
        }
    }
}

public Frogger_justEntered(client) {
    if (IsValidClient(client)) {
    
        // Change the client's class?
        SetClientClass(client, "medic");
        DisableClientWeapons(client);
        
        // Turn him into a frog!
        SetVariantString(FROGGER_MODEL);
        AcceptEntityInput(client, "SetCustomModel");
        SetVariantInt(1);
        AcceptEntityInput(client, "SetCustomModelRotates");
        //SetEntProp(client, Prop_Send, "m_iObserverMode", 1);
        
        Frogger_Position(client);
        NoCollision(true);
        
        CreateTimer(0.1, Frogger_Ragdoll, client);
    }
}

Frogger_Position(client) {
    decl Float:vel[3];
    vel[0] = 0.0;
    vel[1] = 0.0;
    vel[2] = 0.0;
    
    decl Float:ang[3];
    ang[0] = 0.0;
    ang[1] = 90.0;
    ang[2] = 0.0;
    
    decl Float:pos[3]; 
    
    new checkpoint = g_Frogger_Pos[client];

    
    if (checkpoint == 99) {
        pos[0] = 8726.0;
        pos[1] = 3947.0;
        pos[2] = 455.0;
    }
    if (checkpoint == 8) {
        pos[0] = 9468.0;
        pos[1] = 2935.0;
        pos[2] = 293.0;
        ang[1] = 0.0;
    }
    if (checkpoint == 7) {
        pos[0] = 8932.0;
        pos[1] = 2831.0;
        pos[2] = 293.0;
        ang[1] = 0.0;
    }
    if (checkpoint == 6) {
        pos[0] = 8020.0;
        pos[1] = 2873.0;
        pos[2] = 293.0;
        ang[1] = 0.0;
    }
    if (checkpoint == 5) {
        pos[0] = 8654.0;
        pos[1] = 1873.0;
        pos[2] = 195.0;
    }
    if (checkpoint == 4) {
        pos[0] = 8654.0;
        pos[1] = 1322.0;
        pos[2] = 195.0;
    }
    if (checkpoint == 3) {
        pos[0] = 8615.0;
        pos[1] = 603.0;
        pos[2] = 191.0;
    }
    if (checkpoint == 2) {
        pos[0] = 8615.0;
        pos[1] = -209.0;
        pos[2] = 191.0;
    }
    if (checkpoint == 1) {
        pos[0] = 8654.0;
        pos[1] = -1566.0;
        pos[2] = 223.0;
    }
    if (checkpoint == 0) {
        new column = client;
        new row = 0;
        while (column > 7) {
            column = column - 7;
            row = row + 1;
        }
        
        pos[0] = 8497.0  + float(column*55);
        pos[1] = -3438.0 + float(row*55);
        pos[2] = 216.0;
    }

    TeleportEntity(client, pos, ang, vel);
    ClientCommand(client, "wait; thirdperson");

}

public Frogger_PlayerDeath(client) {
    if (IsValidClient(client) && status == 2) {
        SetVariantString("");
        AcceptEntityInput(client, "SetCustomModel");
        SetVariantInt(0);
        AcceptEntityInput(client, "SetCustomModelRotates");
        SetEntProp(client, Prop_Send, "m_iObserverMode", 0);
        CreateTimer(1.0, Frogger_Respawn, client);
        
        CreateTimer(0.0, Frogger_Ragdoll, client);
    }
}

public Action:Frogger_Ragdoll(Handle:timer, any:client) {
    if(IsValidClient(client)) {
        new rag = GetEntPropEnt(client, Prop_Send, "m_hRagdoll");
        if (rag > MaxClients && IsValidEntity(rag))
            AcceptEntityInput(rag, "Kill");
    }
    return Plugin_Handled;
}

public Action:Frogger_Respawn(Handle:hTimer, any:client) {
    if (status == 2) {
        RespawnClient(client, true, false);
    }
}

public Frogger_OnGameFrame() {
    if (status != 2) return;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            
            // checkpoints
            if (pos[1] >= -1566.0 && g_Frogger_Pos[i] < 1) g_Frogger_Pos[i] = 1;
            if (pos[1] >= -209.0 && g_Frogger_Pos[i] < 2) g_Frogger_Pos[i] = 2;
            if (pos[1] >= 603.0 && g_Frogger_Pos[i] < 3) g_Frogger_Pos[i] = 3;
            if (pos[1] >= 1322.0 && g_Frogger_Pos[i] < 4) g_Frogger_Pos[i] = 4;
            if (pos[1] >= 1873.0 && g_Frogger_Pos[i] < 5) g_Frogger_Pos[i] = 5;
            if (pos[1] >= 2831.0 && pos[0] >= 8020.0 && g_Frogger_Pos[i] < 6) g_Frogger_Pos[i] = 6;
            if (pos[1] >= 2831.0 && pos[0] >= 8932.0 && g_Frogger_Pos[i] < 7) g_Frogger_Pos[i] = 7;
            if (pos[1] >= 2935.0 && pos[0] >= 9468.0 && g_Frogger_Pos[i] < 8) g_Frogger_Pos[i] = 8;
            
            // goal
            if (pos[1] > 3751.0 && g_Complete[i] == false ) {
                new bool:bWin = true;
                if (g_Gamemode == GAMEMODE_WIPEOUT && g_Frogger_Goal) bWin = false;
                
                if (bWin) {
                    SetStateClient(i, true, true);
                    
                    CPrintToChatAllEx(i, "{teamcolor}%N{default} made it to the goal first!", i);
                    g_Frogger_Pos[i] = 99;
                    if (!g_Frogger_Goal) {
                        if (g_Gamemode == GAMEMODE_WIPEOUT) {
                            CreateTimer(0.1, Frogger_Timer);
                            g_Frogger_Timer = 0;
                        } else {
                            CreateTimer(0.0, Frogger_Timer);
                            CPrintToChatAll("The boss will end in {red}10 seconds{default}!");
                        }
                    }
                    g_Frogger_Goal = true;
                }
            }
        }
    }
}