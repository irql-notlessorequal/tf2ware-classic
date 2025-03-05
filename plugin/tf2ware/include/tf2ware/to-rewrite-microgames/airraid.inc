// Airraid module

#define PROJECTILE_ROCKET 1
#define PROJECTILE_ROCKET_SENTRY 2

#define TEAM_SPEC 1
#define TEAM_RED 2
#define TEAM_BLUE 3


public Airraid_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Airraid_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Airblast_End);
    AddToForward(g_OnAlmostEnd, INVALID_HANDLE, Airblast_CheckDeadPlayers);

    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKHook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
        }
    }
    
    SetStateAll(true);
    g_attack = true;
    CreateTimer(0.5, FireZeeMissiles);  

    RemoveAllWeapons();    
}

public Action:FireZeeMissiles(Handle:hndl, any:data) {
    new Float:pos[3], Float:ang[3];
    ang[0] = 90.0;
    new Float:rand;
    for (new i=-2; i<=2; i++) {
        for (new j=-2; j<=2; j++) {
            rand = GetRandomFloat(200.0, 500.0);
            pos[2] = rand;
            pos[0] = i*244.0;
            pos[1] = j*244.0;
            fireProjectile(pos, ang, 350.0, 300.0, TEAM_SPEC, PROJECTILE_ROCKET, true);
        }
    }
    
}

fireProjectile(Float:vPosition[3], Float:vAngles[3] = NULL_VECTOR, Float:flSpeed = 1100.0, Float:flDamage = 90.0, iTeam = TEAM_SPEC, iType = PROJECTILE_ROCKET, bool:bCritical = false)
{
    new String:strClassname[32] = "";
    new String:strEntname[32] = "";
    
    switch (iType) {
        case PROJECTILE_ROCKET:
        {
            strClassname = "CTFProjectile_Rocket";
            strEntname = "tf_projectile_rocket";
        }
        case PROJECTILE_ROCKET_SENTRY:
        {
            strClassname = "CTFProjectile_SentryRocket";
            strEntname = "tf_projectile_sentryrocket";
        }
        default:
        {
            return -1;
        }
    }

    new iRocket = CreateEntityByName(strEntname);
    
    if(!IsValidEntity(iRocket))
        return -1;
    
    decl Float:vVelocity[3];
    decl Float:vBuffer[3];
    
    GetAngleVectors(vAngles, vBuffer, NULL_VECTOR, NULL_VECTOR);
    
    vVelocity[0] = vBuffer[0]*flSpeed;
    vVelocity[1] = vBuffer[1]*flSpeed;
    vVelocity[2] = vBuffer[2]*flSpeed;
    
    TeleportEntity(iRocket, vPosition, vAngles, vVelocity);
    
    SetEntData(iRocket, FindSendPropOffs(strClassname, "m_nSkin"), (iTeam-2), 1, true);
    SetEntData(iRocket, FindSendPropOffs(strClassname, "m_bCritical"), bCritical, true);
    SetEntDataFloat(iRocket, FindSendPropOffs(strClassname, "m_iDeflected") + 4, flDamage, true); // Credit to voogru
    
    SetVariantInt(iTeam);
    AcceptEntityInput(iRocket, "TeamNum", -1, -1, 0);

    SetVariantInt(iTeam);
    AcceptEntityInput(iRocket, "SetTeam", -1, -1, 0); 
    
    DispatchSpawn(iRocket);
    
    return iRocket;
}


public Airraid_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "pyro");
        GiveClientWeapon(client, "tf_weapon_flamethrower", 0, true);
    }
}