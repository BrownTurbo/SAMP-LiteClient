#pragma once
#ifndef COMMONDTA_H
#define COMMONDTA_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cstdint>

// raknet stuff
#include "../lib/raknet/PacketEnumerations.h"
#include "../lib/raknet/RakNetworkFactory.h"
#include "../lib/raknet/RakClientInterface.h"
#include "../lib/raknet/NetworkTypes.h"
#include "../lib/raknet/BitStream.h"
#include "../lib/raknet/StringCompressor.h"
#include "../lib/raknet/RakPeerInterface.h"

#define RECONNECT_ON_ERROR 0
#define TOTAL_AUTHKEYS (512 + 879) // 0.3.7-R1~2 = 512 + 0.3.7-R3 = 879

#define NETCODE_CONNCOOKIELULZ 0x6969
#define NETGAME_3DL 4062
#define NETGAME_37 4057

extern unsigned short NETGAME_VERSION;

#include "../lib/raknet/SAMP/samp_netencr.h"
#include "../lib/raknet/SAMP/samp_auth.h"
#include "../lib/raknet/SAMP/SAMPRPC.h"
#include "../lib/raknet/SAMP/SAMP_VER.h"

#include "CVector.h"
#include "math_stuff.h"

#include "ThreadWorker.h"

#include "logs.h"
extern Logs *_logs;

enum eRunModes
{
	RUNMODE_STILL,
	RUNMODE_NORMAL,
	RUNMODE_FOLLOWPLAYER,
	RUNMODE_FOLLOWPLAYERSVEHICLE,
	RUNMODE_PLAYROUTES
};
extern eRunModes g_myRunMode;

enum eSTATE
{
	STATE_VOID,		   // (not connected and haven't initalize the game)
	STATE_NOT_SPAWNED, // (connected, but haven't spawned yet, and the game has been initialized)
	STATE_DEAD,
	STATE_ONFOOT,
	STATE_DRIVER,
	STATE_PASSENGER
};
extern eSTATE g_myState;
eSTATE GetState();
bool IsSpawned();
bool IsDead();
void SetState(eSTATE state);

#define REJECT_REASON_BAD_VERSION 1
#define REJECT_REASON_BAD_NICKNAME 2
#define REJECT_REASON_BAD_MOD 3
#define REJECT_REASON_BAD_PLAYERID 4

#define BP _asm int 3

#define MAX_PLAYERS 1004
#define MAX_VEHICLES 2000
#define MAX_PICKUPS 512
#define MAX_SCRIPTS 32

#define MAX_PLAYER_NAME 32

#define MAX_WEAPON_SLOTS 13
#define MAX_SKILLS 11

#define EVENT_TYPE_PAINTJOB 1
#define EVENT_TYPE_CARCOMPONENT 2
#define EVENT_TYPE_CARCOLOR 3
#define EVENT_ENTEREXIT_MODSHOP 4

#define BULLET_HIT_TYPE_NONE 0
#define BULLET_HIT_TYPE_PLAYER 1
#define BULLET_HIT_TYPE_VEHICLE 2
#define BULLET_HIT_TYPE_OBJECT 3
#define BULLET_HIT_TYPE_PLAYER_OBJECT 4

#define KEY_UP -128
#define KEY_DOWN 128
#define KEY_LEFT -128
#define KEY_RIGHT 128
#define KEY_ACTION 1
#define KEY_CROUCH 2
#define KEY_FIRE 4
#define KEY_SPRINT 8
#define KEY_SECONDARY_ATTACK 16
#define KEY_JUMP 32
#define KEY_LOOK_RIGHT 64
#define KEY_HANDBRAKE 128
#define KEY_LOOK_LEFT 256
#define KEY_SUBMISSION 512
#define KEY_LOOK_BEHIND 512
#define KEY_WALK 1024
#define KEY_ANALOG_UP 2048
#define KEY_ANALOG_DOWN 4096
#define KEY_ANALOG_LEFT 8192
#define KEY_ANALOG_RIGHT 16384

typedef unsigned short PLAYERID;
typedef unsigned short VEHICLEID;
typedef unsigned short LABELID;
typedef unsigned short OBJECTID;
typedef unsigned short PICKUPID;

#pragma pack(1)
typedef struct _VECTOR
{
	float X, Y, Z;
} VECTOR, *PVECTOR;

// ...
#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA
{
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	float fQuaternion[4];
	float fHealth;
	float fArmour;
	uint8_t BYTECurrentWeapon;
	uint8_t BYTESpecialAction;
	float fMoveSpeed[3];
	float fSurfOffsets[3];
	uint16_t wSurfInfo;
	int AnimationIndex;
	int AnimationFlags;
	float fCurrentPosition[3];
	float fCurrentAngle;
	uint8_t BYTEInteriorId;
	uint8_t BYTESkin;
	uint8_t BYTETeam;
	uint8_t FightingStyle;
	uint16_t SkillLevel[MAX_SKILLS];
} ONFOOT_SYNC_DATA;

#pragma pack(1)
typedef struct _INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	float fQuaternion[4];
	float vecPos[3];
	float fMoveSpeed[3];
	float fHealth;
	float fPlayerHealth;
	float fPlayerArmour;
	uint8_t BYTECurrentWeapon;
	uint8_t BYTESirenOn;
	uint8_t BYTELandingGearState;
	uint16_t TrailerID_or_ThrustAngle;
	float fTrainSpeed;
} INCAR_SYNC_DATA;

#pragma pack(1)
typedef struct _PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;
	uint8_t BYTESeatFlags : 7;
	uint8_t BYTEDriveBy : 1;
	uint8_t BYTECurrentWeapon;
	float fPlayerHealth;
	float fPlayerArmour;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	float vecPos[3];
} PASSENGER_SYNC_DATA;

enum eWeaponState
{
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};

#pragma pack(1)
typedef struct _AIM_SYNC_DATA
{
	uint8_t BYTECamMode;
	float vecAimf1[3];
	float vecAimPos[3];
	float fAimZ;
	uint8_t BYTEWeaponState : 2; // see eWeaponState
	uint8_t BYTECamExtZoom : 6;	 // 0-63 normalized
	uint8_t bAspectRatio;
} AIM_SYNC_DATA;

#pragma pack(1)
typedef struct _UNOCCUPIED_SYNC_DATA // 67
{
	VEHICLEID VehicleID;
	short cvecRoll[3];
	short cvecDirection[3];
	uint8_t unk[13]; // FIXME ???
	float vecPos[3];
	float vecMoveSpeed[3];
	float vecTurnSpeed[3]; // FIXME ???
	float fHealth;
} UNOCCUPIED_SYNC_DATA;

#pragma pack(1)
typedef struct _SPECTATOR_SYNC_DATA
{
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	float vecPos[3];
} SPECTATOR_SYNC_DATA;

#pragma pack(1)
typedef struct _BULLET_SYNC_DATA
{
	uint8_t bHitType;
	unsigned short iHitID;
	float fHitOrigin[3];
	float fHitTarget[3];
	float fCenterOfHit[3];
	uint8_t bWeaponID;
} BULLET_SYNC_DATA;

#pragma pack(1)
typedef struct _PLAYER_SPAWN_INFO
{
	uint8_t BYTETeam;
	int iSkin;
	uint8_t unk;
	float vecPos[3];
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;

#pragma pack(1)
typedef struct _TEXT_DRAW_TRANSMIT
{
	union
	{
		uint8_t BYTEFlags;
		struct
		{
			uint8_t BYTEBox : 1;
			uint8_t BYTELeft : 1;
			uint8_t BYTERight : 1;
			uint8_t BYTECenter : 1;
			uint8_t BYTEProportional : 1;
			uint8_t BYTEPadding : 3;
		};
	};
	float fLetterWidth;
	float fLetterHeight;
	uint32_t dwLetterColor;
	float fLineWidth;
	float fLineHeight;
	uint32_t dwBoxColor;
	uint8_t BYTEShadow;
	uint8_t BYTEOutline;
	uint32_t dwBackgroundColor;
	uint8_t BYTEStyle;
	uint8_t BYTESelectable;
	float fX;
	float fY;
	uint16_t wModelID;
	float fRotX;
	float fRotY;
	float fRotZ;
	float fZoom;
	uint16_t wColor1;
	uint16_t wColor2;
} TEXT_DRAW_TRANSMIT;

#pragma pack(1)
typedef struct _NEW_VEHICLE
{
	VEHICLEID VehicleId;
	int iVehicleType;
	float vecPos[3];
	float fRotation;
	uint8_t aColor1;
	uint8_t aColor2;
	float fHealth;
	uint8_t BYTEInterior;
	uint32_t dwDoorDamageStatus;
	uint32_t dwPanelDamageStatus;
	uint8_t BYTELightDamageStatus;
	uint8_t BYTETireDamageStatus;
	uint8_t BYTEAddSiren;
	uint8_t BYTEModSlots[14];
	uint8_t BYTEPaintjob;
	uint32_t cColor1;
	uint32_t cColor2;
	uint8_t BYTEUnk;
} NEW_VEHICLE;

typedef struct _VEHICLE_SYNC_DATA
{
	VEHICLEID VehicleId;
	uint8_t Seat;
} VEHICLE_SYNC_DATA;

typedef struct _WEAPON_SYNC_DATA
{
	uint8_t weaponId;
	uint8_t weaponAmmo;
} WEAPON_SYNC_DATA;

struct PlayerPool
{
	bool bIsConnected;
	bool bIsStreamedIn;
	bool bIsControllable;
	bool bGotMarkersPos;
	int iWeaponSlot;
	char szPlayerName[MAX_PLAYER_NAME];
	int iScore;
	int iWantedLevel;
	uint32_t dwPing;
	int bAreWeInAVehicle;
	uint8_t BYTEIsNPC;

	uint32_t RGBAcolor;
	QString HEXcolor;

	// STORED INFO
	ONFOOT_SYNC_DATA onfootData;
	INCAR_SYNC_DATA incarData;
	PASSENGER_SYNC_DATA passengerData;
	AIM_SYNC_DATA aimData;
	BULLET_SYNC_DATA bulletData;
	VEHICLE_SYNC_DATA vehicleData;
	WEAPON_SYNC_DATA weaponData[MAX_WEAPON_SLOTS];
};

struct stCheckpointData
{
	bool bActive;
	float fPosition[3];
	float fSize;
};

///////////////////////////////////////////
#define INVALID_WEAPON_SLOT -1
#define INVALID_WEAPON_ID -1
#define INVALID_DIALOG_ID -1
#define INVALID_VEHICLE_ID (VEHICLEID) - 1
#define INVALID_PLAYER_ID (PLAYERID) - 1
#define INVALID_ACTOR_ID 0xFFFF
#define INVALID_OBJECT_ID 0xFFF
#define INVALID_PICKUP_ID (PICKUPID) - 1
///////////////////////////////////////////
extern const int8_t weaponSlots[];
///////////////////////////////////////////
#define MAX_MENU_ITEMS 12
#define MAX_MENU_LINE 32
#define MAX_MENU_COLUMNS 2

#define VEHICLE_SEAT_DRIVER 0
#define VEHICLE_SEAT_PASSENGER 1

#define VEHICLE_LIST_ID_START 400
#define VEHICLE_LIST_SIZE 212

#define VEHICLE_CLASS_CAR 0
#define VEHICLE_CLASS_CAR_FAST 1
#define VEHICLE_CLASS_HEAVY 2
#define VEHICLE_CLASS_HELI 3
#define VEHICLE_CLASS_AIRPLANE 4
#define VEHICLE_CLASS_BIKE 5
#define VEHICLE_CLASS_BOAT 6
#define VEHICLE_CLASS_MINI 7
#define VEHICLE_CLASS_TRAILER 8
#define VEHICLE_CLASS_COUNT 9 /* # of classes */
struct vehicle_entry
{
	int id;			  // model id
	int class_id;	  // class id
	const char *name; // vehicle name
	int passengers;	  // total passenger seats, 0-10
};

extern struct vehicle_entry vehicle_list[VEHICLE_LIST_SIZE];

struct VehiclePool
{
	int iExist;
	float fPos[3];
	float fRotation;
	uint8_t BYTEInterior;
	float fHealth;
	int iModelID;
};

struct PickupPool
{
	int iModelID;
	int iType;
	float fPosX;
	float fPosY;
	float fPosZ;
};

extern char szFollowingPlayerName[MAX_PLAYER_NAME];
extern int iFollowingWithVehicleID;
extern float fFollowXOffset;
extern float fFollowYOffset;
extern float fFollowZOffset;

extern int iMoney, iDrunkLevel;
extern bool iGameInited;
extern int iSpawnsAvailable;

#define RECONNECT_TIME 2
extern int iReconnectTime;
extern bool receivedResponse;

enum eGameState
{
	GAMESTATE_WAIT_CONNECT,
	GAMESTATE_CONNECTING,
	GAMESTATE_AWAIT_JOIN,
	GAMESTATE_CONNECTED,
	GAMESTATE_DISCONNECTED,
	GAMESTATE_CONNECTION_REQUESTED,
	GAMESTATE_DISCONNECTION_REQUESTED
};

extern eGameState iGameState;
extern eGameState iOldGameState;
extern uint32_t dwLastConnectAttempt;

eGameState GetGameState();
eGameState GetOldGameState();
void SetGameState(eGameState GameState);

extern PLAYERID g_myPlayerID;
extern int g_myClassID;

extern PLAYER_SPAWN_INFO SpawnInfo;
extern unsigned int uRespawnTick;
extern unsigned int uUpdateTick;
extern uint8_t m_bLagCompensation;

extern int bIsSpectating;

extern struct PlayerPool playerPool[MAX_PLAYERS];
extern struct VehiclePool vehiclePool[MAX_VEHICLES];
extern struct PickupPool pickupPool[MAX_PICKUPS];
extern bool pickupExist[MAX_PICKUPS];

extern bool bInClassSelection;

extern RakClientInterface *pRakClient;

struct weapon_entry
{
	int id;
	int slot;
	int model_id;
	const char *name;
};

extern const struct weapon_entry weapon_list[];

extern struct stCheckpointData CurrentCheckpoint;

// Threading shit
extern ThreadManager *threadManager;

// ??
extern uint8_t g_srvWorldTime;
extern uint8_t g_srvWeather;
#endif
