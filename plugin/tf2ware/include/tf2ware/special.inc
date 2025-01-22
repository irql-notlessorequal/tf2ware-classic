/*
    1:      Speed Up        Everything is faster
    2:      Don't Touch     Touch other players and fail
    3:      x2 Bosses       Two bosses
    4:      Singleplayer    Everyone is invisible
    5:      Worst is Best   The person with the least points win
    6:      Bonk            Everyone gets a bat to push people with    
    7:      Thirdperson     Forced into thirdperson!    

*/

#define SPECIAL_TOTAL 7

new bool:Special_TwoBosses = false;

new String:var_special_name[][] = {"SUPER SPEED", "NO TOUCHING", "x2 BOSS BATTLE", "SINGLEPLAYER", "LEAST IS BEST", "BONK", "THIRDPERSON"};
new String:var_special_desc[][] = {"The Game is Faster Than Usual!!", "Don't Touch Any Enemy Players or You'll Lose!!", "Two Boss Battles!!", "You're Playing Alone...?", "The Player With The Least Points Win!!", "Push Players With Your Bat!!", "Everything Is At A New Perspective!!"};


public Special_NoTouch(entity, other) {
    if (SpecialRound != 2) return;
    if (StrEqual(minigame, "Hugging")) return;

    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && GetClientTeam(entity) != GetClientTeam(other)) {
        SetStateClient(entity, false, true);
        SetStateClient(other, false, true);
        ForcePlayerSuicide(entity);
        ForcePlayerSuicide(other);
        CPrintToChatEx(entity, other, "You touched: {teamcolor}%N{default}", other);
        CPrintToChat(entity, "In this SPECIAL ROUND, you are {red}not allowed to touch anyone{default}!");
        CPrintToChatEx(other, entity, "You touched: {teamcolor}%N{default}", entity);
        CPrintToChat(other, "In this SPECIAL ROUND, you are {red}not allowed to touch anyone{default}!");
    }
}


public Action:Special_DamagePush(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(attacker) && IsValidClient(victim) && SpecialRound == 6) {
        decl String:Weapon[128];
        GetClientWeapon(attacker, Weapon, sizeof(Weapon));
        if (StrEqual(Weapon, "tf_weapon_bat")) {
            decl Float:ang[3];
            decl Float:vel[3];
            GetClientEyeAngles(attacker, ang);
            vel[0] -= 300.0 * Cosine(DegToRad(ang[1])) * -1.0;
            vel[1] -= 300.0 * Sine(DegToRad(ang[1])) * -1.0;
            vel[2] += 300.0;
            TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, vel);
            damage = 0.0;
        }
    }
    return Plugin_Changed;
}

ResetSpecialRoundEffect() {
    if (SpecialRound == 2) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) SDKUnhook(i, SDKHook_Touch, Special_NoTouch);
        }
    }
    if (SpecialRound == 6) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) SDKUnhook(i, SDKHook_OnTakeDamage, Special_DamagePush);
        }
    }
    if (SpecialRound == 4) {
        NoCollision(false);
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                SetEntityRenderColor(i, 255, 255, 255, 255);
                SetEntityRenderMode(i, RENDER_NORMAL);
            }
        }
    }
    
    if (SpecialRound == 7) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && !IsFakeClient(i)) ClientCommand(i, "wait; firstperson");
        }
    }
}

AddSpecialRoundEffect() {
    if (SpecialRound == 2) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) SDKHook(i, SDKHook_Touch, Special_NoTouch);
        }
    }
    if (SpecialRound == 3) Special_TwoBosses = false;
    if (SpecialRound == 4) {
        NoCollision(true);
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                SetEntityRenderColor(i, 255, 255, 255, 0);
                SetEntityRenderMode(i, RENDER_NONE);
            }
        }
    }
    if (SpecialRound == 6) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) SDKHook(i, SDKHook_OnTakeDamage, Special_DamagePush);
        }
    }
    if (SpecialRound == 7) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && !IsFakeClient(i)) ClientCommand(i, "wait; thirdperson");
        }
    }
}


SpecialPrecache() {
}