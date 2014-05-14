/*
 * linesensor.h
 *
 * Created: 2014-03-27 09:15:08
 *  Author: Karl
 */ 


#ifndef LINESENSOR_H_
#define LINESENSOR_H_

/*
 *	@brief Set up ADC and direction ports for the linesensor.
 */
void line_init();

/*
 *	@brief Updates the linesensor, calculates line weight and detects pickup stations.
 */
void update_linesensor();

/*
 *	@brief Saves the current value of ADCH in sensor_values.
 */
void update_linesensor_values();

/*
 *	@brief Check if there is a pickupstation or not.
 *	@detailed Will check if there is tape on either side, if there is a iterator (pickup_iterator) will start counting down.
 *		If there is tape on the opposite side on any of these iterations, it will be considered a crossing and start over.
 *		If the iterator is zero it will return pickupstation.
 */
void pickup_station_detection();

/*
 *	@brief Calculate the center of mass of the tape, will save the result in the global line_weight.
 */
void calculate_line_weight();

/*
 *	@brief Setup ADC for calibration-routine.
 */
void init_linesensor_calibration();

/*
 *	@brief Check if the current sensor has tape or floor under itself.
 * 
 *	@param sensor_id Sensor to check.
 *
 *	@return Tape if the sensor has tape under itself, otherwise returns floor.
 */
uint8_t get_sensor_surface(uint8_t sensor_id);

/*
 *	@brief Formats the output to accomodate the chassi and transmits it on the bus.
 *
 *	@param id Bus id for the function, unused.
 *	@param metadata Metadata from the bus, unesed.
 *
 *	@return High byte: Station data. Low byte: Center of mass of the line.
 */
uint16_t return_line_weight(uint8_t id, uint16_t metadata);

/*
 *	@brief Will return the values of a pair of sensors, 0 - 5.
 *
 *	@param id Bus id of the function, unused.
 *	@param sensor_pair The pair that will be returned.
 *
 *	@return High byte: value of the first sensor. Low byte: value of the second sensor.
 */
uint16_t return_linesensor(uint8_t id, uint16_t sensor_pair);

/*
 *	@brief Set the tape reference to new value.
 *
 *	@param id Bus id of the function, unused.
 *	@param input_tape_reference The new value of tape_reference.
 */
uint16_t set_tape_reference(uint8_t id, uint16_t input_tape_reference);

/*
 *	@breif Returns the width of the tape.
 *
 *	@return The width of the tape, will be integer between 0 and 11.
 */
uint8_t get_tape_width();

/*
 *	@brief Check if there is tape to mark a pickupstation right.
 *
 *	@return 1 if the 4 sensors furthest to the right has tape underneath, otherwise 0.
 */
uint8_t is_tape_right();

/*
 *	@brief Check if there is tape to mark a pickupstation left.
 *
 *	@return 1 if the 4 sensors furthest to the left has tape underneath, otherwise 0.
 */
uint8_t is_tape_left();

/*
 *	@brief Read 10 values from one sensor, will return the average.
 *
 *	@param sensor_id ID of the sensor.
 *
 *	@return The average value of 10 readings.
 */
uint8_t line_read_sensor(uint8_t sensor_id);

/*
 *	@brief Calibrate the tape reference.
 *
 *	@param id Bus id of the function, unused.
 *	@param metadata Metadata to the function, unused.
 */
void calibrate_linesensor(uint8_t id, uint16_t metadata);

/*
 *	@brief Clears the pickupstation data.
 */
void clear_pickupstation();

#endif /* LINESENSOR_H_ */