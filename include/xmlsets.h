#ifndef XMLSETS_H
#define XMLSETS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>

#include <QtCore/QString>

#include "../lib/tinyxml/tinyxml2.h"

#include "utils.h"
#include "common.h"
#include "Networking/netrpc.h"
#include "WorkerClass.h"

#define MAX_AUTORUN_CMDS 32
#define MAX_FIND_ITEMS 128
#define MAX_TELEPORT_ITEMS 200

struct stServer
{
	char szAddr[256];
	int iPort;
	char szNickname[20];
	char szPassword[32];
};

struct stSettings
{
	struct stServer server;

	int UpdateRate;

	int RespawnTick;

	int iUpdateStats;
	int iMinFPS;
	int iMaxFPS;

	char szClientVersion[20];

	unsigned int uiObjectsLogging;
	unsigned int uiPickupsLogging;
	unsigned int uiTextLabelsLogging;
	unsigned int uiTextDrawsLogging;

	unsigned int uiChallange;
};
extern struct stSettings settings;

void LoadSettings();
void UnLoadSettings();
void ReloadSettings();
#endif
