/**
 *	@file arm.c
 *	@author Andreas Runfalk
 *
 *	Functions for handling common arm operations such as moving joints
 */

#include "arm.h"

/**
 *	Setup sane defaults for arm servos regarding speed and compliance margins
 */
void arm_init(void)
{
	servo_write(
		SERVO_BROADCASTING_ID,
		SERVO_CW_COMPLIANCE_MARGIN,
		0x00, 0x00, 0x30, 0x30);

	servo_write_uint16(SERVO_BROADCASTING_ID, SERVO_PUNCH_L, 10);
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
 *	@return 0 if valid joint and angle, else status code from servo_receive()
 */
uint8_t arm_move_add(uint8_t joint, uint16_t angle) {
	uint8_t status;

	if (!arm_valid_angle(joint, angle)) {
		return 12;
	}

	switch (joint) {
		case ARM_JOINT_BASE:
			status = servo_reg_write_uint16(ARM_SERVO_BASE, SERVO_GOAL_POSITION_L, angle);
			break;
		case ARM_JOINT_SHOULDER:
			status = servo_reg_write_uint16(
				ARM_SERVO_SHOULDER_1, SERVO_GOAL_POSITION_L, angle);

			if (status) {
				break;
			}

			status = servo_reg_write_uint16(
				ARM_SERVO_SHOULDER_2, SERVO_GOAL_POSITION_L, 1023 - angle);
			break;
		case ARM_JOINT_ELBOW:
			status = servo_reg_write_uint16(
				ARM_SERVO_ELBOW_1, SERVO_GOAL_POSITION_L, angle);

			if (status) {
				break;
			}

			status = servo_reg_write_uint16(
				ARM_SERVO_ELBOW_2, SERVO_GOAL_POSITION_L, 1023 - angle);
			break;
		case ARM_JOINT_WRIST:
			status = servo_reg_write_uint16(ARM_SERVO_WRIST, SERVO_GOAL_POSITION_L, angle);
			break;
		case ARM_JOINT_WRIST_ROTATE:
			status = servo_reg_write_uint16(
				ARM_SERVO_WRIST_ROTATE, SERVO_GOAL_POSITION_L, angle);
			break;
		case ARM_JOINT_CLAW:
			status = servo_reg_write_uint16(ARM_SERVO_CLAW, SERVO_GOAL_POSITION_L, angle);
			break;
		default:
			// We should never get here
			return 13;
	}

	return status;
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
 *	@return 0 if valid joint and angle, else error from servo_receive()
 */
uint8_t arm_move(uint8_t joint, uint16_t angle) {
	uint8_t status = arm_move_add(joint, angle);
	if (status) {
		return status;
	}

	arm_move_perform();
	return 0;
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

	// Try to detect 10 times if joint is still moving before giving up and
	// assume it doesn't
	for (i = 0; i < 10; i++) {
		if (!servo_read_uint8(arm_joint_to_servo(joint), SERVO_MOVING, &moving)) {
			return moving;
		}

		// Retry after a while
		_delay_ms(20);
	}

	return 0;
}

/**
 *	Check if any joint is moving in arm
 */
uint8_t arm_is_moving(void) {
	// Relies on C's short circuit evaluation. It will never try more than needed
	return
		arm_joint_is_moving(ARM_JOINT_BASE) ||
		arm_joint_is_moving(ARM_JOINT_SHOULDER) ||
		arm_joint_is_moving(ARM_JOINT_ELBOW) ||
		arm_joint_is_moving(ARM_JOINT_WRIST) ||
		arm_joint_is_moving(ARM_JOINT_WRIST_ROTATE) ||
		arm_joint_is_moving(ARM_JOINT_CLAW);
}

/**
 *	Given angles in radians for shoulder, elbow and wrist joint will add move
 *	commands.
 *
 *	@param joint_angles Angles in radians as provided by the inverse kinematics
 *	                    library.
 */
uint8_t arm_move_to_angles(arm_joint_angles joint_angles) {
	// Save errors for
	uint8_t status;

	status = arm_move_add(ARM_JOINT_BASE,
		ik_joint_rad_to_angle(ARM_JOINT_BASE, joint_angles.t0));
	if (status) {
		return 1;
	}

	status = arm_move_add(ARM_JOINT_SHOULDER,
		ik_joint_rad_to_angle(ARM_JOINT_SHOULDER, joint_angles.t1));
	if (status) {
		return 2;
	}

	status = arm_move_add(ARM_JOINT_ELBOW,
		ik_joint_rad_to_angle(ARM_JOINT_ELBOW, joint_angles.t2));
	if (status) {
		return 3;
	}

	status = arm_move_add(ARM_JOINT_WRIST,
		ik_joint_rad_to_angle(ARM_JOINT_WRIST, joint_angles.t3));
	if (status) {
		return 4;
	}

	return 0;
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

/**
 *	Return arm to resting position. Block until operation is complete
 */
void arm_resting_position(void) {
	// Raise arm before turning base to prevent collision with self
	arm_move_add(ARM_JOINT_SHOULDER, 511);
	arm_move_add(ARM_JOINT_ELBOW, 511);
	arm_move_add(ARM_JOINT_WRIST, ARM_JOINT_WRIST_ANGLE_MAX);
	arm_move_perform();


	// Wait for shoulder and elbow to complete movement
	do {
		_delay_ms(20);
	} while (
		arm_joint_is_moving(ARM_JOINT_SHOULDER) ||
		arm_joint_is_moving(ARM_JOINT_ELBOW));

	// Final position
	arm_move_add(ARM_JOINT_BASE, 511);
	arm_move_add(ARM_JOINT_SHOULDER, ARM_JOINT_SHOULDER_ANGLE_MIN);
	arm_move_add(ARM_JOINT_ELBOW, ARM_JOINT_ELBOW_ANGLE_MIN);
	arm_move_perform();

	// Wait for shoulder and elbow to complete movement
	do {
		_delay_ms(20);
	} while (
		arm_joint_is_moving(ARM_JOINT_SHOULDER) ||
		arm_joint_is_moving(ARM_JOINT_ELBOW));
}

/**
 *	Move arm claw to the given coordinate and block until operation is complete
 */
uint8_t arm_move_to_coordinate(arm_coordinate coord) {
	arm_joint_angles joint_angles;

	uint8_t status = ik_angles(coord, &joint_angles);
	if (status) {
		return status;
	}

	status = arm_move_to_angles(joint_angles);
	if (status) {
		return status + 2;
	}

	return 0;
}

/**
 *	Calculate current arm joint angles as an arm coordinate
 */
uint8_t arm_angles(arm_joint_angles *joint_angles) {
	uint16_t position;

	if (servo_read_uint16(
		arm_joint_to_servo(ARM_JOINT_BASE), SERVO_PRESENT_POSITION_L, &position))
	{
		return 1;
	}
	joint_angles->t0 = ik_joint_angle_to_rad(ARM_JOINT_BASE, position);

	if (servo_read_uint16(
		arm_joint_to_servo(ARM_JOINT_SHOULDER), SERVO_PRESENT_POSITION_L, &position))
	{
		return 2;
	}
	joint_angles->t1 = ik_joint_angle_to_rad(ARM_JOINT_SHOULDER, position);

	if (servo_read_uint16(
		arm_joint_to_servo(ARM_JOINT_ELBOW), SERVO_PRESENT_POSITION_L, &position))
	{
		return 3;
	}
	joint_angles->t2 = ik_joint_angle_to_rad(ARM_JOINT_ELBOW, position);

	if (servo_read_uint16(
		arm_joint_to_servo(ARM_JOINT_WRIST), SERVO_PRESENT_POSITION_L, &position))
	{
		return 4;
	}
	joint_angles->t3 = ik_joint_angle_to_rad(ARM_JOINT_WRIST, position);

	return 0;
}

/**
 *	Calculate current arm position as an arm coordinate.
 *
 *	@return Status code as defined by arm_angles()
 */
uint8_t arm_position(arm_coordinate *coord) {
	arm_joint_angles joint_angles;
	uint8_t status = arm_angles(&joint_angles);

	if (status) {
		return status;
	}

	*coord = ik_calculate_coordinate(joint_angles);

	return 0;
}

/**
 *	Perform all queued moves and block until done. Base movement is performed
 *	before the rest of the arm is moved.
 */
void arm_move_perform_block(void) {
	// Move base first
	servo_action(arm_joint_to_servo(ARM_JOINT_BASE));

	// Wait for base movement to complete
	do {
		_delay_ms(10);
	} while (arm_joint_is_moving(ARM_JOINT_SHOULDER));

	// Move rest of arm
	arm_move_perform();

	// Wait for movement to complete
	do {
		_delay_ms(10);
	} while (arm_is_moving());
}
