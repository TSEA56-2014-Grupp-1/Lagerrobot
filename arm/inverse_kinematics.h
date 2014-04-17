#ifndef INVERSE_KINEMATICS_H_
#define INVERSE_KINEMATICS_H_

/**
 *	@file inverse_kinematics.h
 *	@author Andreas Runfalk and Johan Lind
 *
 *	Contains data structures and constants for calculating inverse kinematics of
 *	the PhantomX Reactor arm.
 */

#include <avr/io.h>
#include <stdint.h>
#include <math.h>

#if !defined(F_CPU)
	#define F_CPU 16000000UL
#endif

/**
 *	@name Arm link lengths
 *
 *	Integer length in mm for each arm link.
 *
 *	@{
 */
#define ARM_LENGTH_LINK_1 152L
#define ARM_LENGTH_LINK_2 149L
#define ARM_LENGTH_LINK_3 152L
// @}

/**
 *	@name Joint origin offsets
 *
 *	Offsets between 0 and 1023 that specifies where the angle 0 is for each joint
 *
 *	@{
 */
#define ARM_JOINT1_ORIGIN_OFFSET 511.0f
#define ARM_JOINT2_ORIGIN_OFFSET 740.0f
#define ARM_JOINT3_ORIGIN_OFFSET 765.0f
#define ARM_JOINT4_ORIGIN_OFFSET 511.0f
#define ARM_JOINT5_ORIGIN_OFFSET 511.0f
// @}

/**
 *	Height in mm from the arm's origin where the robot's bounding box edge is
 *	considered.
 */
#define ARM_Y_LIMIT 0L

/**
 *	Shortest horizontal distance in mm from the arm's origin to the robot's edge.
 *	This is used when calculating valid positions for the arm's joints.
 */
#define ARM_ROBOT_EDGE 140L

/**
 *	Handles two dimensional cartesian coordinates
 */
typedef struct {
	float x;
	float y;
}  coordinate;

/**
 *	Handles triplets of joint angles in radians. t1 is the first joint.
 */
typedef struct {
	float t1;
	float t2;
	float t3;
} angles;

uint16_t ik_rad_to_servo_angle(uint8_t id, float angle);
coordinate ik_calculate_coordinate(angles joint_angles);
uint16_t ik_calculate_x_limit(float angle);
uint8_t ik_angles(coordinate coord, uint16_t x_limit, angles *joint_angles);

#endif /* INVERSE_KINEMATICS_H_ */
