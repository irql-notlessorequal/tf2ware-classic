// Barrel module

new g_Barrels[MAXPLAYERS+1] = -1;

public Barrel_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Barrel_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Barrel_End);
    
    g_attack = true;
    
    decl Float:pos[3];
    decl Float:vel[3];

    DestroyAllBarrels();
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i)) {
            GetClientAbsOrigin(i, pos);
            g_Barrels[i] = CreateEntityByName("prop_physics");
            
            if (IsValidEdict(g_Barrels[i])) {
                DispatchKeyValue(g_Barrels[i], "model", "models/props_farm/wooden_barrel.mdl");
                DispatchSpawn(g_Barrels[i]);
                vel[0]=GetRandomFloat(-300.0, 300.0);
                vel[1]= GetRandomFloat(-300.0, 300.0);
                vel[2] = GetRandomFloat(100.0, 300.0);
                pos[2] += 150;
                TeleportEntity(g_Barrels[i], pos, NULL_VECTOR, vel);
                SDKHook(g_Barrels[i], SDKHook_OnTakeDamage, Barrel_OnTakeDamage);
            }    
        }
    }
}

public Barrel_justEntered(client) {
    if (IsValidClient(client)) {
        RemoveClientWeapons(client);
        SetClientClass(client, "scout");
        GiveClientWeapon(client, "tf_weapon_bat", 2, true);
    }
}

public Barrel_End() {
    DestroyAllBarrels();
}

DestroyAllBarrels() {
        for (new i = 1; i <= MaxClients; i++) {
            if(g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i])) {
                DestroyBarrel(g_Barrels[i]);
            }
            g_Barrels[i] = -1;
        }    
}

public Action:Barrel_OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
    for (new i = 1; i <= MaxClients; i++) {
        if(g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i]) && (g_Barrels[i] == victim)) {
            DestroyBarrel(victim);
            g_Barrels[i] = -1;
            
            if (IsValidClient(attacker) && iMinigame == 3) SetStateClient(attacker, true, true);
        }    
    }
    
    return Plugin_Continue;
}

DestroyBarrel(ent) {
    if (ent > 0 && IsValidEdict(ent)) {
        decl String:strClassname[64];
        GetEdictClassname(ent, strClassname, sizeof(strClassname));
        if (StrEqual(strClassname, "prop_physics", false)) {
            AcceptEntityInput(ent, "Kill" );
        }
    }
}