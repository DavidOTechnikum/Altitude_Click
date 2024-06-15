/**
 * @file    wifi_task.h
 * @author  Volker Tenta, Patrick Schmitt
 * @version 0.0.1
 * @date    2024-04-29
 *
 * @brief This file contains functions for the wifi-task implementation. Station-Mode.
 * Within this mode the WiFi-Click will connect to another WiFi click in access point mode (or any other AP)
 *
 * This very simple Demo initializes the Wifi click in station-mode and auto connects the click to another wifi-click which is in AP mode.
 * For this procedure the application uses simple delays which is really not ideal - we should check here for the replies of the Wifi-Click!
 * Remember that this is only a Demo and not everything is 100% finished. It should only give you a brief starting point for your own implementations!
 *
 * Very simple TCP commands are used to turn on/off an LED at the "server" side (Wifi Click in AP mode)
 *
 *  WARNING! THIS IS A DEMO IMPLEMENTATION! DOES NOT FEATURE ALL ERROR HANDLING / CHECKS!!!
 *
 *  This is only a starting point for your own implementations!
 *
 *  Use this with care - and extend its functionality accordingly!
 *
 */

#ifndef CUSTOM_MIDDLEWARE_03_WIFI_WIFI_H
#define CUSTOM_MIDDLEWARE_03_WIFI_WIFI_H

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



#endif //FREERTOS_WIFI_EXAMPLE_WIFI_TASK_H
