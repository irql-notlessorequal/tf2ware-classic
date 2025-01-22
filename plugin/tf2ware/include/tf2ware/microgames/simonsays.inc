// SimonSays module

#define A_TAUNT 0
#define A_JUMP 1
#define A_CROUCH 2
#define A_LOOKUP 3
#define A_LOOKDOWN 4

// simon_action == 0 | Taunt
// simon_action == 1 | Jump
// simon_action == 2 | Crouch
// simon_action == 3 | Look Up
// simon_action == 4 | Look Down

// NEED TO ALLOW BUILDINGS

new simon_action;
new simon_or_someone;

public SimonSays_OnMinigame() {
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, SimonSays_OnGameFrame);

    simon_action = GetRandomInt(0,2);
    simon_or_someone = GetRandomInt(0,1);
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            g_Mission[i] = simon_action * 2 + simon_or_someone;
        }
    }
    
    // If simon says it (simon_or_someone == 0) set state to false until proven true
    // Otherwise, set to true until proven false
    SetStateAll(simon_or_someone == 1);
}

public SimonSays_OnGameFrame() {
    if (timeleft <= 6) {
        for (new i = 1; i <= MaxClients; i++) {
            // If the player is valid and taunting
            if(IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
                if (IsPlayerDoing(i)) {
                    if (simon_or_someone == 0) {
                    // If simon says it
                        SetStateClient(i, true, true);
                    }
                    else if (g_Complete[i]) {
                    // Otherwise, simon didn't say it and if we still haven't lost
                        new String:input[512];
                        // Display "Simon didn't say it!"
                        Format(input, sizeof(input), "tf2ware_minigame_%d_11", GetMinigameConfNum("SimonSays", "id"));
                        SetOverlay(i, input);
                        SetStateClient(i, false, true);
                    }
                }
            }
        }
    }
}

IsPlayerDoing(client) {
    switch (simon_action) {
        case A_TAUNT: {
            if (TF2_IsPlayerInCondition(client, TFCond_Taunting)) {
                return true;
            }
        }
        case A_JUMP: {
            if (GetClientButtons(client) & IN_JUMP) {
                return true;
            }
        }
        case A_CROUCH: {
            if (GetClientButtons(client) & IN_DUCK) {
                return true;
            }
        }
    }
    return false;
}