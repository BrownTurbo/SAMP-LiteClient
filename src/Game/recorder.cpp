#include "../../include/Game/recorder.h"
#include "../../include/utils.h"

// Thanks to 81Vm3 for the base of the Packets Recorder
namespace PBRecorder
{
	std::vector<uint32_t> v_dwTime;
	std::vector<ONFOOT_SYNC_DATA> v_playerSyncData;
	std::vector<INCAR_SYNC_DATA> v_vehicleSyncData;

	// The process of recording can not be paused
	int iPlaybackType = Playback::PLAYBACK_TYPE_NONE;
	char recordTitle[75] = {0};
	uint32_t dwStartTime;
	int iPacketRecorded = 0;
	int iTargetPlayer = INVALID_PLAYER_ID;

	bool StartRecording(int type, char title[75])
	{
		if (iPlaybackType == Playback::PLAYBACK_TYPE_NONE)
		{
			iPlaybackType = type;
			if (title) {
				strncpy(recordTitle, title, sizeof(recordTitle) - 1);
				recordTitle[sizeof(recordTitle) - 1] = '\0';
			} else {
				recordTitle[0] = '\0';
			}
			dwStartTime = GetTickCount();
			iPacketRecorded = 0;
			_logs->Log(LogLevel::INFO, "[RECORDER] Recording initialized: title='%s', type=%s.",
				recordTitle, type == Playback::PLAYBACK_TYPE_DRIVER ? "DRIVER" : "ONFOOT");
			return true;
		}
		return false;
	}

	void AddOnfootPacket(const ONFOOT_SYNC_DATA *data)
	{
		if (iPlaybackType != Playback::PLAYBACK_TYPE_ONFOOT)
			return;
		v_dwTime.push_back(GetTickCount() - dwStartTime);
		v_playerSyncData.push_back(*data);
		iPacketRecorded++;
	}

	void AddVehiclePacket(const INCAR_SYNC_DATA *data)
	{
		if (iPlaybackType != Playback::PLAYBACK_TYPE_DRIVER)
			return;
		v_dwTime.push_back(GetTickCount() - dwStartTime);
		v_vehicleSyncData.push_back(*data);
		iPacketRecorded++;
	}

	void Reset()
	{
		v_dwTime.clear();
		v_playerSyncData.clear();
		v_vehicleSyncData.clear();

		iPlaybackType = Playback::PLAYBACK_TYPE_NONE;
		memset(recordTitle, 0, sizeof(recordTitle));
	}

	bool Finalize(const char *szFile, char *errmsg)
	{
		if (iPlaybackType == Playback::PLAYBACK_TYPE_NONE)
		{
			if (errmsg != nullptr)
				strcpy(errmsg, "Nothing is being recorded");
			return false;
		}

		char filePath[512];
		snprintf(filePath, sizeof(filePath), "%s/%s.rec", Playback::szPlaybackDirectory, szFile);
		QFile recF{filePath};
		if (!recF.open(QIODevice::WriteOnly))
		{
			if (errmsg != nullptr)
				strcpy(errmsg, "Unable to open the file");
			_logs->Log(LogLevel::ERROR, "[RECORDER] Failed to open '%s' for writing.", filePath);
			return false;
		}

		// Write the playback type and file info
		QString recordStr = QString("### type=%1;title=%2;\n").arg(iPlaybackType).arg(recordTitle);
		char __tmp[512];
		QTextStream output(&recF);
		output << recordStr;
		if (output.status() == output.WriteFailed)
		{
			if (errmsg != nullptr)
				strcpy(errmsg, "Unable to write the record's Metadata to the file");
			return false;
		}

		if (iPlaybackType == Playback::PLAYBACK_TYPE_DRIVER)
		{
			for (int i = 0; i < iPacketRecorded; i++)
			{
				recordStr = QString("# type=%1;timestamp=%2;").arg(iPlaybackType).arg(v_dwTime[i]);

				snprintf(__tmp, sizeof(__tmp), "QuaternionW=%.4f;QuaternionX=%.4f;QuaternionY=%.4f;QuaternionZ=%.4f;", v_vehicleSyncData[i].fQuaternion[0], v_vehicleSyncData[i].fQuaternion[1], v_vehicleSyncData[i].fQuaternion[2], v_vehicleSyncData[i].fQuaternion[3]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "CurrentPosX=%.4f;CurrentPosY=%.4f;CurrentPosZ=%.4f;", v_vehicleSyncData[i].vecPos[0], v_vehicleSyncData[i].vecPos[1], v_vehicleSyncData[i].vecPos[2]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "VelocityX=%.4f;VelocityY=%.4f;VelocityZ=%.4f;", v_vehicleSyncData[i].fMoveSpeed[0], v_vehicleSyncData[i].fMoveSpeed[1], v_vehicleSyncData[i].fMoveSpeed[2]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "CurrentWeapon=%d;", v_vehicleSyncData[i].BYTECurrentWeapon);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "SirenOn=%d;", v_vehicleSyncData[i].BYTESirenOn);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "LandingGearState=%d;", v_vehicleSyncData[i].BYTELandingGearState);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "TrainSpeed=%.4f;\n", v_vehicleSyncData[i].fTrainSpeed);
				recordStr.append(__tmp);

				output << recordStr;
				if (output.status() == output.WriteFailed)
				{
					if (errmsg != nullptr)
						strcpy(errmsg, "Unable to write the record's v_vehicleSyncData to the file");
					break;
				}
			}
		}
		else if (iPlaybackType == Playback::PLAYBACK_TYPE_ONFOOT)
		{
			for (int i = 0; i < iPacketRecorded; i++)
			{
				recordStr = QString("# type=%1;timestamp=%2;").arg(iPlaybackType).arg(v_dwTime[i]);

				snprintf(__tmp, sizeof(__tmp), "CurrentWeapon=%d;", v_playerSyncData[i].BYTECurrentWeapon);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "SpecialAction=%d;", v_playerSyncData[i].BYTESpecialAction);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "AnimationIndex=%d;AnimationFlags=%d;", v_playerSyncData[i].AnimationIndex, v_playerSyncData[i].AnimationFlags);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "QuaternionW=%.4f;QuaternionX=%.4f;QuaternionY=%.4f;QuaternionZ=%.4f;", v_playerSyncData[i].fQuaternion[0], v_playerSyncData[i].fQuaternion[1], v_playerSyncData[i].fQuaternion[2], v_playerSyncData[i].fQuaternion[3]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "CurrentPosX=%.4f;CurrentPosY=%.4f;CurrentPosZ=%.4f;", v_playerSyncData[i].fCurrentPosition[0], v_playerSyncData[i].fCurrentPosition[1], v_playerSyncData[i].fCurrentPosition[2]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "CurrentAngle=%.4f;", v_playerSyncData[i].fCurrentAngle);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "VelocityX=%.4f;VelocityY=%.4f;VelocityZ=%.4f;", v_playerSyncData[i].fMoveSpeed[0], v_playerSyncData[i].fMoveSpeed[1], v_playerSyncData[i].fMoveSpeed[2]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "SurfOffsetX=%.4f;SurfOffsetY=%.4f;SurfOffsetZ=%.4f;", v_playerSyncData[i].fSurfOffsets[0], v_playerSyncData[i].fSurfOffsets[1], v_playerSyncData[i].fSurfOffsets[2]);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "CurrentInterior=%d;", v_playerSyncData[i].BYTEInteriorId);
				recordStr.append(__tmp);
				snprintf(__tmp, sizeof(__tmp), "FightingStyle=%d;", v_playerSyncData[i].FightingStyle);
				recordStr.append(__tmp);
				for (int skillID = 0; skillID < MAX_SKILLS; skillID++)
				{
					snprintf(__tmp, sizeof(__tmp), "Skill%dLevel=%d;", skillID, v_playerSyncData[i].SkillLevel[skillID]);
					recordStr.append(__tmp);
				}
				recordStr.append("\n");

				output << recordStr;
				if (output.status() == output.WriteFailed)
				{
					if (errmsg != nullptr)
						strcpy(errmsg, "Unable to write the record's v_playerSyncData to the file");
					break;
				}
			}
		}

		iPlaybackType = Playback::PLAYBACK_TYPE_NONE;

		recF.close();
		_logs->Log(LogLevel::INFO, "[RECORDER] Recording saved to '%s' (%d packets).", filePath, iPacketRecorded);
		return true;
	}
}
