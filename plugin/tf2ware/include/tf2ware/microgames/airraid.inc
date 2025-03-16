#include "../microgame.inc"

#define PROJECTILE_ROCKET 1
#define PROJECTILE_ROCKET_SENTRY 2

#define TEAM_SPEC 1
#define TEAM_RED 2
#define TEAM_BLUE 3

methodmap AirRaid < Microgame
{
	public AirRaid()
	{
		return view_as<AirRaid>(MG_AIR_RAID);
	}

	public void OnMicrogameStart()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsValidClient(i))
			{
				SDKHook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
			}
		}
	
		SetStateAll(true);
		CreateTimer(0.5, FireZeeMissiles);

		g_attack = true;
	}

	public void OnClientJustEntered(int client)
	{
		if (IsValidClient(client))
		{
			RemoveClientWeapons(client);
			SetClientClass(client, "pyro");
			GiveClientWeapon(client, "tf_weapon_flamethrower", 0, true);
			SetClientClass(client, "pyro");
		}
	}
}

Action FireZeeMissiles(Handle hndl, any data)
{
	float pos[3];
	float ang[3];

	ang[0] = 90.0;

	float rand;
	for (int i = -2; i <= 2; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			rand = GetRandomFloat(200.0, 500.0);
			pos[2] = rand;
			pos[0] = i * 244.0;
			pos[1] = j * 244.0;
			fireProjectile(pos, ang, 350.0, 300.0, TEAM_SPEC, PROJECTILE_ROCKET, true);
		}
	}
	
	return Plugin_Stop;
}

int fireProjectile(float vPosition[3], float vAngles[3] = NULL_VECTOR, float flSpeed = 1100.0, float flDamage = 90.0,
	int iTeam = TEAM_SPEC, int iType = PROJECTILE_ROCKET, bool bCritical = false)
{
	char strClassname[32];
	char strEntname[32];
	
	switch (iType)
	{
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

	int iRocket = CreateEntityByName(strEntname);
	if (!IsValidEntity(iRocket))
		return -1;
	
	float vVelocity[3];
	float vBuffer[3];
	
	GetAngleVectors(vAngles, vBuffer, NULL_VECTOR, NULL_VECTOR);
	
	vVelocity[0] = vBuffer[0] * flSpeed;
	vVelocity[1] = vBuffer[1] * flSpeed;
	vVelocity[2] = vBuffer[2] * flSpeed;
	
	TeleportEntity(iRocket, vPosition, vAngles, vVelocity);
	
	SetEntData(iRocket, FindSendPropOffs(strClassname, "m_nSkin"), (iTeam - 2), 1, true);
	SetEntData(iRocket, FindSendPropOffs(strClassname, "m_bCritical"), bCritical, true);
	SetEntDataFloat(iRocket, FindSendPropOffs(strClassname, "m_iDeflected") + 4, flDamage, true); // Credit to voogru
	
	SetVariantInt(iTeam);
	AcceptEntityInput(iRocket, "TeamNum", -1, -1, 0);

	SetVariantInt(iTeam);
	AcceptEntityInput(iRocket, "SetTeam", -1, -1, 0); 
	
	DispatchSpawn(iRocket);
	
	return iRocket;
}