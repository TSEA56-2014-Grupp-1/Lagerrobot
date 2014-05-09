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
	
	const uint16_t DISTANCE_SENSOR_2[25] = {
		542, 462, 398, 370, 322, 288, 267, 246, 232, 210,
		196, 185, 177, 165, 157, 149, 140, 136, 128, 124,
		120, 116, 107, 104, 103};
		
	const uint16_t DISTANCE_SENSOR_3[26] = {
		437, 390, 334, 297, 264, 236, 217, 204, 189, 173,
		161, 154, 142, 134, 126, 122, 115, 111, 103, 99,
		95, 92, 87, 84, 83, 79};
	
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
	for(uint8_t i = 1; i <= 24; ++i)	{
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