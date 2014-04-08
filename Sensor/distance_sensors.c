/*
 * distance_sensors.c
 *
 * Created: 4/5/2014 3:49:46 PM
 *  Author: phini326
 */ 
#include "distance_sensors.h"
#include <avr/io.h>
#include <avr/interrupt.h>



uint16_t get_distance_sensor_1(int8_t index)	{
	const uint16_t DISTANCE_SENSOR_1[31] = {
		610, 610, 598, 547, 495, 456, 416, 388, 359, 338,
		319, 301, 286, 271, 260, 249, 240, 229, 221, 212, 204,
	195, 187, 183, 175, 171, 167, 160, 156, 153, 149};
	if(index < 0)
	return DISTANCE_SENSOR_1[sizeof(DISTANCE_SENSOR_1) / sizeof(uint16_t) - index];
	else
	return DISTANCE_SENSOR_1[index];
}

uint16_t get_distance_sensor_2(int8_t index)	{
	const uint16_t DISTANCE_SENSOR_2[26] = {
		457, 399, 349, 313, 283, 264, 238, 219, 205, 194,
		185, 170, 165, 152, 144, 140, 132, 128, 120, 116,
	115, 107, 103, 100, 96, 92};
	if(index < 0)
	return DISTANCE_SENSOR_2[sizeof(DISTANCE_SENSOR_2) / sizeof(uint16_t) - index];
	else
	return DISTANCE_SENSOR_2[index];
}

uint16_t get_distance_sensor_3(int8_t index)	{
	const uint16_t DISTANCE_SENSOR_3[26] =
	{437, 390, 334, 297, 264, 236, 217, 204, 189, 173,
		161, 154, 142, 134, 126, 122, 115, 111, 103, 99,
	95, 92, 87, 84, 83, 79};
	if(index < 0)
	return DISTANCE_SENSOR_3[sizeof(DISTANCE_SENSOR_3) / sizeof(uint16_t) - index];
	else
	return DISTANCE_SENSOR_3[index];
}



//takes a converted value and the sensor index that it came from, returns the distance in [mm]
uint16_t ad_interpolate(uint16_t ad_in, uint8_t sensor)	{
	double ad_in_internal = ad_in;
	double distance_1_internal;
	double distance_2_internal;
	switch(sensor)	{
		case(1):	{
			for(uint8_t i = 1; i<=26;i++)	{
				if(ad_in==get_distance_sensor_1(i))
					return (5+i)*10;
				else if((ad_in > get_distance_sensor_1(i)) && (ad_in < get_distance_sensor_1(i-1)))	{
					distance_1_internal = get_distance_sensor_1(i-1);
					distance_2_internal = get_distance_sensor_1(i);
					return 50 + (i*10) - 10*(ad_in_internal-distance_2_internal)/(distance_1_internal - distance_2_internal);
				}
				else if (ad_in < get_distance_sensor_1(-1))
					return 300;
				else if(ad_in > get_distance_sensor_1(0))
					return 50;
			}
			
		break;
		}
		case(2):	{
			for(uint8_t i = 1; i<=26;i++)	{
				if(ad_in==get_distance_sensor_2(i))
					return (5+i)*10;
				else if((ad_in > get_distance_sensor_2(i)) && (ad_in < get_distance_sensor_2(i-1)))	{
					distance_1_internal = get_distance_sensor_2(i-1);
					distance_2_internal = get_distance_sensor_2(i);
					return 50 + (i*10) - 10*(ad_in_internal-distance_2_internal)/(distance_1_internal - distance_2_internal);
				}
				else if(ad_in < get_distance_sensor_2(-1))
					return 300;
				else if(ad_in > get_distance_sensor_2(0))
					return 50;
			}
		break;
		}
		case(3):	{
			for(uint8_t i = 1; i<=26;i++)	{
				if(ad_in==get_distance_sensor_3(i))
				return (5+i)*10;
				else if((ad_in > get_distance_sensor_3(i)) && (ad_in < get_distance_sensor_3(i-1)))	{
					distance_1_internal = get_distance_sensor_3(i-1);
					distance_2_internal = get_distance_sensor_3(i);
					return 50 + (i*10) - 10*(ad_in_internal-distance_2_internal)/(distance_1_internal - distance_2_internal);
				}
				else if(ad_in < get_distance_sensor_3(-1))
					return 300;
				else if(ad_in > get_distance_sensor_3(0))
					return 50;
			}
			
		break;
		}
	}
return ad_in;
}

