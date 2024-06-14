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
extern bool ok_bool;

osThreadId_t wifiTaskHandle;
const osThreadAttr_t wifiTask_attributes = {
		.name = WIFI_TASK_NAME,
		.stack_size = WIFI_TASK_SIZE,
		.priority = (osPriority_t) WIFI_TASK_PRIORITY,
};

//Those variable store the commands of the WiFi-Click which are used in this demo.
//uint8_t at_cmd[] = "AT\r\n";
//uint8_t restore_factory[] = "AT+RESTORE\r\n";
//uint8_t cwstate_cmd[] = "AT+CWSTATE?\r\n";
//uint8_t rst_cmd[] = "AT+RST\r\n";
//uint8_t set_cwmode_cmd[]  = "AT+CWMODE=1\r\n";
//uint8_t set_cwsap_cmd[]  = "AT+CWSAP=\"ESP_SSID\",\"1234567890\",5,3\r\n";
//uint8_t get_cwsap_cmd[] = "AT+CWSAP?\r\n";
//uint8_t connect_to_ap[] = "AT+CWJAP=\"ESP_SSID\",\"1234567890\"\r\n";
//uint8_t connect_to_TCP[] = "AT+CIPSTART=\"TCP\",\"192.168.9.1\",5000\r\n";
//uint8_t get_ip_cmd[] = "AT+CIFSR\r\n";


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
void receive_wifi_command(uint8_t* cmd) {
	osDelay(400);
	if(is_uart_success == true) {
		printf("\r\nReceived from %s\r\n", cmd);
		printf("%s\r\n", uart_buffer);
		if (strcmp(uart_buffer, "OK") == 0) {
			ok_bool = true;
		}
		memset(uart_buffer, 0, MAX_BUFFER_SIZE);
	}
	else {
		printf("\r\nReceiving %s failed\r\n", cmd);
	}
	is_uart_success = false;
}

/**
 * @brief Sends a command via TCP at the Wifi click module. There is no error handling here!
 *
 * @param const char* command A pointer to the stored command data.
 *
 * @return void This function does not return a value.
 */
void sendTCPCommand(const char* command) {
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "AT+CIPSEND=%d\r\n", strlen(command));
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	HAL_Delay(1000);  // Delay for CIPSEND ready signal
	HAL_UART_Transmit(&huart1, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
	receive_wifi_command((uint8_t*)buffer);
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
void check_wifi_response(uint8_t* response){
	HAL_Delay(200);
	if(is_uart_success == true) {
		printf("\r\nReceived %s\r\n", uart_buffer);
		memcpy(response,uart_buffer, strlen((char*)uart_buffer));
		memset(uart_buffer, 0, MAX_BUFFER_SIZE);
	}
	else {
		printf("\r\nReceiving failed\r\n");
	}
	is_uart_success = false;
}

/**
 * @brief A function which initializes the wifi-task. It creates the task itself.
 * Clears both UART interfaces (1 and 2) as well as sets the UART1 to receive data.
 *
 * @return void This function does not return a value.
 */
//void init_wifi_task(void) {
//	printf("wifi_task init\r\n");
//
//	wifiTaskHandle = osThreadNew(wifiTask_f, NULL, &wifiTask_attributes);
//
//	if (wifiTaskHandle == NULL) {
//		printf("creating wifi_task_failed\r\n");
//	}
//
//	clear_buffer_overflow(&huart1);
//	clear_buffer_overflow(&huart2);
//	HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer_uart_1, MAX_BUFFER_SIZE);
//}

/**
 * @brief Task function for the wifi-task.
 * During the first call of this task function the wifi-click module gets initialized.
 * "restore_factory" --> set all factory settings to the wifi click to have defined starting point
 * "rst_cmd" --> resets the wifi-click
 * "set_cwmode_cmd" --> sets the wifi-click to act in station mode
 * "connect_to_ap" --> command to establish a connection to the access point (in our case another Wifi-click)
 *
 * Note that the successful execution of those commands is not checked here! --> you will have to implement this!
 *
 * After that we continuously try to establish a TCP connection. How long this will take varies so we check here if we receive the "CONNECT" message.
 * Only if "CONNECT" was received a flag gets set and the code execution continues to the main while forever loop.
 *
 * In this loop the command for turning on/off the LED on the other side gets called repeatedly.
 *
 * @param void *pvParameters Can be used to give the tasks some parameters during execution. Not used here.
 *
 * @return void This function does not return a value.
 */
//void wifiTask_f(void *pvParameters) {
//
//	uint8_t response_check = 0;
//	uint8_t response_buffer[MAX_BUFFER_SIZE];
//
//
//	if(send_wifi_command(restore_factory, ARRAY_SIZE(restore_factory)) == HAL_OK) {
//		receive_wifi_command(restore_factory);
//	}
//	HAL_Delay(15000);
//
//	if(send_wifi_command(rst_cmd, ARRAY_SIZE(rst_cmd)) == HAL_OK) {
//		receive_wifi_command(rst_cmd);
//	}
//	HAL_Delay(5000);
//
//	if(send_wifi_command(set_cwmode_cmd, ARRAY_SIZE(set_cwmode_cmd)) == HAL_OK) {
//		receive_wifi_command(set_cwmode_cmd);
//	}
//	HAL_Delay(5000);
//
//	if(send_wifi_command(connect_to_ap, ARRAY_SIZE(connect_to_ap)) == HAL_OK) {
//		receive_wifi_command(connect_to_ap);
//	}
//	HAL_Delay(20000);
//
//	while(!response_check){
//		if(send_wifi_command(connect_to_TCP, ARRAY_SIZE(connect_to_TCP)) == HAL_OK) {
//			check_wifi_response(response_buffer);
//			if (strstr((char*)response_buffer, "CONNECT")) {
//				response_check = 1;
//				memset(response_buffer, 0, MAX_BUFFER_SIZE);
//			}
//		}
//		HAL_Delay(1000);
//	}
//	response_check = 0;
//
//	while (1) {
//
//		HAL_Delay(1000);
//		sendTCPCommand("on\r\n");  // Send command to turn LED on
//		HAL_Delay(5000);           // Wait 5 seconds
//		sendTCPCommand("off\r\n");  // Send command to turn LED on
//		HAL_Delay(5000);           // Wait 5 seconds
//	}
//}
