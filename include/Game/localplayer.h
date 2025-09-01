#include "../common.h"
#include "../utils.h"
#include "../xmlsets.h"

void SendOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, int sendDeathNoti, PLAYERID followPlayerID);
void SendInCarFullSyncData(INCAR_SYNC_DATA *picSync, int iUseCarPos, PLAYERID followPlayerID);
void SendPassengerFullSyncData(VEHICLEID vehicleID);
void SendAimSyncData(uint32_t dwAmmoInClip, int iReloading, PLAYERID copyFromPlayer);
void SendUnoccupiedSyncData(UNOCCUPIED_SYNC_DATA *punocSync);
void SendSpectatorData(SPECTATOR_SYNC_DATA *pSpecData);
void SendBulletData(BULLET_SYNC_DATA *pBulletData);

void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger);
void SendExitVehicleNotification(VEHICLEID VehicleID);
void SendWastedNotification(uint8_t uint8_tDeathReason, PLAYERID WhoWasResponsible);
void NotifyVehicleDeath(VEHICLEID VehicleID);
void SendDamageVehicle(uint16_t vehicleID, uint32_t panel, uint32_t door, uint8_t lights, uint8_t tires);

void sendServerCommand(char *szCommand);
void SendScoreboardClick(int playerid);
void sendChat(char *szMessage);
void sendScmEvent(int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3);
void sendDialogResponse(uint16_t wDialogID, uint8_t bButtonID, uint16_t wListBoxItem, const char *szInputResp);
void sendPickUp(int iPickupID);
void selectTextDraw(int iTextDrawID);

void SetToDeathState(PLAYERID murderer, uint8_t reason);
void TakeBulletDamage(PLAYERID attacker, uint8_t weaponid);

void SetAnimation(short sAnimationId, float fDelta, bool bLoop, bool bLockX, bool bLockY, bool bFreeze, int iTime);
// void SetAnimationByName(char *szName, float fDelta, bool bLoop, bool bLockX, bool bLockY, bool bFreeze, int iTime);
// bool LoadAnimationFromConfig(const stConfig *config);
void ResetAnimation();
