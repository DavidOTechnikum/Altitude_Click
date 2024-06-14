/**
 * @file    wifi_task.c
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

#include "../custom_libraries/wifi/wifi.h"

#include "../custom_libraries/uart/uart.h"

extern uint8_t rxBuffer_uart_1[MAX_BUFFER_SIZE];
extern volatile bool is_uart_success;
extern uint8_t uart_buffer[MAX_BUFFER_SIZE];
extern osTimerId_t led_timerHandle;
extern bool no_tcp_connection;



// Disclaimer: ESP32 command variables and buffer variable copied from lecturer's demo code.
uint8_t at_cmd[] = "AT\r\n";
uint8_t restore_factory[] = "AT+RESTORE\r\n";
uint8_t cwstate_cmd[] = "AT+CWSTATE?\r\n";
uint8_t rst_cmd[] = "AT+RST\r\n";
uint8_t set_cwmode_cmd[]  = "AT+CWMODE=1\r\n";
uint8_t set_cwsap_cmd[]  = "AT+CWSAP=\"ESP_SSID\",\"1234567890\",5,3\r\n";
uint8_t get_cwsap_cmd[] = "AT+CWSAP?\r\n";
uint8_t connect_to_ap[] = "AT+CWJAP=\"SHWM97\",\"1234567890\"\r\n";
uint8_t connect_to_TCP[] = "AT+CIPSTART=\"TCP\",\"192.168.9.1\",5000\r\n";
uint8_t get_ip_cmd[] = "AT+CIFSR\r\n";


/**
 * @brief Transmits a command to the Wifi Click using UART1
 *
 * This function transmits a given command by invoking HAL_UART_Transmit with HAL_MAX_DELAY (blocking)
 * It has a simple error check which in case of an error prints a failure message. It does not really handle the error.
 *
 * @param uint8_t *cmd A pointer to the stored command data.
 * @param uint8_t cmd_size The size in bytes of the to be transfered CMD
 *
 * @return HAL_StatusTypeDef This function returns the status code of the HAL_UART_Transmit function.
 */
HAL_StatusTypeDef send_wifi_command(uint8_t* cmd, uint8_t cmd_size) {
	HAL_StatusTypeDef result = HAL_UART_Transmit(&huart1, cmd, cmd_size, HAL_MAX_DELAY);
	if(result != HAL_OK) {
		printf("Sending %s failed with %d\r\n", cmd, result);
	}
	return result;
}

/**
 * @brief Receives a wifi command by checking on flag "is_uart_success" which gets triggered by the UART receive callback.
 *
 * If this flag is true the data from the UART buffer gets printed using printf. The uart_buffer gets flushed.
 * If the flag is false an error message is printed.
 * In both situations the "is_uart_success" flag is set to false.
 *
 * As you can see this function really just prints the received data from the wifi-click and resets the uart flag and the buffer.
 *
 * @param uint8_t *cmd A pointer to the stored command data.
 *
 * @return void This function does not return a value.
 */
bool receive_wifi_command(uint8_t* cmd) {
	osDelay(400);
	if(is_uart_success == true) {
		printf("\r\nReceived from %s\r\n", cmd);
		printf("%s\r\n", uart_buffer);
		if (strcmp((char*)uart_buffer, "OK") == 0) {
			no_tcp_connection = true;
		}
		memset(uart_buffer, 0, MAX_BUFFER_SIZE);
	}
	else {
		printf("\r\nReceiving %s failed\r\n", cmd);
		return false;
	}
	is_uart_success = false;
	return true;
}

/**
 * @brief Sends a command via TCP at the Wifi click module.
 *
 * @param const char* command A pointer to the stored command data.
 *
 * @return void This function does not return a value.
 */
HAL_StatusTypeDef send_TCP_command(const char* command) {
	char buffer[16];
	HAL_StatusTypeDef result;
	snprintf(buffer, sizeof(buffer), "AT+CIPSEND=%d\r\n", strlen(command));
	result = HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	if (result != HAL_OK) {
		return result;
	}
	osDelay(1000);  // Delay for CIPSEND ready signal
	result = HAL_UART_Transmit(&huart1, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
	if (!receive_wifi_command((uint8_t*)buffer)) {
		result = HAL_ERROR;
	}
	return result;
}

/**
 * @brief A function which checks the response message of the wifi-click during a TCP connect attempt.
 * This function is only used during the TCP connect phase to capture the "CONNECT" message of the Wifi-click.
 *
 * This only works for this particular situation. A better solution would be to independently monitor the data received by
 * wifi-click to react to other messages as well.
 *
 * @param uint8_t* response A pointer to a buffer which holds the response message of the wifi-click.
 *
 * @return void This function does not return a value.
 */
bool check_wifi_response(uint8_t* response){
	bool result = true;
	HAL_Delay(200);
	if(is_uart_success == true) {
		printf("\r\nReceived %s\r\n", uart_buffer);
		memcpy(response,uart_buffer, strlen((char*)uart_buffer));
		memset(uart_buffer, 0, MAX_BUFFER_SIZE);
	}
	else {
		result = false;
		printf("\r\nReceiving failed\r\n");
	}
	is_uart_success = false;
	return result;
}

bool wifi_init_stage_and_TCP_check(uint8_t* cmd) {
	bool result = true;
	if (send_wifi_command(cmd, strlen((char*)cmd)) == HAL_OK) {
		printf("%s sent\n", cmd);
		if (!receive_wifi_command(cmd)) {
			result = false;
		}
	} else {
		result = false;
	}
	return result;
}

bool wifi_init() {
	bool success_check = false;
	bool response_check = false;
	uint8_t response_buffer[MAX_BUFFER_SIZE];

	clear_buffer_overflow(&huart1);
	clear_buffer_overflow(&huart2);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer_uart_1, MAX_BUFFER_SIZE);

	do {
		success_check = wifi_init_stage_and_TCP_check(restore_factory);
	} while (!success_check);
	osDelay(3 * WIFI_DELAY);

	do {
		success_check = wifi_init_stage_and_TCP_check(rst_cmd);
	} while (!success_check);
	osDelay(WIFI_DELAY);

	do {
		success_check = wifi_init_stage_and_TCP_check(set_cwmode_cmd);
	} while (!success_check);
	osDelay(WIFI_DELAY);

	do {
		success_check = wifi_init_stage_and_TCP_check(connect_to_ap);
	} while (!success_check);
	osDelay(4 * WIFI_DELAY);

	//		send_wifi_command(at_cmd, ARRAY_SIZE(at_cmd));
	//		receive_wifi_command(at_cmd);

	while (!response_check) {
		if (send_wifi_command(connect_to_TCP, ARRAY_SIZE(connect_to_TCP))
				== HAL_OK) {
			if (!check_wifi_response(response_buffer)) {
				continue;
			}
			if (strstr((char*) response_buffer, "CONNECT")) {
				response_check = true;
				memset(response_buffer, 0, MAX_BUFFER_SIZE);
			}
		}
		osDelay(SECOND);
	}
	no_tcp_connection = false;

	return success_check;
}
