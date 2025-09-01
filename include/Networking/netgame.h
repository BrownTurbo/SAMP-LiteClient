#pragma once
#ifndef NETGAME_H
#define NETGAME_H
#include <cstdint>
#include <thread>
#include <chrono>
#include "../common.h"
//#include "../mainwindow.h"
#include "../utils.h"
#include <cstdlib>
#include "../xmlsets.h"
#include "../Game/misc_funcs.h"
#include "../Game/localplayer.h"
#include "../WorkerClass.h"
#include "../globals.h"
#include "../Game/playback.h"
#include "../Game/recorder.h"

void resetPools();
void UpdatePlayerScoresAndPings(int iWait, int iMS, RakClientInterface *pRakClient);
void UpdateNetwork(RakClientInterface *pRakClient, Packet *pkt);
#endif
