/*
 * GPS_config.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Mostafa Edrees
 */

#ifndef GPS_NEO_7M_GPS_CONFIG_H_
#define GPS_NEO_7M_GPS_CONFIG_H_

#include "GPS_priv.h"

#define TIMEOUT_MS			15000

typedef struct
{
	LOCATION gps_location;
	TIME reading_time;
	DATE reading_date;
}GPS_Data;

#endif /* GPS_NEO_7M_GPS_CONFIG_H_ */
