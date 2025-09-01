#include "../../include/Networking/netrpc.h"

int iNetModeNormalOnfootSendRate, iNetModeNormalIncarSendRate, iNetModeFiringSendRate, iNetModeSendMultiplier;

char g_szHostName[256];

bool iGettingNewName = false;

struct stGTAMenu GTAMenu;

struct stSAMPDialog sampDialog;

int bIsSpectating = 0;

void ServerJoin(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szPlayerName[256];
	PLAYERID playerId;
	uint8_t BYTENameLen = 0;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	int dwColor = 0;
	bsData.Read(dwColor);

	uint8_t red = (dwColor >> 24) & 0xFF;
	uint8_t green = (dwColor >> 16) & 0xFF;
	uint8_t blue = (dwColor >> 8) & 0xFF;
	QString HexColor = QString("#%1%2%3").arg(red, 2, 16, QChar('0')).arg(green, 2, 16, QChar('0')).arg(blue, 2, 16, QChar('0')).toUpper();

	playerPool[playerId].RGBAcolor = dwColor;
	playerPool[playerId].HEXcolor = HexColor;

	uint8_t bIsNPC = 0;
	bsData.Read(bIsNPC);
	bsData.Read(BYTENameLen);
	if (BYTENameLen > 20)
		return;
	bsData.Read(szPlayerName, BYTENameLen);
	szPlayerName[BYTENameLen] = '\0';

	playerPool[playerId].bIsConnected = true;
	playerPool[playerId].BYTEIsNPC = bIsNPC;
	strcpy((char *)playerPool[playerId].szPlayerName, szPlayerName);

	AppendLogF("***[JOIN] (%d) %s", playerId, szPlayerName);
	_logs->Log(LogLevel::DEBUG, "***[JOIN] (%d) %s", playerId, szPlayerName);
}

void ServerQuit(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	uint8_t uint8_tReason;

	bsData.Read(playerId);
	bsData.Read(uint8_tReason);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	playerPool[playerId].bIsConnected = false;
	playerPool[playerId].BYTEIsNPC = 0;
	AppendLogF("***[QUIT:%d] (%d) %s", uint8_tReason, playerId, playerPool[playerId].szPlayerName);
	_logs->Log(LogLevel::DEBUG, "***[QUIT:%d] (%d) %s", uint8_tReason, playerId, playerPool[playerId].szPlayerName);
	memset(playerPool[playerId].szPlayerName, 0, 20);
}

void InitGame(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsInitGame((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID MyPlayerID;
	bool bLanMode, bStuntBonus;
	uint8_t uint8_tVehicleModels[212];

	bool m_bZoneNames, m_bUseCJWalk, m_bAllowWeapons, m_bLimitGlobalChatRadius;
	float m_fGlobalChatRadius, m_fNameTagDrawDistance;
	bool m_bDisableEnterExits, m_bNameTagLOS, m_bManualVehicleEngineAndLight;
	bool m_bShowPlayerTags;
	int m_iShowPlayerMarkers;
	uint8_t m_uint8_tWorldTime, m_uint8_tWeather;
	float m_fGravity;
	int m_iDeathDropMoney;
	bool m_bInstagib;

	bsInitGame.ReadCompressed(m_bZoneNames);
	bsInitGame.ReadCompressed(m_bUseCJWalk);
	bsInitGame.ReadCompressed(m_bAllowWeapons);
	bsInitGame.ReadCompressed(m_bLimitGlobalChatRadius);
	bsInitGame.Read(m_fGlobalChatRadius);
	bsInitGame.ReadCompressed(bStuntBonus);
	bsInitGame.Read(m_fNameTagDrawDistance);
	bsInitGame.ReadCompressed(m_bDisableEnterExits);
	bsInitGame.ReadCompressed(m_bNameTagLOS);
	bsInitGame.ReadCompressed(m_bManualVehicleEngineAndLight); //
	bsInitGame.Read(iSpawnsAvailable);
	bsInitGame.Read(MyPlayerID);
	bsInitGame.ReadCompressed(m_bShowPlayerTags);
	bsInitGame.Read(m_iShowPlayerMarkers);
	bsInitGame.Read(m_uint8_tWorldTime);
	g_srvWorldTime = m_uint8_tWorldTime;
	bsInitGame.Read(m_uint8_tWeather);
	g_srvWeather = m_uint8_tWeather;
	bsInitGame.Read(m_fGravity);
	bsInitGame.ReadCompressed(bLanMode);
	bsInitGame.Read(m_iDeathDropMoney);
	bsInitGame.ReadCompressed(m_bInstagib);

	// Server's send rate restrictions
	bsInitGame.Read(iNetModeNormalOnfootSendRate);
	bsInitGame.Read(iNetModeNormalIncarSendRate);
	bsInitGame.Read(iNetModeFiringSendRate);
	bsInitGame.Read(iNetModeSendMultiplier);

	bsInitGame.Read(m_bLagCompensation);

	/*uint8_t unk;
	bsInitGame.Read(unk);
	bsInitGame.Read(unk);
	bsInitGame.Read(unk);*/

	uint8_t uint8_tStrLen;
	bsInitGame.Read(uint8_tStrLen);
	if (uint8_tStrLen)
	{
		memset(g_szHostName, 0, sizeof(g_szHostName));
		bsInitGame.Read(g_szHostName, uint8_tStrLen);
	}
	g_szHostName[uint8_tStrLen] = '\0';

	bsInitGame.Read((char *)&uint8_tVehicleModels[0], 212);

	SetGameState(GAMESTATE_CONNECTED);
	g_myPlayerID = MyPlayerID;

	AppendChatBox("Connected to %.64s | %s(ID:%d)", g_szHostName, playerPool[g_myPlayerID].szPlayerName, g_myPlayerID);
	AppendLogF("Connected to %.64s | %s(ID:%d)", g_szHostName, playerPool[g_myPlayerID].szPlayerName, g_myPlayerID);
	_logs->Log(LogLevel::INFO, "Connected to %.64s | %s(ID:%d)", g_szHostName, playerPool[g_myPlayerID].szPlayerName, g_myPlayerID);

	iGameInited = true;
}

void WorldPlayerAdd(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	uint8_t BYTEFightingStyle = 4;
	uint16_t wSkillLevel[11];
	uint8_t BYTETeam = 0;
	int iSkin = 0;
	float vecPos[3];
	float fRotation = 0;
	uint32_t dwColor = 0;

	bsData.Read(playerId);
	bsData.Read(BYTETeam);
	bsData.Read(iSkin);
	bsData.Read(vecPos[0]);
	bsData.Read(vecPos[1]);
	bsData.Read(vecPos[2]);
	bsData.Read(fRotation);
	bsData.Read(dwColor);
	bsData.Read(BYTEFightingStyle);
	bsData.Read(wSkillLevel);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	uint8_t red = (dwColor >> 24) & 0xFF;
	uint8_t green = (dwColor >> 16) & 0xFF;
	uint8_t blue = (dwColor >> 8) & 0xFF;
	QString HexColor = QString("#%1%2%3").arg(red, 2, 16, QChar('0')).arg(green, 2, 16, QChar('0')).arg(blue, 2, 16, QChar('0')).toUpper();

	playerPool[playerId].RGBAcolor = dwColor;
	playerPool[playerId].HEXcolor = HexColor;

	playerPool[playerId].onfootData.FightingStyle = BYTEFightingStyle;
	playerPool[playerId].onfootData.BYTESkin = iSkin;
	playerPool[playerId].onfootData.fCurrentAngle = fRotation;
	playerPool[playerId].onfootData.BYTETeam = BYTETeam;

	playerPool[playerId].bIsStreamedIn = true;
	playerPool[playerId].onfootData.fCurrentPosition[0] =
		playerPool[playerId].incarData.vecPos[0] = vecPos[0];
	playerPool[playerId].onfootData.fCurrentPosition[1] =
		playerPool[playerId].incarData.vecPos[1] = vecPos[1];
	playerPool[playerId].onfootData.fCurrentPosition[2] =
		playerPool[playerId].incarData.vecPos[2] = vecPos[2];
	memcpy(playerPool[playerId].onfootData.SkillLevel, wSkillLevel, sizeof(uint16_t) * 11);

	AppendLogF("[WORLD ADD] Player [%d]", playerId);
	_logs->Log(LogLevel::DEBUG, "[WORLD ADD] Player [%d]", playerId);
}

void WorldPlayerDeath(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	bsData.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	if(playerId == g_myPlayerID)
		SetState(STATE_DEAD);

	AppendLogF("[PLAYER_DEATH] %d", playerId);
	_logs->Log(LogLevel::DEBUG, "[PLAYER_DEATH] %d", playerId);
}

void WorldPlayerRemove(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId = 0;
	bsData.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	playerPool[playerId].bIsStreamedIn = false;
	vect3_zero(playerPool[playerId].incarData.vecPos);

	AppendLogF("[PLAYER_REMOVE] %d", playerId);
	_logs->Log(LogLevel::DEBUG, "[PLAYER_REMOVE] %d", playerId);
}

void WorldVehicleAdd(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	NEW_VEHICLE NewVehicle;

	bsData.Read((char *)&NewVehicle, sizeof(NEW_VEHICLE));

	if (NewVehicle.VehicleId < 0 || NewVehicle.VehicleId >= MAX_VEHICLES)
		return;

	vehiclePool[NewVehicle.VehicleId].iExist = 1;
	vect3_copy(vehiclePool[NewVehicle.VehicleId].fPos, NewVehicle.vecPos);
	vehiclePool[NewVehicle.VehicleId].fRotation = NewVehicle.fRotation;
	vehiclePool[NewVehicle.VehicleId].fHealth = NewVehicle.fHealth;
	vehiclePool[NewVehicle.VehicleId].BYTEInterior = NewVehicle.BYTEInterior;
	vehiclePool[NewVehicle.VehicleId].iModelID = NewVehicle.iVehicleType;

	AppendLogF("[VEHICLE_ADD:%d] ModelID: %d, Position: %0.2f, %0.2f, %0.2f",
			   NewVehicle.VehicleId, NewVehicle.iVehicleType, NewVehicle.vecPos[0], NewVehicle.vecPos[1], NewVehicle.vecPos[2]);
	_logs->Log(LogLevel::DEBUG, "[VEHICLE_ADD:%d] ModelID: %d, Position: %0.2f, %0.2f, %0.2f",
			   NewVehicle.VehicleId, NewVehicle.iVehicleType, NewVehicle.vecPos[0], NewVehicle.vecPos[1], NewVehicle.vecPos[2]);
}

void WorldVehicleRemove(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	VEHICLEID VehicleID;

	bsData.Read(VehicleID);

	if (VehicleID < 0 || VehicleID >= MAX_VEHICLES)
		return;

	vehiclePool[VehicleID].iExist = 0;
	vect3_zero(vehiclePool[VehicleID].fPos);

	AppendLogF("[VEHICLE_REMOVE] %d", VehicleID);
	_logs->Log(LogLevel::DEBUG, "[VEHICLE_REMOVE] %d", VehicleID);
}

void WorldActorAdd(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t actorId;
	uint32_t iSkinId;
	VECTOR vecPos;
	float fRotation;
	float fHealth;
	bool bInvulnerable;

	bsData.Read(actorId);
	bsData.Read(iSkinId);
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	bsData.Read(fRotation);
	bsData.Read(fHealth);
	bsData.Read(bInvulnerable);

	AppendLogF("[WorldActorAdd:%d] skin=%d, posX=%f,posY=%f, posZ=%f, rot=%f, health=%f, invulnerabe=%s", actorId, iSkinId, vecPos.X, vecPos.Y, vecPos.Z, fRotation, fHealth, (bInvulnerable ? "Yes" : "No"));
	_logs->Log(LogLevel::DEBUG, "[WorldActorAdd:%d] skin=%d, posX=%f,posY=%f, posZ=%f, rot=%f, health=%f, invulnerabe=%s", actorId, iSkinId, vecPos.X, vecPos.Y, vecPos.Z, fRotation, fHealth, (bInvulnerable ? "Yes" : "No"));
}

void WorldActorRemove(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t actorId;

	bsData.Read(actorId);

	AppendLogF("[WorldActorRemove:%d] DELETED.", actorId);
	_logs->Log(LogLevel::DEBUG, "[WorldActorRemove:%d] DELETED.", actorId);
}

void ConnectionRejected(RPCParameters *rpcParams)
{
	WorkerClass worker(Globals::instance().getCentralWidget());
	worker.moveToThread(QApplication::instance()->thread());
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	uint8_t BYTERejectReason;

	bsData.Read(BYTERejectReason);

	if (BYTERejectReason == REJECT_REASON_BAD_VERSION)
	{
		AppendChatBox("[SAMP] Bad SA-MP version.");
		_logs->Log(LogLevel::FATAL, "[SAMP] Bad SA-MP version.");
		emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was Rejected: Bad SA-MP version."), QMessageBox::Ok, QMessageBox::Critical);
	}
	else if (BYTERejectReason == REJECT_REASON_BAD_NICKNAME)
	{
		char szNewNick[MAX_PLAYER_NAME], randgen[5];

		iGettingNewName = true;

		gen_random(randgen, sizeof(randgen));
		sprintf(szNewNick, "TMP_%s", randgen);

		AppendChatBox("[SAMP] Bad nickname. Changing name to %s");
		_logs->Log(LogLevel::ERROR, "[SAMP] Bad nickname. Changing name to %s");
		emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was Rejected: Bad nickname.\nPlease choose another nick between 3-16 characters\ncontaining only A-Z a-z 0-9 [ ] or _\nChanging name..."), QMessageBox::Ok, QMessageBox::Warning);

		strcpy(playerPool[g_myPlayerID].szPlayerName, szNewNick);
		resetPools();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		SetGameState(GAMESTATE_WAIT_CONNECT);
		return;
	}
	else if (BYTERejectReason == REJECT_REASON_BAD_MOD)
	{
		AppendChatBox("[SAMP] Bad mod version.");
		_logs->Log(LogLevel::FATAL, "[SAMP] Bad mod version.");
		emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was Rejected: Bad mod version."), QMessageBox::Ok, QMessageBox::Critical);
	}
	else if (BYTERejectReason == REJECT_REASON_BAD_PLAYERID)
	{
		AppendChatBox("[SAMP] Bad player ID.");
		_logs->Log(LogLevel::FATAL, "[SAMP] Bad player ID.");
		emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was Rejected: Bad player ID."), QMessageBox::Ok, QMessageBox::Critical);
	}
	else
	{
		AppendChatBox("Connection Rejected: Unknown(BYTERejectReason: %d)", BYTERejectReason);
		_logs->Log(LogLevel::INFO, "Connection Rejected: Unknown(BYTERejectReason: %d)", BYTERejectReason);
		emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] The connection was Rejected."), QMessageBox::Ok, QMessageBox::Information);
	}
	pRakClient->Disconnect(500);
}

void __ClientMessage(RPCParameters *rpcParams)
{
	// if(!iGameInited) return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	uint32_t dwStrLen, dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);
	char *szMsg = (char *)malloc(dwStrLen + 1);
	bsData.Read(szMsg, dwStrLen);
	szMsg[dwStrLen] = 0;

	uint8_t red = (dwColor >> 24) & 0xFF;
	uint8_t green = (dwColor >> 16) & 0xFF;
	uint8_t blue = (dwColor >> 8) & 0xFF;
	QString HexColor = QString("#%1%2%3").arg(red, 2, 16, QChar('0')).arg(green, 2, 16, QChar('0')).arg(blue, 2, 16, QChar('0')).toUpper();

	AppendChatBox("[CMSG] <span color=\"%s\">%s</span>", HexColor.toUtf8().data(), formatColorMessage(removeMarkupTags(szMsg)));

	free(szMsg);
}

void Chat(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	uint8_t uint8_tTextLen;

	unsigned char szText[256];
	memset(szText, 0, 256);

	bsData.Read(playerId);
	bsData.Read(uint8_tTextLen);
	bsData.Read((char *)szText, uint8_tTextLen);
	szText[uint8_tTextLen] = 0;

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	char *_text = removeMarkupTags((char *)szText);
	AppendChatBox("[CHAT] <span style=\"color: %s\">%s</span>: %s", QString(playerPool[playerId].HEXcolor).toUtf8().data(), playerPool[playerId].szPlayerName, formatColorMessage((const char *)_text));
}

void UpdateScoresPingsIPs(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	int iPlayerScore;
	uint32_t dwPlayerPing;

	for (PLAYERID i = 0; i < (iBitLength / 8) / 9; i++)
	{
		bsData.Read(playerId);
		bsData.Read(iPlayerScore);
		bsData.Read(dwPlayerPing);

		if (playerId < 0 || playerId >= MAX_PLAYERS)
			continue;

		playerPool[playerId].iScore = iPlayerScore;
		playerPool[playerId].dwPing = dwPlayerPing;
	}
}

void SetCheckpoint(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(CurrentCheckpoint.fPosition[0]);
	bsData.Read(CurrentCheckpoint.fPosition[1]);
	bsData.Read(CurrentCheckpoint.fPosition[2]);
	bsData.Read(CurrentCheckpoint.fSize);

	CurrentCheckpoint.bActive = true;

	AppendLogF("[CP] Checkpoint set to %.2f %.2f %.2f position. (size: %.2f)", CurrentCheckpoint.fPosition[0], CurrentCheckpoint.fPosition[1], CurrentCheckpoint.fPosition[2], CurrentCheckpoint.fSize);
	_logs->Log(LogLevel::DEBUG, "[CP] Checkpoint set to %.2f %.2f %.2f position. (size: %.2f)", CurrentCheckpoint.fPosition[0], CurrentCheckpoint.fPosition[1], CurrentCheckpoint.fPosition[2], CurrentCheckpoint.fSize);
}

void DisableCheckpoint(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	CurrentCheckpoint.bActive = false;

	AppendLogF("[CP] Current checkpoint disabled.");
	_logs->Log(LogLevel::DEBUG, "[CP] Current checkpoint disabled.");
}

void SetRaceCheckpoint(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	VECTOR pos, next;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.IgnoreBits(8);
	bsData.Read(next.X);
	bsData.Read(next.Y);
	bsData.Read(next.Z);

	bsData.Read(next.X);
	bsData.Read(next.Y);
	bsData.Read(next.Z);

	CurrentCheckpoint.bActive = true;

	AppendLogF("[rCP] Race Checkpoint set to %.2f %.2f %.2f position. Next Race Checkpoint set to %.2f %.2f %.2f position.", pos.X, pos.Y, pos.Z, next.X, next.Y, next.Z);
	_logs->Log(LogLevel::DEBUG, "[rCP] Race Checkpoint set to %.2f %.2f %.2f position. Next Race Checkpoint set to %.2f %.2f %.2f position.", pos.X, pos.Y, pos.Z, next.X, next.Y, next.Z);
}

void DisableRaceCheckpoint(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	CurrentCheckpoint.bActive = false;

	AppendLogF("[rCP] Race Current checkpoint disabled.");
	_logs->Log(LogLevel::DEBUG, "[rCP] Race Current checkpoint disabled.");
}

void Pickup(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int PickupID;
	PickupPool Pickup;

	bsData.Read(PickupID);
	if (PickupID >= MAX_PICKUPS)
		return;

	bsData.Read((char *)&Pickup, sizeof(PickupPool));

	if (!pickupExist[PickupID])
	{
		pickupPool[PickupID] = Pickup;
		pickupExist[PickupID] = true;

		if (settings.uiPickupsLogging != 0)
		{
			AppendLogF("[CREATEPICKUP] ID: %d | Model: %d | Type: %d | X: %.2f | Y: %.2f | Z: %.2f", PickupID, Pickup.iModelID, Pickup.iType, Pickup.fPosX, Pickup.fPosY, Pickup.fPosZ);
			_logs->Log(LogLevel::DEBUG, "[CREATEPICKUP] ID: %d | Model: %d | Type: %d | X: %.2f | Y: %.2f | Z: %.2f", PickupID, Pickup.iModelID, Pickup.iType, Pickup.fPosX, Pickup.fPosY, Pickup.fPosZ);
		}
	}
}

void DestroyPickup(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int PickupID;
	PickupPool tmpPickup;

	bsData.Read(PickupID);
	if (PickupID >= MAX_PICKUPS)
		return;

	if (pickupExist[PickupID])
	{
		pickupPool[PickupID] = tmpPickup;
		pickupExist[PickupID] = false;

		if (settings.uiPickupsLogging != 0)
		{
			AppendLogF("[DESTROYPICKUP] %d", PickupID);
			_logs->Log(LogLevel::DEBUG, "[DESTROYPICKUP] %d", PickupID);
		}
	}
}

void RequestClass(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint8_t BYTERequestOutcome = 0;

	bsData.Read(BYTERequestOutcome);

	if (BYTERequestOutcome)
	{
		bsData.Read((char *)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

		playerPool[g_myPlayerID].onfootData.BYTESkin = SpawnInfo.iSkin;
	}
}

void RequestSpawn(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteRequestOutcome = 0;
	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteRequestOutcome);

	AppendLogF("[RequestSpawn] %d", byteRequestOutcome);
	_logs->Log(LogLevel::DEBUG, "[RequestSpawn] %d", byteRequestOutcome);
}

void SetTimeEx(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	uint8_t byteHour;
	uint8_t byteMinute;
	bsData.Read(byteHour);
	bsData.Read(byteMinute);

	AppendLogF("[SetTimeEx] %d hour %d minutes", byteHour, byteMinute);
	_logs->Log(LogLevel::DEBUG, "[SetTimeEx] %d hour %d minutes", byteHour, byteMinute);
}

void ToggleClock(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	uint8_t byteClock;
	bsData.Read(byteClock);

	AppendLogF("[ToggleClock] %s", (byteClock ? "TRUE" : "FALSE"));
	_logs->Log(LogLevel::DEBUG, "[ToggleClock] %s", (byteClock ? "TRUE" : "FALSE"));
}

void VehicleDestroy(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	VEHICLEID VehicleID;

	bsData.Read(VehicleID);

	AppendLogF("[VehicleDestroy:%d] DELETED.", VehicleID);
	_logs->Log(LogLevel::DEBUG, "[VehicleDestroy:%d] DELETED.", VehicleID);
}

void ScrInitMenu(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	memset(&GTAMenu, 0, sizeof(struct stGTAMenu));

	uint8_t BYTEMenuID;
	bool HasTwoColumns;
	char cText[MAX_MENU_LINE];
	char *mTitle;
	float fX;
	float fY;
	float fCol1W;
	float fCol2W = 0.0;

	bsData.Read(BYTEMenuID);
	AppendLogF("Processing Menu ID %s", BYTEMenuID);
	_logs->Log(LogLevel::DEBUG, "Processing Menu ID %s", BYTEMenuID);
	bsData.Read(HasTwoColumns);
	bsData.Read(GTAMenu.szTitleL);
	bsData.Read(GTAMenu.szTitle, GTAMenu.szTitleL);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fCol1W);
	if (HasTwoColumns)
		bsData.Read(fCol2W);
	bsData.Read(GTAMenu.szEnabled);
	for (uint8_t idx = 0; idx < MAX_MENU_ITEMS; idx++)
		bsData.Read(GTAMenu.bRowState[0][idx]);

	bsData.Read(GTAMenu.szColumnHeader[0], MAX_MENU_LINE);
	bsData.Read(GTAMenu.szColumnCount[0]);
	char *__RowL;
	for (uint8_t idx = 0; idx < GTAMenu.szColumnCount[0]; idx++)
	{
		strcpy(__RowL, GTAMenu.szRowLine[0][idx]);
		bsData.Read(__RowL, MAX_MENU_LINE);
	}

	if (HasTwoColumns)
	{
		bsData.Read(GTAMenu.szColumnHeader[1], MAX_MENU_LINE);
		bsData.Read(GTAMenu.szColumnCount[1]);
		for (uint8_t idx = 0; idx < GTAMenu.szColumnCount[1]; idx++)
		{
			strcpy(__RowL, GTAMenu.szRowLine[1][idx]);
			bsData.Read(__RowL, MAX_MENU_LINE);
		}
	}

	if (!SAMPMenu::instance().isMenuVisible())
	{
		char *Col1RowPointers[MAX_MENU_ITEMS];
		for (int i = 0; i < MAX_MENU_ITEMS; i++)
			Col1RowPointers[i] = GTAMenu.szRowLine[0][i];
		char *Col2RowPointers[MAX_MENU_ITEMS];
		for (int i = 0; i < MAX_MENU_ITEMS; i++)
			Col2RowPointers[i] = GTAMenu.szRowLine[1][i];
		SAMPMenu menuBox(GTAMenu.szTitle, (HasTwoColumns ? 2 : 1), Char2DArrayToQStringList(Col1RowPointers, 1), GTAMenu.bRowState[0], GTAMenu.szColumnHeader[0], (HasTwoColumns ? Char2DArrayToQStringList(Col2RowPointers, 2) : QStringList()), GTAMenu.bRowState[1], GTAMenu.szColumnHeader[1], getMainWidget());
		QObject::connect(&menuBox, &SAMPMenu::menuResponse, [&](int rowID, bool response)
						 {
			RakNet::BitStream RbsSend;
			if(response) {
				RbsSend.Write(rowID);
				if(!pRakClient->RPC(&RPC_MenuSelect, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
				{
					AppendLogF("ScrInitMenu -> Something went wrong!");
					_logs->Log(LogLevel::ERROR, "ScrInitMenu -> Something went wrong!");
				}
			}
			else {
				if (!pRakClient->RPC(&RPC_MenuQuit, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
				{
					AppendLogF("ScrInitMenu -> Something went wrong!");
					_logs->Log(LogLevel::ERROR, "ScrInitMenu -> Something went wrong!");
				}
			} });
		menuBox.show();
	}
}

void ScrDialogBox(RPCParameters *rpcParams)
{
	if (!iGameInited)
		return;

	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(sampDialog.wDialogID);
	bsData.Read(sampDialog.bDialogStyle);

	bsData.Read(sampDialog.bTitleLength);
	bsData.Read(sampDialog.szTitle, sampDialog.bTitleLength);
	sampDialog.szTitle[sampDialog.bTitleLength] = 0;

	bsData.Read(sampDialog.bButton1Len);
	bsData.Read(sampDialog.szButton1, sampDialog.bButton1Len);
	sampDialog.szButton1[sampDialog.bButton1Len] = 0;

	bsData.Read(sampDialog.bButton2Len);
	bsData.Read(sampDialog.szButton2, sampDialog.bButton2Len);
	sampDialog.szButton2[sampDialog.bButton2Len] = 0;

	stringCompressor->DecodeString(sampDialog.szInfo, 256, &bsData);

	RakNet::BitStream RbsSend;
	switch (sampDialog.bDialogStyle)
	{
	case DIALOG_STYLE_MSGBOX:
	{
		if (!SAMPDialog::instance().isDialogVisible())
		{
			SAMPDialog msgBoxDialog(SAMPDialog::MSGBOX, QString::fromUtf8(sampDialog.szTitle), QString::fromUtf8(sampDialog.szInfo), QString::fromUtf8(sampDialog.szButton1), QString::fromUtf8(sampDialog.szButton2), getMainWidget());
			QObject::connect(&msgBoxDialog, &SAMPDialog::dialogResponse, [&](int buttonId, int listBoxItem, const QString &text, bool response)
							 {
	                RbsSend.Write(sampDialog.wDialogID);
	                RbsSend.Write(buttonId);
	                RbsSend.Write(listBoxItem);
	                RbsSend.Write(text.size());
	                RbsSend.Write(text.toStdString().c_str(), text.size());
	                if(!pRakClient->RPC(&RPC_DialogResponse, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
					{
						AppendLogF("ScrDialogBox -> Something went wrong!");
						_logs->Log(LogLevel::ERROR, "ScrDialogBox -> Something went wrong!");
					} });
			msgBoxDialog.exec();
		}
		break;
	}
	case DIALOG_STYLE_INPUT:
	{
		if (!SAMPDialog::instance().isDialogVisible())
		{
			SAMPDialog msgBoxDialog(SAMPDialog::INPUT, QString::fromUtf8(sampDialog.szTitle), QString::fromUtf8(sampDialog.szInfo), QString::fromUtf8(sampDialog.szButton1), QString::fromUtf8(sampDialog.szButton2), getMainWidget());
			QObject::connect(&msgBoxDialog, &SAMPDialog::dialogResponse, [&](int buttonId, int listBoxItem, const QString &text, bool response)
							 {
	                RbsSend.Write(sampDialog.wDialogID);
	                RbsSend.Write(buttonId);
	                RbsSend.Write(listBoxItem);
	                RbsSend.Write(text.size());
	                RbsSend.Write(text.toStdString().c_str(), text.size());
	                if(!pRakClient->RPC(&RPC_DialogResponse, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
					{
						AppendLogF("ScrDialogBox -> Something went wrong!");
						_logs->Log(LogLevel::ERROR, "ScrDialogBox -> Something went wrong!");
					} });
			msgBoxDialog.exec();
		}
		break;
	}
	case DIALOG_STYLE_LIST:
	{
		if (!SAMPDialog::instance().isDialogVisible())
		{
			SAMPDialog msgBoxDialog(SAMPDialog::LIST, QString::fromUtf8(sampDialog.szTitle), QString::fromUtf8(sampDialog.szInfo), QString::fromUtf8(sampDialog.szButton1), QString::fromUtf8(sampDialog.szButton2), getMainWidget());
			QObject::connect(&msgBoxDialog, &SAMPDialog::dialogResponse, [&](int buttonId, int listBoxItem, const QString &text, bool response)
							 {
	                RbsSend.Write(sampDialog.wDialogID);
	                RbsSend.Write(buttonId);
	                RbsSend.Write(listBoxItem);
	                RbsSend.Write(text.size());
	                RbsSend.Write(text.toStdString().c_str(), text.size());
	                if(!pRakClient->RPC(&RPC_DialogResponse, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
					{
						AppendLogF("ScrDialogBox -> Something went wrong!");
						_logs->Log(LogLevel::ERROR, "ScrDialogBox -> Something went wrong!");
					} });
			msgBoxDialog.exec();
		}
		break;
	}
	case DIALOG_STYLE_PASSWORD:
	{
		if (!SAMPDialog::instance().isDialogVisible())
		{
			SAMPDialog msgBoxDialog(SAMPDialog::PASSWORD, QString::fromUtf8(sampDialog.szTitle), QString::fromUtf8(sampDialog.szInfo), QString::fromUtf8(sampDialog.szButton1), QString::fromUtf8(sampDialog.szButton2), getMainWidget());
			QObject::connect(&msgBoxDialog, &SAMPDialog::dialogResponse, [&](int buttonId, int listBoxItem, const QString &text, bool response)
							 {
	                RbsSend.Write(sampDialog.wDialogID);
	                RbsSend.Write(buttonId);
	                RbsSend.Write(listBoxItem);
	                RbsSend.Write(text.size());
	                RbsSend.Write(text.toStdString().c_str(), text.size());
	                if(!pRakClient->RPC(&RPC_DialogResponse, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
					{
						AppendLogF("ScrDialogBox -> Something went wrong!");
						_logs->Log(LogLevel::ERROR, "ScrDialogBox -> Something went wrong!");
					} });
			msgBoxDialog.exec();
		}
		break;
	}
	case DIALOG_STYLE_TABLIST:
	{
		if (!SAMPDialog::instance().isDialogVisible())
		{
			SAMPDialog msgBoxDialog(SAMPDialog::TABLIST, QString::fromUtf8(sampDialog.szTitle), QString::fromUtf8(sampDialog.szInfo), QString::fromUtf8(sampDialog.szButton1), QString::fromUtf8(sampDialog.szButton2), getMainWidget());
			QObject::connect(&msgBoxDialog, &SAMPDialog::dialogResponse, [&](int buttonId, int listBoxItem, const QString &text, bool response)
							 {
	                RbsSend.Write(sampDialog.wDialogID);
	                RbsSend.Write(buttonId);
	                RbsSend.Write(listBoxItem);
	                RbsSend.Write(text.size());
	                RbsSend.Write(text.toStdString().c_str(), text.size());
	                if(!pRakClient->RPC(&RPC_DialogResponse, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
					{
						AppendLogF("ScrDialogBox -> Something went wrong!");
						_logs->Log(LogLevel::ERROR, "ScrDialogBox -> Something went wrong!");
					} });
			msgBoxDialog.exec();
		}
		break;
	}
	case DIALOG_STYLE_TABLIST_HEADERS:
	{
		if (!SAMPDialog::instance().isDialogVisible())
		{
			SAMPDialog msgBoxDialog(SAMPDialog::HEADERLIST, QString::fromUtf8(sampDialog.szTitle), QString::fromUtf8(sampDialog.szInfo), QString::fromUtf8(sampDialog.szButton1), QString::fromUtf8(sampDialog.szButton2), getMainWidget());
			QObject::connect(&msgBoxDialog, &SAMPDialog::dialogResponse, [&](int buttonId, int listBoxItem, const QString &text, bool response)
							 {
	                RbsSend.Write(sampDialog.wDialogID);
	                RbsSend.Write(buttonId);
	                RbsSend.Write(listBoxItem);
	                RbsSend.Write(text.size());
	                RbsSend.Write(text.toStdString().c_str(), text.size());
	                if(!pRakClient->RPC(&RPC_DialogResponse, &RbsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL))
					{
						AppendLogF("ScrDialogBox -> Something went wrong!");
						_logs->Log(LogLevel::ERROR, "ScrDialogBox -> Something went wrong!");
					} });
			msgBoxDialog.exec();
		}
		break;
	}
	default:
	{
		if (SAMPDialog::instance().isDialogVisible())
			SAMPDialog::instance().closeDialog();
		break;
	}
	}
}

void ScrSetPlayerColor(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	uint8_t playerId;
	uint32_t dwColor;

	bsData.Read(playerId);
	bsData.Read(dwColor);

	uint8_t red = (dwColor >> 24) & 0xFF;
	uint8_t green = (dwColor >> 16) & 0xFF;
	uint8_t blue = (dwColor >> 8) & 0xFF;
	QString HexColor = QString("#%1%2%3").arg(red, 2, 16, QChar('0')).arg(green, 2, 16, QChar('0')).arg(blue, 2, 16, QChar('0')).toUpper();

	playerPool[playerId].RGBAcolor = dwColor;
	playerPool[playerId].HEXcolor = HexColor;
}

void EnterVehicle(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	VEHICLEID VehicleID = 0;
	uint8_t bytePassenger = 0;
	bool bPassenger = false;

	bsData.Read(playerId);
	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	playerPool[playerId].bAreWeInAVehicle = true;
	playerPool[playerId].vehicleData.VehicleId = VehicleID;
	playerPool[playerId].vehicleData.Seat = (bytePassenger ? VEHICLE_SEAT_PASSENGER : VEHICLE_SEAT_DRIVER);

	if (bytePassenger)
		bPassenger = true;

	if (playerId == g_myPlayerID)
	{
		if (bPassenger)
			SetState(STATE_PASSENGER);
		else
			SetState(STATE_DRIVER);
	}

	AppendLogF("[EnterVehicle]: playerid=%d, vehicleid=%d, ispassenger=%s", playerId, VehicleID, (bytePassenger ? "Yes" : "No"));
	_logs->Log(LogLevel::DEBUG, "[EnterVehicle]: playerid=%d, vehicleid=%d, ispassenger=%s", playerId, VehicleID, (bytePassenger ? "Yes" : "No"));
}

void ExitVehicle(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	VEHICLEID VehicleID = 0;

	bsData.Read(playerId);
	bsData.Read(VehicleID);

	playerPool[playerId].bAreWeInAVehicle = false;
	playerPool[g_myPlayerID].vehicleData.VehicleId = -1;
	playerPool[g_myPlayerID].vehicleData.Seat = -1;

	if (playerId == g_myPlayerID)
		SetState(STATE_ONFOOT);

	AppendLogF("[ExitVehicle]: playerid=%d, vehicleid=%d", playerId, VehicleID);
	_logs->Log(LogLevel::DEBUG, "[ExitVehicle]: playerid=%d, vehicleid=%d", playerId, VehicleID);
}

void ScrPutPlayerInVehicle(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	VEHICLEID vehicleid;
	uint8_t seatid;
	bsData.Read(vehicleid);
	bsData.Read(seatid);

	playerPool[g_myPlayerID].bAreWeInAVehicle = true;
	playerPool[g_myPlayerID].vehicleData.VehicleId = vehicleid;
	playerPool[g_myPlayerID].vehicleData.Seat = seatid;

	if (seatid > 0)
		SetState(STATE_PASSENGER);
	else
		SetState(STATE_DRIVER);
}

void ScrRemovePlayerFromVehicle(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	playerPool[g_myPlayerID].bAreWeInAVehicle = false;
	playerPool[g_myPlayerID].vehicleData.VehicleId = -1;
	playerPool[g_myPlayerID].vehicleData.Seat = -1;

	SetState(STATE_ONFOOT);
}

void ScrSetPlayerName(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID bytePlayerID;
	uint8_t byteNickLen;
	char szNewName[MAX_PLAYER_NAME];
	uint8_t byteSuccess;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(bytePlayerID);
	bsData.Read(byteNickLen);

	if (byteNickLen > MAX_PLAYER_NAME)
		return;

	bsData.Read(szNewName, byteNickLen);
	bsData.Read(byteSuccess);

	szNewName[byteNickLen] = '\0';

	if (byteSuccess == 1)
		strcpy((char *)playerPool[bytePlayerID].szPlayerName, szNewName);
}

void ScrGameText(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	char szMessage[400];
	int iType, iTime, iLength;

	bsData.Read(iType);
	bsData.Read(iTime);
	bsData.Read(iLength);

	if (iLength > 400)
		return; // tsk tsk, kye

	bsData.Read(szMessage, iLength);
	szMessage[iLength] = '\0';

	AppendChatBox("[GAMETEXT] %s", szMessage);
	AppendLogF("[GAMETEXT] %s", szMessage);
	_logs->Log(LogLevel::DEBUG, "[GAMETEXT] %s", szMessage);
}

void ScrPlayAudioStream(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	unsigned char bURLLen;
	char szURL[256];

	bsData.Read(bURLLen);
	bsData.Read(szURL, bURLLen);
	szURL[bURLLen] = 0;

	AppendLogF("[AUDIO_STREAM] %s", szURL);
	_logs->Log(LogLevel::DEBUG, "[AUDIO_STREAM] %s", szURL);

	AudioStream audioStream;
	audioStream.PlayAudioStream(szURL, 0.5);
}

void ScrStopAudioStream(RPCParameters *rpcParams)
{
	AppendLogF("[AUDIO_STREAM] Stopping...");
	_logs->Log(LogLevel::DEBUG, "[AUDIO_STREAM] Stopping...");

	AudioStream audioStream;
	audioStream.StopAudioStream();
}

void ScrSetDrunkLevel(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(iDrunkLevel);

	AppendLogF("[DRUNK_LEVEL] %d", iDrunkLevel);
	_logs->Log(LogLevel::DEBUG, "[DRUNK_LEVEL] %d", iDrunkLevel);
}

void ScrHaveSomeMoney(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int iGivenMoney;
	bsData.Read(iGivenMoney);

	AppendLogF("[MONEY_CHANGE] %d += %d", iMoney, iGivenMoney);
	_logs->Log(LogLevel::DEBUG, "[MONEY_CHANGE] %d += %d", iMoney, iGivenMoney);

	iMoney += iGivenMoney;
}

void ScrResetMoney(RPCParameters *rpcParams)
{
	iMoney = 0;

	AppendLogF("[MONEY_RESET] %d", iMoney);
	_logs->Log(LogLevel::DEBUG, "[MONEY_RESET] %d", iMoney);
}

void ScrSetPlayerPos(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(playerPool[g_myPlayerID].onfootData.fCurrentPosition[0]);
	bsData.Read(playerPool[g_myPlayerID].onfootData.fCurrentPosition[1]);
	bsData.Read(playerPool[g_myPlayerID].onfootData.fCurrentPosition[2]);
}

void ScrSetInterior(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(playerPool[g_myPlayerID].onfootData.BYTEInteriorId);
}

void ScrSetPlayerFacingAngle(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(playerPool[g_myPlayerID].onfootData.fCurrentAngle);
}

void ScrSetSpawnInfo(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYER_SPAWN_INFO SpawnInfo;

	bsData.Read((char *)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	vect3_copy(playerPool[g_myPlayerID].onfootData.fCurrentPosition, SpawnInfo.vecPos);
}

void ScrSetPlayerHealth(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(playerPool[g_myPlayerID].onfootData.fHealth);
}

void ScrSetPlayerArmour(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bsData.Read(playerPool[g_myPlayerID].onfootData.fArmour);
}

void ScrSetPlayerSkin(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int iPlayerID;
	unsigned int uiSkin;

	bsData.Read(iPlayerID);
	bsData.Read(uiSkin);

	if (iPlayerID < 0 || iPlayerID >= MAX_PLAYERS)
		return;

	playerPool[iPlayerID].onfootData.BYTESkin = uiSkin;
}

void ScrCreateObject(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	unsigned short ObjectID;
	bsData.Read(ObjectID);

	unsigned long ModelID;
	bsData.Read(ModelID);

	float vecPos[3];
	bsData.Read(vecPos[0]);
	bsData.Read(vecPos[1]);
	bsData.Read(vecPos[2]);

	float vecRot[3];
	bsData.Read(vecRot[0]);
	bsData.Read(vecRot[1]);
	bsData.Read(vecRot[2]);

	float fDrawDistance;
	bsData.Read(fDrawDistance);

	if (settings.uiObjectsLogging != 0)
	{
		AppendLogF("[OBJECT] %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.2f", ObjectID, ModelID, vecPos[0], vecPos[1], vecPos[2], vecRot[0], vecRot[1], vecRot[2], fDrawDistance);
		_logs->Log(LogLevel::DEBUG, "[OBJECT] %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.2f", ObjectID, ModelID, vecPos[0], vecPos[1], vecPos[2], vecRot[0], vecRot[1], vecRot[2], fDrawDistance);
	}
}

void ScrCreate3DTextLabel(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t LabelID;
	char Text[256];
	uint32_t dwColor;
	float vecPos[3];
	float DrawDistance;
	uint8_t UseLOS;
	uint16_t PlayerID;
	uint16_t VehicleID;

	bsData.Read(LabelID);
	bsData.Read(dwColor);
	bsData.Read(vecPos[0]);
	bsData.Read(vecPos[1]);
	bsData.Read(vecPos[2]);
	bsData.Read(DrawDistance);
	bsData.Read(UseLOS);
	bsData.Read(PlayerID);
	bsData.Read(VehicleID);

	stringCompressor->DecodeString(Text, 256, &bsData);

	AppendLogF("[TEXTLABEL:%d] %s (%X, %.3f, %.3f, %.3f, %.2f, %i, %d, %d)", LabelID, Text, dwColor, vecPos[0], vecPos[1], vecPos[2], DrawDistance, UseLOS, PlayerID, VehicleID);
	_logs->Log(LogLevel::DEBUG, "[TEXTLABEL:%d] %s (%X, %.3f, %.3f, %.3f, %.2f, %i, %d, %d)", LabelID, Text, dwColor, vecPos[0], vecPos[1], vecPos[2], DrawDistance, UseLOS, PlayerID, VehicleID);
}

void ScrUpdate3DTextLabel(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t LabelID;
	bsData.Read(LabelID);

	AppendLogF("[TEXTLABEL:%d] DELETED.", LabelID);
	_logs->Log(LogLevel::DEBUG, "[TEXTLABEL:%d] DELETED.", LabelID);
}

void ScrShowTextDraw(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t wTextID;
	TEXT_DRAW_TRANSMIT TextDrawTransmit;

	char cText[1024];
	unsigned short cTextLen = 0;

	bsData.Read(wTextID);
	bsData.Read((char *)&TextDrawTransmit, sizeof(TEXT_DRAW_TRANSMIT));
	bsData.Read(cTextLen);
	bsData.Read(cText, cTextLen);
	cText[cTextLen] = '\0';

	if (settings.uiTextDrawsLogging != 0)
		SaveTextDrawData(wTextID, &TextDrawTransmit, cText);

	if (TextDrawTransmit.BYTESelectable)
	{
		AppendLogF("[SELECTABLE-TEXTDRAW] ID: %d, Text: %s.", wTextID, cText);
		_logs->Log(LogLevel::DEBUG, "[SELECTABLE-TEXTDRAW] ID: %d, Text: %s.", wTextID, cText);
	}
}

void ScrHideTextDraw(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t wTextID;
	bsData.Read(wTextID);

	if (settings.uiTextDrawsLogging != 0)
	{
		AppendLogF("[TEXTDRAW:HIDE] ID: %d.", wTextID);
		_logs->Log(LogLevel::DEBUG, "[TEXTDRAW:HIDE] ID: %d.", wTextID);
	}
}

void ScrEditTextDraw(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint16_t wTextID;
	char cText[1024];
	unsigned short cTextLen = 0;

	bsData.Read(wTextID);
	bsData.Read(cTextLen);
	bsData.Read(cText, cTextLen);
	cText[cTextLen] = '\0';

	if (settings.uiTextDrawsLogging != 0)
	{
		AppendLogF("[TEXTDRAW:EDIT] ID: %d, Text: %s.", wTextID, cText);
		_logs->Log(LogLevel::DEBUG, "[TEXTDRAW:EDIT] ID: %d, Text: %s.", wTextID, cText);
	}
}

void ScrTogglePlayerSpectating(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	bool bToggle;

	bsData.Read(bToggle);

	if (bIsSpectating && !bToggle && !IsSpawned())
	{
		sampRequestSpawn();
		sampSpawn();
	}

	bIsSpectating = bToggle;
}

void ScrTogglePlayerControllable(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	uint8_t byteControllable;
	bsData.Read(byteControllable);

	playerPool[g_myPlayerID].bIsControllable = byteControllable;
}

void ScrResetPlayerWeapons(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	for (int slotID = 0; slotID < MAX_WEAPON_SLOTS; slotID++)
	{
		playerPool[g_myPlayerID].weaponData[slotID].weaponId = INVALID_WEAPON_ID;
		playerPool[g_myPlayerID].weaponData[slotID].weaponAmmo = 0;
	}
}

void ScrGivePlayerWeapon(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int iWeaponID;
	int iAmmo;
	int slotID = -1;
	bsData.Read(iWeaponID);
	bsData.Read(iAmmo);

	if (iWeaponID < 0 || iWeaponID > 46)
		return;

	slotID = weaponSlots[iWeaponID];

	if (slotID < 0 || slotID >= MAX_WEAPON_SLOTS)
		return;

	playerPool[g_myPlayerID].weaponData[slotID].weaponId = iWeaponID;
	playerPool[g_myPlayerID].weaponData[slotID].weaponAmmo = iAmmo;
}

void SetArmedWeapon(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int iWeaponID;
	bsData.Read(iWeaponID);

	if (iWeaponID < 0 || iWeaponID > 46)
		return;

	playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon = iWeaponID;
}

void ScrSetWeaponAmmo(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint8_t byteWeapon;
	uint16_t weapAmmo;

	bsData.Read(byteWeapon);
	bsData.Read(weapAmmo);

	playerPool[g_myPlayerID].weaponData[weaponSlots[byteWeapon]].weaponAmmo = weapAmmo;
}

void ScrDeathMessage(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteKiller, byteKillee, byteWeapon;
	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);
	char *szKillerName = NULL;
	char *szKilleeName = NULL;

	bsData.Read(byteKiller);
	bsData.Read(byteKillee);
	bsData.Read(byteWeapon);

	if (byteKillee == INVALID_PLAYER_ID)
		return;

	if (byteKiller == INVALID_PLAYER_ID)
		szKillerName = NULL;
	else
	{
		if (g_myPlayerID == byteKiller)
			szKillerName = playerPool[g_myPlayerID].szPlayerName;
		else
		{
			if (playerPool[byteKiller].bIsConnected == true)
				szKillerName = playerPool[byteKiller].szPlayerName;
			else
				szKillerName = NULL;
		}
	}

	if (g_myPlayerID == byteKillee)
		szKilleeName = playerPool[g_myPlayerID].szPlayerName;
	else
	{
		if (playerPool[byteKillee].bIsConnected == true)
			szKilleeName = playerPool[byteKillee].szPlayerName;
		else
			szKilleeName = NULL;
	}

	AppendChatBox("[DeathMessage] killer=%s killee=%s weapon=%d", szKillerName == NULL ? "??" : szKillerName, szKilleeName == NULL ? "??" : szKillerName, byteWeapon);
	AppendLogF("[DeathMessage] killer=%s killee=%s weapon=%d", szKillerName == NULL ? "??" : szKillerName, szKilleeName == NULL ? "??" : szKillerName, byteWeapon);
	_logs->Log(LogLevel::DEBUG, "[DeathMessage] killer=%s killee=%s weapon=%d", szKillerName == NULL ? "??" : szKillerName, szKilleeName == NULL ? "??" : szKillerName, byteWeapon);
}

void ScrSetPlayerWantedLevel(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint8_t byteLevel;
	bsData.Read(byteLevel);
	playerPool[g_myPlayerID].iWantedLevel = byteLevel;
}

void ScrForceClassSelection(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	SetState(STATE_NOT_SPAWNED);
	bInClassSelection = true;

	AppendChatBox("[ForceClassSelection] The server is forcing class selection..");
	AppendLogF("[ForceClassSelection] The server is forcing class selection..");
	_logs->Log(LogLevel::DEBUG, "[ForceClassSelection] The server is forcing class selection..");
}

void ScrSetPlayerFightingStyle(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId = 0;
	bsData.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	uint8_t fightingStyle;
	bsData.Read(fightingStyle);

	playerPool[playerId].onfootData.FightingStyle = fightingStyle;

	AppendLogF("[SetPlayerFightingStyle#%d] CHANGED to %d", playerId, fightingStyle);
	_logs->Log(LogLevel::DEBUG, "[SetPlayerFightingStyle#%d] CHANGED to %d", playerId, fightingStyle);
}

void ScrSetPlayerSkillLevel(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId = 0;
	bsData.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	uint32_t dSkillID;
	bsData.Read(dSkillID);

	if (dSkillID < 0 || dSkillID > MAX_SKILLS)
		return;

	uint16_t wSkillLevel;
	bsData.Read(wSkillLevel);

	playerPool[playerId].onfootData.SkillLevel[dSkillID] = wSkillLevel;

	AppendLogF("[SetPlayerSkillLevel#%d] CHANGED to skillID=%d skillLevel=%d", playerId, dSkillID, wSkillLevel);
	_logs->Log(LogLevel::DEBUG, "[SetPlayerSkillLevel#%d] CHANGED to skillID=%d skillLevel=%d", playerId, dSkillID, wSkillLevel);
}

void ScrSetPlayerTeam(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId = 0;
	bsData.Read(playerId);

	if (playerId < 0 || playerId >= MAX_PLAYERS)
		return;

	uint8_t Team;
	bsData.Read(Team);

	playerPool[playerId].onfootData.BYTETeam = Team;

	AppendLogF("[SetPlayerTeam#%d] CHANGED to %d", playerId, Team);
	_logs->Log(LogLevel::DEBUG, "[SetPlayerTeam#%d] CHANGED to %d", playerId, Team);
}

void SetWeather(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint8_t WeatherID;
	bsData.Read(WeatherID);

	g_srvWeather = WeatherID;

	AppendLogF("[SetWeather] CHANGED to %d", WeatherID);
	_logs->Log(LogLevel::DEBUG, "[SetWeather] CHANGED to %d", WeatherID);
}

void WorldTime(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	uint8_t bHours;
	bsData.Read(bHours);

	g_srvWorldTime = bHours;

	AppendLogF("[SetWorldTime] CHANGED to %d", bHours);
	_logs->Log(LogLevel::DEBUG, "[SetWorldTime] CHANGED to %d", bHours);
}

void ScrSetVehiclePos(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int vehicleId;
	float vehiclePosX, vehiclePosY, vehiclePosZ;
	bsData.Read(vehicleId);
	if (vehicleId < 0 || vehicleId >= MAX_VEHICLES)
		return;

	bsData.Read(vehiclePosX);
	bsData.Read(vehiclePosY);
	bsData.Read(vehiclePosZ);

	vehiclePool[vehicleId].fPos[0] = vehiclePosX;
	vehiclePool[vehicleId].fPos[1] = vehiclePosY;
	vehiclePool[vehicleId].fPos[2] = vehiclePosZ;

	AppendLogF("[SetVehiclePos#%d] CHANGED to X=%0.2f Y=%0.2f Z=%0.2f", vehicleId, vehiclePosX, vehiclePosY, vehiclePosZ);
	_logs->Log(LogLevel::DEBUG, "[SetVehiclePos#%d] CHANGED to X=%0.2f Y=%0.2f Z=%0.2ff", vehicleId, vehiclePosX, vehiclePosY, vehiclePosZ);
}

void ScrSetVehicleHealth(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int vehicleId;
	float vehicleHealth;
	bsData.Read(vehicleId);
	if (vehicleId < 0 || vehicleId >= MAX_VEHICLES)
		return;

	bsData.Read(vehicleHealth);

	vehiclePool[vehicleId].fHealth = vehicleHealth;

	AppendLogF("[SetVehicleHealth#%d] CHANGED to %0.2f", vehicleId, vehicleHealth);
	_logs->Log(LogLevel::DEBUG, "[SetVehicleHealth#%d] CHANGED to %0.2f", vehicleId, vehicleHealth);
}

void ScrLinkVehicleToInterior(RPCParameters *rpcParams)
{
	char *Data = reinterpret_cast<char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data, (iBitLength / 8) + 1, false);

	int vehicleId, vehicleInterior;
	bsData.Read(vehicleId);
	if (vehicleId < 0 || vehicleId >= MAX_VEHICLES)
		return;

	bsData.Read(vehicleInterior);

	vehiclePool[vehicleId].BYTEInterior = vehicleInterior;

	AppendLogF("[LinkVehicleToInterior#%d] CHANGED to %d", vehicleId, vehicleInterior);
	_logs->Log(LogLevel::DEBUG, "[LinkVehicleToInterior#%d] CHANGED to %d", vehicleId, vehicleInterior);
}

void GameModeRestart(RPCParameters *rpcParams)
{
	AppendLogF("[GameModeRestart] The server is restarting..");
	_logs->Log(LogLevel::DEBUG, "[GameModeRestart] The server is restarting..");
}

void RegisterRPCs(RakClientInterface *pRakClient)
{
	if (pRakClient == ::pRakClient)
	{
		// Core RPCs
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ServerJoin);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ServerQuit);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_InitGame, InitGame);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, WorldPlayerAdd);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, WorldPlayerDeath);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, WorldPlayerRemove);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, WorldVehicleAdd);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, WorldVehicleRemove);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ConnectionRejected);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, __ClientMessage);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_Chat, Chat);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, UpdateScoresPingsIPs);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, SetCheckpoint);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, DisableCheckpoint);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, SetRaceCheckpoint);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, DisableRaceCheckpoint);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_Pickup, Pickup);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, DestroyPickup);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RequestClass);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RequestSpawn);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, SetTimeEx);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_GameModeRestart, GameModeRestart);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, EnterVehicle);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ExitVehicle);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ScrPutPlayerInVehicle);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ScrRemovePlayerFromVehicle);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ToggleClock, ToggleClock);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_VehicleDestroyed, VehicleDestroy);

		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ShowActor, WorldActorAdd);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_HideActor, WorldActorRemove);

		pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, SetArmedWeapon);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldTime, WorldTime);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_Weather, SetWeather);

		// Scripting RPCs
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrInitMenu, ScrInitMenu);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ScrDialogBox);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ScrSetPlayerName);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ScrSetPlayerColor);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ScrGameText);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ScrPlayAudioStream);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ScrStopAudioStream);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkLevel, ScrSetDrunkLevel);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ScrHaveSomeMoney);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ScrResetMoney);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ScrSetPlayerPos);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ScrSetPlayerFacingAngle);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ScrSetSpawnInfo);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ScrSetPlayerHealth);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ScrSetPlayerArmour);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ScrSetPlayerSkin);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ScrCreateObject);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ScrCreate3DTextLabel);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrUpdate3DTextLabel, ScrUpdate3DTextLabel);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ScrShowTextDraw);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ScrHideTextDraw);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ScrEditTextDraw);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ScrTogglePlayerSpectating);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ScrTogglePlayerControllable);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ScrGivePlayerWeapon);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ScrResetPlayerWeapons);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDeathMessage, ScrDeathMessage);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ScrSetWeaponAmmo);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrForceClassSelection, ScrForceClassSelection);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFightingStyle, ScrSetPlayerFightingStyle);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerTeam, ScrSetPlayerTeam);

		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ScrSetVehiclePos);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ScrSetVehicleHealth);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ScrLinkVehicleToInterior);
		pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkillLevel, ScrSetPlayerSkillLevel);
	}
}

void UnRegisterRPCs(RakClientInterface *pRakClient)
{
	if (pRakClient == ::pRakClient)
	{
		// Core RPCs
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerJoin);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerQuit);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_InitGame);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ConnectionRejected);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClientMessage);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Chat);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetCheckpoint);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableCheckpoint);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Pickup);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DestroyPickup);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetTimeEx);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_GameModeRestart);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ToggleClock);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_VehicleDestroyed);

		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ShowActor);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_HideActor);

		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Weather);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldTime);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetArmedWeapon);

		// Scripting RPCs
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrInitMenu);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDialogBox);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_PlayAudioStream);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_StopAudioStream);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkLevel);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetMoney);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateObject);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDeathMessage);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrForceClassSelection);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFightingStyle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerTeam);

		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle);
		pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkillLevel);
	}
}
