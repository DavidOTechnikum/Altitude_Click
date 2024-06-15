/**
 * Disclaimer: 	This library is entirely taken from a demo project provided by the lecturer.
 * 				It is not the work of this project's author.
 * 				The printf function is used for debugging.
 * 				If DEBUGGING is not defined in main.h, putchar() will not write to uart.
 *
 * @file    printf.c
 * @author  Volker Tenta, Patrick Schmitt
 * @version 0.0.1
 * @date    2024-04-29
 *
 * @brief This library contains function to retarget the printf function to use the microcontroller onboard
 * UART interface HUART2.
 *
 */

#include "printf.h"

#include "main.h"

extern UART_HandleTypeDef huart2;

/**
 * @brief   Print given character on UART 2. Translate '\n' to "\r\n" on the fly.
 *          Retargets printf function from stdout to UART interface by overwriting putchar function.
 *          This function is a custom implementation of the low-level IO function used by printf.
 *          By retargeting stdout to UART, we enable printf to send formatted strings over UART.
 * @note    This approach is particularly useful in embedded systems where direct console
 *          access is not available, and UART is commonly used for logs and debugging output.
 * @param   int ch ... Character to be transmitted.
 * @retval  (int) ... Return character.
 */
int __io_putchar(int ch)
{
#ifdef DEBUGGING
	int ret;
	while ((ret=HAL_UART_GetState(&huart2)) == HAL_UART_STATE_BUSY_TX)
		;

	if (ch == '\n')
	{
		static uint8_t buf[2] = { '\r', '\n' };
		HAL_UART_Transmit(&huart2, buf, sizeof(buf), 1000);
	}
	else
	{
		static char buf;
		buf = ch;
		HAL_UART_Transmit(&huart2, (uint8_t *)&buf, 1, 1000);
	}
	return ch;
#endif
}

/**
 * @brief   Called by "printf" to output formatted string by calling __io_putchar() function.
 *          Used to retarget printf function from stdout to UART interface.
 *          This is part of the low-level system IO overrides that direct printf output
 *          through our custom UART transmission logic.
 * @note    _write is a system call that newlib's printf relies on to output characters.
 *          By overriding this function, all standard IO operations can be redirected to
 *          use UART, making printf an effective tool for embedded debugging.
 * @param   int file ... parameter not used by this retarget implementation (normally used as a file pointer)
 * @param   char *ptr ... ptr to data array
 * @param   int len ... length of data to be transmitted (in byte)
 * @retval  (int) ... Return length in byte.
 */
int _write(int file, char *ptr, int len)
{
	for(int DataIdx = 0; DataIdx < len; DataIdx++) {
		__io_putchar(*ptr++);
	}
	return len;
}
