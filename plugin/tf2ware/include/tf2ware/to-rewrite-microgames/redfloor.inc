// RedFloor module

public RedFloor_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, RedFloor_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, RedFloor_End);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, RedFloor_PlayerDeath);
    
    g_attack = true;
    SetStateAll(true);
    
    CreateTimer(5.0, Minigame16_Timer);
    SetConVarInt(FindConVar("mp_friendlyfire"), 1);
}

public Action:RedFloor_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(attacker) && IsValidClient(victim)) {
        decl Float:ang[3];
        decl Float:vel[3];
        GetClientEyeAngles(attacker, ang);
        GetEntPropVector(victim, Prop_Data, "m_vecVelocity", vel);
        vel[0] -= 300.0 * Cosine(DegToRad(ang[1])) * -1.0 * damage*0.01;
        vel[1] -= 300.0 * Sine(DegToRad(ang[1])) * -1.0 * damage*0.01;
        vel[2] += 450.0;
        TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, vel);
        damage = 0.0;
    }
    return Plugin_Changed;
}

public RedFloor_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_OnTakeDamage, RedFloor_OnTakeDamageClient);
        
        RemoveClientWeapons(client);
        SetClientClass(client, "spy");
        GiveClientWeapon(client, "tf_weapon_knife", 2, true);
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new total = GetClientCount(true);
        new posa = 360 / total * (g_Id[client]-1);
        
        decl Float:pos[3];
        pos[0] = -4639.0   + (Cosine(DegToRad(float(posa)))*220.0);
        pos[1] = -1311.0  - (Sine(DegToRad(float(posa)))*220.0);
        pos[2] = -31.49;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = float(180-posa);
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}

public RedFloor_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKUnhook(i, SDKHook_OnTakeDamage, RedFloor_OnTakeDamageClient);
        }
    }
    SetConVarInt(FindConVar("mp_friendlyfire"), 0);
}

public Action:Minigame16_Timer(Handle:hTimer) {
    if (!StrEqual(minigame, "RedFloor")) return Plugin_Stop;
    
    if (status == 2) {
        new activeplayers = 0;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && g_Complete[i] && IsClientParticipating(i)) {
                activeplayers += 1;
            }
        }
            
        if (activeplayers > 2) {
            CreateTimer(5.0, Minigame16_Timer);
        }
        else {
            TriggerTimer(microgametimer);
        }
    }
    return Plugin_Stop;
}

public RedFloor_PlayerDeath(client) {
    SetStateClient(client, false, true);
}