/*
 * GPS_int.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Mostafa Edrees
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../Services/UART_Ring_Buffer/uartRingBuffer.h"

#include "GPS_priv.h"
#include "GPS_config.h"


GPSSTRUCT GPS_LiveReadings;
GPS_Data GPS_LastFixReadings;
GPS_Request_t GPS_Req = GPS_READINGS_NOT_NOW;

static char GGA_Buffer[100];
static char RMC_Buffer[100];

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_15)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
		GPS_Req = GPS_READINGS_NOW;
	}
}

void GPS_voidInit()
{
	Ringbuf_init();

	HAL_Delay(500);
}

void GPS_voidReading()
{
	if(Wait_for("GGA") == 1)
	{
		Copy_upto("*", GGA_Buffer);

		decodeGGA(GGA_Buffer, &GPS_LiveReadings.ggastruct);
	}

	if(Wait_for("RMC") == 1)
	{
		Copy_upto("*", RMC_Buffer);

		decodeRMC(RMC_Buffer, &GPS_LiveReadings.rmcstruct);
	}

	if(GPS_LiveReadings.ggastruct.isfixValid && GPS_LiveReadings.rmcstruct.isValid)
	{
		GPS_LastFixReadings.gps_location = GPS_LiveReadings.ggastruct.location;
		GPS_LastFixReadings.reading_time = GPS_LiveReadings.ggastruct.tim;
		GPS_LastFixReadings.reading_date = GPS_LiveReadings.rmcstruct.date;
	}
}

void GPS_voidGetReading(GPS_Data *Copy_PstrGPS_CurrentReadings)
{
	if(GPS_Req == GPS_READINGS_NOW)
	{
		uint32_t Start_Time = HAL_GetTick();

		while(!GPS_LiveReadings.ggastruct.isfixValid && !GPS_LiveReadings.rmcstruct.isValid)
		{
			GPS_voidReading();

			if(HAL_GetTick() - Start_Time >= TIMEOUT_MS)
			{
				Copy_PstrGPS_CurrentReadings->gps_location = GPS_LastFixReadings.gps_location;
				Copy_PstrGPS_CurrentReadings->reading_time = GPS_LastFixReadings.reading_time;
				Copy_PstrGPS_CurrentReadings->reading_date = GPS_LastFixReadings.reading_date;
				break;
			}
		}

		if(HAL_GetTick() - Start_Time < TIMEOUT_MS)
		{
			Copy_PstrGPS_CurrentReadings->gps_location = GPS_LiveReadings.ggastruct.location;
			Copy_PstrGPS_CurrentReadings->reading_time = GPS_LiveReadings.ggastruct.tim;
			Copy_PstrGPS_CurrentReadings->reading_date = GPS_LiveReadings.rmcstruct.date;
		}

		GPS_Req = GPS_READINGS_NOT_NOW;
	}
	else
	{
		GPS_voidReading();
	}
}


//------------------------------------------------------------------------------------------
//										Decode NMEA
//------------------------------------------------------------------------------------------

/* Egypt GMT+2 */
int GMT = +200;


int inx = 0;
int hr=0,min=0,day=0,mon=0,yr=0;
int daychange = 0;

/* Decodes the GGA Data
   @GGAbuffer is the buffer which stores the GGA Data
   @GGASTRUCT is the pointer to the GGA Structure (in the GPS Structure)
   @Returns 0 on success
   @ returns 1, 2 depending on where the return statement is excuted, check function for more details
 */

static int decodeGGA (char *GGAbuffer, GGASTRUCT *gga)
{
	inx = 0;
	char buffer[12];
	int i = 0;
	while (GGAbuffer[inx] != ',') inx++;  // 1st ','
	inx++;
	while (GGAbuffer[inx] != ',') inx++;  // After time ','
	inx++;
	while (GGAbuffer[inx] != ',') inx++;  // after latitude ','
	inx++;
	while (GGAbuffer[inx] != ',') inx++;  // after NS ','
	inx++;
	while (GGAbuffer[inx] != ',') inx++;  // after longitude ','
	inx++;
	while (GGAbuffer[inx] != ',') inx++;  // after EW ','
	inx++;  // reached the character to identify the fix
	if ((GGAbuffer[inx] == '1') || (GGAbuffer[inx] == '2') || (GGAbuffer[inx] == '6'))   // 0 indicates no fix yet
	{
		gga->isfixValid = 1;   // fix available
		inx = 0;   // reset the index. We will start from the inx=0 and extract information now
	}
	else
	{
		gga->isfixValid = 0;   // If the fix is not available
		return 1;  // return error
	}
	while (GGAbuffer[inx] != ',') inx++;  // 1st ','


	/*********************** Get TIME ***************************/
	//(Update the GMT Offset at the top of this file)

	inx++;   // reach the first number in time
	memset(buffer, '\0', 12);
	i=0;
	while (GGAbuffer[inx] != ',')  // copy upto the we reach the after time ','
	{
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}

	hr = (atoi(buffer)/10000) + GMT/100;   // get the hours from the 6 digit number

	min = ((atoi(buffer)/100)%100) + GMT%100;  // get the minutes from the 6 digit number

	// adjust time.. This part still needs to be tested
	if (min > 59)
	{
		min = min-60;
		hr++;
	}
	if (hr<0)
	{
		hr=24+hr;
		daychange--;
	}
	if (hr>=24)
	{
		hr=hr-24;
		daychange++;
	}

	// Store the time in the GGA structure
	gga->tim.hour = hr;
	gga->tim.min = min;
	gga->tim.sec = atoi(buffer)%100;

	/***************** Get LATITUDE  **********************/
	inx++;   // Reach the first number in the lattitude
	memset(buffer, '\0', 12);
	i=0;
	while (GGAbuffer[inx] != ',')   // copy upto the we reach the after lattitude ','
	{
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	if (strlen(buffer) < 6) return 2;  // If the buffer length is not appropriate, return error
	char lat_deg_str[3] = {0}; // Degrees part (2 digits for latitude)
	char lat_min_str[8] = {0}; // Minutes part (up to 7 digits for latitude)
	strncpy(lat_deg_str, buffer, 2); // Extract degrees
	strncpy(lat_min_str, buffer + 2, 7); // Extract minutes
	int lat_deg = atoi(lat_deg_str); // Convert degrees to integer
	double lat_min = atof(lat_min_str); // Convert minutes to double
	float lat = lat_deg + (lat_min / 60.0); // Calculate decimal latitude
	gga->location.latitude = lat;  // save the lattitude data into the strucure
	inx++;
	gga->location.NS = GGAbuffer[inx];  // save the N/S into the structure


	/***********************  GET LONGITUDE **********************/
	inx++;  // ',' after NS character
	inx++;  // Reach the first number in the longitude
	memset(buffer, '\0', 12);
	i=0;
	while (GGAbuffer[inx] != ',')  // copy upto the we reach the after longitude ','
	{
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	char lon_deg_str[4] = {0}; // Degrees part (3 digits for longitude)
	char lon_min_str[8] = {0}; // Minutes part (up to 7 digits for longitude)
	strncpy(lon_deg_str, buffer, 3); // Extract degrees
	strncpy(lon_min_str, buffer + 3, 7); // Extract minutes
	int lon_deg = atoi(lon_deg_str); // Convert degrees to integer
	double lon_min = atof(lon_min_str); // Convert minutes to double
	float lon = lon_deg + (lon_min / 60.0); // Calculate decimal longitude
	gga->location.longitude = lon;  // save the longitude data into the strucure
	inx++;
	gga->location.EW = GGAbuffer[inx];  // save the E/W into the structure

	/**************************************************/
	// skip positition fix
	inx++;   // ',' after E/W
	inx++;   // position fix
	inx++;   // ',' after position fix;

	// number of sattelites
	inx++;  // Reach the first number in the satellites
	memset(buffer, '\0', 12);
	i=0;
	while (GGAbuffer[inx] != ',')  // copy upto the ',' after number of satellites
	{
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	gga->numofsat = atoi(buffer);   // convert the buffer to number and save into the structure


	/***************** skip HDOP  *********************/
	inx++;
	while (GGAbuffer[inx] != ',') inx++;


	/*************** Altitude calculation ********************/
	inx++;
	memset(buffer, '\0', 12);
	i=0;
	while (GGAbuffer[inx] != ',')
	{
		buffer[i] = GGAbuffer[inx];
		i++;
		inx++;
	}
	int num = (atoi(buffer));
	int j = 0;
	while (buffer[j] != '.') j++;
	j++;
	int declen = (strlen(buffer))-j;
	int dec = atoi ((char *) buffer+j);
	lat = (num) + (dec/pow(10, (declen)));
	gga->alt.altitude = lat;

	inx++;
	gga->alt.unit = GGAbuffer[inx];

	return 0;

}


static int decodeRMC (char *RMCbuffer, RMCSTRUCT *rmc)
{
	inx = 0;
	char buffer[12];
	int i = 0;
	while (RMCbuffer[inx] != ',') inx++;  // 1st ,
	inx++;
	while (RMCbuffer[inx] != ',') inx++;  // After time ,
	inx++;
	if (RMCbuffer[inx] == 'A')  // Here 'A' Indicates the data is valid, and 'V' indicates invalid data
	{
		rmc->isValid = 1;
	}
	else
	{
		rmc->isValid =0;
		return 1;
	}
	inx++;
	inx++;
	while (RMCbuffer[inx] != ',') inx++;  // after latitude,
	inx++;
	while (RMCbuffer[inx] != ',') inx++;  // after NS ,
	inx++;
	while (RMCbuffer[inx] != ',') inx++;  // after longitude ,
	inx++;
	while (RMCbuffer[inx] != ',') inx++;  // after EW ,

	// Get Speed
	inx++;
	i=0;
	memset(buffer, '\0', 12);
	while (RMCbuffer[inx] != ',')
	{
		buffer[i] = RMCbuffer[inx];
		i++;
		inx++;
	}

	if (strlen (buffer) > 0){          // if the speed have some data
		int16_t num = (atoi(buffer));  // convert the data into the number
		int j = 0;
		while (buffer[j] != '.') j++;   // same as above
		j++;
		int declen = (strlen(buffer))-j;
		int dec = atoi ((char *) buffer+j);
		float lat = num + (dec/pow(10, (declen)));
		rmc->speed = lat;
	}
	else rmc->speed = 0;

	// Get Course
	inx++;
	i=0;
	memset(buffer, '\0', 12);
	while (RMCbuffer[inx] != ',')
	{
		buffer[i] = RMCbuffer[inx];
		i++;
		inx++;
	}

	if (strlen (buffer) > 0){  // if the course have some data
		int16_t num = (atoi(buffer));   // convert the course data into the number
		int j = 0;
		while (buffer[j] != '.') j++;   // same as above
		j++;
		int declen = (strlen(buffer))-j;
		int dec = atoi ((char *) buffer+j);
		float lat = num + (dec/pow(10, (declen)));
		rmc->course = lat;
	}
	else
	{
		rmc->course = 0;
	}

	// Get Date
	inx++;
	i=0;
	memset(buffer, '\0', 12);
	while (RMCbuffer[inx] != ',')
	{
		buffer[i] = RMCbuffer[inx];
		i++;
		inx++;
	}

	// Date in the format 280222
	day = atoi(buffer)/10000;  // extract 28
	mon = (atoi(buffer)/100)%100;  // extract 02
	yr = atoi(buffer)%100;  // extract 22

	day = day+daychange;   // correction due to GMT shift

	// save the data into the structure
	rmc->date.Day = day;
	rmc->date.Mon = mon;
	rmc->date.Yr = yr;

	return 0;
}



