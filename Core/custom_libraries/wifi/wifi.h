/**
 * Disclaimer: 	Some of the functions are work by the lecturers,
 * 				some originate in the lecturers' work.
 * 				Details are specified in the comments.
 *
 * @file    wifi_task.h
 * @author  David Oberleitner/Volker Tenta, Patrick Schmitt
 * @version 1.0
 * @date    2024-06-15
 *
 * @brief 	Library with WIFI Click functions.
 *
 * 			This library contains the basic functions of the Mikroe WIFI Click
 * 			featuring an Espressif ESP32 WIFI and Bluetooth module as
 * 			well as operational functions.
 *
 */

#ifndef CUSTOM_LIBRARIES_WIFI_WIFI_H
#define CUSTOM_LIBRARIES_WIFI_WIFI_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "cmsis_os2.h"
#include "usart.h"

#include "../custom_libraries/uart/uart.h"


#define WIFI_TASK_NAME "wifi_task"
#define WIFI_TASK_SIZE 512 * 4
#define WIFI_TASK_PRIORITY osPriorityNormal
#define WIFI_DELAY 5000
#define SECOND 1000
#define RETRY 10
#define MESSAGE_LEN 25
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


HAL_StatusTypeDef send_wifi_command(uint8_t* cmd, uint8_t cmd_size);
bool receive_wifi_command(uint8_t* cmd);
HAL_StatusTypeDef send_TCP_command(const char* command);
bool check_wifi_response(uint8_t* response);
bool wifi_init_stage_and_TCP_check(uint8_t* cmd);
bool wifi_init();
void build_message(message received , char *sendmsg);

#endif
