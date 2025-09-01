#include "../../include/Game/playback.h"
#include "../../include/common.h"
#include "../../include/utils.h"

// Thanks to 81Vm3 for the base of the Packets Playback
namespace Playback
{
	const char szPlaybackDirectory[] = "playbacks";
	RECORD_DATA recData[1024];
	ePBType iPlaybackType;
	char *recordTitle;

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

		char fileName[128];
		sprintf(fileName, "%s\\%s.rec", szPlaybackDirectory, szFile);

		unsigned short linesCount = 0;
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
						QString __Data = QString::fromStdString(std::string(data + idx, (size_t)len - idx + 1));
						QStringList vars = __Data.split(';');
						for (int varIDX = 0; varIDX < vars.size(); varIDX++)
						{
							QStringList var = vars.at(varIDX).split('=');
							QString vName = var.at(0);
							QString vValue = var.at(1);
							if (strcmp(vName.toUtf8().data(), "title"))
							{
								recordTitle = vValue.toUtf8().data();
							}
						}
					}
					else if (strncmp(data + idx, "# ", 2) == 0)
					{
						linesCount += 1;
						idx += 2;
						QString __Data = QString::fromStdString(std::string(data + idx, (size_t)len - idx + 1));
						QStringList vars = __Data.split(';');
						for (int varIDX = 0; varIDX < vars.size(); varIDX++)
						{
							QStringList var = vars.at(varIDX).split('=');
							QString vName = var.at(0);
							QString vValue = var.at(1);
							if (vName.toUtf8().data() == "type")
							{
								unsigned short recordType = vValue.toUShort();
								switch (recordType)
								{
									case 0:
									{
										recData[(linesCount - 1)].type = Playback::PLAYBACK_TYPE_NONE;
										break;
									}
									case 1:
									{
										recData[(linesCount - 1)].type = Playback::PLAYBACK_TYPE_ONFOOT;
										break;
									}
									case 2:
									{
										recData[(linesCount - 1)].type = Playback::PLAYBACK_TYPE_DRIVER;
										break;
									}
								}
							}
							else if (vName.toUtf8().data() == "timestamp")
							{
								recData[(linesCount - 1)].timestamp = vValue.toUShort();
							}
							else if (vName.toUtf8().data() == "CurrentPosX")
							{
								recData[(linesCount - 1)].CurrentPosX = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "CurrentPosY")
							{
								recData[(linesCount - 1)].CurrentPosY = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "CurrentPosZ")
							{
								recData[(linesCount - 1)].CurrentPosZ = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "CurrentAngle")
							{
								recData[(linesCount - 1)].CurrentAngle = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "VelocityX")
							{
								recData[(linesCount - 1)].VelocityX = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "VelocityY")
							{
								recData[(linesCount - 1)].VelocityY = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "VelocityZ")
							{
								recData[(linesCount - 1)].VelocityZ = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "QuaternionW")
							{
								recData[(linesCount - 1)].QuaternionW = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "QuaternionX")
							{
								recData[(linesCount - 1)].QuaternionX = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "QuaternionY")
							{
								recData[(linesCount - 1)].QuaternionY = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "QuaternionZ")
							{
								recData[(linesCount - 1)].QuaternionZ = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "TrainSpeed")
							{
								recData[(linesCount - 1)].TrainSpeed = vValue.toFloat();
							}
							else if (vName.toUtf8().data() == "CurrentWeapon")
							{
								recData[(linesCount - 1)].CurrentWeapon = vValue.toUShort();
							}
							else if (vName.toUtf8().data() == "SirenOn")
							{
								recData[(linesCount - 1)].SirenOn = vValue.toUShort();
							}
							else if (vName.toUtf8().data() == "LandingGearState")
							{
								recData[(linesCount - 1)].LandingGearState = vValue.toUShort();
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
			return false;
		}
		recF.close();

		for (int lineNum = 0; lineNum < 1024; lineNum++)
		{
			if (recData[lineNum].type == PLAYBACK_TYPE_DRIVER)
			{
				INCAR_SYNC_DATA vehicleSyncData;
				memset(&vehicleSyncData, 0, sizeof(INCAR_SYNC_DATA));
				vehicleSyncData = playerPool[g_myPlayerID].incarData;
				vehicleSyncData.vecPos[0] = recData[lineNum].CurrentPosX;
				vehicleSyncData.vecPos[1] = recData[lineNum].CurrentPosY;
				vehicleSyncData.vecPos[2] = recData[lineNum].CurrentPosZ;

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

				v_dwTime.push_back(recData[lineNum].timestamp);
				SyncData.push_back(playerSyncData);
			}
		}

		iCurrentIndex = 0;

		return true;
	}

	bool Unload()
	{
		if (bPlaying)
		{
			v_dwTime.clear();
			SyncData.clear();

			bPlaying = false;
			return true;
		}
		return false;
	}

	void StartFirstEver()
	{
		bPlaying = true;
		dwStartTime = GetTickCount();
	}

	void Update()
	{
		dwStartTime = (GetTickCount() - v_dwTime[iCurrentIndex]);
	}

	void Process()
	{
		if(!bPlaying)
			return;

		size_t iCurrentIndex = 0;
		std::ranges::for_each(SyncData, [&iCurrentIndex](auto CurrentValue) {
			if(iCurrentIndex == 0)
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
						ONFOOT_SYNC_DATA data;
						memset(&data, 0, sizeof(ONFOOT_SYNC_DATA));

						data.fHealth = (uint8_t)playerPool[g_myPlayerID].onfootData.fHealth;
						data.fArmour = (uint8_t)playerPool[g_myPlayerID].onfootData.fArmour;
						memcpy(data.fCurrentPosition, playerPool[g_myPlayerID].onfootData.fCurrentPosition, (sizeof playerPool[g_myPlayerID].onfootData.fCurrentPosition[0]) * 3);

						data.AnimationIndex = playerPool[g_myPlayerID].onfootData.AnimationIndex;
						data.AnimationFlags = playerPool[g_myPlayerID].onfootData.AnimationFlags;

						RakNet::BitStream bs;
						bs.Write((uint8_t)ID_PLAYER_SYNC);
						bs.Write((char *)&data, sizeof(ONFOOT_SYNC_DATA));
						pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

						Update();
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

					playerPool[g_myPlayerID].vehicleData.VehicleId = -1;
					SetState(STATE_ONFOOT);
				}
			}
		});
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
