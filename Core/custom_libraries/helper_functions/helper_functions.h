/**
 * @file    helper_functions.h
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

#ifndef CUSTOM_MIDDLEWARE_01_HELPER_FUNCTIONS_HELPER_FUNCTIONS_H
#define CUSTOM_MIDDLEWARE_01_HELPER_FUNCTIONS_HELPER_FUNCTIONS_H

#include "cmsis_os2.h"
#include "main.h"

#define LED_OFF 1
#define LED_ON 0

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

//void all_leds_off();
//void red_led_on();
//void green_led_on();
//void green_led_off();
//void blue_led_on();
//void blue_led_off();
void led_timer_callback(void *argument);
void init_led_timer();

#endif
