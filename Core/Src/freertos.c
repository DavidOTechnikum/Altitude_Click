/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "i2c.h"
#include <stdbool.h>
#include <math.h>

#include "../custom_libraries/altitude_click/altitude.h"
#include "../custom_libraries/uart/uart.h"
#include "../custom_libraries/wifi/wifi.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DATA_DELAY 5000
#define POLLING_DELAY 5000
#define ALT_DELAY 10000
#define WIFI_WAIT 1000
#define ERROR_VALUE -3333

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern uint8_t rxBuffer_uart_1[MAX_BUFFER_SIZE];
bool wifi_on = false;
bool no_tcp_connection = false;
uint8_t obtain_tcp_connection[] = "AT+CIPSTATE?\r\n";

/* USER CODE END Variables */
/* Definitions for alt_mon_task */
osThreadId_t alt_mon_taskHandle;
uint32_t defaultTaskBuffer[ 4000 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t alt_mon_task_attributes = {
  .name = "alt_mon_task",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for data_task */
osThreadId_t data_taskHandle;
uint32_t data_taskBuffer[ 1000 ];
osStaticThreadDef_t data_taskControlBlock;
const osThreadAttr_t data_task_attributes = {
  .name = "data_task",
  .cb_mem = &data_taskControlBlock,
  .cb_size = sizeof(data_taskControlBlock),
  .stack_mem = &data_taskBuffer[0],
  .stack_size = sizeof(data_taskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for wifi_mon_task */
osThreadId_t wifi_mon_taskHandle;
uint32_t wifi_monitor_taBuffer[ 1000 ];
osStaticThreadDef_t wifi_monitor_taControlBlock;
const osThreadAttr_t wifi_mon_task_attributes = {
  .name = "wifi_mon_task",
  .cb_mem = &wifi_monitor_taControlBlock,
  .cb_size = sizeof(wifi_monitor_taControlBlock),
  .stack_mem = &wifi_monitor_taBuffer[0],
  .stack_size = sizeof(wifi_monitor_taBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sender_task */
osThreadId_t sender_taskHandle;
uint32_t sender_taskBuffer[ 1000 ];
osStaticThreadDef_t sender_taskControlBlock;
const osThreadAttr_t sender_task_attributes = {
  .name = "sender_task",
  .cb_mem = &sender_taskControlBlock,
  .cb_size = sizeof(sender_taskControlBlock),
  .stack_mem = &sender_taskBuffer[0],
  .stack_size = sizeof(sender_taskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for data_queue */
osMessageQueueId_t data_queueHandle;
const osMessageQueueAttr_t data_queue_attributes = {
  .name = "data_queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void altitude_monitor_task_func(void *argument);
void data_task_func(void *argument);
void wifi_monitor_task_func(void *argument);
void sender_task_func(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of data_queue */
  data_queueHandle = osMessageQueueNew (16, sizeof(message), &data_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */

  if (data_queueHandle == NULL) {
	  printf("data_queue not created\n");
	  Error_Handler();
  }

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of alt_mon_task */
  alt_mon_taskHandle = osThreadNew(altitude_monitor_task_func, NULL, &alt_mon_task_attributes);

  /* creation of data_task */
  data_taskHandle = osThreadNew(data_task_func, NULL, &data_task_attributes);

  /* creation of wifi_mon_task */
  wifi_mon_taskHandle = osThreadNew(wifi_monitor_task_func, NULL, &wifi_mon_task_attributes);

  /* creation of sender_task */
  sender_taskHandle = osThreadNew(sender_task_func, NULL, &sender_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  if (alt_mon_taskHandle == NULL) {
  	printf("altitude monitoring task not created\n");
  	Error_Handler();
  }

  if (data_taskHandle == NULL) {
  	printf("data task not created\n");
  	Error_Handler();
  }

  if (wifi_mon_taskHandle == NULL) {
  	printf("wifi monitoring task not created\n");
  	Error_Handler();
  }

  if (sender_taskHandle == NULL) {
  	printf("sender task not created\n");
  	Error_Handler();
  }

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_altitude_monitor_task_func */
/**
  * @brief  Monitoring task for Altitude Click.
  * @note	This task initializes the Altitude Click and performs one
  * 		altitude read before staying in pressure mode.
  * 		It then checks the status (sysmod) periodically and will
  * 		restart the initialization, if the device is not active.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_altitude_monitor_task_func */
void altitude_monitor_task_func(void *argument)
{
  /* USER CODE BEGIN altitude_monitor_task_func */
	uint8_t sysmod = 0;

  /* Infinite loop */

	for (;;) {
		if (sysmod == 0) {
			printf("setup \n");

			if (calibration(&hi2c1) != HAL_OK) {
				continue;
			}
			if (altitude_read(&hi2c1, data_queueHandle) != HAL_OK) {
				continue;
			}
		}
		// check altitude click status (0 = not active)
		osDelay(ALT_DELAY);
		HAL_I2C_Mem_Read(&hi2c1, 0xC0, 0x11, 1, &sysmod, 1, HAL_MAX_DELAY);
	}


  /* USER CODE END altitude_monitor_task_func */
}

/* USER CODE BEGIN Header_data_task_func */
/**
* @brief 	Task reads from Altitude Click and sends the data to
* 			sender task via message queue.
* @note		Task will wait for WIFI Click to be started.
* 			It is set to polling every 5 seconds (POLLING_DELAY).
* 			Each read gets pressure and temperature value.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_data_task_func */
void data_task_func(void *argument)
{
  /* USER CODE BEGIN data_task_func */
	uint8_t low_byte, high_byte, higher_byte;
	uint8_t temp_msb, temp_lsb;
	uint8_t sysmod = 0;
	float pressure = 0;
	float temp = 0;

	/* Infinite loop */
	while (wifi_on == false) {
		osDelay(DATA_DELAY);
	}
	for (;;) {
		osDelay(POLLING_DELAY);
		HAL_I2C_Mem_Read(&hi2c1, 0xC0, 0x11, 1, &sysmod, 1, HAL_MAX_DELAY);
		if (sysmod == 1) {

			if (read_data(&hi2c1, &low_byte, &high_byte, &higher_byte,
					&temp_lsb, &temp_msb) == HAL_OK) {
				pressure = calculate_pressure(low_byte, high_byte, higher_byte);
				temp = calculate_temperature(temp_lsb, temp_msb);
			} else {
				pressure = ERROR_VALUE;
				temp = ERROR_VALUE;
			}

			data_in_queue("TP", temp, data_queueHandle);
			data_in_queue("P", pressure, data_queueHandle);
		}
	}
  /* USER CODE END data_task_func */
}

/* USER CODE BEGIN Header_wifi_monitor_task_func */
/**
* @brief 	Task starts WIFI Click and monitors it.
* @note 	Task will start initialization of WIFI Click and
* 			TCP connection to base station. Once a
* 			connection is established, it will be checked periodically.
* 			If connection is over, WIFI Click will reinitialize.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_wifi_monitor_task_func */
void wifi_monitor_task_func(void *argument)
{
  /* USER CODE BEGIN wifi_monitor_task_func */
	bool success_check = false;
	uint8_t i;
  /* Infinite loop */
	// Next line: delete '//' for troubleshooting
	//wifi_on = true;
	for (;;) {
		wifi_on = wifi_init();
		if (!wifi_on) {
			continue;
		}

		for (;;) {
			i = 0;
			do {
				success_check = wifi_init_stage_and_TCP_check(
						obtain_tcp_connection);
				i++;
			} while (!success_check && i < RETRY);
			if (no_tcp_connection || i == RETRY) {
				no_tcp_connection = false;
				break;
			}
			osDelay(2 * WIFI_DELAY);
		}
	}


  /* USER CODE END wifi_monitor_task_func */
}

/* USER CODE BEGIN Header_sender_task_func */
/**
* @brief 	Sender task gets data from sensor and sends TCP message.
* @note 	This task waits for a message struct in the message queue.
* 			It then builds the TCP message and sends it to the
* 			base station.
* 			Sending will be retried if unsuccesful, but
* 			only 5 (RETRY/2) times. Since the program polls
* 			every 5 seconds, single values are not so important
* 			and will be discarded so as not to block the task.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_sender_task_func */
void sender_task_func(void *argument)
{
  /* USER CODE BEGIN sender_task_func */
	message received;
	char sendmsg[MESSAGE_LEN];
	HAL_StatusTypeDef TCP_success;
	uint8_t i;
	/* Infinite loop */
	for (;;) {
		if (wifi_on == true) {
			if (osMessageQueueGet(data_queueHandle, &received, NULL,
					osWaitForever) == osOK) {
				build_message(received, sendmsg);

				i = 0;
			do {
				TCP_success = send_TCP_command(sendmsg);
				i++;
			} while (TCP_success != HAL_OK && i < RETRY/2);

			}
		} else {
			osDelay(WIFI_WAIT);
		}
	}
  /* USER CODE END sender_task_func */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

