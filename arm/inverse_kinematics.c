/*#include <avr/io.h>*/
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "inverse_kinematics.h"

coordinate ik_calculate_coordinate(int theta1, int theta2, int theta3)
{
	coordinate temp;
	temp.x = ARM_LENGTH_LINK_1 * cos(theta1) + ARM_LENGTH_LINK_2*cos(theta2) + ARM_LENGTH_LINK_3*cos(theta3);
	temp.y = ARM_LENGTH_LINK_1 * sin(theta1) + ARM_LENGTH_LINK_2*sin(theta2) + ARM_LENGTH_LINK_3*sin(theta3);
	return temp;
}

/**
 *	Convert float angle in radians to servo angle
 */
uint16_t ik_rad_to_servo_angle(float rad, uint16_t ref_position) {
	// TODO
	return 0;
}

uint8_t ik_coordinate_to_angles(coordinate coord, uint8_t solution, angles *joint_angles) {
	float cos_theta2;
	float sin_theta2;

	float theta2_1;
	float theta2_2;

	float det_cmatrix;

	float cos_theta1_1;
	float cos_theta1_2;
	float sin_theta1_1;
	float sin_theta1_2;


	cos_theta2 = pow(coord.x, 2) + pow(coord.y, 2);
	cos_theta2 -= pow(ARM_LENGTH_LINK_1, 2) + pow(ARM_LENGTH_LINK_2, 2);
	cos_theta2 /= 2 * ARM_LENGTH_LINK_1 * ARM_LENGTH_LINK_2;

	if (cos_theta2 > 1.0 || cos_theta2 < -1.0) {
		return 1;
	}

	sin_theta2 = sqrt(1 - pow(cos_theta2, 2));

	det_cmatrix = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 + cos_theta2, 2);
	det_cmatrix += pow(ARM_LENGTH_LINK_2 * sin_theta2, 2);

	cos_theta1_1 = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 * cos_theta2) * coord.x;
	cos_theta1_1 += ARM_LENGTH_LINK_2 * -sin_theta2 * coord.y;
	cos_theta1_1 /= det_cmatrix;

	cos_theta1_2 = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 * cos_theta2) * coord.x;
	cos_theta1_2 += ARM_LENGTH_LINK_2 * sin_theta2 * coord.y;
	cos_theta1_2 /= det_cmatrix;

	sin_theta1_1 = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 * cos_theta2) * coord.y;
	sin_theta1_1 -= ARM_LENGTH_LINK_2 * -sin_theta2 * coord.x;
	sin_theta1_1 /= det_cmatrix;

	sin_theta1_2 = (ARM_LENGTH_LINK_1 + ARM_LENGTH_LINK_2 * cos_theta2) * coord.y;
	sin_theta1_2 -= ARM_LENGTH_LINK_2 * sin_theta2 * coord.x;
	sin_theta1_2 /= det_cmatrix;

	if (solution) {
		joint_angles->t1 = atan2f(sin_theta1_2, cos_theta1_2);
		joint_angles->t2 = atan2f(sin_theta2, cos_theta2);
		joint_angles->t3 = 0.0;
	} else {
		joint_angles->t1 = atan2f(sin_theta1_1, cos_theta1_1);
		joint_angles->t2 = atan2f(-sin_theta2, cos_theta2);
		joint_angles->t3 = 0.0;
	}

	return 0;
}

//ik_validate_solution()

float rad_to_deg(float rad) {
	return rad * 180.0 / M_PI;
}

int main(void)
{
	angles ang;
	coordinate coord = {.x = 100, .y = 100};
	coordinate coord_impossible = {.x = 160, .y = -10};

	printf("Status: %d\n", ik_coordinate_to_angles(coord, 0, &ang));
	printf("Rad t1: %.2f\n", rad_to_deg(ang.t1));
	printf("Rad t2: %.2f\n", rad_to_deg(ang.t2));

	printf("\n");

	printf("Status: %d\n", ik_coordinate_to_angles(coord, 1, &ang));
	printf("Rad t1: %.2f\n", rad_to_deg(ang.t1));
	printf("Rad t2: %.2f\n", rad_to_deg(ang.t2));

	printf("Status: %d\n", ik_coordinate_to_angles(coord_impossible, 0, &ang));
	printf("Rad t1: %.2f\n", rad_to_deg(ang.t1));
	printf("Rad t2: %.2f\n", rad_to_deg(ang.t2));
}
