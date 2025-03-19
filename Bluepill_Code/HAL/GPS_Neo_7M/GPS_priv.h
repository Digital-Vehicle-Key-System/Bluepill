/*
 * GPS_priv.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Mostafa Edrees
 */

#ifndef GPS_NEO_7M_GPS_PRIV_H_
#define GPS_NEO_7M_GPS_PRIV_H_

typedef struct {
	int hour;
	int min;
	int sec;
}TIME;

typedef struct {
	float latitude;
	char NS;
	float longitude;
	char EW;
}LOCATION;

typedef struct {
	float altitude;
	char unit;
}ALTITUDE;

typedef struct {
	int Day;
	int Mon;
	int Yr;
}DATE;

typedef struct {
	LOCATION location;
	TIME tim;
	int isfixValid;
	ALTITUDE alt;
	int numofsat;
}GGASTRUCT;

typedef struct {
	DATE date;
	float speed;
	float course;
	int isValid;
}RMCSTRUCT;

typedef struct {
	GGASTRUCT ggastruct;
	RMCSTRUCT rmcstruct;
}GPSSTRUCT;

typedef enum
{
	GPS_READINGS_NOT_NOW,
	GPS_READINGS_NOW
}GPS_Request_t;

static int decodeGGA (char *GGAbuffer, GGASTRUCT *gga);

static int decodeRMC (char *RMCbuffer, RMCSTRUCT *rmc);


#endif /* GPS_NEO_7M_GPS_PRIV_H_ */
