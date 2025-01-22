// Spycrab module

//#define SPRITE_SPYCRAB_SKY "materials/tf2ware/spycrab.vmt"

//new Spycrab_sprite_sky = -1;

public Spycrab_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Spycrab_justEntered);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, Spycrab_TimerMinigame);

    SetStateAll(true);
    
    /* NOTE: I couldn't get the sprite to face downwards. I don't know why, it makes no sense.
    //Let's put a cute little spycrab in the spy
    
    new Float:vOrigin[3];
    new Float:vAngle[3];
    
    vOrigin[0] = -5.0;
    vOrigin[1] = -5.0;
    vOrigin[2] = 470.0;
    vAngle[0] = 90.0;
    vAngle[1] = 0.0;
    vAngle[2] = 0.0;
    
    new ent = CreateEntityByName("env_sprite_oriented");
    if (IsValidEntity(ent)) {            
        DispatchKeyValue(ent, "classname", "env_sprite");
        DispatchKeyValue(ent, "spawnflags", "1");
        DispatchKeyValue(ent, "scale", "1.0");
        DispatchKeyValue(ent, "rendermode", "1");
        DispatchKeyValue(ent, "rendercolor", "255 255 255");
        DispatchKeyValue(ent, "model", SPRITE_SPYCRAB_SKY);
        DispatchSpawn(ent);
        TeleportEntity(ent, vOrigin, vAngle, NULL_VECTOR);
        Spycrab_sprite_sky = ent;
    } else {
        Spycrab_sprite_sky = -1;
    }*/
}

public Spycrab_TimerMinigame(timer) {
    if (timer == 3) {
        new Float:min;
        new Float:ang[3];
        new duck;
        
        for (new i = 1; i <= MaxClients; i++) {
            min = -35.0;
            if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
                if (GetEntProp(i, Prop_Send, "m_nPlayerCond") & 16) SetEntProp(i, Prop_Send, "m_nPlayerCond", 0);
                SetEntPropFloat(i, Prop_Send, "m_flCloakMeter", 0.0);
                TF2_RemovePlayerDisguise(i);
                
                duck = (GetEntData(i, FindSendPropOffs("CTFPlayer", "m_fFlags")) & 2);
              
                GetClientEyeAngles(i, ang);
                if (ang[0] > min || duck == 0) {
                    ForcePlayerSuicide(i);
                    SetStateClient(i, false, true);
                    // Explain to the client what he should've done so he isn't left clueless.
                    // We should probably add a translation file but I have no clue how those work D:
                    if (g_Country[i] == 1) {
                    if (ang[0] > min && duck == 0) CPrintToChat(i, "{default}Le spycrab {red}si abbassano {default}e {red}guardano su{default}!");
                    else if (ang[0] > min) CPrintToChat(i, "{default}Le spycrab {default}si abbassano e {red}guardano su{default}!");
                    else CPrintToChat(i, "{default}Le spycrab {red}si abbassano{default} e guardano su!");
                    }
                    else {
                        if (ang[0] > min && duck == 0) CPrintToChat(i, "{default}Spycrabs {red}crouch {default}and {red}look up{default}!");
                        else if (ang[0] > min) CPrintToChat(i, "{default}Spycrabs {default}crouch and {red}look up{default}!");
                        else CPrintToChat(i, "{default}Spycrabs {red}crouch{default} and look up!");
                    }
                }
            }
        }                
    }
}

public Spycrab_justEntered(client) {
    if (IsValidClient(client)) {
        RemoveClientWeapons(client);
        SetClientClass(client, "spy");
        GiveClientWeapon(client, "tf_weapon_pda_spy", 3, true);
    }
}