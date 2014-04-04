/*
 * inverse_kinematics.h
 *
 * Created: 4/4/2014 2:01:54 PM
 *  Author: johli887
 */ 


#ifndef INVERSE_KINEMATICS_H_
#define INVERSE_KINEMATICS_H_

#define LENGTH_OF_ARM_1 18
#define LENGTH_OF_ARM_2 17
#define LENGTH_OF_ARM_3	8

typedef struct {
	uint8_t x;
	uint8_t y;
}  coordinate;

coordinate calculate_coordinate(int theta1, int theta2, int theta3);


#endif /* INVERSE_KINEMATICS_H_ */