/**
 * Disclaimer: 	Some of the functions are work by the lecturers,
 * 				some originate in the lecturers' work.
 * 				Details are specified in the comments.
 *
 * @file    wifi_task.c
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

#include "../custom_libraries/wifi/wifi.h"
#include "../custom_libraries/uart/uart.h"

extern uint8_t rxBuffer_uart_1[MAX_BUFFER_SIZE];
extern volatile bool is_uart_success;
extern uint8_t uart_buffer[MAX_BUFFER_SIZE];
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
 * Disclaimer: Function entirely taken from lecturers' code.
 *
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
 * Disclaimer: 	Function taken from lecturers' code, extended by boolean return value for
 * 				error handling and no_tcp_connection flag.
 *
 * @brief Receives a wifi command by checking on flag "is_uart_success" which gets triggered by the UART receive callback.
 *
 * If this flag is true the data from the UART buffer gets printed using printf. The uart_buffer gets flushed.
 * If the flag is false an error message is printed.
 * In both situations the "is_uart_success" flag is set to false.
 *
 * In case the message is "OK", the no_tcp_connection flag is set to true,
 * which indicates to the wifi monitor task, that it has to reinitialize.
 *
 *
 * @param uint8_t *cmd A pointer to the stored command data.
 *
 * @retval (bool) ... Returns true if a message has been received via uart,
 * 					  false in case of no message.
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
 * Disclaimer: 	Function taken from lecturers' code, extended by
 * 				HAL status for error handling.
 *
 * @brief Sends a command via TCP at the Wifi click module.
 *
 * @param const char* command A pointer to the stored command data.
 *
 * @retval (HAL_StatusTypeDef) ... Returns HAL error in case one transmitting
 * 								   operation has been unsuccessful.
 */
HAL_StatusTypeDef send_TCP_command(const char* command) {
	char buffer[16];
	HAL_StatusTypeDef result;
	snprintf(buffer, sizeof(buffer), "AT+CIPSEND=%d\r\n", strlen(command));
	result = HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	if (result != HAL_OK) {
		return result;
	}
	osDelay(SECOND);  // Delay for CIPSEND ready signal
	result = HAL_UART_Transmit(&huart1, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
	if (!receive_wifi_command((uint8_t*)buffer)) {
		result = HAL_ERROR;
	}
	return result;
}

/**
 * Disclaimer: 	Function taken from lecturers' code, extended by
 * 				return value for error handling.
 *
 * @brief A function which checks the response message of the wifi-click during a TCP connect attempt.
 * This function is only used during the TCP connect phase to capture the "CONNECT" message of the Wifi-click.
 *
 * @param uint8_t* response A pointer to a buffer which holds the response message of the wifi-click.
 *
 * @retval 	(bool) ... Returns false in case no message has been received.
 */
bool check_wifi_response(uint8_t* response){
	bool result = true;
	osDelay(200);
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

/**
 * @brief 	Wrapper function for send_wifi_command used in the initialization
 * 			stages and also when checking for active TCP connection.
 * @note	Each stage in the initialization process calls one command.
 * 			This function checks for errors.
 * @param 	uint8_t* cmd ... WIFI command.
 * @retval 	(bool) ... returns true in case command has been sent, false
 * 						in case of unsuccessful operation.
 */
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

/**
 * Disclaimer: Structure and basic init stages have been taken
 * 				from lecturers' code. Extended by error handling and
 * 				refactored.
 *
 * @brief 	WIFI Click initialization function.
 * @note 	Function calls the command one by one, which turn on and
 * 			configure the WIFI Click. Each step is controlled and
 * 			checked, if sending has been successful.
 * 			In the end, only a successful TCP connection to
 * 			base station server is checked. In case there is
 * 			no connection, initialization is restarted by WIFI monitor task.
 * 			TCP connection check is only repeated 10 (RETRY define) times.
 * 			In case one of the steps before has not been successful, the
 * 			program will not be stuck in the TCP connection check loop.
 * @retval 	(bool) ... Sending the config messages of each stage is checked as
 * 					   well as TCP connection. Returns false in case of
 * 					   error, true in case of success.
 */
bool wifi_init() {
	bool success_check = false;
	bool response_check = false;
	uint8_t response_buffer[MAX_BUFFER_SIZE];
	uint8_t i;

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

	i = 0;
	while (!response_check) {
		if (i >= RETRY) {
			return false;
		}
		if (send_wifi_command(connect_to_TCP, ARRAY_SIZE(connect_to_TCP))
				== HAL_OK) {
			if (!check_wifi_response(response_buffer)) {
				i++;
				continue;
			}
			if (strstr((char*) response_buffer, "CONNECT")) {
				response_check = true;
				memset(response_buffer, 0, MAX_BUFFER_SIZE);
			}
		}
		osDelay(SECOND);
		i++;
	}
	no_tcp_connection = false;

	return success_check;
}

/**
 * @brief 	Function builds message TCP send with sensor values.
 * @note	Message for base station is built here.
 * 			Format: "<device id>,<physical quantity>,<value in float>;"
 * 			Modes: P for pressure, TP for temperature (indicating that
 * 			this is the temperature measurement by this station), ALT
 * 			for altitude measurement.
 * 			Pressure and altitude have two decimal places, temperature just one.
 * @param 	message received ... Message struct from queue containing sensor values.
 * @param 	char *sendmsg ... Message to be sent to base station via TCP.
 * @retval 	None.
 */
void build_message(message received, char *sendmsg) {
	uint8_t device_id = DEVICE_ID;
	if (strcmp(received.mode, "P") == 0 || strcmp(received.mode, "ALT") == 0) {
		snprintf(sendmsg, MESSAGE_LEN, "%d,%s,%.2f;\r\n", device_id,
				received.mode, received.value);
	} else if (strcmp(received.mode, "TP") == 0) {
		snprintf(sendmsg, MESSAGE_LEN, "%d,%s,%.1f;\r\n", device_id,
				received.mode, received.value);
	}
	printf(sendmsg);
}
