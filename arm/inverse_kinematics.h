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

#include "types.h"

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
#define ARM_JOINT_BASE_ORIGIN_OFFSET         518.0f
#define ARM_JOINT_SHOULDER_ORIGIN_OFFSET     740.0f
#define ARM_JOINT_ELBOW_ORIGIN_OFFSET        765.0f
#define ARM_JOINT_WRIST_ORIGIN_OFFSET        511.0f
#define ARM_JOINT_WRIST_ROTATE_ORIGIN_OFFSET 511.0f
// @}

/**
 *	Height in mm from the arm's origin where the robot's bounding box edge is
 *	considered.
 */
#define ARM_Y_LIMIT 50L

/**
 *	Y value to consider as floor
 */
#define ARM_FLOOR_LEVEL -180L

/**
 *	Shortest horizontal distance in mm from the arm's origin to the robot's edge.
 *	This is used when calculating valid positions for the arm's joints.
 */
#define ARM_ROBOT_EDGE 140L

uint16_t ik_joint_rad_to_angle(uint8_t id, float angle);

//XXX: @return float or uint16_t?
float ik_joint_angle_to_rad(uint8_t id, uint16_t angle);
arm_coordinate ik_calculate_coordinate(arm_joint_angles joint_angles);
uint8_t ik_angles(arm_coordinate coord, arm_joint_angles *joint_angles);
uint8_t ik_valid_coordinate(arm_coordinate coord);

#endif /* INVERSE_KINEMATICS_H_ */
