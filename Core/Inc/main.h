/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define MODE_LEN 4

typedef struct {
	char mode[MODE_LEN];
	float value;
} message;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
//#define NO_WIFI
#define DEBUGGING
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define WIFI_CS_Pin GPIO_PIN_0
#define WIFI_CS_GPIO_Port GPIOB
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
//// MPL3115A2 register definitions
//#define _OUT_P_MSB   0x01    // Pressure/Altitude data (MSB)
//#define _OUT_P_CSB   0x02    // Pressure/Altitude data (middle)
//#define _OUT_P_LSB   0x03    // Pressure/Altitude data (LSB)
//#define _OUT_T_MSB   0x04    // Temperature data (MSB)
//#define _OUT_T_LSB   0x05    // Temperature data (LSB)
//#define _PT_DATA_CFG 0x13    // Data event flag configuration
//#define _BAR_IN_MSB  0x14    // Barometric input for Altitude calculation
//#define _BAR_IN_LSB  0x15    // Barometric input for Altitude calculation
//#define _CTRL_REG1   0x26    // Control register 1
//#define _OFFH        0x2D    // Altitude Data User Offset Register
//#define _MPL3115A2_W_ADDRESS  0xC0
//#define _MPL3115A2_R_ADDRESS  0xC1

#define DEVICE_ID 1

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
