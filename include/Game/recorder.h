#pragma once
#ifndef RECORDER_H
#define RECORDER_H
#include "../common.h"

#include <iostream>
#include <vector>

// Thanks to 81Vm3 for the base of the Packets Recorder
namespace PBRecorder {
	extern int iPlaybackType;
	extern char *recordTitle;
	extern int iPacketRecorded;
	extern int iTargetPlayer;

	bool StartRecording(int type, char title[75]);
	void AddOnfootPacket(const ONFOOT_SYNC_DATA *data);
	void AddVehiclePacket(const INCAR_SYNC_DATA *data);

	void Reset();
	bool Finalize(const char *szFile, char *errmsg = nullptr);
}

#endif
