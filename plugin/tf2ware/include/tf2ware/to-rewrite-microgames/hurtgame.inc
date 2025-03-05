// hurtgame module
// You constantly get hurt and need to grab medkits. There's less and less medkits as time goes on.

public hurtgame_OnMinigame() {
    // These are forwards if you need a specific function!
    AddToForward(g_justEntered, INVALID_HANDLE, hurtgame_justEntered);
    //AddToForward(g_OnEndMinigame, INVALID_HANDLE, hurtgame_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, hurtgame_OnGameFrame);

    // Should the person be able to attack?
    //g_attack = true;
    
    //The below function needs to be there if you're planning on giving clients a weapon!
    //RemoveAllWeapons();
}

/*
public hurtgame_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            // do something?
        }
    }
}
*/


public hurtgame_justEntered(client) {
    if (IsValidClient(client)) {

        // Change the client's class?
        SetClientClass(client, "heavy");
        
        // Give the client a weapon? GiveClientWeapon(client, weapon name, slot, visible);
        //GiveClientWeapon(client, "tf_weapon_revolver", 0, true);
    }
}



public hurtgame_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) {
            SetEntityHealth(i, GetClientHealth(i)-1);
        }
    }
}
