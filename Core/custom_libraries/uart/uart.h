/**
 * @file    uart.h
 * @author  Volker Tenta, Patrick Schmitt
 * @version 0.0.1
 * @date    2024-04-29
 *
 * @brief This library features a few useful function for handling data via the UART interface.
 *
 *  WARNING! THIS IS A DEMO IMPLEMENTATION! DOES NOT FEATURE ALL ERROR HANDLING / CHECKS!!!
 *
 *  This is only a starting point for your own implementations!
 *
 *  Use this with care - and extend its functionality accordingly!
 *
 */
/**************************************************************************/
#ifndef CUSTOM_MIDDLEWARE_00_UART_LIB_UART_H
#define CUSTOM_MIDDLEWARE_00_UART_LIB_UART_H

#include "stdio.h"
#include <string.h>
#include <stdbool.h>
#include "usart.h"

#define MAX_BUFFER_SIZE 255
#define UART_OVERFLOW "UART OVERFLOW"

bool check_for_buffer_overflow(UART_HandleTypeDef *huart);
void clear_buffer_overflow(UART_HandleTypeDef *huart);

#endif
