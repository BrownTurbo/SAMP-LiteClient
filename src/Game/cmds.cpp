#include "../../include/Game/cmds.h"

int RunCommand(char *szCMD)
{
	// return 0: should run server sided command.
	// return 1: found local command.
	// return 2: local command not found.
	// return 3: exit process.

	if (szCMD[0] == 0x00)
		return 2;

	if (szCMD[0] != '!')
	{
		// SERVER CHAT OR COMMAND
		if (szCMD[0] == '/')
		{
			_logs->Log(LogLevel::INFO, "[CMD] Server Command: %s", szCMD);
			sendServerCommand(szCMD);
		}
		else
		{
			_logs->Log(LogLevel::INFO, "[CHAT] Outgoing Chat: %s", szCMD);
			sendChat(szCMD);
		}

		return 0;
	}

	szCMD++;

	// EXIT
	if (!strncmp(szCMD, "exit", 4) || !strncmp(szCMD, "EXIT", 4) ||
		!strncmp(szCMD, "quit", 4) || !strncmp(szCMD, "QUIT", 4))
	{
		_logs->Log(LogLevel::INFO, "[CMD] !exit executed.");
		if (GetGameState() != GAMESTATE_DISCONNECTED)
		{
			SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);
			std::this_thread::sleep_for(std::chrono::milliseconds(1250));
		}
		QApplication::quit();

		return 3;
	}

	// DISCONNECT
	if (!strncmp(szCMD, "disconnect", 10) || !strncmp(szCMD, "DISCONNECT", 10))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			_logs->Log(LogLevel::WARN, "[CMD] !disconnect rejected: Game not initialized or already disconnected.");
			return 0;
		}
		AppendChatBox("Disconnection Requested.");
		AppendLogF("Disconnection Requested.");
		_logs->Log(LogLevel::INFO, "[CMD] Disconnection Requested.");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);

		return 1;
	}

	// CONNECT
	if (!strncmp(szCMD, "connect", 7) || !strncmp(szCMD, "CONNECT", 7))
	{
		if (GetGameState() != GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: You're already (pre-)connected!");
			_logs->Log(LogLevel::WARN, "[CMD] !connect rejected: Already connected or connecting.");
			return 0;
		}
		AppendChatBox("Connection Requested.");
		AppendLogF("Connection Requested.");
		_logs->Log(LogLevel::INFO, "[CMD] Connection Requested to %s:%d.", settings.server.szAddr, settings.server.iPort);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		SetGameState(GAMESTATE_CONNECTION_REQUESTED);

		return 1;
	}

	// RECONNECT
	if (!strncmp(szCMD, "reconnect", 9) || !strncmp(szCMD, "RECONNECT", 9))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			_logs->Log(LogLevel::WARN, "[CMD] !reconnect rejected: Game not initialized or disconnected.");
			return 0;
		}
		_logs->Log(LogLevel::INFO, "[CMD] Reconnection Requested.");
		SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);
		resetPools();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		SetGameState(GAMESTATE_WAIT_CONNECT);

		return 1;
	}

	// RELOAD SETTINGS
	if (!strncmp(szCMD, "reload", 6) || !strncmp(szCMD, "RELOAD", 6))
	{
		_logs->Log(LogLevel::INFO, "[CMD] Reloading settings from config.xml...");
		ReloadSettings();

		return 1;
	}

	// SET RUNMODE
	if (!strncmp(szCMD, "runmode", 7) || !strncmp(szCMD, "RUNMODE", 7))
	{
		if (strlen(szCMD) > 7 && szCMD[7] == ' ')
		{
			unsigned short iRunModeID = atoi(&szCMD[8]);

			if (iRunModeID > 0 && iRunModeID < 6)
			{
				g_myRunMode = (eRunModes)iRunModeID;
				AppendChatBox("Runmode set to %d.", g_myRunMode);
				_logs->Log(LogLevel::INFO, "[CMD] Runmode set to %d.", g_myRunMode);
			}
			else
			{
				AppendChatBox("ERROR: Invalid runmode (1-5).");
				_logs->Log(LogLevel::WARN, "[CMD] Invalid runmode (%d). Allowed values: 1-5.", iRunModeID);
			}
		}
		else
		{
			AppendChatBox("USAGE: !runmode <1-5> (Current: %d)", g_myRunMode);
		}

		return 1;
	}

	// PLAYER LIST
	if (!strncmp(szCMD, "players", 7) || !strncmp(szCMD, "PLAYERS", 7))
	{
		if (GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: You're not connected yet!");
			return 0;
		}
		int iPlayerCount = 0;
		AppendChatBox("============ PLAYER LIST ============");
		AppendChatBox("(ID: %d) %s - score: %d, ping: %d", g_myPlayerID, playerPool[g_myPlayerID].szPlayerName, playerPool[g_myPlayerID].iScore, playerPool[g_myPlayerID].dwPing);
		for (PLAYERID i = 0; i < MAX_PLAYERS; i++)
		{
			if (g_myPlayerID == i)
				continue;

			if (!playerPool[i].bIsConnected)
				continue;

			if (playerPool[i].BYTEIsNPC != 0)
				continue;

			AppendChatBox("(ID: %d) %s - score: %d, ping: %d, streamed %s", i, playerPool[i].szPlayerName, playerPool[i].iScore, playerPool[i].dwPing, (playerPool[i].bIsStreamedIn ? "in" : "out"));

			iPlayerCount++;
		}
		AppendChatBox(" ");
		AppendChatBox("Count: %d.", iPlayerCount);
		AppendChatBox("=================================");
		AppendChatBox(" ");
		return 1;
	}

	// NPC LIST
	if (!strncmp(szCMD, "npcs", 7) || !strncmp(szCMD, "NPCS", 7))
	{
		if (GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: You're not connected yet!");
			return 0;
		}
		int iPlayerCount = 0;
		AppendChatBox(" ");
		AppendChatBox("============ NPC LIST ============");
		for (PLAYERID i = 0; i < MAX_PLAYERS; i++)
		{
			if (g_myPlayerID == i)
				continue;

			if (!playerPool[i].bIsConnected)
				continue;

			if (!playerPool[i].BYTEIsNPC)
				continue;

			AppendChatBox("(ID: %d) %s - score: %d, ping: %d, streamed %s", i, playerPool[i].szPlayerName, playerPool[i].iScore, playerPool[i].dwPing, (playerPool[i].bIsStreamedIn ? "in" : "out"));
			iPlayerCount++;
		}
		AppendChatBox(" ");
		AppendChatBox("Count: %d.", iPlayerCount);
		AppendChatBox("===============================");
		AppendChatBox(" ");
		return 1;
	}

	// VEHICLE LIST
	if (!strncmp(szCMD, "vlist", 5) || !strncmp(szCMD, "VLIST", 5))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		for (VEHICLEID i = 0; i < MAX_VEHICLES; i++)
		{
			if (!vehiclePool[i].iExist)
				continue;

			const struct vehicle_entry *vehicle = gta_vehicle_get_by_id(vehiclePool[i].iModelID);
			if (vehicle)
				AppendChatBox("[VLIST] %d (%s)", i, vehicle->name);
		}

		return 1;
	}

	// REQUEST CLASS
	if (!strncmp(szCMD, "class", 5) || !strncmp(szCMD, "CLASS", 5))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		if (!bInClassSelection)
		{
			AppendChatBox("ERROR: You're not in Class selection!");
			return 0;
		}
		if (strlen(szCMD) > 5 && szCMD[5] == ' ')
		{
			int iClassID = atoi(&szCMD[6]);
			if (iClassID < 0 || iClassID > 319)
			{
				AppendChatBox("ERROR: Invalid Class ID (0-319)!");
				_logs->Log(LogLevel::WARN, "[CMD] Invalid Class ID: %d", iClassID);
				return 0;
			}
			_logs->Log(LogLevel::INFO, "[CMD] Requesting class ID %d.", iClassID);
			sampRequestClass(static_cast<unsigned short>(iClassID));
		}
		else
		{
			AppendChatBox("USAGE: !class <ClassID>");
		}
		return 1;
	}

	// SPAWNS THE FAKE PLAYER
	if (!strncmp(szCMD, "spawn", 5) || !strncmp(szCMD, "SPAWN", 5))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			_logs->Log(LogLevel::WARN, "[CMD] !spawn rejected: Game not inited.");
			return 0;
		}
		if (IsDead())
		{
			_logs->Log(LogLevel::WARN, "[CMD] !spawn rejected: Player is dead.");
			return 0;
		}
		if (GetState() != STATE_NOT_SPAWNED && GetState() != STATE_VOID)
		{
			AppendChatBox("ERROR: You're already spawned!");
			_logs->Log(LogLevel::WARN, "[CMD] !spawn rejected: Already spawned.");
			return 0;
		}
		if (bInClassSelection)
		{
			AppendChatBox("ERROR: You've to proceed in the Class selection first!");
			_logs->Log(LogLevel::WARN, "[CMD] !spawn rejected: Must complete class selection first.");
			return 0;
		}
		_logs->Log(LogLevel::INFO, "[CMD] Requesting player spawn...");
		sampRequestSpawn();
		sampSpawn();
		return 1;
	}

	// SEND WE PICKED UP A PICKUP :-)
	if (!strncmp(szCMD, "pickup", 6) || !strncmp(szCMD, "PICKUP", 6))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		if (!IsSpawned())
		{
			AppendChatBox("ERROR: You're not spawned yet!");
			return 0;
		}
		if (strlen(szCMD) > 6 && szCMD[6] == ' ')
		{
			int iPickupID = atoi(&szCMD[7]);

			if (iPickupID < 0 || iPickupID >= MAX_PICKUPS || !pickupExist[iPickupID])
			{
				AppendChatBox("ERROR: Invalid Pickup ID!");
				_logs->Log(LogLevel::WARN, "[CMD] Invalid pickup ID: %d", iPickupID);
				return 0;
			}
			sendPickUp(static_cast<PICKUPID>(iPickupID));
			AppendChatBox("Picked up ID %d pickup.", iPickupID);
			_logs->Log(LogLevel::INFO, "[CMD] Picked up pickup ID %d.", iPickupID);
		}
		else
		{
			AppendChatBox("USAGE: !pickup <PickupID>");
		}
		return 1;
	}

	// SET THE FAKE PLAYER'S CURRENT WEAPON
	if (!strncmp(szCMD, "weapon", 6) || !strncmp(szCMD, "WEAPON", 6))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		if (!IsSpawned())
		{
			AppendChatBox("ERROR: You're not spawned yet!");
			return 0;
		}
		if (strlen(szCMD) > 6 && szCMD[6] == ' ')
		{
			playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon = (uint8_t)atoi(&szCMD[7]);
			AppendChatBox("Client's current weapon set to %d.", playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon);
			_logs->Log(LogLevel::INFO, "[CMD] Current weapon set to %d.", playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon);
		}
		else
		{
			AppendChatBox("USAGE: !weapon <WeaponID>");
		}
		return 1;
	}

	// SET THE FOLLOWED PLAYER'S NAME
	if (!strncmp(szCMD, "selplayer", 9) || !strncmp(szCMD, "SELPLAYER", 9))
	{
		if (strlen(szCMD) > 9 && szCMD[9] == ' ')
		{
			char *szPlayerName = &szCMD[10];

			snprintf(szFollowingPlayerName, MAX_PLAYER_NAME, "%s", szPlayerName);

			g_myRunMode = RUNMODE_FOLLOWPLAYER;

			AppendChatBox("[SELPLAYER] Following player changed to %s.", szFollowingPlayerName);
			_logs->Log(LogLevel::INFO, "[CMD] Following player changed to '%s'.", szFollowingPlayerName);
		}
		else
		{
			AppendChatBox("USAGE: !selplayer <PlayerName>");
		}
		return 1;
	}

	// SET THE FAKE PLAYER'S VEHICLE
	if (!strncmp(szCMD, "selveh", 6) || !strncmp(szCMD, "SELVEH", 6))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		if (!IsSpawned())
		{
			AppendChatBox("ERROR: You're not spawned yet!");
			return 0;
		}
		if (strlen(szCMD) > 6 && szCMD[6] == ' ')
		{
			unsigned int iSelectedVeh = atoi(&szCMD[7]);

			if (g_myRunMode == RUNMODE_FOLLOWPLAYERSVEHICLE)
				iFollowingWithVehicleID = (VEHICLEID)iSelectedVeh;

			AppendChatBox("[SELVEH] Changed to vehicle ID to %d.", iSelectedVeh);
			_logs->Log(LogLevel::INFO, "[CMD] Vehicle ID changed to %d.", iSelectedVeh);
		}
		else
		{
			AppendChatBox("USAGE: !selveh <VehicleID>");
		}

		return 1;
	}

	// CHANGE FOLLOWING OFFSET
	if (!strncmp(szCMD, "follow", 6) || !strncmp(szCMD, "FOLLOW", 6))
	{
		char szType[32];
		float fValue;

		if (sscanf(&szCMD[7], "%s%f", szType, &fValue) < 2)
		{
			AppendChatBox("USAGE: !follow <x/y/z> <value>");
			return 1;
		}

		if (szType[0] == 'x')
			fFollowXOffset = fValue;

		else if (szType[0] == 'y')
			fFollowYOffset = fValue;

		else if (szType[0] == 'z')
			fFollowZOffset = fValue;

		_logs->Log(LogLevel::INFO, "[CMD] Follow offset changed: %s = %.2f", szType, fValue);
		return 1;
	}

	// SEND SCM EVENT
	if (!strncmp(szCMD, "scmevent", 8) || !strncmp(szCMD, "SCMEVENT", 8))
	{
		int iEvent, iParam1, iParam2, iParam3;

		if (sscanf(&szCMD[9], "%d%d%d%d", &iEvent, &iParam1, &iParam2, &iParam3) < 4)
		{
			AppendChatBox("USAGE: !scmevent <type> <param1> <param2> <param3>");
			return 1;
		}

		sendScmEvent(iEvent, iParam1, iParam2, iParam3);
		_logs->Log(LogLevel::INFO, "[CMD] SCM Event: %d, %d, %d, %d", iEvent, iParam1, iParam2, iParam3);
		return 1;
	}

	// SELECT TEXTDRAW
	if (!strncmp(szCMD, "seltd", 5) || !strncmp(szCMD, "SELTD", 5))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		unsigned int iSelectableTextDrawID;

		if (sscanf(&szCMD[6], "%d", &iSelectableTextDrawID) < 1)
		{
			AppendChatBox("USAGE: !seltd <TextDrawID>");
			return 1;
		}

		selectTextDraw(iSelectableTextDrawID);
		_logs->Log(LogLevel::INFO, "[CMD] Selected TextDraw ID %d.", iSelectableTextDrawID);
		return 1;
	}

	// SHOW SENDRATES
	if (!strncmp(szCMD, "sendrates", 9) || !strncmp(szCMD, "SENDRATES", 9))
	{
		AppendChatBox("[SENDRATES] onfoot: %d, incar: %d, firing %d, multiplier %d.", iNetModeNormalOnfootSendRate, iNetModeNormalIncarSendRate, iNetModeFiringSendRate, iNetModeSendMultiplier);
		_logs->Log(LogLevel::INFO, "[SENDRATES] onfoot: %d, incar: %d, firing %d, multiplier %d.", iNetModeNormalOnfootSendRate, iNetModeNormalIncarSendRate, iNetModeFiringSendRate, iNetModeSendMultiplier);
		return 1;
	}

	AppendChatBox("Command %s was not found.", szCMD);
	_logs->Log(LogLevel::WARN, "[CMD] Unknown command: '%s'", szCMD);

	return 2;
}
