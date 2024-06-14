/*
 * altitude.c
 *
 *  Created on: Jun 13, 2024
 *      Author: Dave
 */

#include "../../custom_libraries/altitude_click/altitude.h"


float calculate_pressure (uint8_t low_byte, uint8_t high_byte, uint8_t higher_byte) {
	float pressure = 0;
	int32_t pressure_int = -1;

	if ((low_byte & 0x20) > 0) {
		pressure += 0.5;
	}
	if ((low_byte & 0x10) > 0) {
		pressure += 0.25;
	}

	pressure_int = (int32_t)higher_byte;
	pressure_int = pressure_int << 16;

	pressure_int = pressure_int | ((int32_t)high_byte << 8);
	pressure_int = pressure_int | (int32_t)low_byte;
	pressure_int = pressure_int / 64;

	pressure += (float)pressure_int;

	return pressure;
}

float calculate_temperature (uint8_t temp_lsb, uint8_t temp_msb) {
	float temp = 0;
	int32_t temp_int = -1;
    temp_int = temp_int & (temp_msb << 8);
    temp_int = temp_int | (int32_t)temp_lsb;

    temp_int = temp_int*10/256;
    temp = (float)temp_int/10;
    return temp;
}

float calculate_altitude (uint8_t low_byte, uint8_t high_byte, uint8_t higher_byte) {
	float altitude = 0;
	int32_t altitude_int = -1;

    altitude_int = ((int32_t)higher_byte << 24);
    altitude_int = altitude_int | ((int32_t)high_byte << 16);
    altitude_int = altitude_int | ((int32_t)low_byte << 8);
    altitude = (float)altitude_int/6553500;

    return altitude;
}

HAL_StatusTypeDef read_data (I2C_HandleTypeDef *hi2c1, uint8_t *low_byte, uint8_t *high_byte, uint8_t *higher_byte,
					uint8_t *temp_lsb, uint8_t *temp_msb) {
	HAL_StatusTypeDef retval = HAL_OK;

    if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x03, 1, low_byte, 1, HAL_MAX_DELAY) != HAL_OK) {
    	retval = HAL_ERROR;
    }

    //high_byte = MPL3115A2_Read(_OUT_P_CSB);
    if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x02, 1, high_byte, 1, HAL_MAX_DELAY) != HAL_OK) {
    	retval = HAL_ERROR;
    }

    //higher_byte = MPL3115A2_Read(_OUT_P_MSB);
    if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x01, 1, higher_byte, 1, HAL_MAX_DELAY) != HAL_OK)
    	retval = HAL_ERROR;

    // Temp-Daten:
    if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x04, 1, temp_msb, 1, HAL_MAX_DELAY) != HAL_OK) {
    	retval = HAL_ERROR;
    }

    if (HAL_I2C_Mem_Read(hi2c1, 0xC0, 0x05, 1, temp_lsb, 1, HAL_MAX_DELAY) != HAL_OK) {
    	retval = HAL_ERROR;
    }

    return retval;
}


HAL_StatusTypeDef calibration (I2C_HandleTypeDef *hi2c1) {
	uint8_t low_byte, high_byte, higher_byte;
    uint8_t temp_msb, temp_lsb;
	uint8_t data;
	float calibration_pressure, sea_pressure, current_pressure;

	  // Altitude offset set to 0
	  data = 0x00;
	  HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x2D, 1, &data, 1, HAL_MAX_DELAY);		// 0x2D: altitude offset register; set to 0

	  // Clear value
	  calibration_pressure = 0;

	  // Calculate current pressure level
	  for (uint8_t i = 0; i < 8; i++) {
	    // 0x26: ctrl reg 1, pressure, on, immediate measurement
	    data = 0x3B;	// barometer

	    HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY);

	    //MPL3115A2_Write(_CTRL_REG1, 0b00111001);   // Clear oversampling bit
	    data = 0x39;
	    HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY);

	    osDelay(CALIBRATE_DELAY);                             // Wait for sensor to read pressure

	    if (read_data (hi2c1, &low_byte, &high_byte, &higher_byte, &temp_lsb, &temp_msb) != HAL_OK) {
	    	return HAL_ERROR;
	    }
	    calibration_pressure += calculate_pressure(low_byte, high_byte, higher_byte);
	  }


	  // Find average value of current pressure level readings
	  current_pressure = calibration_pressure / 8;
//
//	  // Calculate barometric pressure at mean sea level based on a starting altitude
	  sea_pressure = current_pressure /pow(1-START_ALTITUDE*0.0000225577, 5.255877);
//
//	  // Calibrate the sensor according to the sea level pressure for the current measurement location (2 Pa per LSB) :
//	  MPL3115A2_Write(_BAR_IN_MSB, (unsigned int)(sea_pressure / 2) >> 8);
	  data = (uint8_t)(sea_pressure / 2) >> 8;
	  HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x14, 1, &data, 1, HAL_MAX_DELAY);
//	  MPL3115A2_Write(_BAR_IN_LSB, (unsigned int)(sea_pressure / 2) & 0xFF);
	  data = (uint8_t)(sea_pressure / 2) & 0xFF;
	  HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x15, 1, &data, 1, HAL_MAX_DELAY);

return HAL_OK;
}


HAL_StatusTypeDef altitude_read (I2C_HandleTypeDef *hi2c1, osMessageQueueId_t data_queueHandle) {
	uint8_t data, low_byte, high_byte, higher_byte, temp_lsb, temp_msb;
	float altitude;

	  data = 0xBB;	// altimeter
	  HAL_I2C_Mem_Write(hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY);
	  if (read_data (hi2c1, &low_byte, &high_byte, &higher_byte, &temp_lsb, &temp_msb) != HAL_OK) {
		  return HAL_ERROR;
	  }
	  altitude = calculate_altitude(low_byte, high_byte, higher_byte);

	  data_in_queue("m", altitude, data_queueHandle);

	    return HAL_OK;
}

void data_in_queue (char *unit, float value, osMessageQueueId_t data_queueHandle) {
	message msg;
    strcpy(msg.unit, unit);
    msg.value = value;
    osMessageQueuePut(data_queueHandle, &msg, 0, osWaitForever);
}
