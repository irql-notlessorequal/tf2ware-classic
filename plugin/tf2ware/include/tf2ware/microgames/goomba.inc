// Goomba module

new g_JumpManTeam = 2;

public Goomba_OnMinigame() {
    // These are forwards if you need a specific function!
    AddToForward(g_justEntered, INVALID_HANDLE, Goomba_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Goomba_End);
    
    g_JumpManTeam = GetRandomInt(2,3);
}


public Goomba_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_Touch, Goomba_Touch);
        
        if (g_JumpManTeam == GetClientTeam(client)) {
            SetClientClass(client, "scout");
            g_Mission[client] = 0;
        }
        else {
            SetClientClass(client, "heavy");
            g_Mission[client] = 1;
            SDKHook(client, SDKHook_PreThink, Goomba_RemoveJump);
            SetStateClient(client, true);
        }
        
        DisableClientWeapons(client);
    }
}

public Goomba_Touch(client, other) {
    if (status == 2) {
        decl String:edictName[32];
        GetEdictClassname(other, edictName, sizeof(edictName));

        if(StrEqual(edictName, "player") && IsValidClient(client) && IsValidClient(other) && IsPlayerAlive(client) && IsPlayerAlive(other) && g_Mission[client] == 0 && g_Mission[other] == 1 && GetClientTeam(client) != GetClientTeam(other)) {
            decl Float:ClientPos[3];
            decl Float:VictimPos[3];
            GetClientAbsOrigin(client, ClientPos);
            GetClientAbsOrigin(other, VictimPos);

            new Float:HeightDiff = ClientPos[2] - VictimPos[2];
            
            if(HeightDiff >= 81 && HeightDiff <= 84) {
                decl Float:vec[3];
                GetEntPropVector(client, Prop_Data, "m_vecAbsVelocity", vec);

                if(vec[2] < -10.0) {
                    SetStateClient(other, false, true);
                    ForcePlayerSuicide(other);
                    SetStateClient(client, true, true);
                    
                    decl Float:vecAng[3], Float:vecVel[3];
                    GetClientEyeAngles(client, vecAng);
                    vecAng[0] = DegToRad(vecAng[0]);
                    vecAng[1] = DegToRad(vecAng[1]);
                    vecVel[0] = 300.0*Cosine(vecAng[0])*Cosine(vecAng[1]);
                    vecVel[1] = 300.0*Cosine(vecAng[0])*Sine(vecAng[1]);
                    vecVel[2] = 300.0+100.0;
                    
                    TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecVel);
                }
            }
        } 
    }
}

public Goomba_RemoveJump(client) {
    if ((status == 2) && GetConVarBool(ww_enable) && g_enabled) {
        new iButtons = GetClientButtons(client);
        if ((iButtons & IN_JUMP)) {
            iButtons &= ~IN_JUMP;
            SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
        }
    }
}

public Goomba_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKUnhook(i, SDKHook_Touch, Goomba_Touch);
            SDKUnhook(i, SDKHook_PreThink, Goomba_RemoveJump);
        }
    }
}