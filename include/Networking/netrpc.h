#pragma once
#ifndef NETRPC_H
#define NETRPC_H
#include <cstdint>
#include <thread>
#include <chrono>
#include "../common.h"
#include "../xmlsets.h"
#include "netgame.h"
#include "../Game/cmds.h"
#include "../utils.h"
#include "../Game/dialogs.h"
#include "../Game/menus.h"
#include "../WorkerClass.h"
#include "../Game/audiostream.h"

extern int iNetModeNormalOnfootSendRate, iNetModeNormalIncarSendRate, iNetModeFiringSendRate, iNetModeSendMultiplier;
extern char g_szHostName[256];

struct stGTAMenu
{
	bool szEnabled;
	char szTitle[MAX_MENU_LINE];
	int szTitleL;
	char szSeparator[MAX_MENU_LINE];
	char szColumnHeader[MAX_MENU_COLUMNS][MAX_MENU_LINE];
	uint8_t szColumnCount[MAX_MENU_COLUMNS];
	bool bRowState[MAX_MENU_COLUMNS][MAX_MENU_ITEMS];
	char szRowLine[MAX_MENU_COLUMNS][MAX_MENU_ITEMS][MAX_MENU_LINE];
};

#define DIALOG_STYLE_MSGBOX		0
#define DIALOG_STYLE_INPUT		1
#define DIALOG_STYLE_LIST		2
#define DIALOG_STYLE_PASSWORD		3
#define DIALOG_STYLE_TABLIST		4
#define DIALOG_STYLE_TABLIST_HEADERS		5

struct stSAMPDialog
{
	int iIsActive;
	uint8_t bDialogStyle;
	uint16_t wDialogID;
	uint8_t bTitleLength;
	char szTitle[257];
	uint8_t bButton1Len;
	char szButton1[257];
	uint8_t bButton2Len;
	char szButton2[257];
	char szInfo[257];
};
extern struct stSAMPDialog sampDialog;

extern bool iGettingNewName;

void RegisterRPCs(RakClientInterface *pRakClient);
void UnRegisterRPCs(RakClientInterface * pRakClient);
#endif
