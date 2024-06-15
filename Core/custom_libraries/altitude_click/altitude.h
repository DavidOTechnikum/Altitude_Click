/**
 * @file    altitude.h
 * @author  David Oberleitner
 * @version 1.0
 * @date    2024-05-16
 *
 * @brief This library contains the functions for the Mikroe Altitude Click.
 *
 *
 * 			The Altitude Click is an add-on board with an MPL3115A2, a MEMS
 * 			pressure sensor providing pressure/altitude and temperature data
 * 			measurements from NXP Semiconductors.
 *
 *			Disclaimer: Some code structures were inspired by/copied from
 *			the Mikroe example library.
 *
 */


#ifndef INC_ALTITUDE_H_
#define INC_ALTITUDE_H_

#include "main.h"
#include <stdio.h>
#include <math.h>
#include "cmsis_os2.h"
#include <string.h>

#define CALIBRATE_DELAY 550
#define START_ALTITUDE 170		// Altitude of Höchstädtplatz in 1200 Vienna, Austria

float calculate_pressure (uint8_t pr_lsb, uint8_t pr_csb, uint8_t pr_msb);
float calculate_temperature (uint8_t temp_lsb, uint8_t temp_msb);
float calculate_altitude (uint8_t alt_lsb, uint8_t alt_csb, uint8_t alt_msb);
HAL_StatusTypeDef read_data (I2C_HandleTypeDef *hi2c1, uint8_t *alt_lsb, uint8_t *alt_csb, uint8_t *alt_msb,
					uint8_t *temp_lsb, uint8_t *temp_msb);
HAL_StatusTypeDef calibration (I2C_HandleTypeDef *hi2c1);
HAL_StatusTypeDef altitude_read (I2C_HandleTypeDef *hi2c1, osMessageQueueId_t data_queueHandle);
void data_in_queue (char *mode, float value, osMessageQueueId_t data_queueHandle);

#endif /* INC_ALTITUDE_H_ */

