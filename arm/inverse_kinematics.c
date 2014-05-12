/**
 *	@file inverse_kinematics.c
 *	@author Andreas Runfalk and Johan Lind
 *
 *	Functions for calculating inverse kinematics of the PhantomX Reactor arm.
 */

#include "inverse_kinematics.h"
#include "arm.h"
#include <stdlib.h>

/**
 *	Given three angles will calculate the target coordinate. This function is
 *	the reverse of ik_angles().
 *
 *	@param joint_angles Angles in radians for all joints
 *
 *	@return Struct of two dimensional cartesian coordinates
 */
arm_coordinate ik_calculate_coordinate(arm_joint_angles joint_angles) {
	arm_coordinate coord = {.x = 0, .y = 0};

	coord.x += ARM_LENGTH_LINK_1 * cos(joint_angles.t1);
	coord.y += ARM_LENGTH_LINK_1 * sin(joint_angles.t1);

	coord.x += ARM_LENGTH_LINK_2 * cos(joint_angles.t1 + joint_angles.t2);
	coord.y += ARM_LENGTH_LINK_2 * sin(joint_angles.t1 + joint_angles.t2);

	coord.x += ARM_LENGTH_LINK_3 * cos(
		joint_angles.t1 + joint_angles.t2 + joint_angles.t3);
	coord.y += ARM_LENGTH_LINK_3 * sin(
		joint_angles.t1 + joint_angles.t2 + joint_angles.t3);

	return coord;
}

/**
 *	Calculate robot's shell edge based on arm rotation.
 *
 *	@param angle Arm's rotation angle in radians. 0 means arm is pointing
 *	             straight ahead. pi/2 means pointing to the left and -pi/2 means
 *	             pointing to the right. 0 ± n * pi are invalid angles.
 *
 *	@return Distance to edge in mm
 */
uint16_t ik_calculate_x_limit(float angle)
{
	return ARM_ROBOT_EDGE / sin((float)abs(angle));
}

/**
 *	Convert float angle in radians to servo angle for given joint.
 *
 *	@param id ID of joint to convert angle for
 *	@param angle Angle in radians where 0 is the reference angle
 *
 *	@return Value between 0 and 1024 depending on given joint ID and angle
 */
uint16_t ik_joint_rad_to_angle(uint8_t joint, float angle) {
	// Converts radians to servo angle
	float common_factor = 1023.0f * 180.0f / (300.0f * M_PI);

	switch (joint) {
		case ARM_JOINT_BASE:
			return (uint16_t)(ARM_JOINT_BASE_ORIGIN_OFFSET - angle * common_factor + 0.5f);
		case ARM_JOINT_SHOULDER:
			return (uint16_t)(ARM_JOINT_SHOULDER_ORIGIN_OFFSET - angle * common_factor + 0.5f);
		case ARM_JOINT_ELBOW:
			return (uint16_t)(ARM_JOINT_ELBOW_ORIGIN_OFFSET + angle * common_factor + 0.5f);
		case ARM_JOINT_WRIST:
			return (uint16_t)(ARM_JOINT_WRIST_ORIGIN_OFFSET + angle * common_factor + 0.5f);
		case ARM_JOINT_WRIST_ROTATE:
			return (uint16_t)(ARM_JOINT_WRIST_ROTATE_ORIGIN_OFFSET - angle * common_factor + 0.5f);
	}

	// If we get to here an invalid joint was returned
	return 0;
}

/**
 *	Convert servo angle to float angle in radians for given joint.
 *
 *	@param id ID of joint to convert angle for
 *	@param angle Value between 0 and 1024
 *
 *	@return Angle in radians where 0 is the reference angle
 */
float ik_joint_angle_to_rad(uint8_t joint, uint16_t angle) {
	// Converts radians to servo angle
	float common_factor = 300.0f * M_PI / (1023.0f * 180.0f);

	switch (joint) {
		case ARM_JOINT_BASE:
			return -(angle - ARM_JOINT_BASE_ORIGIN_OFFSET) * common_factor;
		case ARM_JOINT_SHOULDER:
			return -(angle - ARM_JOINT_SHOULDER_ORIGIN_OFFSET) * common_factor;
		case ARM_JOINT_ELBOW:
			return +(angle - ARM_JOINT_ELBOW_ORIGIN_OFFSET) * common_factor;
		case ARM_JOINT_WRIST:
			return +(angle - ARM_JOINT_WRIST_ORIGIN_OFFSET) * common_factor;
		case ARM_JOINT_WRIST_ROTATE:
			return -(angle - ARM_JOINT_WRIST_ROTATE_ORIGIN_OFFSET) * common_factor;
	}

	// If we get to here an invalid joint was returned
	return 0;
}


/**
 *	Calculate a point P given coordinates for a target. The point P will always
 *	be horizontally left of target if possible. If the point P comes too close
 *	to the x-limit when y <= ARM_Y_LIMIT. P will shifted upwards.
 *
 *	@param coord Coordinate for target
 */
arm_coordinate ik_find_p(arm_coordinate coord) {
	uint16_t x_limit = ik_calculate_x_limit(coord.angle);

	// Limit wrist start coordinate when close to robot body
	if (coord.x - ARM_LENGTH_LINK_3 < x_limit && coord.y <= ARM_Y_LIMIT) {
		return (arm_coordinate){
			.x = x_limit,
			.y = coord.y + sqrt(pow(ARM_LENGTH_LINK_3, 2) - pow((coord.x - x_limit), 2)),
			.angle = coord.angle
		};
	} else {
		return (arm_coordinate){
			.x = coord.x - ARM_LENGTH_LINK_3,
			.y = coord.y,
			.angle = coord.angle
		};
	}
}

/**
 *	Calculate the joint angles t1 and t2 for an intermediate coordinate P.
 *	Possible status codes are:
 *
 *	- 0 if successful
 *	- 1 if P was unreachable
 *
 *	@param[in] coord Coordinates in mm for P
 *	@param[out] joint_angles Reference to an ::angles struct
 *
 *	@return Status code
 */
uint8_t ik_angles_p(arm_coordinate coord, arm_joint_angles *joint_angles) {
	float cos_theta2;
	float sin_theta2;

	float det_cmatrix;

	float cos_theta1;
	float sin_theta1;

	cos_theta2 = pow(coord.x, 2) + pow(coord.y, 2);
	cos_theta2 -= pow(ARM_LENGTH_LINK_1, 2) + pow(ARM_LENGTH_LINK_2, 2);
	cos_theta2 /= 2 * ARM_LENGTH_LINK_1 * ARM_LENGTH_LINK_2;

	if (cos_theta2 > 1.0 || cos_theta2 < -1.0) {
		return 1;
	}

	sin_theta2 = -sqrt(1 - pow(cos_theta2, 2));

	det_cmatrix = pow(ARM_LENGTH_LINK_1, 2) + pow(ARM_LENGTH_LINK_2, 2);
	det_cmatrix += 2 * ARM_LENGTH_LINK_2 * cos_theta2;

	cos_theta1 = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 * cos_theta2) * coord.x;
	cos_theta1 += ARM_LENGTH_LINK_2 * sin_theta2 * coord.y;
	cos_theta1 /= det_cmatrix;

	sin_theta1 = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 * cos_theta2) * coord.y;
	sin_theta1 -= ARM_LENGTH_LINK_2 * sin_theta2 * coord.x;
	sin_theta1 /= det_cmatrix;

	joint_angles->t1 = atan2f(sin_theta1, cos_theta1);
	joint_angles->t2 = atan2f(sin_theta2, cos_theta2);

	return 0;
}

/**
 *	Calculate joint angles required to reach the given coordinate. This function
 *	is the reverse of ik_calculate_coordinate(). Possible status codes are:
 *
 *	- 0 if successful
 *	- 1 if P was unreachable
 *
 *	@param[in] coord Coordinate for target
 *	@param[out] joint_angles Joint angles in radians for each joint
 *
 *	@return Status code
 */
uint8_t ik_angles(arm_coordinate coord, arm_joint_angles *joint_angles)
{
	arm_coordinate p = ik_find_p(coord);

	if (!ik_valid_coordinate(coord) || ik_angles_p(p, joint_angles) != 0) {
		return 1;
	}

	joint_angles->t0 = coord.angle;

	float sin_theta3 = coord.y - p.y;
	float cos_theta3 = coord.x - p.x;

	joint_angles->t3 = atan2f(
		sin_theta3, cos_theta3) - joint_angles->t2 - joint_angles->t1;

	return 0;
}

/**
 *	Validate given coordinates.
 *
 *	@param coord Coordinate for a target
 *
 *	@return 1 if coordinate is valid, else 0
 */
uint8_t ik_valid_coordinate(arm_coordinate coord) {
	uint16_t x_limit = ik_calculate_x_limit(coord.angle);

	// Check if goal is reachable at all
	if (sqrt(pow(coord.x, 2) + pow(coord.y, 2)) >= 450) {
		return 0;
	}

	// Prevent floor collision
	if (coord.y < ARM_FLOOR_LEVEL) {
		return 0;
	}

	// Prevent collision with chassis
	if (coord.y <= ARM_Y_LIMIT && coord.x < x_limit) {
		return 0;
	}

	return 1;
}
