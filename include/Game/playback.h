#pragma once
#ifndef PLAYBACK_H
#define PLAYBACK_H
#include "../common.h"

#include <iostream>
#include <vector>
#include <variant>
#include <ranges>

// Thanks to 81Vm3 for the base of the Packets Playback
namespace Playback {
	extern const char szPlaybackDirectory[];

	enum ePBType
	{
		PLAYBACK_TYPE_NONE,
		PLAYBACK_TYPE_DRIVER,
		PLAYBACK_TYPE_ONFOOT
	};

	extern const char szPlaybackDirectory[];
	extern ePBType iPlaybackType;
	extern bool bPlaying;

	bool Load(const char *szFile, char *errmsg = nullptr);
	bool Unload();
	void StartFirstEver();
	void Update();
	void Process();

	int GetTotalPacket();
	int GetCurrentIndex();
	float GetProgessPercent();

	typedef struct _RECORD_DATA
	{
		ePBType type;
		unsigned long timestamp;
		float QuaternionW;
		float QuaternionX;
		float QuaternionY;
		float QuaternionZ;
		float CurrentPosX;
		float CurrentPosY;
		float CurrentPosZ;
		float CurrentAngle;
		float VelocityX;
		float VelocityY;
		float VelocityZ;
		uint8_t CurrentWeapon;
		uint8_t SirenOn;
		uint8_t LandingGearState;
		float TrainSpeed;
	} RECORD_DATA;
}

#endif
