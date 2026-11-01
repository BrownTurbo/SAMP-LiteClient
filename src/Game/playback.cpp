#include "../../include/Game/playback.h"
#include "../../include/common.h"
#include "../../include/utils.h"

// Thanks to 81Vm3 for the base of the Packets Playback
namespace Playback
{
	const char szPlaybackDirectory[] = "playbacks";
	RECORD_DATA recData[1024];
	ePBType iPlaybackType;
	char recordTitle[128] = {0};

	uint16_t dwStartTime;
	bool bPlaying = false;
	int iCurrentIndex = 0;

	std::vector<uint16_t> v_dwTime;
	std::vector<std::variant<ONFOOT_SYNC_DATA, INCAR_SYNC_DATA>> SyncData;

	bool Load(const char *szFile, char *errmsg)
	{
		if (bPlaying)
		{
			if (errmsg != nullptr)
				strcpy(errmsg, "There is playback that has been load already, please unload it manually");
			return false;
		}

		char fileName[256];
		snprintf(fileName, sizeof(fileName), "%s/%s.rec", szPlaybackDirectory, szFile);

		unsigned short linesCount = 0;
		memset(recData, 0, sizeof(recData));
		QFile recF{fileName};
		if (recF.open(QFile::ReadOnly | QFile::Text))
		{
			while (!recF.atEnd())
			{
				QByteArray line = recF.readLine();
				char const *data = line.constData();
				unsigned short len = line.size();
				unsigned short idx = 0;
				if (idx < len)
				{
					if (strncmp(data + idx, "### ", 4) == 0)
					{
						idx += 4;
						QString __Data = QString::fromUtf8(data + idx, len - idx).trimmed();
						QStringList vars = __Data.split(';', Qt::SkipEmptyParts);
						for (int varIDX = 0; varIDX < vars.size(); varIDX++)
						{
							QStringList var = vars.at(varIDX).split('=');
							if (var.size() < 2) continue;
							QString vName = var.at(0).trimmed();
							QString vValue = var.at(1).trimmed();
							if (vName == "title")
							{
								strncpy(recordTitle, vValue.toUtf8().constData(), sizeof(recordTitle) - 1);
								recordTitle[sizeof(recordTitle) - 1] = '\0';
							}
							else if (vName == "type")
							{
								unsigned short recordType = vValue.toUShort();
								if (recordType == 1 || recordType == Playback::PLAYBACK_TYPE_DRIVER)
									iPlaybackType = Playback::PLAYBACK_TYPE_DRIVER;
								else if (recordType == 2 || recordType == Playback::PLAYBACK_TYPE_ONFOOT)
									iPlaybackType = Playback::PLAYBACK_TYPE_ONFOOT;
							}
						}
					}
					else if (strncmp(data + idx, "# ", 2) == 0)
					{
						if (linesCount >= 1024)
							break;
						linesCount += 1;
						recData[linesCount - 1].type = iPlaybackType;
						idx += 2;
						QString __Data = QString::fromUtf8(data + idx, len - idx).trimmed();
						QStringList vars = __Data.split(';', Qt::SkipEmptyParts);
						for (int varIDX = 0; varIDX < vars.size(); varIDX++)
						{
							QStringList var = vars.at(varIDX).split('=');
							if (var.size() < 2) continue;
							QString vName = var.at(0).trimmed();
							QString vValue = var.at(1).trimmed();
							if (vName == "type")
							{
								unsigned short recordType = vValue.toUShort();
								if (recordType == 1 || recordType == Playback::PLAYBACK_TYPE_DRIVER)
									recData[(linesCount - 1)].type = Playback::PLAYBACK_TYPE_DRIVER;
								else if (recordType == 2 || recordType == Playback::PLAYBACK_TYPE_ONFOOT)
									recData[(linesCount - 1)].type = Playback::PLAYBACK_TYPE_ONFOOT;
								else
									recData[(linesCount - 1)].type = Playback::PLAYBACK_TYPE_NONE;
							}
							else if (vName == "timestamp" || vName == "dwTime")
							{
								recData[(linesCount - 1)].timestamp = vValue.toULong();
							}
							else if (vName == "CurrentPosX")
							{
								recData[(linesCount - 1)].CurrentPosX = vValue.toFloat();
							}
							else if (vName == "CurrentPosY")
							{
								recData[(linesCount - 1)].CurrentPosY = vValue.toFloat();
							}
							else if (vName == "CurrentPosZ")
							{
								recData[(linesCount - 1)].CurrentPosZ = vValue.toFloat();
							}
							else if (vName == "CurrentAngle")
							{
								recData[(linesCount - 1)].CurrentAngle = vValue.toFloat();
							}
							else if (vName == "VelocityX")
							{
								recData[(linesCount - 1)].VelocityX = vValue.toFloat();
							}
							else if (vName == "VelocityY")
							{
								recData[(linesCount - 1)].VelocityY = vValue.toFloat();
							}
							else if (vName == "VelocityZ")
							{
								recData[(linesCount - 1)].VelocityZ = vValue.toFloat();
							}
							else if (vName == "QuaternionW")
							{
								recData[(linesCount - 1)].QuaternionW = vValue.toFloat();
							}
							else if (vName == "QuaternionX")
							{
								recData[(linesCount - 1)].QuaternionX = vValue.toFloat();
							}
							else if (vName == "QuaternionY")
							{
								recData[(linesCount - 1)].QuaternionY = vValue.toFloat();
							}
							else if (vName == "QuaternionZ")
							{
								recData[(linesCount - 1)].QuaternionZ = vValue.toFloat();
							}
							else if (vName == "TrainSpeed")
							{
								recData[(linesCount - 1)].TrainSpeed = vValue.toFloat();
							}
							else if (vName == "CurrentWeapon")
							{
								recData[(linesCount - 1)].CurrentWeapon = (uint8_t)vValue.toUShort();
							}
							else if (vName == "SirenOn")
							{
								recData[(linesCount - 1)].SirenOn = (uint8_t)vValue.toUShort();
							}
							else if (vName == "LandingGearState")
							{
								recData[(linesCount - 1)].LandingGearState = (uint8_t)vValue.toUShort();
							}
						}
					}
				}
			}
		}
		else
		{
			if (errmsg != nullptr)
				strcpy(errmsg, "Unable to open the file");
			_logs->Log(LogLevel::ERROR, "[PLAYBACK] Could not open playback file '%s'.", fileName);
			return false;
		}
		recF.close();

		v_dwTime.clear();
		SyncData.clear();

		for (int lineNum = 0; lineNum < linesCount; lineNum++)
		{
			if (recData[lineNum].type == PLAYBACK_TYPE_DRIVER)
			{
				INCAR_SYNC_DATA vehicleSyncData;
				memset(&vehicleSyncData, 0, sizeof(INCAR_SYNC_DATA));
				vehicleSyncData = playerPool[g_myPlayerID].incarData;
				vehicleSyncData.vecPos[0] = recData[lineNum].CurrentPosX;
				vehicleSyncData.vecPos[1] = recData[lineNum].CurrentPosY;
				vehicleSyncData.vecPos[2] = recData[lineNum].CurrentPosZ;
				vehicleSyncData.fQuaternion[0] = recData[lineNum].QuaternionW;
				vehicleSyncData.fQuaternion[1] = recData[lineNum].QuaternionX;
				vehicleSyncData.fQuaternion[2] = recData[lineNum].QuaternionY;
				vehicleSyncData.fQuaternion[3] = recData[lineNum].QuaternionZ;
				vehicleSyncData.fMoveSpeed[0] = recData[lineNum].VelocityX;
				vehicleSyncData.fMoveSpeed[1] = recData[lineNum].VelocityY;
				vehicleSyncData.fMoveSpeed[2] = recData[lineNum].VelocityZ;
				vehicleSyncData.BYTECurrentWeapon = recData[lineNum].CurrentWeapon;
				vehicleSyncData.BYTESirenOn = recData[lineNum].SirenOn;
				vehicleSyncData.BYTELandingGearState = recData[lineNum].LandingGearState;
				vehicleSyncData.fTrainSpeed = recData[lineNum].TrainSpeed;

				v_dwTime.push_back(recData[lineNum].timestamp);
				SyncData.push_back(vehicleSyncData);
			}
			else if (recData[lineNum].type == PLAYBACK_TYPE_ONFOOT)
			{
				ONFOOT_SYNC_DATA playerSyncData;
				memset(&playerSyncData, 0, sizeof(ONFOOT_SYNC_DATA));
				playerSyncData = playerPool[g_myPlayerID].onfootData;
				playerSyncData.fCurrentPosition[0] = recData[lineNum].CurrentPosX;
				playerSyncData.fCurrentPosition[1] = recData[lineNum].CurrentPosY;
				playerSyncData.fCurrentPosition[2] = recData[lineNum].CurrentPosZ;
				playerSyncData.fQuaternion[0] = recData[lineNum].QuaternionW;
				playerSyncData.fQuaternion[1] = recData[lineNum].QuaternionX;
				playerSyncData.fQuaternion[2] = recData[lineNum].QuaternionY;
				playerSyncData.fQuaternion[3] = recData[lineNum].QuaternionZ;
				playerSyncData.fCurrentAngle = recData[lineNum].CurrentAngle;
				playerSyncData.fMoveSpeed[0] = recData[lineNum].VelocityX;
				playerSyncData.fMoveSpeed[1] = recData[lineNum].VelocityY;
				playerSyncData.fMoveSpeed[2] = recData[lineNum].VelocityZ;
				playerSyncData.BYTECurrentWeapon = recData[lineNum].CurrentWeapon;

				v_dwTime.push_back(recData[lineNum].timestamp);
				SyncData.push_back(playerSyncData);
			}
		}

		iCurrentIndex = 0;
		_logs->Log(LogLevel::INFO, "[PLAYBACK] Loaded '%s' with %d packets (Title: '%s', Type: %s).",
			szFile, linesCount, recordTitle, iPlaybackType == PLAYBACK_TYPE_DRIVER ? "DRIVER" : "ONFOOT");

		return true;
	}

	bool Unload()
	{
		_logs->Log(LogLevel::INFO, "[PLAYBACK] Playback unloaded.");
		v_dwTime.clear();
		SyncData.clear();
		bPlaying = false;
		iCurrentIndex = 0;
		dwStartTime = 0;
		return true;
	}

	void StartFirstEver()
	{
		bPlaying = true;
		dwStartTime = GetTickCount();
		iCurrentIndex = 0;
		_logs->Log(LogLevel::INFO, "[PLAYBACK] Playback started: '%s' (%zu packets).", recordTitle, SyncData.size());
	}

	void Update()
	{
		if (iCurrentIndex >= 0 && iCurrentIndex < static_cast<int>(v_dwTime.size()))
			dwStartTime = (GetTickCount() - v_dwTime[iCurrentIndex]);
	}

	void Process()
	{
		if (!bPlaying || SyncData.empty())
			return;

		if (iCurrentIndex == 0 && dwStartTime == 0)
			StartFirstEver();

		size_t packetIdx = 0;
		std::ranges::for_each(SyncData, [&packetIdx](const auto &CurrentValue) {
			if (packetIdx != static_cast<size_t>(iCurrentIndex))
			{
				packetIdx++;
				return;
			}

			if (iCurrentIndex == 0 && dwStartTime == 0)
				StartFirstEver();

			// Check the time
			if ((GetTickCount() - dwStartTime) >= v_dwTime[iCurrentIndex])
			{
				// Read the first recording data
				if (recData[iCurrentIndex].type == PLAYBACK_TYPE_DRIVER)
				{
					// Get the vehicle interface
					if (!IsVehicleIndexValid(playerPool[g_myPlayerID].vehicleData.VehicleId))
					{
						ONFOOT_SYNC_DATA data = playerPool[g_myPlayerID].onfootData;

						RakNet::BitStream bs;
						bs.Write((uint8_t)ID_PLAYER_SYNC);
						bs.Write((char *)&data, sizeof(ONFOOT_SYNC_DATA));
						pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

						Update();
						iCurrentIndex++;
						packetIdx++;
						return;
					}

					// Get the in car sync data
					INCAR_SYNC_DATA data = std::get<INCAR_SYNC_DATA>(CurrentValue);

					memcpy(playerPool[g_myPlayerID].onfootData.fCurrentPosition, data.vecPos, (sizeof(float)) * 3);
					memcpy(vehiclePool[playerPool[g_myPlayerID].vehicleData.VehicleId].fPos, data.vecPos, (sizeof(float)) * 3);

					// Apply the sync data
					data.VehicleID = playerPool[g_myPlayerID].vehicleData.VehicleId;
					data.fPlayerHealth = (uint8_t)playerPool[g_myPlayerID].onfootData.fHealth;
					data.fPlayerArmour = (uint8_t)playerPool[g_myPlayerID].onfootData.fArmour;

					RakNet::BitStream bs;
					bs.Write((uint8_t)ID_VEHICLE_SYNC);
					bs.Write((char *)&data, sizeof(INCAR_SYNC_DATA));
					pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

					SetState(STATE_DRIVER);
					iCurrentIndex++;
				}
				else if (recData[iCurrentIndex].type == PLAYBACK_TYPE_ONFOOT)
				{
					// Get the on foot sync data
					ONFOOT_SYNC_DATA data = std::get<ONFOOT_SYNC_DATA>(CurrentValue);

					memcpy(playerPool[g_myPlayerID].onfootData.fCurrentPosition, data.fCurrentPosition, (sizeof(float)) * 3);

					data.fHealth = (uint8_t)playerPool[g_myPlayerID].onfootData.fHealth;
					data.fArmour = (uint8_t)playerPool[g_myPlayerID].onfootData.fArmour;

					RakNet::BitStream bs;
					bs.Write((uint8_t)ID_PLAYER_SYNC);
					bs.Write((char *)&data, sizeof(ONFOOT_SYNC_DATA));
					pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

					playerPool[g_myPlayerID].vehicleData.VehicleId = (VEHICLEID)-1;
					SetState(STATE_ONFOOT);
					iCurrentIndex++;
				}
			}
			packetIdx++;
		});

		if (iCurrentIndex >= static_cast<int>(SyncData.size()))
		{
			_logs->Log(LogLevel::INFO, "[PLAYBACK] Playback '%s' finished successfully (%zu packets).", recordTitle, SyncData.size());
			bPlaying = false;
			iCurrentIndex = 0;
		}
	}

	int GetTotalPacket()
	{
		return (int)v_dwTime.size();
	}

	int GetCurrentIndex()
	{
		return iCurrentIndex;
	}

	float GetProgessPercent()
	{
		if (v_dwTime.size() == 0)
			return 0.f;
		return (float)iCurrentIndex / (float)v_dwTime.size();
	}
}
