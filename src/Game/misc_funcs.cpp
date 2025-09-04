#include "../../include/Game/misc_funcs.h"

uint32_t dwLastDisconnection = GetTickCount();

int dd = 0;

// false - down | true - up
int bHealthPulseDirection = false;
int bArmourPulseDirection = true;

// following functions
void onFootUpdateAtNormalPos()
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	ofSync.fHealth = playerPool[g_myPlayerID].onfootData.fHealth;
	ofSync.fArmour = playerPool[g_myPlayerID].onfootData.fArmour;
	ofSync.fQuaternion[3] = playerPool[g_myPlayerID].onfootData.fCurrentAngle;
	ofSync.fCurrentPosition[0] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[0];
	ofSync.fCurrentPosition[1] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[1];
	ofSync.fCurrentPosition[2] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[2];

	SendOnFootFullSyncData(&ofSync, 0, -1);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));

	playerPool[g_myPlayerID].aimData.BYTECamMode = 4;
	playerPool[g_myPlayerID].aimData.vecAimf1[0] = 0.1f;
	playerPool[g_myPlayerID].aimData.vecAimf1[1] = 0.1f;
	playerPool[g_myPlayerID].aimData.vecAimf1[2] = 0.1f;
	playerPool[g_myPlayerID].aimData.vecAimPos[0] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[0];
	playerPool[g_myPlayerID].aimData.vecAimPos[1] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[1];
	playerPool[g_myPlayerID].aimData.vecAimPos[2] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[2];

	SendAimSyncData(0, 0, -1);
}

void onFootUpdateFollow(PLAYERID followID)
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	SendOnFootFullSyncData(&ofSync, 0, followID);
	SendAimSyncData(0, 0, followID);
}

uint32_t inCarUpdateTick = GetTickCount();
void inCarUpdateFollow(PLAYERID followID, VEHICLEID withVehicleID)
{
	if (playerPool[g_myPlayerID].bAreWeInAVehicle)
	{
		INCAR_SYNC_DATA icSync;
		memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));
		icSync.VehicleID = withVehicleID;
		SendInCarFullSyncData(&icSync, 0, followID);
		SendAimSyncData(0, 0, followID);
	}

	if (inCarUpdateTick && inCarUpdateTick < (GetTickCount() - 10000))
	{
		if (!playerPool[g_myPlayerID].bAreWeInAVehicle)
		{
			if (playerPool[followID].incarData.VehicleID == (VEHICLEID)-1)
				return;

			SendEnterVehicleNotification((VEHICLEID)iFollowingWithVehicleID, 0);
			playerPool[g_myPlayerID].bAreWeInAVehicle = true;
			inCarUpdateTick = 0;
		}

		inCarUpdateTick = GetTickCount();
	}
	else
	{
		if (!playerPool[g_myPlayerID].bAreWeInAVehicle)
			onFootUpdateFollow(followID);
	}
}

void spectatorUpdate()
{
	SPECTATOR_SYNC_DATA spSync;
	memset(&spSync, 0, sizeof(SPECTATOR_SYNC_DATA));

	spSync.vecPos[0] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[0];
	spSync.vecPos[1] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[1];
	spSync.vecPos[2] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[2];

	SendSpectatorData(&spSync);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));

	playerPool[g_myPlayerID].aimData.BYTECamMode = 4;
	playerPool[g_myPlayerID].aimData.vecAimf1[0] = 0.1f;
	playerPool[g_myPlayerID].aimData.vecAimf1[1] = 0.1f;
	playerPool[g_myPlayerID].aimData.vecAimf1[2] = 0.1f;
	playerPool[g_myPlayerID].aimData.vecAimPos[0] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[0];
	playerPool[g_myPlayerID].aimData.vecAimPos[1] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[1];
	playerPool[g_myPlayerID].aimData.vecAimPos[2] = playerPool[g_myPlayerID].onfootData.fCurrentPosition[2];

	SendAimSyncData(0, 0, -1);
}

bool __sampConnect(char *szHostname, int iPort, RakClientInterface *pRakClient)
{
	if (GetGameState() != GAMESTATE_WAIT_CONNECT)
		return false;
	time_t curr_time = time(NULL);
	struct tm *localTime = localtime(&curr_time);
	WorkerClass worker(Globals::instance().getCentralWidget());
	worker.moveToThread(QApplication::instance()->thread());
	AppendChatBox("[<b>%02d:%02d:%02d</b>] Connecting to %s:%d..", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, szHostname, iPort);
	AppendLogF("Connecting to %s:%d..", szHostname, iPort);
	_logs->Log(LogLevel::INFO, "Connecting to %s:%d..", szHostname, iPort);
	emit worker.setStateMessage("Trying to connect to server...");
	dwLastConnectAttempt = GetTickCount();
	SetGameState(GAMESTATE_CONNECTING);

	QSharedPointer<QPushButton> _connectBtn = Globals::instance().getConnectButton();
	if (_connectBtn.isNull())
	{
		qWarning() << "_connectBtn is not a valid pointer";
		return false;
	}
	QPushButton *connectBtn = _connectBtn.get();
	if (connectBtn == nullptr || connectBtn->parent() == nullptr)
	{
		qWarning() << "QPushButton instance is not valid.";
		return false;
	}
	connectBtn->setText("Abort");

	return pRakClient->Connect(szHostname, iPort, 0, 0, 5);
}

int sampConnect(char *szHostname, int iPort, char *szNickname, char *szPassword, RakClientInterface *pRakClient)
{
	if (GetGameState() == GAMESTATE_CONNECTED || pRakClient->IsConnected())
		return 0;
	strcpy(playerPool[g_myPlayerID].szPlayerName, szNickname);
	if (pRakClient == NULL)
		return 0;

	pRakClient->SetPassword(szPassword);

	dwLastConnectAttempt = GetTickCount();
	SetGameState(GAMESTATE_WAIT_CONNECT);

	return (int)__sampConnect(szHostname, iPort, pRakClient);
}

void sampDisconnect(int iTimeout)
{
	if (pRakClient == NULL)
		return;

	SetGameState(GAMESTATE_DISCONNECTED);
	ToggleChatBox();
	ToggleSendButton();
	TogglePlaybackButton();
	AppendChatBox("Disconnected.");
	AppendLogF("Disconnected.");
	_logs->Log(LogLevel::INFO, "Disconnected.");

	WorkerClass worker(Globals::instance().getCentralWidget());
	worker.moveToThread(QApplication::instance()->thread());
	if (iTimeout)
	{
		pRakClient->Disconnect(0);
		emit worker.setStateMessage("Disconnected : Timeout");
	}
	else
	{
		pRakClient->Disconnect(500);
		emit worker.setStateMessage("Disconnected");
	}

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
	connectBtn->setText("Connect");
}

void sampRequestClass(int iClass)
{
	if (pRakClient == NULL)
		return;
	if (!bInClassSelection)
		return;

	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write(iClass);
	pRakClient->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	bInClassSelection = false;
}

void sampRequestSpawn()
{
	if (pRakClient == NULL)
		return;
	if (bInClassSelection)
		return;

	RakNet::BitStream bsSendRequestSpawn;
	pRakClient->RPC(&RPC_RequestSpawn, &bsSendRequestSpawn, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void sampSpawn()
{
	if (pRakClient == NULL)
		return;
	if (bInClassSelection)
		return;

	if (IsDead())
		playerPool[g_myPlayerID].onfootData.fHealth = 100;

	if (!IsSpawned())
	{
		playerPool[g_myPlayerID].onfootData.BYTESkin = SpawnInfo.iSkin;
		playerPool[g_myPlayerID].onfootData.fCurrentPosition[0] = SpawnInfo.vecPos[0];
		playerPool[g_myPlayerID].onfootData.fCurrentPosition[1] = SpawnInfo.vecPos[1];
		playerPool[g_myPlayerID].onfootData.fCurrentPosition[2] = SpawnInfo.vecPos[2];
		playerPool[g_myPlayerID].onfootData.fCurrentAngle = SpawnInfo.fRotation;
	}
	else
		return;

	RakNet::BitStream bsSendSpawn;
	pRakClient->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

	bIsSpectating = 0;
	SetState(STATE_ONFOOT);

	AppendLogF("Spawned.");
	AppendChatBox("You have been spawned!");
	_logs->Log(LogLevel::INFO, "Spawned.");
}

int isPlayerConnected(PLAYERID iPlayerID)
{
	if (iPlayerID < 0 || iPlayerID >= MAX_PLAYERS)
		return 0;

	if (!playerPool[iPlayerID].bIsConnected)
		return 0;

	return 1;
}

int getPlayerID(char *szPlayerName)
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (playerPool[i].bIsConnected == false)
			continue;

		if (!strcmp(playerPool[i].szPlayerName, szPlayerName))
			return i;
	}

	return -1;
}

char *getPlayerName(PLAYERID iPlayerID)
{
	if (playerPool[iPlayerID].bIsConnected && iPlayerID >= 0 && iPlayerID <= MAX_PLAYERS)
		return playerPool[iPlayerID].szPlayerName;

	return 0;
}

int getPlayerPos(PLAYERID iPlayerID, float *fPos)
{
	if (!playerPool[iPlayerID].bIsConnected)
		return 0;

	if (iPlayerID > 0 && iPlayerID <= MAX_PLAYERS)
	{
		memcpy(fPos, playerPool[iPlayerID].onfootData.fCurrentPosition, sizeof(float) * 3);
		return 1;
	}

	return 0;
}

PLAYERID getPlayerIDFromPlayerName(char *szName)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!playerPool[i].bIsConnected)
			continue;
		if (!strcmp(playerPool[i].szPlayerName, szName))
			return (PLAYERID)i;
	}

	return -1;
}

unsigned short getPlayerCount()
{
	unsigned short count = 0;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!playerPool[i].bIsConnected)
			continue;
		count++;
	}
	return count;
}
unsigned short getPlayerCountWoNPC()
{
	unsigned short count = 0;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!playerPool[i].bIsConnected)
			continue;
		if (playerPool[i].BYTEIsNPC)
			continue;
		count++;
	}
	return count;
}
const struct vehicle_entry *gta_vehicle_get_by_id(int id)
{
	id -= VEHICLE_LIST_ID_START;

	if (id < 0 || id >= VEHICLE_LIST_SIZE)
		return NULL;

	return &vehicle_list[id];
}

int gen_gpci(char buf[64], unsigned long factor) /* by bartekdvd */
{
	unsigned char out[6 * 4] = {0};

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < 6 * 4; ++i)
		out[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	out[6 * 4] = 0;

	BIG_NUM_MUL((unsigned long *)out, (unsigned long *)out, factor);

	unsigned int notzero = 0;
	buf[0] = '0';
	buf[1] = '\0';

	if (factor == 0)
		return 1;

	int pos = 0;
	for (int i = 0; i < 24; i++)
	{
		unsigned char tmp = out[i] >> 4;
		unsigned char tmp2 = out[i] & 0x0F;

		if (notzero || tmp)
		{
			buf[pos++] = (char)((tmp > 9) ? (tmp + 55) : (tmp + 48));
			if (!notzero)
				notzero = 1;
		}

		if (notzero || tmp2)
		{
			buf[pos++] = (char)((tmp2 > 9) ? (tmp2 + 55) : (tmp2 + 48));
			if (!notzero)
				notzero = 1;
		}
	}
	buf[pos] = 0;

	return pos;
}

void SetStringFromCommandLine(char *szCmdLine, char *szString)
{
	while (*szCmdLine == ' ')
		szCmdLine++;
	while (*szCmdLine && *szCmdLine != ' ' && *szCmdLine != '-' && *szCmdLine != '/')
	{
		*szString = *szCmdLine;
		szString++;
		szCmdLine++;
	}
	*szString = '\0';
}
