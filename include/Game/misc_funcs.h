#include "../common.h"
#include "../utils.h"
#include "../xmlsets.h"
#include "./dialogs.h"
#include "../WorkerClass.h"
#include "../globals.h"
#include <cstdint>

void onFootUpdateAtNormalPos();
void onFootUpdateFollow(PLAYERID followID);
void inCarUpdateFollow(PLAYERID followID, VEHICLEID withVehicleID);
void spectatorUpdate();

int sampConnect(char *szHostname, int iPort, char *szNickname, char *szPassword, RakClientInterface *pRakClient);
void sampDisconnect(int iTimeout);
void sampRequestClass(int iClass);
void sampRequestSpawn();
void sampSpawn();

int isPlayerConnected(PLAYERID iPlayerID);
int getPlayerID(char *szPlayerName);
char *getPlayerName(PLAYERID iPlayerID);
int getPlayerPos(PLAYERID iPlayerID, float *fPos);

PLAYERID getPlayerIDFromPlayerName(char *szName);
unsigned short getPlayerCount();
unsigned short getPlayerCountWoNPC();

const struct vehicle_entry *gta_vehicle_get_by_id ( int id );

int gen_gpci(char buf[64], unsigned long factor);

void SetStringFromCommandLine(char *szCmdLine, char *szString);
