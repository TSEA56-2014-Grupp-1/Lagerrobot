/**
 *	@file inverse_kinematics.c
 *	@author Andreas Runfalk and Johan Lind
 *
 *	Functions for calculating inverse kinematics of the PhantomX Reactor arm.
 */

#include "inverse_kinematics.h"

/**
 *	Given three angles will calculate the target coordinate. This function is
 *	the reverse of ik_angles().
 *
 *	@param joint_angles Angles in radians for all joints
 *
 *	@return Struct of two dimensional cartesian coordinates
 */
coordinate ik_calculate_coordinate(angles joint_angles) {
	coordinate coord = {.x = 0, .y = 0};

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
 *	             pointing to the right. 0 and pi are invalid angles.
 *
 *	@return Distance to edge in mm
 */
uint16_t ik_calculate_x_limit(float angle)
{
	return ARM_ROBOT_EDGE / sin(angle);
}

/**
 *	Convert float angle in radians to servo angle for given joint.
 *
 *	@param id ID of joint to convert angle for
 *	@param angle Angle in radians where 0 is the reference angle
 *
 *	@return Value between 0 and 1024 depending on given joint ID and angle
 */
uint16_t ik_rad_to_servo_angle(uint8_t id, float angle) {
	// Converts radians to servo angle
	float common_factor = 1023.0f * 180.0f / (300.0f * M_PI);

	switch (id) {
		case 1:
			return (uint16_t)(ARM_JOINT1_ORIGIN_OFFSET - angle * common_factor + 0.5f);
		case 2:
			return (uint16_t)(ARM_JOINT2_ORIGIN_OFFSET - angle * common_factor + 0.5f);
		case 3:
			return (uint16_t)(ARM_JOINT3_ORIGIN_OFFSET + angle * common_factor + 0.5f);
		case 4:
			return (uint16_t)(ARM_JOINT4_ORIGIN_OFFSET + angle * common_factor + 0.5f);
		case 5:
			return (uint16_t)(ARM_JOINT5_ORIGIN_OFFSET - angle * common_factor + 0.5f);
	}

	return 0;
}

/**
 *	Calculate a point P given coordinates for a target. The point P will always
 *	be horizontally left of target if possible. If the point P comes too close
 *	to the x-limit when y <= ARM_Y_LIMIT. P will shifted upwards.
 *
 *	@param coord Coordinate for target
 *	@param x_limit x-limit in mm when P is considered to be too close
 */
coordinate ik_find_p(coordinate coord, uint16_t x_limit)
{
	// Limit wrist start coordinate when close to robot body
	if (coord.x - ARM_LENGTH_LINK_3 < x_limit && coord.y <= ARM_Y_LIMIT)
	{
		return (coordinate){
			.x = x_limit,
			.y = coord.y + sqrt(pow(ARM_LENGTH_LINK_3, 2) - pow((coord.x - x_limit), 2))
		};
	} else {
		return (coordinate){.x = coord.x - ARM_LENGTH_LINK_3, .y = coord.y};
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
uint8_t ik_angles_p(coordinate coord, angles *joint_angles) {
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
 *	@param[in] x_limit x-limit in mm where arm must not be
 *	@param[out] joint_angles Joint angles in radians for each joint
 *
 *	@return Status code
 */
uint8_t ik_angles(coordinate coord, uint16_t x_limit, angles *joint_angles)
{
	coordinate p = ik_find_p(coord, x_limit);

	if (ik_angles_p(p, joint_angles) != 0) {
		return 1;
	}

	float sin_theta3 = coord.y - p.y;
	float cos_theta3 = coord.x - p.x;

	joint_angles->t3 = atan2f(
		sin_theta3, cos_theta3) - joint_angles->t2 - joint_angles->t1;

	return 0;
}
