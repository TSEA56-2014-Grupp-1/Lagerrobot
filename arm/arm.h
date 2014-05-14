#ifndef ARM_H_
#define ARM_H_
/**
 *	@file arm.h
 *	@author Andreas Runfalk
 *
 *	Constants for arm
 */

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#include "servo.h"
#include "types.h"
#include "inverse_kinematics.h"

#include <avr/io.h>
#include <util/delay.h>


#define ARM_SERVO_DEFAULT_SPEED 0x040

/**
 *	@name Servo IDs
 *
 *	IDs for all controllable arm servos.
 *
 *	@{
 */
#define ARM_SERVO_BASE         7
#define ARM_SERVO_SHOULDER_1   2
#define ARM_SERVO_SHOULDER_2   3
#define ARM_SERVO_ELBOW_1      4
#define ARM_SERVO_ELBOW_2      5
#define ARM_SERVO_WRIST        6
#define ARM_SERVO_WRIST_ROTATE 1
#define ARM_SERVO_CLAW         8
// @}

/**
 *	@name Arm joint IDs
 *
 *	IDs for all joints of the arm.
 *
 *	@{
 */
#define ARM_JOINT_BASE         1
#define ARM_JOINT_SHOULDER     2
#define ARM_JOINT_ELBOW        3
#define ARM_JOINT_WRIST        4
#define ARM_JOINT_WRIST_ROTATE 5
#define ARM_JOINT_CLAW         6
// @}

/**
 *	Number of joints for arm
 */
#define ARM_JOINT_COUNT 6

/**
 *	@name Minimum angles for all joints
 *
 *	For servos with multiple joints the first servo is the reference servo.
 *
 *	@{
 */
#define ARM_JOINT_BASE_ANGLE_MIN         0
#define ARM_JOINT_SHOULDER_ANGLE_MIN     200
#define ARM_JOINT_ELBOW_ANGLE_MIN        200
#define ARM_JOINT_WRIST_ANGLE_MIN        155
#define ARM_JOINT_WRIST_ROTATE_ANGLE_MIN 0
#define ARM_JOINT_CLAW_ANGLE_MIN         0
// @}

/**
 *	@name Maximum angles for all joints
 *
 *	For servos with multiple joints the first servo is the reference servo.
 *
 *	@{
 */
#define ARM_JOINT_BASE_ANGLE_MAX         1023
#define ARM_JOINT_SHOULDER_ANGLE_MAX     820
#define ARM_JOINT_ELBOW_ANGLE_MAX        950
#define ARM_JOINT_WRIST_ANGLE_MAX        810
#define ARM_JOINT_WRIST_ROTATE_ANGLE_MAX 1023
#define ARM_JOINT_CLAW_ANGLE_MAX         511
// @}

/**
 *	Detect if x is between a and b
 */
#define ARM_JOINT_BETWEEN(a, x, b) (x >= a && x <= b)

void arm_init(void);

uint8_t arm_move(uint8_t joint, uint16_t angle);
uint8_t arm_move_add(uint8_t joint, uint16_t angle);
uint8_t arm_move_to_angles(arm_joint_angles joint_angles);
uint8_t arm_move_to_coordinate(arm_coordinate coord);
void arm_move_perform(void);
void arm_move_perform_block(void);

void arm_resting_position(void);
uint8_t arm_position(arm_coordinate *coord);

uint8_t arm_is_moving(void);
uint8_t arm_joint_is_moving(uint8_t joint);

uint8_t arm_claw_open(void);
uint8_t arm_claw_close(void);

#endif /* ARM_H_ */
