/**
 *	@file arm.c
 *	@author Andreas Runfalk
 *
 *	Functions for handling common arm operations such as moving joints
 */

#include "arm.h"
#include "inverse_kinematics.h"

/**
 *	Setup sane defaults for arm servos regarding speed and compliance margins
 */
void arm_init(void)
{
	servo_write(
		SERVO_BROADCASTING_ID,
		SERVO_CW_COMPLIANCE_MARGIN,
		0x00, 0x00, 0x40, 0x40);

	servo_write_uint16(SERVO_BROADCASTING_ID, SERVO_GOAL_SPEED_L, 0x040);
}

/**
 *	Verify that the angle is valid for a given joint.
 *
 *	@param joint Joint index. Use ARM_JOINT_* constants.
 *	@param angle Angle to move joint to
 *
 *	@return 1 if valid joint and angle, else 0
 */
uint8_t arm_valid_angle(uint8_t joint, uint16_t angle) {
	switch (joint) {
		case ARM_JOINT_BASE:
			return ARM_JOINT_BETWEEN(
				ARM_JOINT_BASE_ANGLE_MIN, angle, ARM_JOINT_BASE_ANGLE_MAX);
		case ARM_JOINT_SHOULDER:
			return ARM_JOINT_BETWEEN(
				ARM_JOINT_SHOULDER_ANGLE_MIN, angle, ARM_JOINT_SHOULDER_ANGLE_MAX);
		case ARM_JOINT_ELBOW:
			return ARM_JOINT_BETWEEN(
				ARM_JOINT_ELBOW_ANGLE_MIN, angle, ARM_JOINT_ELBOW_ANGLE_MAX);
		case ARM_JOINT_WRIST:
			return ARM_JOINT_BETWEEN(
				ARM_JOINT_WRIST_ANGLE_MIN, angle, ARM_JOINT_WRIST_ANGLE_MAX);
		case ARM_JOINT_WRIST_ROTATE:
			return ARM_JOINT_BETWEEN(
				ARM_JOINT_WRIST_ROTATE_ANGLE_MIN, angle, ARM_JOINT_WRIST_ROTATE_ANGLE_MAX);
		case ARM_JOINT_CLAW:
			return ARM_JOINT_BETWEEN(
				ARM_JOINT_CLAW_ANGLE_MIN, angle, ARM_JOINT_CLAW_ANGLE_MAX);
		default:
			// If we got here the received joint was not known
			return 0;

	}
}

/**
 *	Registers a move for the given joint.
 *
 *	@param joint Joint index. Use ARM_JOINT_* constants.
 *	@param angle Angle to move joint to
 *
 *	@return 1 if valid joint and angle, else 0
 */
uint8_t arm_move_add(uint8_t joint, uint16_t angle) {
	if (!arm_valid_angle(joint, angle)) {
		return 0;
	}

	switch (joint) {
		case ARM_JOINT_BASE:
			servo_move_add(ARM_SERVO_BASE, angle);
			break;
		case ARM_JOINT_SHOULDER:
			servo_move_add(ARM_SERVO_SHOULDER_1, angle);
			servo_move_add(ARM_SERVO_SHOULDER_2, 1023 - angle);
			break;
		case ARM_JOINT_ELBOW:
			servo_move_add(ARM_SERVO_ELBOW_1, angle);
			servo_move_add(ARM_SERVO_ELBOW_2, 1023 - angle);
			break;
		case ARM_JOINT_WRIST:
			servo_move_add(ARM_SERVO_WRIST, angle);
			break;
		case ARM_JOINT_WRIST_ROTATE:
			servo_move_add(ARM_SERVO_WRIST_ROTATE, angle);
			break;
		case ARM_JOINT_CLAW:
			servo_move_add(ARM_SERVO_CLAW, angle);
			break;
		default:
			// We should never get here
			return 0;
	}

	return 1;
}

/**
 *	Perform all moves registered with arm_move_add().
 */
void arm_move_perform(void) {
	servo_action(SERVO_BROADCASTING_ID);
}

/**
 *	Immediately perform move for the given joint.
 *
 *	@param joint Joint index. Use ARM_JOINT_* constants.
 *	@param angle Angle to move joint to
 *
 *	@return 1 if valid joint and angle, else 0
 */
uint8_t arm_move(uint8_t joint, uint16_t angle) {
	if (!arm_move_add(joint, angle)) {
		return 0;
	}

	arm_move_perform();
	return 1;
}

/**
 *	Convert joint to servo ID. If joint corresponds to multiple servos this
 *	function will return first ID for that joint.
 *
 *	@return Servo ID if valid joint, else 0
 */
uint8_t arm_joint_to_servo(uint8_t joint) {
	switch (joint) {
		case ARM_JOINT_BASE:
			return ARM_SERVO_BASE;
		case ARM_JOINT_SHOULDER:
			return ARM_SERVO_SHOULDER_1;
		case ARM_JOINT_ELBOW:
			return ARM_SERVO_ELBOW_1;
		case ARM_JOINT_WRIST:
			return ARM_SERVO_WRIST;
		case ARM_JOINT_WRIST_ROTATE:
			return ARM_SERVO_WRIST_ROTATE;
		case ARM_JOINT_CLAW:
			return ARM_SERVO_CLAW;
		default:
			return 0;
	}
}

/**
 *	Checks if given joint is moving.
 *
 *	@param joint Joint index. Use ARM_JOINT_* constants.
 *
 *	@return 1 if joint is currently moving, else 0
 */
uint8_t arm_joint_is_moving(uint8_t joint) {
	uint8_t i;
	uint8_t moving;

	// Try to detect 5 times if joint is still moving before giving up and
	// assume it doesn't
	for (i = 0; i < 5; i++) {
		if (!servo_read_uint8(arm_joint_to_servo(joint), SERVO_MOVING, &moving)) {
			return moving;
		}

		// Retry after 50 ms
		_delay_ms(10);
	}

	return 0;
}

/**
 *	Given angles in radians for shoulder, elbow and wrist joint will add move
 *	commands to related servos.
 *
 *	@param joint_angles Angles in radians as provided by the inverse kinematics
 *	                    library.
 */
void arm_move_to_angles(arm_joint_angles joint_angles) {
	arm_move_add(ARM_JOINT_BASE, ik_joint_rad_to_angle(ARM_JOINT_BASE, joint_angles.t0));
	arm_move_add(2, ik_joint_rad_to_angle(2, joint_angles.t1));
	arm_move_add(3, ik_joint_rad_to_angle(3, joint_angles.t2));
	arm_move_add(4, ik_joint_rad_to_angle(4, joint_angles.t3));
}

/**
 *	Open claw and block until operation is complete
 */
uint8_t arm_claw_open(void) {
	uint8_t status_code = servo_write(
		ARM_SERVO_CLAW, SERVO_GOAL_POSITION_L, 0x00, 0x02);

	if (status_code) {
		return status_code;
	}


	while (arm_joint_is_moving(ARM_JOINT_CLAW));

	return 0;
}

/**
 *	Close claw and block until complete. If the claw grips something it will
 *	stop gripping prematurely to prevent overloading the servos.
 *
 *	@return status code as provided by servo_receive()
 */
uint8_t arm_claw_close(void) {
	uint8_t status_code = servo_write(
		ARM_SERVO_CLAW, SERVO_GOAL_POSITION_L, 0x00, 0x00);
	uint16_t load;

	if (status_code) {
		return status_code;
	}

	while (arm_joint_is_moving(ARM_JOINT_CLAW)) {
		// Continiously check load to stop when somethings is gripped
		if (servo_read_uint16(ARM_SERVO_CLAW, SERVO_PRESENT_LOAD_L, &load) == 0) {
			if ((load & 0x1ff) > 100) {
				// Disable torque to stop claw
				servo_write(ARM_SERVO_CLAW, SERVO_TORQUE_ENABLE, 0);

				return 0;
			}
		}
	}

	return 0;
}
