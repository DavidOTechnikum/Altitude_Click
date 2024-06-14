/**
 * @file    uart.c
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

#include "../../custom_libraries/uart/uart.h"


uint8_t rxBuffer_uart_1[MAX_BUFFER_SIZE] = {0};
uint8_t uart_buffer[MAX_BUFFER_SIZE] = {0};
volatile bool is_uart_success = false;

extern UART_HandleTypeDef huart1;

/**
 * @brief Checks for a buffer overflow condition in a UART handler
 *
 * This function returns true if the overrun error flag (UART_FLAG_ORE) of
 * the particular UART handler is set, indicating a buffer overflow. Otherwise,
 * it returns false.
 *
 * @param UART_HandleTypeDef *huart A pointer to a UART_HandleTypeDef structure that contains
 *               					the configuration information for the specified UART module.
 * @return bool true if buffer overflow. Otherwise, false.
 */
bool check_for_buffer_overflow(UART_HandleTypeDef *huart) {
	return __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
}


/**
 * @brief Clears the buffer overflow condition in a UART handler
 *
 * This function clears the overrun error flag (UART_FLAG_ORE) of the
 * UART handler. Then, it requests to flush the received data which could
 * be potentially invalid due to the buffer overflow.
 *
 * @param UART_HandleTypeDef *huart A pointer to a UART_HandleTypeDef structure that contains
 *               					the configuration information for the specified UART module.
 * @return void. This function does not return a value.
 */
void clear_buffer_overflow(UART_HandleTypeDef *huart) {

	__HAL_UART_CLEAR_OREFLAG(huart);//Is cleared without notice
	//Discard the received data
	__HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
}

/**
 * @brief UART receive callback which is called by an UART receive interrupt
 *
 * This function handles the readout of UART data if an interrupt occurred.
 * It also handles buffer overflows by simply flushing the buffer.
 * It copies the content of its receive buffer to a global buffer for further processing.
 * A flag signalizes that new data is available.
 * It also re-arms the UART interface to listen for further bytes received.
 * The "ToIdle" functionality reacts to incoming data as long as no idle condition is met.
 * This simplifies the detection of "finished" messages.
 *
 * TODO: Double buffers really necessary? Error handling.
 *
 * @param UART_HandleTypeDef *huart A pointer to a UART_HandleTypeDef structure that contains
 *               					the configuration information for the specified UART module.
 * @param uint16_t size A variable which holds the number of data available by the UART interface.
 * @return void. This function does not return a value.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
	if (huart->Instance == USART1) {

		if(check_for_buffer_overflow(huart)) {
			printf("overflow happened\r\n");
			clear_buffer_overflow(huart);
		} else {

			memcpy(uart_buffer, rxBuffer_uart_1, size);

			is_uart_success = true;
		}
		memset(rxBuffer_uart_1, 0, MAX_BUFFER_SIZE);
	}
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer_uart_1, MAX_BUFFER_SIZE);
}
