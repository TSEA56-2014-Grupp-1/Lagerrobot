/*
 * inverse_kinematics.h
 *
 * Created: 4/4/2014 2:01:54 PM
 *  Author: johli887
 */


#ifndef INVERSE_KINEMATICS_H_
#define INVERSE_KINEMATICS_H_

// TODO: Update these to reflect reality
#define ARM_LENGTH_LINK_1 152L
#define ARM_LENGTH_LINK_2 149L
#define ARM_LENGTH_LINK_3 152L
#define ARM_Y_LIMIT 0L
#define SHORTEST_X_DISTANCE_FROM_ORIGO 140L
#define ARM_ADJUSTMENT_OF_ANGLE 0.3L	//the faulty angle due to the arm crook

typedef struct {
	float x;
	float y;
}  coordinate;

typedef struct {
	float t1;
	float t2;
	float t3;
} angles;

coordinate ik_calculate_coordinate(float theta1, float theta2, float theta3);
coordinate ik_find_p(coordinate coord, float x_limit);
float ik_calculate_x_limit(float beta);

uint16_t ik_rad_to_servo_angle(uint8_t id, float rad);
uint8_t ik_angles_p(coordinate coord, angles *joint_angles);
uint8_t ik_angles(coordinate coord,float x_limit, angles *joint_angles);

#endif /* INVERSE_KINEMATICS_H_ */
