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
#include "../custom_libraries/helper_functions/helper_functions.h"
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
#define ALT_DELAY 10000
#define WIFI_WAIT 1000
#define ERROR_VALUE -3333


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

// Disclaimer: ESP32 command variables and buffer variable copied from lecturer's demo code.
uint8_t at_cmd[] = "AT\r\n";
uint8_t restore_factory[] = "AT+RESTORE\r\n";
uint8_t cwstate_cmd[] = "AT+CWSTATE?\r\n";
uint8_t rst_cmd[] = "AT+RST\r\n";
uint8_t set_cwmode_cmd[]  = "AT+CWMODE=1\r\n";
uint8_t set_cwsap_cmd[]  = "AT+CWSAP=\"ESP_SSID\",\"1234567890\",5,3\r\n";
uint8_t get_cwsap_cmd[] = "AT+CWSAP?\r\n";
uint8_t connect_to_ap[] = "AT+CWJAP=\"ESP_SSID\",\"1234567890\"\r\n";
uint8_t connect_to_TCP[] = "AT+CIPSTART=\"TCP\",\"192.168.9.1\",5000\r\n";
uint8_t get_ip_cmd[] = "AT+CIFSR\r\n";
uint8_t obtain_tcp_connection[] = "AT+CIPSTATE?\r\n";

extern uint8_t rxBuffer_uart_1[MAX_BUFFER_SIZE];

bool wifi_on = false;

bool ok_bool = false;
/* USER CODE END Variables */
/* Definitions for monitor_task */
osThreadId_t monitor_taskHandle;
uint32_t defaultTaskBuffer[ 4000 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t monitor_task_attributes = {
  .name = "monitor_task",
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
/* Definitions for wifi_monitor_ta */
osThreadId_t wifi_monitor_taHandle;
uint32_t wifi_monitor_taBuffer[ 1000 ];
osStaticThreadDef_t wifi_monitor_taControlBlock;
const osThreadAttr_t wifi_monitor_ta_attributes = {
  .name = "wifi_monitor_ta",
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

void start_monitor_task(void *argument);
void start_data_task(void *argument);
void start_wifi_monitor_task(void *argument);
void start_sender_task(void *argument);

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
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of monitor_task */
  monitor_taskHandle = osThreadNew(start_monitor_task, NULL, &monitor_task_attributes);

  /* creation of data_task */
  data_taskHandle = osThreadNew(start_data_task, NULL, &data_task_attributes);

  /* creation of wifi_monitor_ta */
  wifi_monitor_taHandle = osThreadNew(start_wifi_monitor_task, NULL, &wifi_monitor_ta_attributes);

  /* creation of sender_task */
  sender_taskHandle = osThreadNew(start_sender_task, NULL, &sender_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_start_monitor_task */
/**
  * @brief  Function implementing the monitor_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_monitor_task */
void start_monitor_task(void *argument)
{
  /* USER CODE BEGIN start_monitor_task */
    uint8_t sysmod = 0;
	 uint8_t data;

	 for (;;) {
	 if (sysmod == 0)
		 {
		 printf("setup \n");

		 if (calibration(&hi2c1) != HAL_OK) {
			 continue;
		 }
		 if (altitude_read(&hi2c1, data_queueHandle) != HAL_OK) {
			 continue;
		 }
	  data = 0x39;
	  HAL_I2C_Mem_Write(&hi2c1, 0xC0, 0x26, 1, &data, 1, HAL_MAX_DELAY);
		 }
	 osDelay(ALT_DELAY);
	    HAL_I2C_Mem_Read(&hi2c1, 0xC0, 0x11, 1, &sysmod, 1, HAL_MAX_DELAY);			// check system status: if 0, not active -> start and calibrate
	 }
  /* USER CODE END start_monitor_task */
}

/* USER CODE BEGIN Header_start_data_task */
/**
* @brief Function implementing the data_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_data_task */
void start_data_task(void *argument)
{
  /* USER CODE BEGIN start_data_task */
  /* Infinite loop */

	uint8_t low_byte, high_byte, higher_byte;
    uint8_t temp_msb, temp_lsb;
    uint8_t sysmod = 0;
    float pressure = 0;
    float temp = 0;

while(wifi_on == false) {
	osDelay(DATA_DELAY);
}
	 for(;;) {
		 osDelay(DATA_DELAY);
		 HAL_I2C_Mem_Read(&hi2c1, 0xC0, 0x11, 1, &sysmod, 1, HAL_MAX_DELAY);
		 if (sysmod == 1) {

			if (read_data (&hi2c1, &low_byte, &high_byte, &higher_byte, &temp_lsb, &temp_msb) == HAL_OK) {
			    pressure = calculate_pressure(low_byte, high_byte, higher_byte);
			    temp = calculate_temperature (temp_lsb, temp_msb);
			} else {
				pressure = ERROR_VALUE;
				temp = ERROR_VALUE;
			}

			data_in_queue("C", temp, data_queueHandle);
			data_in_queue("Pa", pressure, data_queueHandle);
		 }
	 }

  /* USER CODE END start_data_task */
}

/* USER CODE BEGIN Header_start_wifi_monitor_task */
/**
* @brief Function implementing the wifi_monitor_ta thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_wifi_monitor_task */
void start_wifi_monitor_task(void *argument)
{
  /* USER CODE BEGIN start_wifi_monitor_task */
  /* Infinite loop */
/*
 * State-Machine:
 *
 * WIFI-Status?
 *
 * 1) einschalten/verbinden
 * 		Timer
 * 2) Verbindung ok? -> Timer
 *
 * (Debuggen: ok-Nachricht an printf)
 *
 */
wifi_on = true;
	for (;;) {
//		clear_buffer_overflow(&huart1);
//		clear_buffer_overflow(&huart2);
//		HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer_uart_1, MAX_BUFFER_SIZE);
//
//	uint8_t response_check = 0;
//	uint8_t response_buffer[MAX_BUFFER_SIZE];
//
//
//	if(send_wifi_command(restore_factory, ARRAY_SIZE(restore_factory)) == HAL_OK) {
//		printf("RESTORE sent\n");
//		receive_wifi_command(restore_factory);
//	}
//
//	osDelay(15000);
//
//	if(send_wifi_command(rst_cmd, ARRAY_SIZE(rst_cmd)) == HAL_OK) {
//			printf("RST sent\n");
//			receive_wifi_command(rst_cmd);
//	}
//	osDelay(5000);
//
//	if(send_wifi_command(set_cwmode_cmd, ARRAY_SIZE(set_cwmode_cmd)) == HAL_OK) {
//				printf("CWMODE sent\n");
//				receive_wifi_command(set_cwmode_cmd);
//
//	}
//	osDelay(5000);
//
//	if(send_wifi_command(connect_to_ap, ARRAY_SIZE(connect_to_ap)) == HAL_OK) {
//				printf("CONNECT sent\n");
//				receive_wifi_command(connect_to_ap);
//
//	}
//
//	HAL_Delay(20000);
//	send_wifi_command(at_cmd, ARRAY_SIZE(at_cmd));
//	receive_wifi_command(at_cmd);
//
//	while(!response_check){
//		if(send_wifi_command(connect_to_TCP, ARRAY_SIZE(connect_to_TCP)) == HAL_OK) {
//			check_wifi_response(response_buffer);
//			if (strstr((char*)response_buffer, "CONNECT")) {
//				response_check = 1;
//				memset(response_buffer, 0, MAX_BUFFER_SIZE);
//			}
//		}
//		osDelay(1000);
//	}
//	response_check = 0;
//wifi_on = true;
//ok_bool = false;
//
//
//	for(;;)
//	{
//		if(send_wifi_command(obtain_tcp_connection, ARRAY_SIZE(obtain_tcp_connection)) == HAL_OK) {
//			printf("OBTAIN TCP CONNECTION sent\n");
//			receive_wifi_command(obtain_tcp_connection);
//		}
//		if(ok_bool) {
//			break;
//		}
//
//    osDelay(10000);
//	}

	}
  /* USER CODE END start_wifi_monitor_task */
}

/* USER CODE BEGIN Header_start_sender_task */
/**
* @brief Function implementing the sender_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_sender_task */
void start_sender_task(void *argument)
{
  /* USER CODE BEGIN start_sender_task */
	message received;
	char sendmsg[20];
  /* Infinite loop */
  for(;;)
  {
	  if (wifi_on == true) {
		  if (osMessageQueueGet	(data_queueHandle, &received, NULL, osWaitForever) ==  osOK) {
			  if (strcmp(received.unit, "Pa") == 0) {
				  snprintf(sendmsg, sizeof(sendmsg), "P:%.2f\r\n", received.value);
			  } else if (strcmp(received.unit, "C")== 0) {
				  snprintf(sendmsg, sizeof(sendmsg), "TP:%.1f\r\n", received.value);
			  } else if (strcmp(received.unit, "m") == 0) {
				  snprintf(sendmsg, sizeof(sendmsg), "ALT:%.2f\r\n", received.value);
			  }
			  printf(sendmsg);
	//		  sendTCPCommand(sendmsg);
		  }
	  } else {
		  osDelay(WIFI_WAIT);
	  }
  }
  /* USER CODE END start_sender_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

