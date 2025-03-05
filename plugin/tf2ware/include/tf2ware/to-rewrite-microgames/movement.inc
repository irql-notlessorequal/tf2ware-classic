// Movement module

public Movement_OnMinigame() {
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Movement_OnGameFrame);
    
    SetStateAll(true);
    new mission = GetRandomInt(0,1);
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i)) {
            g_Mission[i] = mission;
        }
    }
}

public Movement_OnGameFrame() {
    if (timeleft <= 5) {
        decl Float:fVelocity[3];
        decl Float:speed;
        decl Float:limit;
        speed = 0.0;
        limit = 0.0;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
                limit = GetEntPropFloat(i, Prop_Send, "m_flMaxspeed") - 100.0;
                GetEntPropVector(i, Prop_Data, "m_vecVelocity", fVelocity);
                speed = GetVectorLength(fVelocity);
                if ((g_Mission[i] == 0) && (speed < limit) && (g_Complete[i]) && (IsPlayerAlive(i))) {
                    SetStateClient(i, false, true);
                    ForcePlayerSuicide(i);
                }
                if ((g_Mission[i] >= 1) && (speed > 100.0) && (g_Complete[i]) && (IsPlayerAlive(i))) {
                    SetStateClient(i, false, true);
                    ForcePlayerSuicide(i);
                }
            }
        }
    }
}
