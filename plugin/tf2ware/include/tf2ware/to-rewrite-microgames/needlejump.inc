// Needlejump module

new g_Needle_Type = 0;

public Needlejump_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Needlejump_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Needlejump_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Needlejump_OnGameFrame);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, Needlejump_TimerMinigame);
    
    g_Needle_Type = GetRandomInt(0,2);
    
    SetStateAll(true);
    g_attack = true;
}

public Needlejump_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if(IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
            if (g_Mission[i] == 0) PerformNeedleJump(i);
        }
    }
}

public Action:Needlejump_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(victim) && status == 2) {
        if (victim != attacker) {
            decl Float:fVelocity[3];
            GetEntPropVector(victim, Prop_Data, "m_vecVelocity", fVelocity);
            fVelocity[2] -= 200.0;
            TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, fVelocity);
            damage = 0.0;
            return Plugin_Changed;
        }
    }
    return Plugin_Continue;
}

public Needlejump_TimerMinigame(timer) {
    if ((g_Needle_Type != 1 && timer == 3) || (g_Needle_Type == 1 && timer == 5)) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
                decl Float:pos[3];
                GetClientAbsOrigin(i, pos);
                decl Float:forigin[3];
                forigin[0] = pos[0];
                forigin[1] = pos[1];
                forigin[2] = -140.0;
                        
                PyroExplode(forigin);
                
                if (pos[2] <= -55.0) {
                    ForcePlayerSuicide(i);
                    SetStateClient(i, false, true);
                }
            }
        }
    }
}

public Needlejump_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_OnTakeDamage, Needlejump_OnTakeDamageClient);
    
        RemoveClientWeapons(client);
        g_Mission[client] = g_Needle_Type;
        if (g_Mission[client] == 0) {
            SetClientClass(client, "medic");
            GiveClientWeapon(client, "tf_weapon_syringegun_medic", 0, true);
        }
        if (g_Mission[client] == 1) {
            SetClientClass(client, "soldier");
            GiveClientWeapon(client, "tf_weapon_rocketlauncher", 0, true);
        }
        if (g_Mission[client] == 2) {
            SetClientClass(client, "demoman");
            GiveClientWeapon(client, "tf_weapon_pipebomblauncher", 1, true);
        }
        
        g_NeedleDelay[client] = 50;
    }
}

public Needlejump_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, Needlejump_OnTakeDamageClient);
    }
}


PerformNeedleJump(i) {
    if (g_Mission[i] == 0) {
        decl Float:fEyeAngle[3];
        decl Float:fVelocity[3];
        if (g_NeedleDelay[i] > 0) g_NeedleDelay[i] -= 1;
        if (IsValidClient(i) && (GetClientButtons(i) & IN_ATTACK) && (g_NeedleDelay[i] <= 0)){
            new iWeapon = GetPlayerWeaponSlot(i, 0);
            if (IsValidEdict(iWeapon) && GetEntData(iWeapon, FindSendPropInfo("CTFWeaponBase", "m_iClip1")) != 0) {
                GetClientEyeAngles(i, fEyeAngle);
                GetEntPropVector(i, Prop_Data, "m_vecVelocity", fVelocity);
                new Float:multi = GetSpeedMultiplier(1.0);
                fVelocity[0] += (10.0 * Cosine(DegToRad(fEyeAngle[1])) * -1.0) / multi;
                fVelocity[1] += (10.0 * Sine(DegToRad(fEyeAngle[1])) * -1.0) / multi;
                fVelocity[2] -= (40.0 * Sine(DegToRad(fEyeAngle[0])) * -1.0) / multi;
                if(FloatAbs(fVelocity[0]) > 400.0){
                    if(fVelocity[0] > 0.0)
                        fVelocity[0] = 400.0;
                    else
                        fVelocity[0] = -400.0;
                }
                if(FloatAbs(fVelocity[1]) > 400.0){
                    if(fVelocity[1] > 0.0)
                        fVelocity[1] = 400.0;
                    else
                        fVelocity[1] = -400.0;
                }
                if(fVelocity[2] > 400.0)
                    fVelocity[2] = 400.0;
                TeleportEntity(i, NULL_VECTOR, NULL_VECTOR, fVelocity);
                g_NeedleDelay[i] = 3;
            }
        }
    }
}