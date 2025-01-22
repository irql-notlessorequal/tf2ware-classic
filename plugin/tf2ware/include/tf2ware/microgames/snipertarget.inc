new var_sniper_target = -1;

// NEED TO MODIFY SO THAT IT IS A SHOOTING RANGE

#define SNIPER_TARGET "models/props_training/target_heavy.mdl"
#define SNIPER_BASEPOS 160

new g_Target_Pos = 0;
new g_Target_Random = 0;
new bool:g_Target_Ready = false;

public SniperTarget_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, SniperTarget_OnMapStart);
}   

public SniperTarget_OnMapStart() {
    PrecacheModel(SNIPER_TARGET, true);
}

public SniperTarget_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, SniperTarget_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, SniperTarget_End);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, SniperTarget_TimerMinigame);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, SniperTarget_OnGameFrame);

    g_attack = true;
    
    g_Target_Random = GetRandomInt(0,4);
    g_Target_Pos = 0;
    g_Target_Ready = false;
}

public SniperTarget_TimerMinigame(timer) {
    if (timer == 6) {
        var_sniper_target = CreateEntityByName("prop_physics");
        
        if (IsValidEdict(var_sniper_target)) {                      
            decl String:model[128];
            Format(model, sizeof(model), SNIPER_TARGET);
            
            DispatchKeyValue(var_sniper_target, "model", model);
            DispatchSpawn(var_sniper_target);
            SetEntityMoveType(var_sniper_target, MOVETYPE_NONE);
            
            SDKHook(var_sniper_target, SDKHook_OnTakeDamage, SniperTarget_Hurt2);
            
            g_Target_Ready = true;
            
            Sniper_SetPos();
        }
    }
    if (timer == 2) {
        g_Target_Ready = false;
    }
}

Sniper_SetPos() {
    decl Float:pos[3];
    decl Float:ang[3];
    ang[0] = 0.0;
    ang[1] = 270.0;
    ang[2] = 0.0;
    
    pos[2] = float(SNIPER_BASEPOS + g_Target_Pos);
   
    if (g_Target_Random == 0) {
        pos[0] = 3338.0;
        pos[1] = 2223.0;
    }
    if (g_Target_Random == 1) {
        pos[0] = 4119.0;
        pos[1] = 2240.0;
    }
    if (g_Target_Random == 2) {
        pos[0] = 3690.0;
        pos[1] = 2326.0;
    }
    if (g_Target_Random == 3) {
        pos[0] = 3226.0;
        pos[1] = 2501.0;
    }
    if (g_Target_Random == 4) {
        pos[0] = 4191.0;
        pos[1] = 2558.0;
    }
    
    if (IsValidEdict(var_sniper_target)) TeleportEntity(var_sniper_target, pos, ang, NULL_VECTOR);
}

public SniperTarget_OnGameFrame() {
    if (g_Target_Ready && g_Target_Pos < 100) {
        g_Target_Pos += 6;
        Sniper_SetPos();
    }
    if (g_Target_Ready == false && g_Target_Pos > 0) {
        g_Target_Pos -= 6;
        Sniper_SetPos();
    }
}

public SniperTarget_End() {
    if (var_sniper_target >= 0 && IsValidEntity(var_sniper_target)) {
        SDKUnhook(var_sniper_target, SDKHook_OnTakeDamage, SniperTarget_Hurt2);
        AcceptEntityInput(var_sniper_target, "Kill" );
        var_sniper_target = -1;
    }
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, SniperTarget_Hurt);
    }
}

public Action:SniperTarget_Hurt(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    damage = 0.0;
    return Plugin_Changed;
}

public Action:SniperTarget_Hurt2(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    damage = 0.0;
    if (IsValidClient(attacker)) SetStateClient(attacker, true, true);
    return Plugin_Changed;
}

public SniperTarget_justEntered(client) {
    if (IsValidClient(client)) {
        RemoveClientWeapons(client);
        SetClientClass(client, "sniper");
        GiveClientWeapon(client, "tf_weapon_sniperrifle", 0, true);
        SetClientClass(client, "sniper");
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new column = client;
        new row = 0;
        while (column > 24) {
            column = column - 24;
            row = row + 1;
        }
        
        decl Float:pos[3];
        pos[0] = 2958.0  + float(column*55);
        pos[1] = 806.0 - float(row*55);
        pos[2] = 270.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
        
        // No damage
        SDKHook(client, SDKHook_OnTakeDamage, SniperTarget_Hurt);
    }
}