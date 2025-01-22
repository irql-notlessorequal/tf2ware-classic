// ColorText module
// Thanks to gNatFreak for the idea

// gNatFreak: Microgame where:
// A color comes up in the form of a word on the screen (i.e. “RED”), but the word “RED” is colored say “GREEN”. You must type the color the word is in, before time 
// runs out (and you wouldn’t have much time).

// More examples:
// Word on screen: “Green”. (colored blue), type “blue” in chat within the time limit.
// Word: “orange”. (colored purple), type purple in chat within time limit.

// Time limits would have to be pretty quick. It’s possible abbreviations could be used (i.e. make it so just the first letter of the color it is counts, but the 
// time you have to type that letter is around 2 seconds). So:
// Word: Grey (colored black), type “b” into chat. Care would have to be taken to prevent duplicate colors starting with the same letter.

static g_iColors[8][3] = {{255, 255, 255}, {255, 0, 0}, {0, 255, 0}, {32, 128, 255}, {255, 255, 0}, {128, 64, 255}, {255, 128, 0}, {255, 75, 128}};
static String:g_sColors[8][12] = {"WHITE", "RED", "GREEN", "BLUE", "YELLOW", "PURPLE", "ORANGE", "PINK"};

new written_color;
new shown_color;
new color_target; // either written or shown

new Handle:Colortext_sync = INVALID_HANDLE;

public ColorText_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, ColorText_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, ColorText_End);

    written_color = GetRandomInt(0,sizeof(g_sColors)-1);
    shown_color = GetRandomInt(0,sizeof(g_sColors)-1);
    if (GetRandomInt(0, 1) == 1) {
        color_target = written_color;
        SetMissionAll(0);
    }
    else {
        color_target = shown_color;
        SetMissionAll(1);
    }
    
    RegConsoleCmd("say", ColorText_Player_Say);
    RegConsoleCmd("say_team", ColorText_Player_Say);
    
    Colortext_sync = CreateHudSynchronizer();
}

public ColorText_justEntered(client) {
    SetHudTextParams(-1.0, 0.45, 5.0, g_iColors[shown_color][0], g_iColors[shown_color][1], g_iColors[shown_color][2], 255);
    if (IsValidClient(client)) {
        DisableClientWeapons(client);
        ShowSyncHudText(client, Colortext_sync, g_sColors[written_color]);
    }
}

public ColorText_End() {
    CloseHandle(Colortext_sync);
}

public Action:ColorText_Player_Say(iClient, iArgs) {
    if (!StrEqual(minigame, "ColorText")) return Plugin_Continue;
    if (!(IsValidClient(iClient))) return Plugin_Continue;
    if (!(IsClientParticipating(iClient))) return Plugin_Continue;
    if (iArgs < 1) return Plugin_Continue;
    
    if ((IsPlayerAlive(iClient)) && (status == 2) && (g_Complete[iClient] == false)) {
        // Retrieve the first argument and check it's the correct color
        decl String:strArgument[64]; GetCmdArg(1, strArgument, sizeof(strArgument));
        
        if (!strncmp(g_sColors[color_target], strArgument, 2, false)) {
            SetStateClient(iClient, true, true);
            return Plugin_Handled;
        }
        else {
            ForcePlayerSuicide(iClient);
        }
    }
    return Plugin_Continue;
}