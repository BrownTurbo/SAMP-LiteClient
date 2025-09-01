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
			sendServerCommand(szCMD);
		else
			sendChat(szCMD);

		return 0;
	}

	szCMD++;

	// EXIT
	if (!strncmp(szCMD, "exit", 4) || !strncmp(szCMD, "EXIT", 4) ||
		!strncmp(szCMD, "quit", 4) || !strncmp(szCMD, "QUIT", 4))
	{
		if (GetGameState() != GAMESTATE_DISCONNECTED)
		{
			SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);
			std::this_thread::sleep_for(std::chrono::milliseconds(1250));
		}
		std::exit(0);

		return 3;
	}

	// DISCONNECT
	if (!strncmp(szCMD, "disconnect", 10) || !strncmp(szCMD, "DISCONNECT", 10))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		AppendChatBox("Disconnection Requested.");
		AppendLogF("Disconnection Requested.");
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
			return 0;
		}
		AppendChatBox("Connection Requested.");
		AppendLogF("Connection Requested.");
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
			return 0;
		}
		SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);
		resetPools();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		SetGameState(GAMESTATE_WAIT_CONNECT);

		return 1;
	}

	// RELOAD SETTINGS
	if (!strncmp(szCMD, "reload", 6) || !strncmp(szCMD, "RELOAD", 6))
	{
		ReloadSettings();

		return 1;
	}

	// SET RUNMODE
	if (!strncmp(szCMD, "runmode", 7) || !strncmp(szCMD, "RUNMODE", 7))
	{
		unsigned short iRunModeID = atoi(&szCMD[8]);

		if (iRunModeID > 0 && iRunModeID < 6)
		{
			g_myRunMode = (eRunModes)iRunModeID;
			AppendChatBox("Runmode set to %d.", g_myRunMode);
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
		unsigned short iClassID = atoi(&szCMD[6]);
		if (iClassID > 0)
		{
			AppendChatBox("ERROR: Invalid Class ID!");
			return 0;
		}
		sampRequestClass(iClassID);
		return 1;
	}

	// SPAWNS THE FAKE PLAYER
	if (!strncmp(szCMD, "spawn", 5) || !strncmp(szCMD, "SPAWN", 5))
	{
		if (!iGameInited || GetGameState() == GAMESTATE_DISCONNECTED)
		{
			AppendChatBox("ERROR: Game is not Inited yet!");
			return 0;
		}
		if (IsDead())
			return 0;
		if (GetState() != STATE_NOT_SPAWNED && GetState() != STATE_VOID)
		{
			AppendChatBox("ERROR: You're already spawned!");
			return 0;
		}
		if (bInClassSelection)
		{
			AppendChatBox("ERROR: You've to proceed in the Class selection first!");
			return 0;
		}
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
		PICKUPID iPickupID = atoi(&szCMD[7]);

		if (!pickupExist[iPickupID])
		{
			AppendChatBox("ERROR: Invalid Pickup ID!");
			return 0;
		}
		sendPickUp(iPickupID);
		AppendChatBox("Picked up ID %d pickup.", iPickupID);
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
		playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon = (uint8_t)atoi(&szCMD[7]);
		AppendChatBox("Client's current weapon set to %d.", playerPool[g_myPlayerID].onfootData.BYTECurrentWeapon);
		return 1;
	}

	// SET THE FOLLOWED PLAYER'S NAME
	if (!strncmp(szCMD, "selplayer", 9) || !strncmp(szCMD, "SELPLAYER", 9))
	{
		char *szPlayerName = &szCMD[10];

		snprintf(szFollowingPlayerName, MAX_PLAYER_NAME, "%s", szPlayerName);

		g_myRunMode = RUNMODE_FOLLOWPLAYER;

		AppendChatBox("[SELPLAYER] Following player changed to %s.", szFollowingPlayerName);
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
		unsigned int iSelectedVeh = atoi(&szCMD[7]);

		if (g_myRunMode == RUNMODE_FOLLOWPLAYERSVEHICLE)
			iFollowingWithVehicleID = (VEHICLEID)iSelectedVeh;

		AppendChatBox("[SELVEH] Changed to vehicle ID to %d.", iSelectedVeh);

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
		return 1;
	}

	// SHOW SENDRATES
	if (!strncmp(szCMD, "sendrates", 9) || !strncmp(szCMD, "SENDRATES", 9))
	{
		AppendChatBox("[SENDRATES] onfoot: %d, incar: %d, firing %d, multiplier %d.", iNetModeNormalOnfootSendRate, iNetModeNormalIncarSendRate, iNetModeFiringSendRate, iNetModeSendMultiplier);
		return 1;
	}

	AppendChatBox("Command %s was not found.", szCMD);

	return 2;
}
