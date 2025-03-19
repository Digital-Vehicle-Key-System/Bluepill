/*
 * GPS_int.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Mostafa Edrees
 */

#ifndef GPS_NEO_7M_GPS_INT_H_
#define GPS_NEO_7M_GPS_INT_H_

int decodeGGA (char *GGAbuffer, GGASTRUCT *gga);

int decodeRMC (char *RMCbuffer, RMCSTRUCT *rmc);

void GPS_voidInit();

void GPS_voidReading();

void GPS_voidGetReading();

#endif /* GPS_NEO_7M_GPS_INT_H_ */
