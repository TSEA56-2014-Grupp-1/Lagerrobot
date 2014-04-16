/*#include <avr/io.h>*/
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "inverse_kinematics.h"

coordinate ik_calculate_coordinate(float theta1, float theta2, float theta3)
{
	coordinate temp = {.x = 0, .y = 0};

	temp.x += ARM_LENGTH_LINK_1 * cos(theta1);
	temp.y += ARM_LENGTH_LINK_1 * sin(theta1);

	temp.x += ARM_LENGTH_LINK_2 * cos(theta1 + theta2);
	temp.y += ARM_LENGTH_LINK_2 * sin(theta1 + theta2);

	temp.x += ARM_LENGTH_LINK_3 * cos(theta1 + theta2 + theta3);
	temp.y += ARM_LENGTH_LINK_3 * sin(theta1 + theta2 + theta3);

	return temp;
}

/**
 *	Calculate robot's shell edge based on arm rotation. pi/2 means pointing to the left and -pi/2 means pointing to the right
 */
float ik_calculate_x_limit(float beta)
{
	return SHORTEST_X_DISTANCE_FROM_ORIGO / sin(beta);
}

/**
 *	Convert float angle in radians to servo angle
 */
uint16_t ik_rad_to_servo_angle(uint8_t id, float rad) {
	// Converts radians to servo angle
	float common_factor = 1023.0f * 180.0f / (300.0f * M_PI);

	switch (id) {
		case 1:
			return (uint16_t)(ARM_JOINT1_ORIGIN_OFFSET - rad * common_factor + 0.5f);
		case 2:
			return (uint16_t)(ARM_JOINT2_ORIGIN_OFFSET - rad * common_factor + 0.5f);
		case 3:
			return (uint16_t)(ARM_JOINT3_ORIGIN_OFFSET + rad * common_factor + 0.5f);
		case 4:
			return (uint16_t)(ARM_JOINT4_ORIGIN_OFFSET + rad * common_factor + 0.5f);
		case 5:
			return (uint16_t)(ARM_JOINT5_ORIGIN_OFFSET - rad * common_factor + 0.5f);
	}

	return 0;
}

coordinate ik_find_p(coordinate coord, float x_limit)
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

float dot_product(coordinate a, coordinate b)
{
	return a.x * b.x + a.y * b.y;
}

coordinate vector_between(coordinate a, coordinate b)
{
	return (coordinate){.x = a.x - b.x, .y = a.y - b.y};
}

float vector_length(coordinate a)
{
	return sqrt(dot_product(a, a));
}


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

	det_cmatrix = pow(ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 + cos_theta2, 2);
	det_cmatrix += pow(ARM_LENGTH_LINK_2 * sin_theta2, 2);

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

uint8_t ik_angles(coordinate coord, float x_limit, angles *joint_angles)
{
	coordinate p = ik_find_p(coord, x_limit);

	if (ik_angles_p(p, joint_angles) != 0) {
		return 1;
	}

	coordinate j = {
		.x = ARM_LENGTH_LINK_1 * cos(joint_angles->t1),
		.y = ARM_LENGTH_LINK_1 * sin(joint_angles->t1)
	};

	coordinate u = vector_between(j, p);
	coordinate v = vector_between(p, coord);

	float theta3 = acos(dot_product(u, v) / (vector_length(u) * vector_length(v)));
	coordinate option1 = ik_calculate_coordinate(joint_angles->t1, joint_angles->t2, theta3);
	coordinate option2 = ik_calculate_coordinate(joint_angles->t1, joint_angles->t2, -theta3);

	float option1_length = vector_length(vector_between(coord, option1));
	float option2_length = vector_length(vector_between(coord, option2));

	if (option1_length > option2_length) {
		theta3 *= -1;
	}

	joint_angles->t3 = theta3;
	return 0;
}
