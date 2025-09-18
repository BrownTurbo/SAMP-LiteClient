#include "../include/RakNetWorker.h"

RakNetWorker::RakNetWorker() {}
RakNetWorker::~RakNetWorker() {}

void RakNetWorker::run()
{
	WorkerClass worker(Globals::instance().getCentralWidget());
	worker.moveToThread(QApplication::instance()->thread());
	time_t curr_time = time(NULL);
	struct tm *localTime = localtime(&curr_time);
	Packet *pkt;

	SetGameState(GAMESTATE_DISCONNECTED);
	while (!QThread::currentThread()->isInterruptionRequested())
	{
		int iLastMoney = iMoney;
		int iLastDrunkLevel = iDrunkLevel;
		int iLastStatsUpdate = GetTickCount();
		uint32_t dwAutoRunTick = GetTickCount();

		if (!LoadSettings())
		{
			AppendLogF("Failed to load settings");
			//_logs->Log(LogLevel::ERROR, "Failed to load settings");
			QApplication::exit(1);
		}

		pRakClient = RakNetworkFactory::GetRakClientInterface();
		if (pRakClient == NULL)
			QApplication::exit(1);

		pRakClient->SetMTUSize(576);

		/*if (!LoadAnimationFromConfig(&settings))
		{
			AppendLogF("Invalid animation config");
			_logs->Log(LogLevel::ERROR, "Invalid animation config");
		}*/

		resetPools();
		RegisterRPCs(pRakClient);

		while (1)
		{
			if ((GetTickCount() - dwLastConnectAttempt) > iReconnectTime)
			{
				receivedResponse = false;
				if (GetGameState() == GAMESTATE_CONNECTION_REQUESTED || GetGameState() == GAMESTATE_WAIT_CONNECT)
				{
					if (!iGettingNewName)
						sampConnect(settings.server.szAddr, settings.server.iPort, settings.server.szNickname, settings.server.szPassword, pRakClient);
					else
						sampConnect(settings.server.szAddr, settings.server.iPort, playerPool[g_myPlayerID].szPlayerName, settings.server.szPassword, pRakClient);
				}
				else if (GetGameState() == GAMESTATE_CONNECTING)
				{
					if (!iGettingNewName)
						sampConnect(settings.server.szAddr, settings.server.iPort, settings.server.szNickname, settings.server.szPassword, pRakClient);
					else
						sampConnect(settings.server.szAddr, settings.server.iPort, playerPool[g_myPlayerID].szPlayerName, settings.server.szPassword, pRakClient);
					dwLastConnectAttempt = GetTickCount();
				}
			}

			pkt = pRakClient->Receive();
			UpdateNetwork(pRakClient, pkt);
			if (GetGameState() == GAMESTATE_CONNECTED && iGameInited)
			{
				UpdatePlayerScoresAndPings(1, 3000, pRakClient);

				if (Playback::bPlaying)
					Playback::Process();

				static uint32_t dwLastInfoUpdate = GetTickCount();
				const char *SAZone;
				if (dwLastInfoUpdate && dwLastInfoUpdate < (GetTickCount() - 1000))
				{
					char szHealthText[16], szArmourText[16];

					if (playerPool[g_myPlayerID].onfootData.fHealth > 200.0f || playerPool[g_myPlayerID].onfootData.fHealth < 0.0f)
						snprintf(szHealthText, sizeof(szHealthText), "N/A");
					else
						snprintf(szHealthText, sizeof(szHealthText), "%.2f", playerPool[g_myPlayerID].onfootData.fHealth);

					if (playerPool[g_myPlayerID].onfootData.fArmour > 200.0f || playerPool[g_myPlayerID].onfootData.fArmour < 0.0f)
						snprintf(szArmourText, sizeof(szArmourText), "N/A");
					else
						snprintf(szArmourText, sizeof(szArmourText), "%.2f", playerPool[g_myPlayerID].onfootData.fArmour);

					Get2DZoneByPos(playerPool[g_myPlayerID].onfootData.fCurrentPosition[0], playerPool[g_myPlayerID].onfootData.fCurrentPosition[1], SAZone);

					handleProperty(QString::fromUtf8("Server Hostname"), false, QString::fromUtf8(g_szHostName));
					handleProperty(QString::fromUtf8("Server Players"), false, QString::number(getPlayerCountWoNPC()));
					handleProperty(QString::fromUtf8("Server PlayerBase"), false, QString::number(getPlayerCount()));
					handleProperty(QString::fromUtf8("World Time"), false, QString::number(g_srvWorldTime));
					handleProperty(QString::fromUtf8("Weather"), false, QString::number(g_srvWeather));
					handleProperty(QString::fromUtf8("Player Health"), false, QString::fromUtf8(szHealthText));
					handleProperty(QString::fromUtf8("Player Armour"), false, QString::fromUtf8(szArmourText));
					handleProperty(QString::fromUtf8("Player Skin"), false, QString::number(playerPool[g_myPlayerID].onfootData.BYTESkin));
					handleProperty(QString::fromUtf8("Player ID"), false, QString::number(g_myPlayerID));
					handleProperty(QString::fromUtf8("Player Score"), false, QString::number(playerPool[g_myPlayerID].iScore));
					handleProperty(QString::fromUtf8("Player Ping"), false, QString::number(playerPool[g_myPlayerID].dwPing));
					handleProperty(QString::fromUtf8("Player Ping(RakClient)"), false, QString::number(pRakClient->GetPlayerPing(pRakClient->GetPlayerID())));
					handleProperty(QString::fromUtf8("Player Pos:X"), false, QString::number(playerPool[g_myPlayerID].onfootData.fCurrentPosition[0]));
					handleProperty(QString::fromUtf8("Player Pos:Y"), false, QString::number(playerPool[g_myPlayerID].onfootData.fCurrentPosition[1]));
					handleProperty(QString::fromUtf8("Player Pos:Z"), false, QString::number(playerPool[g_myPlayerID].onfootData.fCurrentPosition[2]));
					handleProperty(QString::fromUtf8("Player Pos:Zone"), false, QString::fromUtf8(SAZone));
					handleProperty(QString::fromUtf8("Player Rotation"), false, QString::number(playerPool[g_myPlayerID].onfootData.fCurrentAngle));
					handleProperty(QString::fromUtf8("Player Wanted Level"), false, QString::number(playerPool[g_myPlayerID].iWantedLevel));
					handleProperty(QString::fromUtf8("Player Fighting Style"), false, QString::number(playerPool[g_myPlayerID].onfootData.FightingStyle));
					handleProperty(QString::fromUtf8("Player Drunk level"), false, QString::number(iDrunkLevel));
					char szSkillsTXT[25];
					for (int skillID = 0; skillID < MAX_SKILLS; skillID++)
					{
						snprintf(szSkillsTXT, sizeof(szSkillsTXT), "Player Skill#%d Level", skillID);
						handleProperty(QString::fromUtf8(szSkillsTXT), false, QString::number(playerPool[g_myPlayerID].onfootData.SkillLevel[skillID]));
					}
					handleProperty(QString::fromUtf8("Player Team"), false, QString::number(playerPool[g_myPlayerID].onfootData.BYTETeam));

					Scoreboard::PlayerDTA ScoreboardDTA;
					Scoreboard *scoreboard_;

					ScoreboardDTA.id = g_myPlayerID;
					ScoreboardDTA.name = playerPool[g_myPlayerID].szPlayerName;
					ScoreboardDTA.score = playerPool[g_myPlayerID].iScore;
					ScoreboardDTA.ping = playerPool[g_myPlayerID].dwPing;
					QMetaObject::invokeMethod(getMainWindow(), [=]()
											  { scoreboard_->handlePlayer(ScoreboardDTA); }, Qt::QueuedConnection);
					QMetaObject::invokeMethod(getMainWindow(), [=]()
											  { scoreboard_->updateTableSize(getPlayerCountWoNPC()); }, Qt::QueuedConnection);
				}

				if (settings.iUpdateStats)
				{
					if ((GetTickCount() - iLastStatsUpdate >= 1000) || iMoney != iLastMoney || iDrunkLevel != iLastDrunkLevel)
					{
						RakNet::BitStream bsSend;

						bsSend.Write((uint8_t)ID_STATS_UPDATE);

						iDrunkLevel -= (rand() % settings.iMaxFPS + settings.iMinFPS);

						if (iDrunkLevel < 0)
							iDrunkLevel = 0;

						bsSend.Write(iMoney);
						bsSend.Write(iDrunkLevel);

						pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);

						iLastMoney = iMoney;
						iLastDrunkLevel = iDrunkLevel;

						iLastStatsUpdate = GetTickCount();
					}
				}

				if (GetState() != STATE_NOT_SPAWNED && GetState() != STATE_VOID)
				{
					if (IsDead() && GetTickCount() - uRespawnTick > settings.RespawnTick)
					{
						sampRequestClass(g_myClassID);
						sampRequestSpawn();
						sampSpawn();
						SetState(STATE_NOT_SPAWNED);
					}
					else
						AppendChatBox("Use <b>!class {CLASS_ID}</b> to select a Class and <b>!spawn</b> to Spawn");
				}
				else
				{
					if (Playback::bPlaying)
						Playback::Process();
					else if (GetTickCount() - uUpdateTick > settings.UpdateRate)
					{
						if (g_myRunMode == RUNMODE_STILL)
						{
							// Nothing left to do. :-)
						}

						if (g_myRunMode == RUNMODE_NORMAL)
						{
							if (!bIsSpectating)
								onFootUpdateAtNormalPos();
							else
								spectatorUpdate();
						}

						// Following player mode.
						if (g_myRunMode == RUNMODE_FOLLOWPLAYER)
						{
							PLAYERID copyingID = getPlayerIDFromPlayerName(szFollowingPlayerName);
							if (copyingID != (PLAYERID)-1)
								onFootUpdateFollow(copyingID);
						}

						// Following a player with a vehicle mode.
						if (g_myRunMode == RUNMODE_FOLLOWPLAYERSVEHICLE)
						{
							PLAYERID copyingID = getPlayerIDFromPlayerName(szFollowingPlayerName);
							if (copyingID != (PLAYERID)-1)
								inCarUpdateFollow(copyingID, (VEHICLEID)iFollowingWithVehicleID);
						}
						uUpdateTick = GetTickCount();
					}
					if (!Playback::bPlaying)
						Playback::Update();
				}
			}

			if (GetOldGameState() == GAMESTATE_CONNECTED && GetGameState() == GAMESTATE_DISCONNECTION_REQUESTED)
				sampDisconnect(0);
			QCoreApplication::processEvents();
			QThread::msleep(10);
		}
	}
}
