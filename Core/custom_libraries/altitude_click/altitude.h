/*
 * altitude.h
 *
 *  Created on: Jun 13, 2024
 *      Author: Dave
 */

#ifndef INC_ALTITUDE_H_
#define INC_ALTITUDE_H_

#include "main.h"
#include <stdio.h>
#include <math.h>
#include "cmsis_os2.h"
#include <string.h>

#define CALIBRATE_DELAY 550
#define START_ALTITUDE 170

float calculate_pressure (uint8_t low_byte, uint8_t high_byte, uint8_t higher_byte);
float calculate_temperature (uint8_t temp_lsb, uint8_t temp_msb);
float calculate_altitude (uint8_t low_byte, uint8_t high_byte, uint8_t higher_byte);
HAL_StatusTypeDef read_data (I2C_HandleTypeDef *hi2c1, uint8_t *low_byte, uint8_t *high_byte, uint8_t *higher_byte,
					uint8_t *temp_lsb, uint8_t *temp_msb);
HAL_StatusTypeDef calibration (I2C_HandleTypeDef *hi2c1);
HAL_StatusTypeDef altitude_read (I2C_HandleTypeDef *hi2c1, osMessageQueueId_t data_queueHandle);
void data_in_queue (char *unit, float value, osMessageQueueId_t data_queueHandle);

#endif /* INC_ALTITUDE_H_ */

