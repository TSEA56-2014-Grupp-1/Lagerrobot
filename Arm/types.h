#ifndef TYPES_H_
#define TYPES_H_

/**
 *	@file types.h
 *	@author Andreas Runfalk
 *
 *	Shared types for describing the arm.
 */

/**
 *	3 dimensional representation of arm. x and y are coordinates in mm. angle is
 *	radians. angle equal to 0 means that the arm is pointing straight ahead.
 *	pi / 2 means to the left and -pi / 2 means to the right.
 */
typedef struct {
	int16_t x;
	int16_t y;
	float angle;
} arm_coordinate;

/**
 *	Handles triplets of joint angles in radians. t1 is the first joint, and so
 *	on.
 */
typedef struct {
	float t0;
	float t1;
	float t2;
	float t3;
} arm_joint_angles;

typedef enum {LEFT, RIGHT} arm_side;

 #endif /* TYPES_H_ */
