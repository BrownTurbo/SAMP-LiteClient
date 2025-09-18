#include "../../include/Networking/netgame.h"

extern int iFollowingPassenger, iFollowingDriver;
extern int iDrunkLevel, iMoney;
extern uint8_t m_bLagCompensation;

uint32_t dwTimeReconnect = 10000;

int iPassengerNotificationSent = 0, iDriverNotificationSent = 0;

void Packet_AUTH_KEY(Packet *p, RakClientInterface *pRakClient)
{
	char *auth_key;
	bool found_key = false;

	for (int x = 0; x < (TOTAL_AUTHKEYS); x++)
	{
		if (!strcmp(((char *)p->data + 2), AuthKeyTable[x][0]))
		{
			auth_key = AuthKeyTable[x][1];
			found_key = true;
		}
	}

	if (found_key)
	{
		RakNet::BitStream bsKey;
		uint8_t BYTEAuthKeyLen;

		BYTEAuthKeyLen = (uint8_t)strlen(auth_key);

		bsKey.Write((uint8_t)ID_AUTH_KEY);
		bsKey.Write((uint8_t)BYTEAuthKeyLen);
		bsKey.Write(auth_key, BYTEAuthKeyLen);

		pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);

		AppendLogF("[AUTH] %s -> %s", ((char *)p->data + 2), auth_key);
		_logs->Log(LogLevel::DEBUG, "[AUTH] %s -> %s", ((char *)p->data + 2), auth_key);
	}
	else
	{
#if RECONNECT_ON_ERROR == 1
		AppendChatBox("Unknown AUTH_IN! (%s), Reconnecting in %d seconds...", ((char *)p->data + 2), iReconnectTime / 1000);
		AppendLog("Unknown AUTH_IN! (%s), Reconnecting in %d seconds...", ((char *)p->data + 2), iReconnectTime / 1000);
		_logs->Log(LogLevel::FATAL, "Unknown AUTH_IN! (%s), Reconnecting in %d seconds...", ((char *)p->data + 2), iReconnectTime / 1000);
		resetPools();
		SetGameState(GAMESTATE_WAIT_CONNECT);
#else
		AppendChatBox("Unknown AUTH_IN! (%s)", ((char *)p->data + 2));
		AppendLogF("Unknown AUTH_IN! (%s), Reconnecting in %d seconds...", ((char *)p->data + 2), iReconnectTime / 1000);
		_logs->Log(LogLevel::FATAL, "Unknown AUTH_IN! (%s)", ((char *)p->data + 2));
		//QApplication::quit();
#endif
		WorkerClass worker(Globals::instance().getCentralWidget());
		worker.moveToThread(QApplication::instance()->thread());
		emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was Rejected: Invalid Authentication credentials(AUTH_KEY)."), QMessageBox::Ok, QMessageBox::Critical);
	}
}

void Packet_ConnectionSucceeded(Packet *p, RakClientInterface *pRakClient)
{
	if (GetGameState() == GAMESTATE_DISCONNECTION_REQUESTED)
	{
		sampDisconnect(0);
		return;
	}
	SetGameState(GAMESTATE_AWAIT_JOIN);

	RakNet::BitStream bsSuccAuth((unsigned char *)p->data, p->length, false);
	PLAYERID myPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8);  // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port

	bsSuccAuth.Read(myPlayerID);

	g_myPlayerID = myPlayerID;
	playerPool[myPlayerID].bIsConnected = true;
	strcpy(playerPool[myPlayerID].szPlayerName, playerPool[g_myPlayerID].szPlayerName);

	bsSuccAuth.Read(uiChallenge);

	settings.uiChallange = uiChallenge;

	AppendChatBox("Connected with ID %d. Joining the game...", myPlayerID);
	AppendLogF("Connected with ID %d. Joining the game...", myPlayerID);
	_logs->Log(LogLevel::INFO, "Connected with ID %d. Joining the game...", myPlayerID);

	char ClientVersion[20];
	NETGAME_VERSION = (startsWith(settings.szClientVersion, "0.3.7") ? NETGAME_37 : NETGAME_3DL);
	snprintf(ClientVersion, sizeof(ClientVersion), "%s", (startsWith(settings.szClientVersion, "0.3.7") ? "0.3.7-R5" : "0.3DL"));
	int iVersion = NETGAME_VERSION;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	char auth_bs[4 * 16] = {0};
	gen_gpci(auth_bs, 0x3e9);

	uint8_t BYTEAuthBSLen;
	BYTEAuthBSLen = (uint8_t)strlen(auth_bs);
	uint8_t BYTENameLen = (uint8_t)strlen(playerPool[g_myPlayerID].szPlayerName);
	uint8_t iClientVerLen = (uint8_t)strlen(ClientVersion);

	RakNet::BitStream bsSend;

	bsSend.Write(iVersion);
	bsSend.Write((uint8_t)1);
	bsSend.Write(BYTENameLen);
	bsSend.Write(playerPool[g_myPlayerID].szPlayerName, BYTENameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(BYTEAuthBSLen);
	bsSend.Write(auth_bs, BYTEAuthBSLen);
	bsSend.Write(iClientVerLen);
	bsSend.Write(ClientVersion, iClientVerLen);

	pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

	playerPool[g_myPlayerID].onfootData.fHealth = 100.0f;
	playerPool[g_myPlayerID].onfootData.fArmour = 0.0f;
	iMoney = 0;
	playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon = 0;
	iDrunkLevel = 0;
	playerPool[g_myPlayerID].onfootData.BYTESkin = 0;
	SetState(STATE_NOT_SPAWNED);

	SetGameState(GAMESTATE_CONNECTED);
	ToggleChatBox();
	ToggleSendButton();
	TogglePlaybackButton();
	ToggleAudioButton();

	QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
	if (_connectBtn.isNull())
	{
		qWarning() << "_connectBtn is not a valid pointer";
		return;
	}
	QPushButton *connectBtn = _connectBtn.get();
	if (connectBtn == nullptr || connectBtn->parent() == nullptr)
	{
		qWarning() << "QPushButton instance is not valid.";
		return;
	}
	connectBtn->setText("Disconnect");
}

void Packet_PlayerSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsPlayerSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	AppendLogF("Packet_PlayerSync: %d \n%s\n", p->length, DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, "Packet_PlayerSync: %d \n%s\n", p->length, DumpMem((unsigned char *)p->data, p->length));

	bool bHasLR, bHasUD;
	bool bHasSurfInfo, bAnimation;

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsPlayerSync.IgnoreBits(8);
	bsPlayerSync.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	// Followed passenger exit
	if (g_myRunMode == RUNMODE_FOLLOWPLAYER && playerId == getPlayerIDFromPlayerName(szFollowingPlayerName))
	{
		if (iPassengerNotificationSent)
		{
			SendExitVehicleNotification(playerPool[playerId].incarData.VehicleID);
			iPassengerNotificationSent = 0;
		}

		iFollowingPassenger = 0;

		if (iDriverNotificationSent)
		{
			SendExitVehicleNotification(playerPool[playerId].incarData.VehicleID);
			iDriverNotificationSent = 0;
		}

		iFollowingDriver = 0;
	}
	playerPool[playerId].incarData.VehicleID = -1;

	// clear last data
	memset(&playerPool[playerId].onfootData, 0, sizeof(ONFOOT_SYNC_DATA));

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if (bHasLR)
		bsPlayerSync.Read(playerPool[playerId].onfootData.lrAnalog);

	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if (bHasUD)
		bsPlayerSync.Read(playerPool[playerId].onfootData.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(playerPool[playerId].onfootData.wKeys);

	// VECTOR POS
	bsPlayerSync.Read(playerPool[playerId].onfootData.fCurrentPosition[0]);
	bsPlayerSync.Read(playerPool[playerId].onfootData.fCurrentPosition[1]);
	bsPlayerSync.Read(playerPool[playerId].onfootData.fCurrentPosition[2]);

	// ROTATION
	bsPlayerSync.ReadNormQuat(
		playerPool[playerId].onfootData.fQuaternion[0],
		playerPool[playerId].onfootData.fQuaternion[1],
		playerPool[playerId].onfootData.fQuaternion[2],
		playerPool[playerId].onfootData.fQuaternion[3]);

	// HEALTH/ARMOUR (COMPRESSED INTO 1 uint8_t)
	uint8_t BYTEHealthArmour;
	uint8_t BYTEHealth, BYTEArmour;
	uint8_t BYTEArmTemp = 0, BYTEHlTemp = 0;

	bsPlayerSync.Read(BYTEHealthArmour);
	BYTEArmTemp = (BYTEHealthArmour & 0x0F);
	BYTEHlTemp = (BYTEHealthArmour >> 4);

	if (BYTEArmTemp == 0xF)
		BYTEArmour = 100;
	else if (BYTEArmTemp == 0)
		BYTEArmour = 0;
	else
		BYTEArmour = BYTEArmTemp * 7;

	if (BYTEHlTemp == 0xF)
		BYTEHealth = 100;
	else if (BYTEHlTemp == 0)
		BYTEHealth = 0;
	else
		BYTEHealth = BYTEHlTemp * 7;

	playerPool[playerId].onfootData.fHealth = BYTEHealth;
	playerPool[playerId].onfootData.fArmour = BYTEArmour;

	// CURRENT WEAPON
	bsPlayerSync.Read(playerPool[playerId].onfootData.BYTECurrentWeapon);

	// Special Action
	bsPlayerSync.Read(playerPool[playerId].onfootData.BYTESpecialAction);

	// READ MOVESPEED VECTORS
	bsPlayerSync.ReadVector(
		playerPool[playerId].onfootData.fMoveSpeed[0],
		playerPool[playerId].onfootData.fMoveSpeed[1],
		playerPool[playerId].onfootData.fMoveSpeed[2]);

	bsPlayerSync.Read(bHasSurfInfo);
	if (bHasSurfInfo)
	{
		bsPlayerSync.Read(playerPool[playerId].onfootData.fSurfOffsets[0]);
		bsPlayerSync.Read(playerPool[playerId].onfootData.fSurfOffsets[1]);
		bsPlayerSync.Read(playerPool[playerId].onfootData.fSurfOffsets[2]);
		bsPlayerSync.Read(playerPool[playerId].onfootData.wSurfInfo);
	}
	else
		playerPool[playerId].onfootData.wSurfInfo = -1;

	bsPlayerSync.Read(bAnimation);
	if (bAnimation)
	{
		bsPlayerSync.Read(playerPool[playerId].onfootData.AnimationIndex);
		bsPlayerSync.Read(playerPool[playerId].onfootData.AnimationFlags);
	}

	playerPool[playerId].bAreWeInAVehicle = false;

	if (PBRecorder::iPlaybackType == Playback::PLAYBACK_TYPE_ONFOOT)
		if (playerId == PBRecorder::iTargetPlayer)
			PBRecorder::AddOnfootPacket(&playerPool[playerId].onfootData);
}

//----------------------------------------------------

void Packet_UnoccupiedSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsUnocSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	AppendLogF("\n%s\n", DumpMem((unsigned char *)p->data + bsUnocSync.GetReadOffset() / 8, p->length));
	_logs->Log(LogLevel::DEBUG, DumpMem((unsigned char *)p->data + bsUnocSync.GetReadOffset() / 8, p->length));

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsUnocSync.IgnoreBits(8);
	bsUnocSync.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	UNOCCUPIED_SYNC_DATA cache;
	bsUnocSync.Read((char *)&cache, sizeof(UNOCCUPIED_SYNC_DATA));

	memset(&vehiclePool[cache.VehicleID], 0, sizeof(UNOCCUPIED_SYNC_DATA));
	vect3_copy(cache.vecPos, vehiclePool[cache.VehicleID].fPos);
}

void Packet_AimSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsAimSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	AppendLogF("Packet_AimSync:\n%s\n", DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, DumpMem((unsigned char *)p->data, p->length));

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsAimSync.IgnoreBits(8);
	bsAimSync.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	memset(&playerPool[playerId].aimData, 0, sizeof(AIM_SYNC_DATA));

	bsAimSync.Read((char *)&playerPool[playerId].aimData, sizeof(AIM_SYNC_DATA));
}

void Packet_VehicleSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	VEHICLEID VehicleID;
	bool bLandingGear;
	bool bHydra, bTrain, bTrailer;
	bool bSiren;

	AppendLogF("Packet_VehicleSync: %d \n%s\n", p->length, DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, "Packet_VehicleSync: %d \n%s\n", p->length, DumpMem((unsigned char *)p->data, p->length));
	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsSync.IgnoreBits(8);
	bsSync.Read(playerId);
	bsSync.Read(VehicleID);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;
	if (VehicleID < 0 || VehicleID >= MAX_VEHICLES)
		return;

	// Follower passenger enter
	playerPool[playerId].incarData.VehicleID = VehicleID;
	if (g_myRunMode == RUNMODE_FOLLOWPLAYER && playerId == getPlayerIDFromPlayerName(szFollowingPlayerName))
	{
		if (!iPassengerNotificationSent)
		{
			SendEnterVehicleNotification(VehicleID, 1);
			iPassengerNotificationSent = 1;
		}

		SendPassengerFullSyncData(VehicleID);
		iFollowingPassenger = 1;
		return;
	}

	// clear last data
	memset(&playerPool[playerId].incarData, 0, sizeof(INCAR_SYNC_DATA));

	// LEFT/RIGHT KEYS
	bsSync.Read(playerPool[playerId].incarData.lrAnalog);

	// UP/DOWN KEYS
	bsSync.Read(playerPool[playerId].incarData.udAnalog);

	// GENERAL KEYS
	bsSync.Read(playerPool[playerId].incarData.wKeys);

	// ROLL / DIRECTION
	// ROTATION
	bsSync.ReadNormQuat(
		playerPool[playerId].incarData.fQuaternion[0],
		playerPool[playerId].incarData.fQuaternion[1],
		playerPool[playerId].incarData.fQuaternion[2],
		playerPool[playerId].incarData.fQuaternion[3]);

	// POSITION
	bsSync.Read(playerPool[playerId].incarData.vecPos[0]);
	bsSync.Read(playerPool[playerId].incarData.vecPos[1]);
	bsSync.Read(playerPool[playerId].incarData.vecPos[2]);
	vect3_copy(playerPool[playerId].incarData.vecPos, vehiclePool[VehicleID].fPos);

	// SPEED
	bsSync.ReadVector(
		playerPool[playerId].incarData.fMoveSpeed[0],
		playerPool[playerId].incarData.fMoveSpeed[1],
		playerPool[playerId].incarData.fMoveSpeed[2]);

	// VEHICLE HEALTH
	uint16_t wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	playerPool[playerId].incarData.fHealth = (float)wTempVehicleHealth;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 uint8_t)
	uint8_t BYTEHealthArmour;
	uint8_t fPlayerHealth, fPlayerArmour;
	uint8_t BYTEArmTemp = 0, BYTEHlTemp = 0;

	bsSync.Read(BYTEHealthArmour);
	BYTEArmTemp = (BYTEHealthArmour & 0x0F);
	BYTEHlTemp = (BYTEHealthArmour >> 4);

	if (BYTEArmTemp == 0xF)
		fPlayerArmour = 100;
	else if (BYTEArmTemp == 0)
		fPlayerArmour = 0;
	else
		fPlayerArmour = BYTEArmTemp * 7;

	if (BYTEHlTemp == 0xF)
		fPlayerHealth = 100;
	else if (BYTEHlTemp == 0)
		fPlayerHealth = 0;
	else
		fPlayerHealth = BYTEHlTemp * 7;

	playerPool[playerId].incarData.fPlayerHealth = fPlayerHealth;
	playerPool[playerId].incarData.fPlayerArmour = fPlayerArmour;

	// CURRENT WEAPON
	bsSync.Read(playerPool[playerId].incarData.BYTECurrentWeapon);

	// SIREN
	bsSync.ReadCompressed(bSiren);
	if (bSiren)
		playerPool[playerId].incarData.BYTESirenOn = 1;

	// LANDING GEAR
	bsSync.ReadCompressed(bLandingGear);
	if (bLandingGear)
		playerPool[playerId].incarData.BYTELandingGearState = 1;

	// HYDRA THRUST ANGLE AND TRAILER ID
	bsSync.ReadCompressed(bHydra);
	bsSync.ReadCompressed(bTrailer);

	uint32_t dwTrailerID_or_ThrustAngle;
	bsSync.Read(dwTrailerID_or_ThrustAngle);
	playerPool[playerId].incarData.TrailerID_or_ThrustAngle = (uint16_t)dwTrailerID_or_ThrustAngle;

	// TRAIN SPECIAL
	uint16_t wSpeed;
	bsSync.ReadCompressed(bTrain);
	if (bTrain)
	{
		bsSync.Read(wSpeed);
		playerPool[playerId].incarData.fTrainSpeed = (float)wSpeed;
	}

	playerPool[playerId].bAreWeInAVehicle = true;

	if (PBRecorder::iPlaybackType == Playback::PLAYBACK_TYPE_DRIVER)
		if (playerId == PBRecorder::iTargetPlayer)
			PBRecorder::AddVehiclePacket(&playerPool[playerId].incarData);
}

void Packet_PassengerSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsPassengerSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;
	PASSENGER_SYNC_DATA psSync;

	AppendLogF("Packet_PassengerSync:\n%s\n", DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, DumpMem((unsigned char *)p->data, p->length));

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsPassengerSync.IgnoreBits(8);
	bsPassengerSync.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	bsPassengerSync.Read((char *)&psSync, sizeof(PASSENGER_SYNC_DATA));

	// Followed wants to drive the vehicle
	playerPool[playerId].passengerData.VehicleID = psSync.VehicleID;
	if (g_myRunMode == RUNMODE_FOLLOWPLAYER && playerId == getPlayerIDFromPlayerName(szFollowingPlayerName))
	{
		if (!iDriverNotificationSent)
		{
			SendEnterVehicleNotification(psSync.VehicleID, 0);
			iDriverNotificationSent = 1;
		}

		INCAR_SYNC_DATA icSync;
		memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));
		icSync.VehicleID = psSync.VehicleID;
		icSync.fHealth = 1000.00f;
		icSync.fPlayerHealth = (uint8_t)playerPool[g_myPlayerID].onfootData.fHealth;
		icSync.fPlayerArmour = (uint8_t)playerPool[g_myPlayerID].onfootData.fArmour;
		SendInCarFullSyncData(&icSync, 1, -1);

		iFollowingDriver = 1;
		return;
	}
}

void Packet_TrailerSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsSpectatorSync((unsigned char *)p->data, p->length, false);

	// uint8_t PacketID;
	PLAYERID playerId;
	// TRAILER_SYNC_DATA trSync;

	AppendLogF("Packet_TrailerSync:\n%s\n", DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, DumpMem((unsigned char *)p->data, p->length));

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsSpectatorSync.IgnoreBits(8);
	// bsSpectatorSync.Read(PacketID);
	bsSpectatorSync.Read(playerId);
	// bsSpectatorSync.Read((char*)&trSync, sizeof(TRAILER_SYNC_DATA));
}

void Packet_MarkersSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsMarkersSync((unsigned char *)p->data, p->length, false);

	int i, iNumberOfPlayers;
	PLAYERID playerID;
	short sPosX, sPosY, sPosZ;
	bool bIsPlayerActive;

	AppendLogF("Packet_MarkersSync:\n%s\n", DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, DumpMem((unsigned char *)p->data, p->length));

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsMarkersSync.IgnoreBits(8);
	bsMarkersSync.Read(iNumberOfPlayers);

	if (iNumberOfPlayers < 0 || iNumberOfPlayers > MAX_PLAYERS)
		return;

	for (i = 0; i < iNumberOfPlayers; i++)
	{
		bsMarkersSync.Read(playerID);

		if (playerID < 0 || playerID >= MAX_PLAYERS)
			return;

		bsMarkersSync.ReadCompressed(bIsPlayerActive);
		if (!bIsPlayerActive)
		{
			playerPool[playerID].bGotMarkersPos = false;
			continue;
		}

		bsMarkersSync.Read(sPosX);
		bsMarkersSync.Read(sPosY);
		bsMarkersSync.Read(sPosZ);

		playerPool[playerID].bGotMarkersPos = true;
		playerPool[playerID].onfootData.fCurrentPosition[0] = (float)sPosX;
		playerPool[playerID].onfootData.fCurrentPosition[1] = (float)sPosY;
		playerPool[playerID].onfootData.fCurrentPosition[2] = (float)sPosZ;

		AppendLogF("Packet_MarkersSync: %d %d %0.2f, %0.2f, %0.2f", playerID, bIsPlayerActive, (float)sPosX, (float)sPosY, (float)sPosZ);
		_logs->Log(LogLevel::DEBUG, "Packet_MarkersSync: %d %d %0.2f, %0.2f, %0.2f", playerID, bIsPlayerActive, (float)sPosX, (float)sPosY, (float)sPosZ);
	}
}

void Packet_BulletSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsBulletSync((unsigned char *)p->data, p->length, false);

	AppendLogF("Packet_BulletSync:\n%s\n", DumpMem((unsigned char *)p->data, p->length));
	_logs->Log(LogLevel::DEBUG, DumpMem((unsigned char *)p->data, p->length));

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	if (m_bLagCompensation)
	{
		PLAYERID PlayerID;

		bsBulletSync.IgnoreBits(8);
		bsBulletSync.Read(PlayerID);

		if (PlayerID < 0 || PlayerID >= MAX_PLAYERS)
			return;

		memset(&playerPool[PlayerID].bulletData, 0, sizeof(BULLET_SYNC_DATA));

		bsBulletSync.Read((char *)&playerPool[PlayerID].bulletData, sizeof(BULLET_SYNC_DATA));

		if (playerPool[g_myPlayerID].onfootData.fHealth > 0)
		{
			if (playerPool[PlayerID].bulletData.iHitID == g_myPlayerID && playerPool[PlayerID].bulletData.bHitType == BULLET_HIT_TYPE_PLAYER)
				TakeBulletDamage(PlayerID, playerPool[PlayerID].bulletData.bWeaponID);
		}

		PLAYERID copyingID = getPlayerIDFromPlayerName(szFollowingPlayerName);

		if (copyingID != (PLAYERID)-1 && (g_myRunMode == RUNMODE_FOLLOWPLAYER || g_myRunMode == RUNMODE_FOLLOWPLAYERSVEHICLE))
		{
			if (copyingID == PlayerID)
				SendBulletData(&playerPool[PlayerID].bulletData);
		}
	}
}

void resetPools()
{
	memset(playerPool, 0, sizeof(PlayerPool));
	memset(vehiclePool, 0, sizeof(VehiclePool));
	memset(pickupPool, 0, sizeof(PickupPool));
	for (PICKUPID pickupIDX; pickupIDX < MAX_PICKUPS; pickupIDX++)
		pickupExist[pickupIDX] = false;
	SetState(STATE_VOID);
}

void UpdatePlayerScoresAndPings(int iWait, int iMS, RakClientInterface *pRakClient)
{
	static uint32_t dwLastUpdateTick = 0;

	if (iWait)
	{
		if ((GetTickCount() - dwLastUpdateTick) > (uint32_t)iMS)
		{
			dwLastUpdateTick = GetTickCount();
			RakNet::BitStream bsParams;
			pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
		}
	}
	else
	{
		RakNet::BitStream bsParams;
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void UpdateNetwork(RakClientInterface *pRakClient, Packet *pkt)
{
	WorkerClass worker(Globals::instance().getCentralWidget());
	worker.moveToThread(QApplication::instance()->thread());
	unsigned char packetIdentifier;
	time_t curr_time = time(NULL);
	struct tm *localTime = localtime(&curr_time);

	if (pkt)
	{
		if ((unsigned char)pkt->data[0] == ID_TIMESTAMP)
		{
			if (pkt->length > sizeof(unsigned char) + sizeof(unsigned int))
				packetIdentifier = (unsigned char)pkt->data[sizeof(unsigned char) + sizeof(unsigned int)];
			else
				return;
		}
		else
			packetIdentifier = (unsigned char)pkt->data[0];

		AppendLogF("Packet received. PacketID: %d.", pkt->data[0]);
		_logs->Log(LogLevel::DEBUG, "Packet received. PacketID: %d.", pkt->data[0]);
		switch (pkt->length)
		{
		case 1:
		{
			AppendLogF("Packet received. PacketID[0]: %d.", pkt->data[0]);
			_logs->Log(LogLevel::DEBUG, "Packet received. PacketID[0]: %d.", pkt->data[0]);
			break;
		}
		case 2:
		{
			AppendLogF("Packet received. PacketID[0]: %d. PacketID[1]?: %d.", pkt->data[0], pkt->data[1]);
			_logs->Log(LogLevel::DEBUG, "Packet received. PacketID[0]: %d. PacketID[1]?: %d.", pkt->data[0], pkt->data[1]);
			break;
		}
		default:
			AppendLogF("Empty packet received.");
			_logs->Log(LogLevel::DEBUG, "Empty packet received.");
		}

		switch (packetIdentifier)
		{
		case ID_RSA_PUBLIC_KEY_MISMATCH:
			if (pRakClient == ::pRakClient)
			{
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>]Failed to initialize encryption. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("Failed to initialize encryption. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::ERROR, "Failed to initialize encryption. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>]Failed to initialize encryption.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("Failed to initialize encryption.");
				_logs->Log(LogLevel::ERROR, "Failed to initialize encryption.");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Internal Failure."), QMessageBox::Ok, QMessageBox::Information);
				ToggleChatBox();
				ToggleSendButton();
				TogglePlaybackButton();
				ToggleAudioButton();
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (pRakClient == ::pRakClient)
			{
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>]Connection was closed by the server. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("Connection was closed by the server. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::INFO, "Connection was closed by the server. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>]Connection was closed by the server.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("Connection was closed by the server.");
				_logs->Log(LogLevel::INFO, "Connection was closed by the server.");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Connection was closed by the server."), QMessageBox::Ok, QMessageBox::Critical);
				pRakClient->Disconnect(2000);
				ToggleChatBox();
				ToggleSendButton();
				TogglePlaybackButton();
				ToggleAudioButton();
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_CONNECTION_BANNED:
			if (pRakClient == ::pRakClient)
			{
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>]You are banned.. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("You are banned.. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::INFO, "You are banned.. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>]You are banned..",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("You are banned..");
				_logs->Log(LogLevel::INFO, "You are banned..");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Information"), QString::fromUtf8("[SAMP] You are banned..."), QMessageBox::Ok, QMessageBox::Information);
				ToggleChatBox();
				ToggleSendButton();
				TogglePlaybackButton();
				ToggleAudioButton();
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			if (pRakClient == ::pRakClient)
			{
				if (GetGameState() != GAMESTATE_DISCONNECTION_REQUESTED)
				{
					ToggleChatBox();
					ToggleSendButton();
					TogglePlaybackButton();
					ToggleAudioButton();
				}
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>] Connection attempt failed. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("Connection attempt failed. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::INFO, "Connection attempt failed. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>] Connection attempt failed.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("Connection attempt failed.");
				_logs->Log(LogLevel::INFO, "Connection attempt failed.");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Connection attempt failed."), QMessageBox::Ok, QMessageBox::Critical);
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			if (pRakClient == ::pRakClient)
			{
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>]The server is full. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("The server is full. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::INFO, "The server is full. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>]The server is full.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("The server is full.");
				_logs->Log(LogLevel::INFO, "The server is full.");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The server is full."), QMessageBox::Ok, QMessageBox::Critical);
				ToggleChatBox();
				ToggleSendButton();
				TogglePlaybackButton();
				ToggleAudioButton();
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_INVALID_PASSWORD:
			if (pRakClient == ::pRakClient)
			{
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>]Invalid password. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("Invalid password. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::INFO, "Invalid password. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>]Invalid password.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("Invalid password.");
				_logs->Log(LogLevel::ERROR, "Invalid password.");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Invalid password."), QMessageBox::Ok, QMessageBox::Critical);
				pRakClient->Disconnect(0);
				ToggleChatBox();
				ToggleSendButton();
				TogglePlaybackButton();
				ToggleAudioButton();
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_CONNECTION_LOST:
			if (pRakClient == ::pRakClient)
			{
#if RECONNECT_ON_ERROR == 1
				AppendChatBox("[<b>%02d:%02d:%02d</b>]The connection was lost. Reconnecting in %d seconds.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
				AppendLogF("The connection was lost. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				_logs->Log(LogLevel::INFO, "The connection was lost. Reconnecting in %d seconds.",
						   iReconnectTime / 1000);
				resetPools();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SetGameState(GAMESTATE_WAIT_CONNECT);
#else
				AppendChatBox("[<b>%02d:%02d:%02d</b>]The connection was lost.",
							  localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
				AppendLogF("The connection was lost.");
				_logs->Log(LogLevel::INFO, "The connection was lost.");
				SetGameState(GAMESTATE_DISCONNECTED);
#endif
				emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was lost."), QMessageBox::Ok, QMessageBox::Critical);
				ToggleChatBox();
				ToggleSendButton();
				TogglePlaybackButton();
				ToggleAudioButton();
				emit worker.setStateMessage("Disconnected.");

				QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
				if (_connectBtn.isNull())
				{
					qWarning() << "_connectBtn is not a valid pointer";
					break;
				}
				QPushButton *connectBtn = _connectBtn.get();
				if (connectBtn == nullptr || connectBtn->parent() == nullptr)
				{
					qWarning() << "QPushButton instance is not valid.";
					break;
				}
				connectBtn->setText("Connect");
				QPointer<Scoreboard> scoreboard_ = new Scoreboard(getMainWidget());
				QMetaObject::invokeMethod(getMainWindow(), [=]()
										  { if (scoreboard_) scoreboard_->removePlayer(g_myPlayerID); }, Qt::QueuedConnection);
			}
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			Packet_ConnectionSucceeded(pkt, pRakClient);
			break;
		case ID_AUTH_KEY:
			Packet_AUTH_KEY(pkt, pRakClient);
			break;
		case ID_PLAYER_SYNC:
			Packet_PlayerSync(pkt, pRakClient);
			break;
		case ID_VEHICLE_SYNC:
			Packet_VehicleSync(pkt, pRakClient);
			break;
		case ID_PASSENGER_SYNC:
			Packet_PassengerSync(pkt, pRakClient);
			break;
		case ID_AIM_SYNC:
			Packet_AimSync(pkt, pRakClient);
			break;
		case ID_TRAILER_SYNC:
			Packet_TrailerSync(pkt, pRakClient);
			break;
		case ID_UNOCCUPIED_SYNC:
			Packet_UnoccupiedSync(pkt, pRakClient);
			break;
		case ID_MARKERS_SYNC:
			Packet_MarkersSync(pkt, pRakClient);
			break;
		case ID_BULLET_SYNC:
			Packet_BulletSync(pkt, pRakClient);
			break;
		default:
		{
			AppendLogF("Received a strange packet ID %d",
					   packetIdentifier);
			_logs->Log(LogLevel::DEBUG, "Received a strange packet ID %d",
					   packetIdentifier);
		}
		}

		pRakClient->DeallocatePacket(pkt);
		pkt = pRakClient->Receive();
	}
	else if (pkt == NULL)
	{
		if ((GetGameState() == GAMESTATE_CONNECTION_REQUESTED || GetGameState() == GAMESTATE_WAIT_CONNECT || GetGameState() == GAMESTATE_CONNECTING) && !receivedResponse)
		{
			AppendChatBox("[<b>%02d:%02d:%02d</b>] No response from server. Attempting to reconnect in %d seconds", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, iReconnectTime / 1000);
			/*emit worker.setStateMessage("Probably disconnected.");*/
			AppendLogF("Packet Receiver returned NULL. No response from server.");
			_logs->Log(LogLevel::DEBUG, "Packet Receiver returned NULL. No response from server.");
			// SetGameState(GAMESTATE_WAIT_CONNECT);
			receivedResponse = true;
		}
	}
}
