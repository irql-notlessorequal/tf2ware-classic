#include <tf2>
#include <tf2_stocks>

#include "mimallet_defines.inc"

static Handle hSDKEquipWearable = INVALID_HANDLE;
static Handle hSDKRemoveWearable = INVALID_HANDLE;

public bool MimalletInitWearables()
{
	GameData gamedata = new GameData("sm-tf2.games");

	if (gamedata == INVALID_HANDLE)
	{
		PrintToServer("[mimallet] Failed to find the game data handle!")
		return false;
	}

	StartPrepSDKCall(SDKCall_Player);
	PrepSDKCall_SetVirtual(gamedata.GetOffset("RemoveWearable") - 1);
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	hSDKEquipWearable = EndPrepSDKCall();

	if (hSDKEquipWearable == INVALID_HANDLE)
	{
		PrintToServer("[Mallet] Failed to hook into CTFPlayer::EquipWearable!")
		return false;
	}

	StartPrepSDKCall(SDKCall_Player);
	PrepSDKCall_SetVirtual(gamedata.GetOffset("RemoveWearable"));
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	hSDKRemoveWearable = EndPrepSDKCall();

	if (hSDKRemoveWearable == INVALID_HANDLE)
	{
		PrintToServer("[mimallet] Failed to hook into CTFPlayer::RemoveWearable!")
		return false;
	}

	delete gamedata;
	return true;
}

bool TypeIsWearable(const char[] name)
{
	return StrEqual(name, "tf_wearable") || StrEqual(name, "tf_wearable_demoshield")
		|| StrEqual(name, "tf_wearable_razorback") || StrEqual(name, "tf_powerup_bottle");
}

public int MalletRemoveWearable(int client, int wearable)
{
	SDKCall(hSDKRemoveWearable, client, wearable);
	return 1;
}

public int MalletEquipWearable(int client, int entity)
{
	if (!IsValidEntity(entity))
	{
		return MALLET_ERROR_INVALID_ENTITY;
	}

	SDKCall(hSDKEquipWearable, client, entity);
	return 1;
}