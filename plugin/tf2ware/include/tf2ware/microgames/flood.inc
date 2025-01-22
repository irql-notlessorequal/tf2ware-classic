// Flood module

public Flood_OnMinigame() {
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Flood_PlayerDeath);
    
    SetStateAll(true);
    
    // Change the clients that are Scout into Demomen
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            new TFClassType:class = TF2_GetPlayerClass(i);
            if (class == TFClass_Scout) {
                SetClientClass(i, "demoman");
                DisableClientWeapons(i);
            }
        }
    }

    // Spawns a barrel over a trigger to create the floor of death? xD
    decl Float:pos[3];
    new prop = CreateEntityByName("prop_physics");
    
    if (IsValidEdict(prop)) {
        DispatchKeyValue(prop, "model", "models/props_farm/wooden_barrel.mdl");
        DispatchSpawn(prop);
        pos[0] = -1237.405029;
        pos[1] = 125.300606;
        pos[2] = -58.932495;
        TeleportEntity(prop, pos, NULL_VECTOR, NULL_VECTOR);
        CreateTimer(0.1, killprop_timer, prop);
    } 
}

public Flood_PlayerDeath(client) {
    SetStateClient(client, false, true);
}
