#include "../../include/Game/localplayer.h"

uint32_t dwLastOnFootDataSentTick = GetTickCount();
int iFollowingPassenger = 0, iFollowingDriver = 0;
void SendOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, int sendDeathNoti, PLAYERID followPlayerID)
{
	if (pofSync == NULL)
		return;

	RakNet::BitStream bsPlayerSync;

	if (dwLastOnFootDataSentTick && dwLastOnFootDataSentTick < (GetTickCount() - iNetModeNormalOnfootSendRate))
	{
		if (followPlayerID != INVALID_PLAYER_ID)
		{
			if (!playerPool[followPlayerID].bIsConnected || !playerPool[followPlayerID].bIsStreamedIn)
				return;
			if (iFollowingPassenger || iFollowingDriver)
				return;

			pofSync->lrAnalog = playerPool[followPlayerID].onfootData.lrAnalog;
			pofSync->udAnalog = playerPool[followPlayerID].onfootData.udAnalog;
			pofSync->wKeys = playerPool[followPlayerID].onfootData.wKeys;

			pofSync->fCurrentPosition[0] = playerPool[followPlayerID].onfootData.fCurrentPosition[0] + fFollowXOffset;
			pofSync->fCurrentPosition[1] = playerPool[followPlayerID].onfootData.fCurrentPosition[1] + fFollowYOffset;
			pofSync->fCurrentPosition[2] = playerPool[followPlayerID].onfootData.fCurrentPosition[2] + fFollowZOffset;

			pofSync->fQuaternion[0] = playerPool[followPlayerID].onfootData.fQuaternion[0];
			pofSync->fQuaternion[1] = playerPool[followPlayerID].onfootData.fQuaternion[1];
			pofSync->fQuaternion[2] = playerPool[followPlayerID].onfootData.fQuaternion[2];
			pofSync->fQuaternion[3] = playerPool[followPlayerID].onfootData.fQuaternion[3];

			pofSync->fHealth = playerPool[followPlayerID].onfootData.fHealth;
			pofSync->fArmour = playerPool[followPlayerID].onfootData.fArmour;

			if (playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon != 0)
				pofSync->BYTECurrentWeapon = playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon;
			else
				pofSync->BYTECurrentWeapon = playerPool[followPlayerID].onfootData.BYTECurrentWeapon;

			pofSync->BYTESpecialAction = playerPool[followPlayerID].onfootData.BYTESpecialAction;

			pofSync->fMoveSpeed[0] = playerPool[followPlayerID].onfootData.fMoveSpeed[0];
			pofSync->fMoveSpeed[1] = playerPool[followPlayerID].onfootData.fMoveSpeed[1];
			pofSync->fMoveSpeed[2] = playerPool[followPlayerID].onfootData.fMoveSpeed[2];

			pofSync->AnimationIndex = playerPool[followPlayerID].onfootData.AnimationIndex;
			pofSync->AnimationFlags = playerPool[followPlayerID].onfootData.AnimationFlags;

			playerPool[g_myPlayerID].onfootData.fCurrentPosition[0] = pofSync->fCurrentPosition[0];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[1] = pofSync->fCurrentPosition[1];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[2] = pofSync->fCurrentPosition[2];

			bsPlayerSync.Write((uint8_t)ID_PLAYER_SYNC);
			bsPlayerSync.Write((char *)pofSync, sizeof(ONFOOT_SYNC_DATA));
			pRakClient->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			if (sendDeathNoti && pofSync->fHealth == 0)
				SendWastedNotification(0, -1);

			dwLastOnFootDataSentTick = GetTickCount();
		}
		else
		{
			if (playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon != 0)
				pofSync->BYTECurrentWeapon = playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon;

			playerPool[g_myPlayerID].onfootData.fCurrentPosition[0] = pofSync->fCurrentPosition[0];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[1] = pofSync->fCurrentPosition[1];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[2] = pofSync->fCurrentPosition[2];

			bsPlayerSync.Write((uint8_t)ID_PLAYER_SYNC);
			bsPlayerSync.Write((char *)pofSync, sizeof(ONFOOT_SYNC_DATA));

			pRakClient->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			if (sendDeathNoti && pofSync->fHealth == 0)
				SendWastedNotification(0, -1);

			dwLastOnFootDataSentTick = GetTickCount();
		}
	}
}

uint32_t dwLastInVehicleDataSentTick = GetTickCount();
void SendInCarFullSyncData(INCAR_SYNC_DATA *picSync, int iUseCarPos, PLAYERID followPlayerID)
{
	if (picSync == NULL)
		return;

	RakNet::BitStream bsVehicleSync;

	if (dwLastInVehicleDataSentTick && dwLastInVehicleDataSentTick < (GetTickCount() - iNetModeNormalIncarSendRate))
	{
		if (followPlayerID != INVALID_PLAYER_ID)
		{
			if (!playerPool[followPlayerID].bIsConnected || !playerPool[followPlayerID].bIsStreamedIn)
				return;

			picSync->VehicleID = (VEHICLEID)iFollowingWithVehicleID;

			picSync->lrAnalog = playerPool[followPlayerID].incarData.lrAnalog;
			picSync->udAnalog = playerPool[followPlayerID].incarData.udAnalog;
			picSync->wKeys = playerPool[followPlayerID].incarData.wKeys;

			picSync->fQuaternion[0] = playerPool[followPlayerID].incarData.fQuaternion[0];
			picSync->fQuaternion[1] = playerPool[followPlayerID].incarData.fQuaternion[1];
			picSync->fQuaternion[2] = playerPool[followPlayerID].incarData.fQuaternion[2];
			picSync->fQuaternion[3] = playerPool[followPlayerID].incarData.fQuaternion[3];

			picSync->vecPos[0] = playerPool[followPlayerID].incarData.vecPos[0] + fFollowXOffset;
			picSync->vecPos[1] = playerPool[followPlayerID].incarData.vecPos[1] + fFollowYOffset;
			picSync->vecPos[2] = playerPool[followPlayerID].incarData.vecPos[2] + fFollowZOffset;

			picSync->fMoveSpeed[0] = playerPool[followPlayerID].incarData.fMoveSpeed[0];
			picSync->fMoveSpeed[1] = playerPool[followPlayerID].incarData.fMoveSpeed[1];
			picSync->fMoveSpeed[2] = playerPool[followPlayerID].incarData.fMoveSpeed[2];

			picSync->fHealth = playerPool[followPlayerID].incarData.fHealth;

			picSync->fPlayerHealth = playerPool[followPlayerID].incarData.fPlayerHealth;
			picSync->fPlayerArmour = playerPool[followPlayerID].incarData.fPlayerArmour;

			if (playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon != 0)
				picSync->BYTECurrentWeapon = playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon;
			else
				picSync->BYTECurrentWeapon = playerPool[followPlayerID].incarData.BYTECurrentWeapon;

			picSync->BYTESirenOn = playerPool[followPlayerID].incarData.BYTESirenOn;
			picSync->BYTELandingGearState = playerPool[followPlayerID].incarData.BYTELandingGearState;

			picSync->TrailerID_or_ThrustAngle = playerPool[followPlayerID].incarData.TrailerID_or_ThrustAngle;
			picSync->fTrainSpeed = playerPool[followPlayerID].incarData.fTrainSpeed;

			playerPool[g_myPlayerID].onfootData.fCurrentPosition[0] = picSync->vecPos[0];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[1] = picSync->vecPos[1];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[2] = picSync->vecPos[2];

			bsVehicleSync.Write((uint8_t)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((char *)picSync, sizeof(INCAR_SYNC_DATA));
			pRakClient->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastInVehicleDataSentTick = GetTickCount();
		}
		else
		{
			VEHICLEID vehicleID = picSync->VehicleID;

			if (!vehiclePool[vehicleID].iExist)
				return;

			if (iUseCarPos)
			{
				picSync->vecPos[0] = vehiclePool[vehicleID].fPos[0];
				picSync->vecPos[1] = vehiclePool[vehicleID].fPos[1];
				picSync->vecPos[2] = vehiclePool[vehicleID].fPos[2];
			}

			playerPool[g_myPlayerID].onfootData.fCurrentPosition[0] = picSync->vecPos[0];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[1] = picSync->vecPos[1];
			playerPool[g_myPlayerID].onfootData.fCurrentPosition[2] = picSync->vecPos[2];

			bsVehicleSync.Write((uint8_t)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((char *)picSync, sizeof(INCAR_SYNC_DATA));
			pRakClient->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastInVehicleDataSentTick = GetTickCount();
		}
	}
}

uint32_t dwLastPassengerDataSentTick = GetTickCount();
void SendPassengerFullSyncData(VEHICLEID vehicleID)
{
	if (!vehiclePool[vehicleID].iExist)
		return;

	if (dwLastPassengerDataSentTick && dwLastPassengerDataSentTick < (GetTickCount() - iNetModeNormalIncarSendRate))
	{
		RakNet::BitStream bsPassengerSync;

		PASSENGER_SYNC_DATA psSync;
		memset(&psSync, 0, sizeof(PASSENGER_SYNC_DATA));

		psSync.VehicleID = vehicleID;

		psSync.vecPos[0] = vehiclePool[vehicleID].fPos[0];
		psSync.vecPos[1] = vehiclePool[vehicleID].fPos[1];
		psSync.vecPos[2] = vehiclePool[vehicleID].fPos[2];

		psSync.fPlayerHealth = (uint8_t)playerPool[g_myPlayerID].onfootData.fHealth;
		psSync.fPlayerArmour = (uint8_t)playerPool[g_myPlayerID].onfootData.fArmour;

		bsPassengerSync.Write((uint8_t)ID_PASSENGER_SYNC);
		bsPassengerSync.Write((char *)&psSync, sizeof(PASSENGER_SYNC_DATA));
		pRakClient->Send(&bsPassengerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		dwLastPassengerDataSentTick = GetTickCount();
	}
}

uint32_t dwLastAimDataSentTick = GetTickCount();
void SendAimSyncData(uint32_t dwAmmoInClip, int iReloading, PLAYERID copyFromPlayer)
{
	if (dwLastAimDataSentTick && dwLastAimDataSentTick < (GetTickCount() - iNetModeFiringSendRate))
	{
		RakNet::BitStream bsAimSync;
		AIM_SYNC_DATA aimSync;

		if (copyFromPlayer != INVALID_PLAYER_ID)
		{
			if (!playerPool[copyFromPlayer].bIsConnected)
				return;

			memcpy((void *)&aimSync, (void *)&playerPool[copyFromPlayer].aimData, sizeof(AIM_SYNC_DATA));

			if (aimSync.vecAimPos[0] == 0.0f && aimSync.vecAimPos[1] == 0.0f && aimSync.vecAimPos[2] == 0.0f)
			{
				aimSync.vecAimPos[0] = 0.25f;
			}

			bsAimSync.Write((uint8_t)ID_AIM_SYNC);
			bsAimSync.Write((char *)&aimSync, sizeof(AIM_SYNC_DATA));

			pRakClient->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastAimDataSentTick = GetTickCount();
		}
		else
		{
			if (iReloading)
				playerPool[g_myPlayerID].aimData.BYTEWeaponState = WS_RELOADING;
			else
				playerPool[g_myPlayerID].aimData.BYTEWeaponState = (dwAmmoInClip > 1) ? WS_MORE_BULLETS : dwAmmoInClip;

			playerPool[g_myPlayerID].aimData.bAspectRatio = 0x55;

			memcpy((void *)&aimSync, (void *)&playerPool[g_myPlayerID].aimData, sizeof(AIM_SYNC_DATA));

			bsAimSync.Write((uint8_t)ID_AIM_SYNC);
			bsAimSync.Write((char *)&aimSync, sizeof(AIM_SYNC_DATA));

			pRakClient->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastAimDataSentTick = GetTickCount();
		}
	}
}

uint32_t dwLastUnocDataSentTick = GetTickCount();
void SendUnoccupiedSyncData(UNOCCUPIED_SYNC_DATA *punocSync)
{
	if (dwLastUnocDataSentTick && dwLastUnocDataSentTick < (GetTickCount() - 30))
	{
		RakNet::BitStream bsUnoccupiedSync;

		bsUnoccupiedSync.Write((uint8_t)ID_UNOCCUPIED_SYNC);
		bsUnoccupiedSync.Write((char *)punocSync, sizeof(UNOCCUPIED_SYNC_DATA));
		pRakClient->Send(&bsUnoccupiedSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		dwLastUnocDataSentTick = GetTickCount();
	}
}

void SendSpectatorData(SPECTATOR_SYNC_DATA *pSpecData)
{
	RakNet::BitStream bsSpecSync;

	bsSpecSync.Write((uint8_t)ID_SPECTATOR_SYNC);
	bsSpecSync.Write((char *)pSpecData, sizeof(SPECTATOR_SYNC_DATA));

	pRakClient->Send(&bsSpecSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void SendBulletData(BULLET_SYNC_DATA *pBulletData)
{
	if (pBulletData->bHitType == BULLET_HIT_TYPE_PLAYER && pBulletData->iHitID == g_myPlayerID)
		return;

	RakNet::BitStream bsBulletSync;

	bsBulletSync.Write((uint8_t)ID_BULLET_SYNC);
	bsBulletSync.Write((char *)pBulletData, sizeof(BULLET_SYNC_DATA));

	pRakClient->Send(&bsBulletSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
	RakNet::BitStream bsSend;
	uint8_t BYTEPassenger = 0;

	if (bPassenger)
		BYTEPassenger = 1;

	bsSend.Write(VehicleID);
	bsSend.Write(BYTEPassenger);
	pRakClient->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(VehicleID);
	pRakClient->RPC(&RPC_ExitVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void SendWastedNotification(uint8_t uint8_tDeathReason, PLAYERID WhoWasResponsible)
{
	RakNet::BitStream bsPlayerDeath;

	bsPlayerDeath.Write(uint8_tDeathReason);
	bsPlayerDeath.Write(WhoWasResponsible);
	pRakClient->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void NotifyVehicleDeath(VEHICLEID VehicleID)
{
	RakNet::BitStream bsDeath;
	bsDeath.Write(VehicleID);
	pRakClient->RPC(&RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void SendDamageVehicle(uint16_t vehicleID, uint32_t panel, uint32_t door, uint8_t lights, uint8_t tires)
{
	RakNet::BitStream bsDamageVehicle;

	bsDamageVehicle.Write(vehicleID);
	bsDamageVehicle.Write(panel);
	bsDamageVehicle.Write(door);
	bsDamageVehicle.Write(lights);
	bsDamageVehicle.Write(tires);
	pRakClient->RPC(&RPC_DamageVehicle, &bsDamageVehicle, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void sendServerCommand(char *szCommand)
{
	if (!strncmp(szCommand + 1, "rcon", 4))
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint8_t)ID_RCON_COMMAND);
		uint32_t len = strlen(szCommand + 4);
		if (len > 0)
		{
			bsSend.Write(len);
			bsSend.Write(szCommand + 6, len);
		}
		else
		{
			bsSend.Write(len);
			bsSend.Write(szCommand + 5, len);
		}
		pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
	}
	else
	{
		RakNet::BitStream bsParams;
		int iStrlen = strlen(szCommand);
		bsParams.Write(iStrlen);
		bsParams.Write(szCommand, iStrlen);
		pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void SendScoreboardClick(int playerid)
{
	RakNet::BitStream bsSend;
	bsSend.Write((uint8_t)playerid);
	bsSend.Write((uint8_t)0);
	pRakClient->RPC(&RPC_ClickPlayer, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void sendChat(char *szMessage)
{
	RakNet::BitStream bsSend;
	uint8_t uint8_tTextLen = strlen(szMessage);
	bsSend.Write(uint8_tTextLen);
	bsSend.Write(szMessage, uint8_tTextLen);
	pRakClient->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void sendScmEvent(int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iEventType);
	bsSend.Write(dwParam1);
	bsSend.Write(dwParam2);
	bsSend.Write(dwParam3);
	pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void sendDialogResponse(uint16_t wDialogID, uint8_t bButtonID, uint16_t wListBoxItem, const char *szInputResp)
{
	uint8_t respLen = (uint8_t)strlen(szInputResp);
	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(bButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInputResp, respLen);
	if (pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, 0))
	{
		AppendLogF("DialogEx -> Yeah!");
		AppendLogF("wDialogID: %i\nszInputResp: %s", wDialogID, szInputResp);
		_logs->Log(LogLevel::DEBUG, "DialogEx -> Yeah!<br />wDialogID: %i\nszInputResp: %s", wDialogID, szInputResp);
	}
	else
	{
		AppendLogF("DialogEx -> None sorry");
		_logs->Log(LogLevel::DEBUG, "DialogEx -> None sorry");
	}
}

void sendPickUp(int iPickupID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iPickupID);
	pRakClient->RPC(&RPC_PickedUpPickup, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void selectTextDraw(int iTextDrawID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iTextDrawID);
	pRakClient->RPC(&RPC_ClickTextDraw, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

//
void SetAnimation(short sAnimationId, float fDelta, bool bLoop, bool bLockX, bool bLockY, bool bFreeze, int iTime)
{
	playerPool[g_myPlayerID].onfootData.AnimationIndex = sAnimationId;
	if (sAnimationId == 0)
		playerPool[g_myPlayerID].onfootData.AnimationFlags = 0;
	else
		playerPool[g_myPlayerID].onfootData.AnimationFlags = (static_cast<uint8_t>(fDelta) & 0xFF) | (bLoop << 8) | (bLockX << 9) | (bLockY << 10) | (bFreeze << 11) | (static_cast<uint8_t>(iTime) << 12);
}

/*void SetAnimationByName(char *szName, float fDelta, bool bLoop, bool bLockX, bool bLockY, bool bFreeze, int iTime) {
	WORD wAnimationId = CAnimationInfo::GetIndexByName(szName);
	SetAnimation(wAnimationId, fDelta, bLoop, bLockX, bLockY, bFreeze, iTime);
}
bool LoadAnimationFromConfig(const stConfig *config)
{
	if (config->sAnimIndex == 0)
		return true;
	else if (config->sAnimIndex < 0 || config->sAnimIndex >= MAX_ANIMATIONS)
		return false;
	SetAnimation(config->sAnimIndex, 4.1f, config->bAnimLoop, config->bAnimLockX, config->bAnimLockY, false, config->iAnimTime);
	return true;
}
*/

void ResetAnimation()
{
	playerPool[g_myPlayerID].onfootData.AnimationIndex = playerPool[g_myPlayerID].onfootData.AnimationFlags = 0;
}

//
void SetToDeathState(PLAYERID murderer, uint8_t reason)
{
	if (!IsDead())
	{
		playerPool[g_myPlayerID].vehicleData.VehicleId = INVALID_VEHICLE_ID;
		SetState(STATE_DEAD);

		Playback::bPlaying = false;

		if (murderer != INVALID_PLAYER_ID)
			SendWastedNotification(reason, murderer);

		uRespawnTick = GetTickCount();
	}
}

void TakeBulletDamage(PLAYERID attacker, uint8_t weaponid)
{
	float amount;
	switch (weaponid)
	{
	case 24: // DesertEagle
		amount = 46.200013f;
		break;
	case 22: // Colt45
		amount = 8.25f;
		break;
	case 32: // Tec9
		amount = 6.599976f;
		break;
	case 28: // Uzi
		amount = 6.599976f;
		break;
	case 23: // SilencedColt
		amount = 13.200012f;
		break;
	case 31: // M4
		amount = 9.900024f;
		break;
	case 30: // AK
		amount = 9.900024f;
		break;
	case 29: // MP5
		amount = 8.25f;
		break;
	case 34: // SniperRifle
		amount = 41.299987f;
		break;
	case 33: // CuntGun
		amount = 24.799987f;
		break;
	case 25: // PumpShotgun
		amount = 30.0f;
		break;
	case 26: // Sawnoff
		amount = 30.0f;
	case 27: // Spaz12
		amount = 30.0f;
		break;
	case 38: // Minigun
		amount = 46.200013f;
		break;
	default:
		amount = -1;
		break;
	}
	if (amount > 0.f)
	{
		RakNet::BitStream bsParams;
		bsParams.Write(true);
		bsParams.Write((uint16_t)attacker);
		bsParams.Write(amount);
		bsParams.Write(weaponid);
		pRakClient->RPC(&RPC_PlayerGiveTakeDamage, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

		float remain = amount;
		if (playerPool[g_myPlayerID].onfootData.fArmour > 0.f)
		{
			playerPool[g_myPlayerID].onfootData.fArmour -= amount;
			if (playerPool[g_myPlayerID].onfootData.fArmour <= 0.f)
			{
				remain = abs(playerPool[g_myPlayerID].onfootData.fArmour);
				playerPool[g_myPlayerID].onfootData.fArmour = 0.f;
			}
			else
				remain = 0.f;
		}
		if (remain > 0.f)
		{
			playerPool[g_myPlayerID].onfootData.fHealth -= remain;
			if (playerPool[g_myPlayerID].onfootData.fHealth <= 0.f)
			{
				playerPool[g_myPlayerID].onfootData.fHealth = 0.f;
				SetToDeathState(attacker, weaponid);
			}
		}
	}
}
