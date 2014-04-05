/*
 * distance_sensors.c
 *
 * Created: 4/5/2014 3:49:46 PM
 *  Author: phini326
 */ 
#include "distance_sensors.h"
#include <avr/io.h>
#include <avr/interrupt.h>


//OBS, reeeeally not finished, don't judge...
uint16_t interpolate(uint8_t x1, uint8_t x2, uint16_t y1,uint16_t y2)	{
	uint16_t interpolated_value;
	interpolated_value = y1 + (y1-y1)*(x2-x1);
	return interpolated_value;
}

uint16_t ad_compare(uint16_t ad_in, uint8_t sensor)	{
	switch(sensor)
		case(1):
		for(uint8_t i = 0; i<=31;i++)	{
			if(ad_in==DISTANCE_SENSOR_1[i])
				return DISTANCE_SENSOR_1[i];
			else if((ad_in > DISTANCE_SENSOR_1[i]) && (ad_in < DISTANCE_SENSOR_1[i+1]))
				return DISTANCE_SENSOR_1[i];
				
		}
		break;
		case(2):
		
		break;
		
		case(3):
		
		break;
			
}