#ifndef PLAYERPOS_H
#define PLAYERPOS_H
/*#if !defined strlen
   #include <string.h>
#endif*/

#define MAX_ZONE_NAME (27)
#define MAX_SA_ZONES (365)

struct MainZone
{
	char Zone_Name[MAX_ZONE_NAME];
	double Zone_Area[6];
};

void Get2DZoneByPos(double mX, double mY, const char *&zone);
#endif
