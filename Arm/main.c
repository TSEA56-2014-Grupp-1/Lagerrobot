#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/wdt.h>

#include "arm.h"
#include "servo.h"
#include "inverse_kinematics.h"
#include "types.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include "../shared/usart.h"

/**
 *	Height of object to pick up
 */
#define OBJECT_HEIGHT 60

/**
 *	Flag that sets whether we may continue startup procedure or not. Set by
 *	communication unit once PC-interface connection is established. Data = 0
 *	means reset, data = 1 means can start.
 */
uint8_t can_start = 0;

/**
 *	Handle emergency stop and startup procedures from communication unit
 */
void emergency_handler(uint8_t callback_id, uint16_t data) {
	if (data == 1) {
		can_start = 1;
	} else {
		// Cease all arm movement
		arm_stop();
		TWCR &= ~(1 << TWEN | 1 << TWIE);
		TWCR |= (1 << TWINT);
		// Trigger restart after 1 second and wait for it to happen
		wdt_enable(WDTO_1S);
		for (;;) { }
	}
}

/**
 *	Flag signaling whether there is a manual target or not
 */
uint8_t has_manual_target = 0;

/**
 *	Stores manual target's position
 */
arm_coordinate manual_target_position;

void manual_target(uint8_t callback_id, uint16_t data) {
	switch (callback_id) {
		case 6:
			manual_target_position.x = data;
			break;
		case 7:
			// Y-level starts at floor level for
			manual_target_position.y = (int16_t)data + ARM_FLOOR_LEVEL;
			break;
		case 8:
			manual_target_position.angle = (float)data / 500;
			break;
		case 9:
			manual_target_position.angle = (float)data / -500;
			break;
		case 10:
			has_manual_target = 1;
			break;
	}
}

/**
 *	Flag for signaling that pickup of object should be done.
 */
uint8_t object_grab = 0;

/**
 *	Position for object to be picked up.
 */
arm_coordinate object_position;

/**
 *	Position where the most recently picked up object was. Used to place object.
 */
arm_coordinate last_object_position;

/**
 *	Flag if arm currently is carrying an object
 */
uint8_t has_object = 0;

/**
 *	Remember on which side of robot object to search for was
 */
arm_side object_side;

/**
 *	Handle data from distance sensors and initiate
 */
void object_pickup(uint8_t id, uint16_t data) {
	switch (id) {
		case 2: // Initiate object search on given side, left = 0, right = 1
			while (bus_transmit(BUS_ADDRESS_SENSOR, 9, (data & 1) + 1));

			switch (data) {
				case LEFT:
					object_side = LEFT;
					break;
				case RIGHT:
					object_side = RIGHT;
					break;
			}
			break;
		case 3: // Angle received
			switch (object_side) {
				case LEFT:
					object_position.angle = (float)data / 500;
					break;
				case RIGHT:
					object_position.angle = (float)data / -500;
					break;
			}
			break;
		case 4: // Distance received
			object_position.x = data + 10; // TODO: Should this be done here?
			object_position.y = ARM_FLOOR_LEVEL + OBJECT_HEIGHT;
			break;
		case 5: // Mark pickup ready
			// Verify that we're not already carrying an object and that an object
			// was found
			if (data && !has_object) {
				object_grab = 1;
			} else {
				// Tell chassis that we failed to find anything
				while (bus_transmit(BUS_ADDRESS_CHASSIS, 2, 2));
			}
			break;
	}
}

/**
 *	Flag if object should be returned
 */
uint8_t object_drop_off = 0;

/**
 *	Listen to request to drop off object
 */
void object_return(uint8_t id, uint16_t data) {
	// Invert angle if we're supposed to drop off at the opposite side we picked
	// up the object
	if (data != object_side) {
		last_object_position.angle *= -1;
	}

	if (has_object) {
		object_drop_off = 1;
	}
}

/**
 *	Manual claw control state.
 *
 *	- 0 is do nothing
 *	- 1 is close claw
 *	- 2 is open claw
 */
uint8_t manual_control_claw = 0;

/**
 *	Open or close claw data = 0 means close and data = 1 means open
 */
void control_claw(uint8_t id, uint16_t data) {
	manual_control_claw = (data & 1) + 1;
}

/**
 *	State of manual control. 0 means no control, 2 means that reference position
 *	should be recalculated and 1 means normal operation
 */
uint8_t manual_control_state = 0;

arm_coordinate manual_control_reference;

/**
 *	Increase current position with values in this struct when controlling manually.
 */
arm_coordinate manual_control_change = {
	.x = 0,
	.y = 0,
	.angle = 0
};

/**
 *	Move arm target manually
 */
void update_manual_control(uint8_t id, uint16_t data) {
	switch (data >> 2) {
		case 0: // x
			if (data & 2) {
				if (data & 1) {
					manual_control_change.x = 10;
				} else {
					manual_control_change.x = -10;
				}
			} else {
				manual_control_change.x = 0;
			}
			break;
		case 1: // y
			if (data & 2) {
				if (data & 1) {
					manual_control_change.y = 10;
				} else {
					manual_control_change.y = -10;
				}
			} else {
				manual_control_change.y = 0;
			}
			break;
		case 2: // angle
			if (data & 2) {
				if (data & 1) {
					manual_control_change.angle = 0.03;
				} else {
					manual_control_change.angle = -0.03;
				}
			} else {
				manual_control_change.angle = 0;
			}
			break;
	}

	if (
		manual_control_change.x != 0 ||
		manual_control_change.y != 0 ||
		manual_control_change.angle != 0)
	{
		// We went from resting state to moving
		if (manual_control_state == 0) {
			manual_control_state = 2;
		}
	} else {
		manual_control_state = 0;
	}
}

/**
 *	Predefined positions to handle:
 *
 *	- 0 is no action
 *	- 1 is resting position
 *	- 2 is pickup left
 *	- 3 is pickup right
 */
uint8_t predefined_position_move = 0;

/**
 *	Move arm to predefined positions
 */
void predefined_positions(uint8_t id, uint16_t data) {
	switch (data) {
		case 2:
			object_pickup(2, 0);
			break;
		case 3:
			object_pickup(2, 1);
			break;
		default:
			predefined_position_move = data;
			break;
	}
}

void dummy(uint8_t data, uint16_t metadata) { return;}

int main(void) {
	// Disable watchdog reset timer to prevent continious resets
	wdt_disable();
	MCUSR = 0;
	can_start = 0;
	
	uint8_t status;
	arm_coordinate current_position;

	servo_init();
	bus_init(BUS_ADDRESS_ARM);

	bus_register_receive(0, emergency_handler);

	bus_register_receive(1, control_claw);

	bus_register_receive(2, object_pickup);
	bus_register_receive(3, object_pickup);
	bus_register_receive(4, object_pickup);
	bus_register_receive(5, object_pickup);

	bus_register_receive(6, manual_target);
	bus_register_receive(7, manual_target);
	bus_register_receive(8, manual_target);
	bus_register_receive(9, manual_target);
	bus_register_receive(10, manual_target);

	bus_register_receive(11, update_manual_control);

	bus_register_receive(12, object_return);
	bus_register_receive(13, predefined_positions);

	

	// Wait for communication unit to say we can continue initialising
	/*while (!can_start) {
		// Delay is needed when looping flags such as this
		_delay_us(1);
	}*/
	
	// Wait for servos to power on before setting initial parameters
	_delay_ms(100);
	arm_init();

	_delay_ms(1000);

	// Make sure claw is open and arm is in a well defined state
	arm_claw_open();
	arm_resting_position();

	// XXX: Trick system into thinking we have reached a pickup station
	//while (bus_transmit(BUS_ADDRESS_SENSOR, 9, 1));
	//object_side = LEFT;

	for (;;) {
		if (object_grab) {
			status = arm_move_to_coordinate(object_position);

			switch (status) {
				case 0:
					// Save position for when dropping off object
					last_object_position = object_position;

					// Perform movement to coordinate
					arm_move_perform_block();

					// Wait for arm to stabilize
					_delay_ms(500);

					// Grip object and return to resting position
					if (arm_claw_close()) {
						display(0, "Picked object");
						arm_resting_position();

						has_object = 1;

						// Send command that arm picked up object to chassis
						while (bus_transmit(BUS_ADDRESS_CHASSIS, 2, 0));
					} else {
						display(0, "Failed pickup");
						arm_resting_position();
						arm_claw_open();

						has_object = 0;

						// Send command that no object was picked up object to chassis
						while (bus_transmit(BUS_ADDRESS_CHASSIS, 2, 2));
					}
					break;
				case 1:
					display(0, "Invalid coordinate");
					break;
				case 2:
					display(0, "No P found");
					break;
				default:
					display(0, "Error %u", status);
			}
			object_grab = 0;
		} else if (object_drop_off) {
			// Move to the position where object was picked up
			status = arm_move_to_coordinate(last_object_position);

			switch (status) {
				case 0:
					// Perform movement to coordinate
					arm_move_perform_block();

					// Wait for arm to stabilize
					_delay_ms(500);

					// Leave object and return to resting position
					if (arm_claw_open()) {
						display(0, "Left object");
						arm_resting_position();
						has_object = 0;
					} else {
						display(0, "Left no object");
						arm_resting_position();
						has_object = 1;
					}

					// Send command that arm is done to chassis
					while (bus_transmit(BUS_ADDRESS_CHASSIS, 2, 1));
					break;
				case 1:
					display(0, "Invalid coordinate");
					break;
				case 2:
					display(0, "No P found");
					break;
				default:
					display(0, "Error %u", status);
			}
			object_drop_off = 0;
		} else if (has_manual_target) {
			display(1, "%d,%d,%d",
				manual_target_position.x,
				manual_target_position.y,
				(uint16_t)(manual_target_position.angle * 1000));

			status = arm_move_to_coordinate(manual_target_position);

			if (status) {
				display(0, "Error %u", status);
			} else {
				arm_move_perform_block();
			}

			has_manual_target = 0;
		} else if (manual_control_state) {
			// Update reference position when we start manual movement
			if (manual_control_state == 2) {
				arm_position(&manual_control_reference);
				manual_control_state = 1;
			}

			manual_control_reference.x += manual_control_change.x;
			manual_control_reference.y += manual_control_change.y;
			manual_control_reference.angle += manual_control_change.angle;

			if (!arm_move_to_coordinate(manual_control_reference)) {
				//arm_move_perform_block();
				arm_move_perform();
				_delay_ms(100);
			}
		} else if (manual_control_claw) {
			if (manual_control_claw == 1) {
				display(0, "Claw close");
				arm_claw_close();
			} else {
				display(0, "Claw open");
				arm_claw_open();
			}
			manual_control_claw = 0;
		} else if (predefined_position_move) {
			switch (predefined_position_move) {
				case 1:
					arm_resting_position();
					break;
			}
			predefined_position_move = 0;
		}
	}
}
