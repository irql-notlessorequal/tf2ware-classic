#include "../microgame.inc"

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
		for (int client = 1; client <= MaxClients; client++)
		{
			if (IsValidClient(client))
			{
				SDKHook(client, SDKHook_PreThink, Airblast_RemoveLeftClick);
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
		}
	}

	public void OnClientDeath(int client)
	{
		SetStateClient(client, false, true);
	}

	public void OnMicrogamePostEnd()
	{
		for (int client = 1; client <= MaxClients; client++)
		{
			SDKUnhook(client, SDKHook_PreThink, Airblast_RemoveLeftClick);
		}
	}
}

Action FireZeeMissiles(Handle handle, any data)
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
			fireProjectile(pos, ang, 350.0, 1000.0, true);
		}
	}
	
	return Plugin_Stop;
}

int fireProjectile(float vPosition[3], float vAngles[3] = NULL_VECTOR, float flSpeed = 1100.0, float flDamage = 90.0,
	bool bCritical = false)
{
	int ent = CreateEntityByName("tf_point_weapon_mimic");
	if (!IsValidEntity(ent))
	{
		return -1;
	}

	GetAngleVectors(vAngles, NULL_VECTOR, NULL_VECTOR, NULL_VECTOR);
	
	DispatchKeyValueVector(ent, "origin", vPosition);
	DispatchKeyValueVector(ent, "angles", vAngles);

	char damage[8];
	FloatToString(flDamage, damage, sizeof (damage));

	char speed[8];
	FloatToString(flSpeed, speed, sizeof (speed));
	
	DispatchKeyValueInt(ent, "Crits", bCritical);

	DispatchKeyValue(ent, "WeaponType", "0");
	DispatchKeyValue(ent, "Damage", damage);
	DispatchKeyValue(ent, "SplashRadius", "256");
	DispatchKeyValue(ent, "SpreadAngle", "5");

	DispatchKeyValue(ent, "SpeedMin", speed);
	DispatchKeyValue(ent, "SpeedMax", speed);
	
	if (!DispatchSpawn(ent))
	{
		return -1;
	}
	else
	{
		SetVariantString("OnUser1 !self:ClearParent::2.95:1");
		AcceptEntityInput(ent, "AddOutput");
		AcceptEntityInput(ent, "FireUser1");
	
		SetVariantString("OnUser2 !self:Kill::10.0:1");
		AcceptEntityInput(ent, "AddOutput");
		AcceptEntityInput(ent, "FireUser2");

		AcceptEntityInput(ent, "FireOnce");
		return ent;
	}
}