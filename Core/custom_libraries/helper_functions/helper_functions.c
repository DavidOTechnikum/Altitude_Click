/**
 * @file    helper_functions.c
 * @author  Volker Tenta, Patrick Schmitt
 * @version 0.0.1
 * @date    2024-04-29
 *
 * @brief This library contains only some helper functions to ease the use of the on-board LEDs.
 *
 *  WARNING! THIS IS A DEMO IMPLEMENTATION! DOES NOT FEATURE ALL ERROR HANDLING / CHECKS!!!
 *
 *  This is only a starting point for your own implementations!
 *
 *  Use this with care - and extend its functionality accordingly!
 *
 */

#include "../../custom_libraries/helper_functions/helper_functions.h"

osTimerId_t led_timerHandle;
const osTimerAttr_t led_timer_attributes = {
        .name = "led_timer"
};

/**
 * @brief Callback function for the timer used to switch off an LED
 *
 * This function gets called by the ISR of a timer unit. It currently only switches of some LEDs.
 *
 * @param void *argument Argument pointer - currently not in use.
 * @return void. This function does not return a value.
 */
void led_timer_callback(void *argument) {
//	green_led_off();
}

/**
 * @brief Function which initializes a fixed timer module
 *
 * This is used to handle the blinking of an LED. Nothing more.
 *
 * @param none
 * @return void. This function does not return a value.
 */
void init_led_timer() {
    led_timerHandle = osTimerNew(led_timer_callback, osTimerOnce, NULL, &led_timer_attributes);
    if (led_timerHandle == NULL) {
//        red_led_on();
        Error_Handler();
    }
}

/**
 * @brief Function which turns off all LEDs of the RGB-LED
 *
 * @param none
 * @return void. This function does not return a value.
 */
//void all_leds_off() {
//    HAL_GPIO_WritePin(GPIOA, LED_BLUE_Pin | LED_RED_Pin | LED_GREEN_Pin, LED_OFF);
//}
//
///**
// * @brief Function which turns on the RED LED of the RGB-LED
// *
// * @param none
// * @return void. This function does not return a value.
// */
//void red_led_on() {
//    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LED_ON);
//}
//
///**
// * @brief Function which turns on the GREEN LED of the RGB-LED
// *
// * @param none
// * @return void. This function does not return a value.
// */
//void green_led_on() {
//    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, LED_ON);
//}
//
///**
// * @brief Function which turns OFF the GREEN LED of the RGB-LED
// *
// * @param none
// * @return void. This function does not return a value.
// */
//void green_led_off() {
//    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, LED_OFF);
//}
//
///**
// * @brief Function which turns on the BLUE LED of the RGB-LED
// *
// * @param none
// * @return void. This function does not return a value.
// */
//void blue_led_on() {
//    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, LED_ON);
//}
//
///**
// * @brief Function which turns OFF the BLUE LED of the RGB-LED
// *
// * @param none
// * @return void. This function does not return a value.
// */
//void blue_led_off() {
//    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, LED_OFF);
//}
