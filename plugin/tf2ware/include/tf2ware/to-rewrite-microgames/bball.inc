// BBall module

public BBall_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKHook(i, SDKHook_OnTakeDamage, BBall_OnTakeDamageClient);
        }
    }

    AddToForward(g_justEntered, INVALID_HANDLE, BBall_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, BBall_End);

    g_attack = true;
}

public Action:BBall_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (status == 2 && IsValidClient(victim)) {
        new iMaxHealth = GetClientMaxHealth(victim);
        SetEntityHealth(victim, iMaxHealth);
        
        if (damage >= float(iMaxHealth-5)) damage = float(iMaxHealth-10);
        if (damage > 150.0) damage = 150.0;
        CreateTimer(0.0, BBall_GiveHealth, victim);
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public BBall_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, BBall_OnTakeDamageClient);
    }
}

public Action:BBall_GiveHealth(Handle:hTimer, any:iClient) {
    if (IsValidClient(iClient) && IsPlayerAlive(iClient)) {
        new iMaxHealth = GetClientMaxHealth(iClient);
        SetEntityHealth(iClient, iMaxHealth);  
    }
}

public OnEntityCreated(entity, const String:classname[]) {
    if (!StrEqual(minigame, "BBall")) return;
    if ((status == 2) && (StrEqual(classname, "tf_projectile_pipe"))) {
        SDKHook(entity, SDKHook_StartTouch, Hook_StartTouch_Pipe);
    }
}

public Action:Hook_StartTouch_Pipe(entity, other) {
    new String:classname[64];
    GetEdictClassname(other, classname, sizeof(classname));
    
    if (StrEqual(classname, "func_button") && status == 2) {
        new owner = GetEntPropEnt(entity, Prop_Send, "m_hThrower");
        if (g_Complete[owner] == false) {
            EmitSoundToClient(owner, SOUND_MINISCORE, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            g_Minipoints[owner] += 1;
            if (g_Minipoints[owner] >= 7) SetStateClient(owner, true, true);
            AcceptEntityInput(entity, "Kill" );
        }
    }
}

public BBall_justEntered(client) {
    if (IsValidClient(client)) {
        RemoveClientWeapons(client);
        SetClientClass(client, "demoman");
        GiveClientWeapon(client, "tf_weapon_grenadelauncher", 0, true);
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new column = client;
        new row = 0;
        while (column > 17) {
            column = column - 17;
            row = row + 1;
        }
        
        decl Float:pos[3];
        pos[0] = 3076.0  + float(column*55);
        pos[1] = -1930.0 + float(row*55);
        pos[2] = -260.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        
        TeleportEntity(client, pos, ang, vel);
    }
}