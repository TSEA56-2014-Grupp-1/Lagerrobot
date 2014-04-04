/*
 * inverse_kinematics.c
 *
 * Created: 4/4/2014 2:00:17 PM
 *  Author: johli887
 */ 


#include <avr/io.h>
#include "inverse_kinematics.h"
#include <math.h>

coordinate calculate_coordinate(int theta1, int theta2, int theta3)
{
	coordinate temp;
	temp.x = LENGTH_OF_ARM_1*cos(theta1) + LENGTH_OF_ARM_2*cos(theta2) + LENGTH_OF_ARM_3*cos(theta3);
	temp.y = LENGTH_OF_ARM_1*sin(theta1) + LENGTH_OF_ARM_2*sin(theta2) + LENGTH_OF_ARM_3*sin(theta3);
	return temp;
}


int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}