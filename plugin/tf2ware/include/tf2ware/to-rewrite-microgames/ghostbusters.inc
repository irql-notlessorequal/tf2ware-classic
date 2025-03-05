// Ghostbusters module

#define GHOSTBUSTERS_MEDIC 0
#define GHOSTBUSTERS_SPY 1
#define GHOSTBUSTERS_HEAVY 2

#define MUSIC_GHOSTB "imgay/tf2ware/minigame_25_alt.mp3"

new bool:Ghostbusters_pp[MAXPLAYERS+1] = false;
new Ghostbusters_target[MAXPLAYERS+1] = 0;
new Ghostbusters_hurt[MAXPLAYERS+1] = 0;
new Ghostbusters_hp[MAXPLAYERS+1] = 0;
new bool:Ghostbusters_ended = false;

public Ghostbusters_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Ghostbusters_OnMapStart);
}   

public Ghostbusters_OnMapStart() {
    precacheSound(MUSIC_GHOSTB);
}

public Ghostbusters_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Ghostbusters_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Ghostbusters_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Ghostbusters_OnGameFrame);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Ghostbusters_PlayerDeath);
    
    SetConVarInt(FindConVar("mp_teams_unbalance_limit"), 0);
    SetConVarInt(FindConVar("mp_autoteambalance"), 0);
    
    for (new i = 1; i <= MaxClients; i++) {
        g_Mission[i] = GHOSTBUSTERS_SPY;
    }

    new AllPlayers = GetClientCount(true);
    new target = 0;
    new attempts = 0;
    new Float:fmedics = float(GetActivePlayers()) / 3.5;
    new medics = RoundToCeil(fmedics);
    new heavies = GetActivePlayers() / 6;
    if (medics <= 1) medics = 2;
    if (medics >= 8) medics = 7;
    if (heavies <= 0) heavies = 1;
    if (heavies >= 4) heavies = 3;
    new medicclients[MAXPLAYERS+1];
    new spyclients[MAXPLAYERS+1];
    new heavyclients[MAXPLAYERS+1];
    
    // Get the medics!
    for (new i = 1; i <= medics; i++) {
        attempts = 0;
        target = GetRandomInt(1, AllPlayers);
        while (((!IsValidClient(target)) || IsClientParticipating(target) == false || IsFakeClient(target) || g_Mission[target] != GHOSTBUSTERS_SPY) && attempts <= 50) {
            target = GetRandomInt(1, AllPlayers);
            attempts += 1;
        }
        if (attempts <= 50) {
            g_Mission[target] = GHOSTBUSTERS_MEDIC;
        }
    }
    
    // Get the heavies!
    for (new i = 1; i <= heavies; i++) {
        attempts = 0;
        target = GetRandomInt(1, AllPlayers);
        while (((!IsValidClient(target)) || IsClientParticipating(target) == false || g_Mission[target] != GHOSTBUSTERS_SPY) && attempts <= 50) {
            target = GetRandomInt(1, AllPlayers);
            attempts += 1;
        }
        if (attempts <= 50) {
            g_Mission[target] = GHOSTBUSTERS_HEAVY;
        }
    }
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i)) {
            if (g_Mission[i] == GHOSTBUSTERS_SPY) {
                SetStateClient(i, true);
                spyclients[i-1] = i;
            }           
            if (g_Mission[i] == GHOSTBUSTERS_MEDIC) {
                SetStateClient(i, false);
                medicclients[i-1] = i;
            }
            if (g_Mission[i] == GHOSTBUSTERS_HEAVY) {
                SetStateClient(i, true);
                heavyclients[i-1] = i;
            }
        }
    }
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) SetEntProp(i, Prop_Send, "m_bGlowEnabled", 1);
    }
    
    AddRandVocalization("ghostslaugh", spyclients, 2.0, 4.0, SNDLEVEL_AIRCRAFT);
    
    g_attack = true;
    Ghostbusters_ended = false;
}

public Ghostbusters_End() {
    SetConVarInt(FindConVar("mp_teams_unbalance_limit"), 1);
    SetConVarInt(FindConVar("mp_autoteambalance"), 1);
    
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, Ghostbusters_Damage);
        if (IsValidClient(i) && IsClientParticipating(i)) {
            StopSound(i, SND_CHANNEL_SPECIFIC, MUSIC_GHOSTB);
            if (IsPlayerAlive(i)) {
                Ghostbusters_FixSlap(i);
                SetEntityRenderMode(i, RENDER_NORMAL);
                SetEntityRenderColor(i, 255, 255, 255, 255);
                
                new default_fov = GetEntProp(i, Prop_Send, "m_iDefaultFOV"); 
                SetEntProp(i, Prop_Send, "m_iFOV", default_fov);
            }
        }
    }
    
    RespawnAll(true, false);
}

new Ghostbusters_Pos[][] = {
{-3051, 8439, -20, 0},
{-2615, 8446, 10, 90},
{-3028, 8548, 551, 90},
{-1654, 9570, -180, -90},
{-72, 9141, 234, -117},
{-2863, 8525, 427, 90},
{-2860, 9083, 11, -90},
{-43, 9147, 11, -90},
{-2865, 8747, 11, 0},
{-2911, 8078, 235, 90},
{-1040, 8563, 427, 180},
{-2260, 8464, 235, 0},
{-3203, 8823, 234, 0},
{-841, 9064, 123, -44},
{-420, 9178, 11, -90},
{-771, 8759, 10, -180},
{-1924, 9034, 426, -138},
{127, 8604, 234, -180},
{-619, 8552, 234, 90},
{-859, 8904, 298, 32}
};

public Ghostbusters_justEntered(client) {
    if (IsValidClient(client)) {   
        decl Float:pos[3];
        decl Float:ang[3];
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        Ghostbusters_hp[client] = GetClientHealth(client);

        if (g_Mission[client] == GHOSTBUSTERS_SPY) {
            if (GetClientTeam(client) == 2) ChangeClientTeamAlive(client, 3);
            SetClientClass(client, "spy");
            DisableClientWeapons(client);
            
            new id = Ghostbusters_GetGroupId(client, g_Mission[client]) -1;
            
            if (id >= sizeof(Ghostbusters_Pos)) id = 0;
        
            pos[0] = float(Ghostbusters_Pos[id][0]);
            pos[1] = float(Ghostbusters_Pos[id][1]);
            pos[2] = float(Ghostbusters_Pos[id][2]);
            ang[0] = 0.0;
            ang[1] = float(Ghostbusters_Pos[id][3]);
            ang[2] = 0.0;
            
            SetEntityRenderMode(client, RENDER_TRANSCOLOR);
            SetEntityRenderColor(client, 255, 255, 255, 60);
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 280.0);
        }
        if (g_Mission[client] == GHOSTBUSTERS_MEDIC) {
            if (GetClientTeam(client) == 3) ChangeClientTeamAlive(client, 3);
            SetClientClass(client, "medic");
            RemoveClientWeapons(client);
            GiveClientWeapon(client, "tf_weapon_medigun", 1, true);
            SetClientClass(client, "medic");
            pos[0] = -1787.0;
            pos[1] = 8389.0;
            pos[2] = 10.0;
            ang[0] = 0.0;
            ang[1] = 90.0;
            ang[2] = 0.0;
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 300.0);
        }
        if (g_Mission[client] == GHOSTBUSTERS_HEAVY) {
            if (GetClientTeam(client) == 3) ChangeClientTeamAlive(client, 2);
            SetClientClass(client, "heavy");
            RemoveClientWeapons(client);
            GiveClientWeapon(client, "tf_weapon_fists", 2, true);
            SetClientClass(client, "heavy");
            pos[0] = -1713.0;
            pos[1] = 8738.0;
            pos[2] = 666.0;
            ang[0] = 0.0;
            ang[1] = 180.0;
            ang[2] = 0.0;
            SetEntityRenderColor(client, 0, 255, 0, 255);
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 220.0);
        }
        TeleportEntity(client, pos, ang, vel);
        Ghostbusters_pp[client] = false;
        Ghostbusters_target[client] = 0;
        SDKHook(client, SDKHook_OnTakeDamage, Ghostbusters_Damage);
    }
}

Ghostbusters_GetGroupId(client, id) {
    new out = 0;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i)) {
            if (g_Mission[i] == id) out += 1;
            if (i == client) return out;
        }
    }
    return 1;
}

public Ghostbusters_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
            if (GetClientTeam(i) == 2 && g_Mission[i] != GHOSTBUSTERS_HEAVY) ChangeClientTeamAlive(i, 3);
            if (GetClientTeam(i) == 3 && g_Mission[i] == GHOSTBUSTERS_HEAVY) ChangeClientTeamAlive(i, 2);
        
            if (g_Mission[i] == GHOSTBUSTERS_MEDIC) {
                new target = GetHealtarget(i);
                if (IsValidClient(target) && IsPlayerAlive(target) && g_Mission[target] != GHOSTBUSTERS_MEDIC) {
                    Ghostbusters_SetPP(i, true);
                    Ghostbusters_SetPP(target, true);
                    Ghostbusters_target[i] = target;
                    
                    if (Ghostbusters_hurt[target] <= 0) {
                        Ghostbusters_Slap(i);
                        //Ghostbusters_Slap(target);
                        new default_fov = GetEntProp(target, Prop_Send, "m_iDefaultFOV"); 
                        new health = Ghostbusters_hp[target];
                        new new_fov = default_fov + RoundFloat(float(125 - health)*0.75);
                        SetEntProp(target, Prop_Send, "m_iFOV", new_fov);
                    
                        new newhealth = health - 3;
                        
                        if (newhealth > 0)
                        {
                            SetEntityHealth(target, newhealth);
                            Ghostbusters_hp[target] = newhealth;
                        }
                        else {
                            Ghostbusters_SetPP(target, false);
                            ForcePlayerSuicide(target);
                        }
                        Ghostbusters_hurt[target] = 6;
                    }
                    else Ghostbusters_hurt[target] -= 1;
                }
                else {
                    Ghostbusters_SetPP(i, false);
                    new target2 = Ghostbusters_target[i];
                    if (target2 != 0) {
                        if (IsPlayerAlive(target2)) Ghostbusters_SetPP(target2, false);
                        Ghostbusters_target[i] = 0;
                    }
                }
            }
        }
    }
}

public Ghostbusters_PlayerDeath(client) {
    SetStateClient(client, false, true);
    Ghostbusters_SetPP(client, false);
    
    CreateTimer(0.1, Ghostbusters_Check);
}

public Action:Ghostbusters_Check(Handle:timer) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Ghostbusters")) return;
    if (Ghostbusters_ended == true) return;

    // Did the medics win?
    new bool:complete_medic = true;
    new bool:complete_spy = true;
    new winner = 0;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_SPY) && IsPlayerAlive(i)) complete_medic = false;
        if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_MEDIC) && IsPlayerAlive(i)) complete_spy = false;
    }
    
    if (complete_spy) winner = 2;
    if (complete_medic) winner = 1;
    if (winner > 0) {
        Ghostbusters_ended = true;
        if (winner == 1) CPrintToChatAll("{lightgreen}All ghost spies are dead... {green}surviving medics win!");
        if (winner == 2) CPrintToChatAll("{lightgreen}All medics are dead... {green}surviving undeads win!");
        for (new i = 1; i <= MaxClients; i++) {
            if (winner == 1) {
                if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_MEDIC) && IsPlayerAlive(i)) SetStateClient(i, true, true);
                if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] != GHOSTBUSTERS_MEDIC)) {
                    SetStateClient(i, false, true);
                    if (IsPlayerAlive(i)) ForcePlayerSuicide(i);
                }
            }
            if (winner == 2) {
                if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] != GHOSTBUSTERS_MEDIC) && IsPlayerAlive(i)) SetStateClient(i, true, true);
                if (IsValidClient(i) && IsClientParticipating(i) && (g_Mission[i] == GHOSTBUSTERS_MEDIC)) {
                    SetStateClient(i, false, true);
                    if (IsPlayerAlive(i)) ForcePlayerSuicide(i);
                }
            }
        }
        CreateTimer(3.0, Ghostbusters_TEnd);
    }
}

public Action:Ghostbusters_TEnd(Handle:timer) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Ghostbusters")) return;
    
    TriggerTimer(microgametimer);
}

Ghostbusters_Slap(client) {
    decl Float:fEye[3];
    GetClientEyeAngles(client, fEye);
    fEye[0] += GetRandomFloat(-10.0,10.0) * 0.1;
    fEye[1] += GetRandomFloat(-10.0,10.0) * 0.1;
    fEye[2] += GetRandomFloat(-10.0,10.0) * 0.1;
    TeleportEntity(client, NULL_VECTOR, fEye, NULL_VECTOR);
}

Ghostbusters_FixSlap(client) {
    decl Float:fEye[3];
    GetClientEyeAngles(client, fEye);
    fEye[2] = 0.0;
    TeleportEntity(client, NULL_VECTOR, fEye, NULL_VECTOR);
}

Ghostbusters_SetPP(client, bool:input) {
    if (IsValidClient(client)) {
        if (input == true && Ghostbusters_pp[client] == false) {
            TF2_SetPlayerPowerPlay(client, true);
            Ghostbusters_pp[client] = true;
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 500.0);
        }
        if (input == false && Ghostbusters_pp[client] == true) {
            TF2_SetPlayerPowerPlay(client, false);
            Ghostbusters_pp[client] = false;
            if (g_Mission[client] == GHOSTBUSTERS_SPY) SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 280.0);
            else SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 300.0);
            Ghostbusters_FixSlap(client);
        }
    }
}

public Action:Ghostbusters_Damage(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(victim) && IsValidClient(attacker) && g_Mission[victim] == GHOSTBUSTERS_SPY && g_Mission[attacker] == GHOSTBUSTERS_HEAVY) {
        damage = 0.0;
        return Plugin_Changed;
    }
    if (IsValidClient(victim) && IsValidClient(attacker) && g_Mission[victim] == GHOSTBUSTERS_HEAVY) {
        damage = 0.0;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}