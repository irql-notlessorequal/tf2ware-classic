// Hit Enemy module

public HitEnemy_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, HitEnemy_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, HitEnemy_End);

    randommini = GetRandomInt(0, 2);

    g_attack = true;

}

public HitEnemy_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, HitEnemy_OnTakeDamageClient);
    }
}

public Action:HitEnemy_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(attacker)) {
        SetStateClient(attacker, true, true);
        damage = 450.0;
    }
    return Plugin_Changed;
}


public HitEnemy_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_OnTakeDamage, HitEnemy_OnTakeDamageClient);

        RemoveClientWeapons(client);
        if (randommini <= 0) {
            SetClientClass(client, "spy");
            GiveClientWeapon(client, "tf_weapon_revolver", 0, true);
        }
        if (randommini == 1) {
            SetClientClass(client, "demoman");
            GiveClientWeapon(client, "tf_weapon_bottle", 2, true);
        }
        if (randommini >= 2) {
            SetClientClass(client, "engineer");
            GiveClientWeapon(client, "tf_weapon_shotgun_primary", 0, true);
        }
    }
}