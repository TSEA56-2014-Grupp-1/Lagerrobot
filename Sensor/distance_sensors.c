/*
 * distance_sensors.c
 *
 * Created: 4/5/2014 3:49:46 PM
 *  Author: phini326
 */ 
#include "distance_sensors.h"

uint16_t get_distance_sensor(int8_t index, sensor sensor_id)	{
	
	const uint16_t DISTANCE_SENSOR_1[31] = {
		610, 610, 598, 547, 495, 456, 416, 388, 359, 338,
		319, 301, 286, 271, 260, 249, 240, 229, 221, 212, 204,
		195, 187, 183, 175, 171, 167, 160, 156, 153, 149};
	
	const uint16_t DISTANCE_SENSOR_2[26] = {
		490, 406, 365, 322, 290, 265, 240, 224, 210, 193,
		182, 170, 158, 141, 134, 123, 112, 109, 97, 89,
		80, 75, 71, 63, 59, 55};
		
	const uint16_t DISTANCE_SENSOR_3[26] = {
		473, 395, 343, 303, 275, 247, 225, 210, 193, 177,
		161, 145, 133, 121, 106, 95, 84, 75, 72, 63, 55,
		51, 44, 40, 38, 37};
	
	switch(sensor_id) {
		case(sensor_arm): 		
			if(index < 0) {
				return DISTANCE_SENSOR_1[sizeof(DISTANCE_SENSOR_1) / sizeof(uint16_t) + index];
			}
			else {
				return DISTANCE_SENSOR_1[index];
			}
			break;

		case(sensor_left):
			if(index < 0) {
				return DISTANCE_SENSOR_2[sizeof(DISTANCE_SENSOR_2) / sizeof(uint16_t) + index];
			}
			else {
				return DISTANCE_SENSOR_2[index];
			}
			break;
			
		case(sensor_right):
			if(index < 0) {
				return DISTANCE_SENSOR_3[sizeof(DISTANCE_SENSOR_3) / sizeof(uint16_t) + index];
			}
			else {
				return DISTANCE_SENSOR_3[index];
			}
			break;
			
		default:
			return 0;
		}
}


//takes a converted value and the sensor index that it came from, returns the distance in [mm]
uint16_t ad_interpolate(uint16_t ad_in, sensor sensor_id)	{
	
	if (ad_in < get_distance_sensor(-1, sensor_id)) {
		return 300;
	}
	else if (ad_in > get_distance_sensor(0, sensor_id)) {
		return 50;
	}
	
	//Distance is in range, interpolate:
	for(uint8_t i = 1; i <= 25; ++i)	{
		double distance_1_internal = get_distance_sensor(i - 1, sensor_id);
		double distance_2_internal = get_distance_sensor(i, sensor_id);
		
		if (ad_in == distance_2_internal) {
			return (5 + i)*10;
		}
		else if ((ad_in > distance_2_internal) && (ad_in < distance_1_internal))	{
			return 50 + (i*10) - 10*(ad_in - distance_2_internal)/(distance_1_internal - distance_2_internal);
		}
	}
	return ad_in;
}