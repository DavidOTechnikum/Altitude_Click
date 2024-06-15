/**
 * @file    altitude.c
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

#include "../../custom_libraries/altitude_click/altitude.h"

/**
 * @brief	Converts the pressure measurement data to one single floating point number.
 * @note	Input consists of three bytes coming from the click's data registers 0x01, 0x02, 0x03,
 * 			which represent MSB, CSB and LSB.
 * 			These bytes combined represent an unsigned fractional 20-bit value in Pascals in Q18.2 format.
 * @param	uint8_t pr_lsb ... The LSB, only bits 7-4 are used, the last two for the fractional part.
 * @param	uint8_t pr_csb ... The CSB.
 * @param	uint8_t pr_msb ... The MSB.
 * @retval 	(float) ... The pressure value in Pa.
 */
float calculate_pressure (uint8_t pr_lsb, uint8_t pr_csb, uint8_t pr_msb) {
	float pressure = 0;
	int32_t pressure_int = -1;

	// Fractional part
	if ((pr_lsb & 0x20) > 0) {
		pressure += 0.5;
	}
	if ((pr_lsb & 0x10) > 0) {
		pressure += 0.25;
	}

	// Integer part
	pressure_int = (int32_t)pr_msb;
	pressure_int = pressure_int << 16;

	pressure_int = pressure_int | ((int32_t)pr_csb << 8);
	pressure_int = pressure_int | (int32_t)pr_lsb;
	pressure_int = pressure_int / 64;

	pressure += (float)pressure_int;

	return pressure;
}

/**
 * @brief	Converts the temperature measurement data to one single floating point number.
 * @note	Input consists of two bytes coming from the click's data registers 0x04 and 0x05,
 * 			which represent MSB and LSB.
 * 			These bytes combined represent a signed fractional 12-bit value in °C in Q12.4 format.
 * 			In the application, however, only one decimal place is used.
 * @param	uint8_t temp_lsb ... The LSB, only bits 7-4 are used, all of them for the fractional part.
 * @param	uint8_t temp_msb ... The MSB, used for the integer part.
 * @retval 	(float) ... The temperature value in °C.
 */
float calculate_temperature (uint8_t temp_lsb, uint8_t temp_msb) {
	float temp = 0;
	int32_t temp_int = -1;
    temp_int = temp_int & (temp_msb << 8);
    temp_int = temp_int | (int32_t)temp_lsb;

    temp_int = temp_int*10/256;
    temp = (float)temp_int/10;
    return temp;
}

/**
 * @brief	Converts the altitude measurement data to one single floating point number.
 * @note	Input consists of three bytes coming from the click's data registers 0x01, 0x02, 0x03,
 * 			which represent MSB, CSB and LSB.
 * 			These bytes combined represent a signed fractional 20-bit value in meters in Q16.4 format.
 * @param	uint8_t alt_lsb ... The LSB, only bits 7-4 are used.
 * @param	uint8_t alt_csb ... The CSB.
 * @param	uint8_t alt_msb ... The MSB.
 * @retval 	(float) ... The altitude value in m.
 */
float calculate_altitude (uint8_t alt_lsb, uint8_t alt_csb, uint8_t alt_msb) {
	float altitude = 0;
	int32_t altitude_int = -1;

    altitude_int = ((int32_t)alt_msb << 24);
    altitude_int = altitude_int | ((int32_t)alt_csb << 16);
    altitude_int = altitude_int | ((int32_t)alt_lsb << 8);
    altitude = (float)altitude_int/6553500;

    return altitude;
}

/**
 * @brief	This function performs a data read operation on the Altitude Click via the I2C interface.
 * @note	The Mem_Read function is used to fetch the altitude/pressure values and the
 * 			temperature values from the data registers (0x01 - 0x05).
 * 			Depending on the click's mode, pressure or altitude data is read from registers 0x01 - 0x03.
 * @param	I2C_HandleTypeDef *hi2c1 ... The I2C interface's handle.
 * @param	uint8_t *low_byte ... Pointer to the altitude/pressure data LSB.
 * @param	uint8_t *high_byte ... Pointer to the altitude/pressure data CSB.
 * @param	uint8_t *higher_byte ... Pointer to the altitude/pressure data MSB.
 * @param	uint8_t *temp_lsb ... Pointer to the temperature data LSB.
 * @param	uint8_t *temp_msb ... Pointer to the temperature data MSB.
 * @retval 	(HAL_StatusTypeDef) ... If one of the reads were unsuccessful, return of HAL error code.
 *
 */
HAL_StatusTypeDef read_data(I2C_HandleTypeDef *hi2c1, uint8_t *low_byte,
		uint8_t *high_byte, uint8_t *higher_byte, uint8_t *temp_lsb,
		uint8_t *temp_msb) {
	HAL_StatusTypeDef retval = HAL_OK;

	// Altitude/Pressure data
	if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x03, 1, low_byte, 1, HAL_MAX_DELAY)
			!= HAL_OK) {
		retval = HAL_ERROR;
	}

	if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x02, 1, high_byte, 1, HAL_MAX_DELAY)
			!= HAL_OK) {
		retval = HAL_ERROR;
	}

	if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x01, 1, higher_byte, 1, HAL_MAX_DELAY)
			!= HAL_OK)
		retval = HAL_ERROR;

	// Temperature data
	if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x04, 1, temp_msb, 1, HAL_MAX_DELAY)
			!= HAL_OK) {
		retval = HAL_ERROR;
	}

	if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x05, 1, temp_lsb, 1, HAL_MAX_DELAY)
			!= HAL_OK) {
		retval = HAL_ERROR;
	}

	return retval;
}

/**
 * @brief	Altitude Click calibration at startup/reset.
 * @note	Takes 8 measures and takes the average as offset for pressure level. Uses
 * 			known altitude level as starting value.
 * 			Registers:
 * 			0x2D - altitude offset register
 * 			0x26 - control register 1
 * 			0x14 - barometric input MSB
 * 			0x15 - barometric input LSB
 *
 * 			Disclaimer: Structure of calibration was inspired by Mikroe's example code.
 * @param	I2C_HandleTypeDef *hi2c1 ... The I2C interface's handle.
 * @retval 	(HAL_StatusTypeDef) ... Returns HAL error in case one operation was unsuccessful.
 */
HAL_StatusTypeDef calibration (I2C_HandleTypeDef *hi2c1) {
	uint8_t pr_lsb, pr_csb, pr_msb;
    uint8_t temp_msb, temp_lsb;
	uint8_t data;
	float calibration_pressure = 0;
	float sea_pressure, current_pressure;
	HAL_StatusTypeDef retval;

	  // Altitude offset set to 0
	  data = 0x00;
	  retval = HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x2D, 1, &data, 1, HAL_MAX_DELAY);

	  // Take 8 samples of pressure value
	  for (uint8_t i = 0; i < 8; i++) {

	    //  data: barometer, on, immediate measurement
	    data = 0x3B;
	    if (HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY)!= HAL_OK) {
	    	retval = HAL_ERROR;
	    }

	    // data: barometer, on, oversampling bit cleared
	    data = 0x39;
	    if (HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
	    	retval = HAL_ERROR;
	    }

	    // Wait for sensor
	    osDelay(CALIBRATE_DELAY);

	    // Take sample
	    if (read_data (hi2c1, &pr_lsb, &pr_csb, &pr_msb, &temp_lsb, &temp_msb) != HAL_OK) {
	    	retval = HAL_ERROR;
	    }
	    calibration_pressure += calculate_pressure(pr_lsb, pr_csb, pr_msb);
	  }

	  current_pressure = calibration_pressure / 8;

	  // Calculate barometric pressure at mean sea level based on a starting altitude
	  sea_pressure = current_pressure /pow(1-START_ALTITUDE*0.0000225577, 5.255877);

	  // Calibrate the sensor according to the sea level pressure for the current measurement location
	  data = (uint8_t)(sea_pressure / 2) >> 8;
	  if (HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x14, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
		  retval = HAL_ERROR;
	  }
	  data = (uint8_t)(sea_pressure / 2) & 0xFF;
	  if (HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x15, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
		  retval = HAL_ERROR;
	  }
return retval;
}

/**
 * @brief	Performs one reading in altimeter mode.
 * @note	Click is set to altimeter mode, one measuring is performed and data
 * 			put into the message queue, then set back into
 * 			pressure mode.
 * 			Register:
 * 			0x26 - control register 1
 * @param	I2C_HandleTypeDef *hi2c1 ... The I2C interface's handle.
 * @param	osMessageQueueId_t data_queueHandle ... Msg queue for altitude data.
 * @retval 	(HAL_StatusTypeDef) ... Returns HAL error in case one operation was unsuccessful.
 */
HAL_StatusTypeDef altitude_read(I2C_HandleTypeDef *hi2c1,
		osMessageQueueId_t data_queueHandle) {
	uint8_t data, alt_lsb, alt_csb, alt_msb, temp_lsb, temp_msb;
	float altitude;
	HAL_StatusTypeDef retval;

	// data: altimeter mode
	data = 0xBB;
	retval = HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY);
	if (read_data(hi2c1, &alt_lsb, &alt_csb, &alt_msb, &temp_lsb,
			&temp_msb) != HAL_OK) {
		retval = HAL_ERROR;
	}
	altitude = calculate_altitude(alt_lsb, alt_csb, alt_msb);

	data_in_queue("ALT", altitude, data_queueHandle);

	// data: pressure mode
	data = 0x39;
	if (HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
		retval = HAL_ERROR;
	}

	return retval;
}

/**
 * @brief	Wrapper function for message queue put.
 * @note	Puts mode and value in message struct and the struct into the queue.
 * @param	char *mode ... P, TP, ALT for pressure/temperature/altitude data.
 * @param	float value ... Measured value .
 * @param	osMessageQueueId_t data_queueHandle ... Msg queue for pressure/temperature/altitude data.
 * @retval ... None.
 */
void data_in_queue (char *mode, float value, osMessageQueueId_t data_queueHandle) {
	message msg;
    strcpy(msg.mode, mode);
    msg.value = value;
    osMessageQueuePut(data_queueHandle, &msg, 0, osWaitForever);
}
