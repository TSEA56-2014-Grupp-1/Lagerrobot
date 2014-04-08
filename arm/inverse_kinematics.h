/*
 * inverse_kinematics.h
 *
 * Created: 4/4/2014 2:01:54 PM
 *  Author: johli887
 */


#ifndef INVERSE_KINEMATICS_H_
#define INVERSE_KINEMATICS_H_

#define ARM_LENGTH_LINK_1 100 // 18
#define ARM_LENGTH_LINK_2 100 // 17
#define ARM_LENGTH_LINK_3 8

typedef struct {
	float x;
	float y;
}  coordinate;

typedef struct {
	float t1;
	float t2;
	float t3;
} angles;

coordinate calculate_coordinate(int theta1, int theta2, int theta3);


#endif /* INVERSE_KINEMATICS_H_ */
