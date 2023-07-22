/**
 * This file is part of esp32s3-keyboard.
 *
 * Copyright (C) 2020-2021 Yuquan He <heyuquan20b at ict dot ac dot cn> 
 * (Institute of Computing Technology, Chinese Academy of Sciences)
 *
 * esp32s3-keyboard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * esp32s3-keyboard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with esp32s3-keyboard. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Keyboard pin configuration
 */
#ifndef MY_PIN_CFG_H
#define MY_PIN_CFG_H

#include "sdkconfig.h"
#include "driver/gpio.h"


/****************************************************************
 * 
 *  GPIO allocation
 * 
 ****************************************************************/

// 8 column scan
#define KB_COL_0   GPIO_NUM_12
#define KB_COL_1   GPIO_NUM_11
#define KB_COL_2   GPIO_NUM_10
#define KB_COL_3   GPIO_NUM_9
#define KB_COL_4   GPIO_NUM_3
#define KB_COL_5   GPIO_NUM_8
#define KB_COL_6   GPIO_NUM_18
#define KB_COL_7   GPIO_NUM_17

// 16 row scan
#define KB_ROW_0      GPIO_NUM_1
#define KB_ROW_1      GPIO_NUM_2
#define KB_ROW_2      GPIO_NUM_42
#define KB_ROW_3      GPIO_NUM_41
#define KB_ROW_4      GPIO_NUM_40
#define KB_ROW_5      GPIO_NUM_39
#define KB_ROW_6      GPIO_NUM_38
#define KB_ROW_7      GPIO_NUM_37
#define KB_ROW_8      GPIO_NUM_36
#define KB_ROW_9      GPIO_NUM_35
#define KB_ROW_10     GPIO_NUM_45
#define KB_ROW_11     GPIO_NUM_48
#define KB_ROW_12     GPIO_NUM_47
#define KB_ROW_13     GPIO_NUM_21
#define KB_ROW_14     GPIO_NUM_14
#define KB_ROW_15     GPIO_NUM_13

// Buttons
// #define BUTTON_MIDDLE 0
#define BUTTON_FN     GPIO_NUM_5

// LED
#define LED_CAPLK     GPIO_NUM_6
// #define LED_FNLK      43
// #define LED_F1        1
// #define LED_NUMLK     44
// #define BACKLIGHT_PWM 40
// #define LED_F4        7

// Trackpad GPIO
#define PS2_DATA_PIN    GPIO_NUM_16
#define PS2_CLK_PIN     GPIO_NUM_15
#define PS2_RESET_PIN   GPIO_NUM_7

// USB charging detection
// #define CHARGING_PIN    2

// #define BUTTON_MIDDLE_STATE gpio_get_level(BUTTON_MIDDLE)
#define BUTTON_FN_STATE     gpio_get_level(BUTTON_FN)

#define LED_CAPLK_ON    gpio_set_level(LED_CAPLK, 0)
#define LED_CAPLK_OFF   gpio_set_level(LED_CAPLK, 1)
// #define LED_FNLK_ON     gpio_set_level(LED_FNLK, 0)
// #define LED_FNLK_OFF    gpio_set_level(LED_FNLK, 1)
// #define LED_F1_ON       gpio_set_level(LED_F1, 0)
// #define LED_F1_OFF      gpio_set_level(LED_F1, 1)
// #define LED_NUMLK_ON    gpio_set_level(LED_NUMLK, 0)
// #define LED_NUMLK_OFF   gpio_set_level(LED_NUMLK, 1)
// #define BACKLIGHT_ON    gpio_set_level(BACKLIGHT_PWM, 1)
// #define BACKLIGHT_OFF   gpio_set_level(BACKLIGHT_PWM, 0)
// #define LED_F4_ON       gpio_set_level(LED_F4, 0)
// #define LED_F4_OFF      gpio_set_level(LED_F4, 1)

#define PS2_CLK_STATE   gpio_get_level(PS2_CLK_PIN)
#define PS2_CLK_OUTPUT  gpio_set_direction(PS2_CLK_PIN,  GPIO_MODE_OUTPUT)
#define PS2_CLK_INPUT   gpio_set_direction(PS2_CLK_PIN,  GPIO_MODE_INPUT)
#define PS2_CLK_LOW     gpio_set_level(PS2_CLK_PIN, 0)
#define PS2_CLK_HIGH    gpio_set_level(PS2_CLK_PIN, 1)
#define PS2_DATA_STATE  gpio_get_level(PS2_DATA_PIN)
#define PS2_DATA_OUTPUT gpio_set_direction(PS2_DATA_PIN, GPIO_MODE_OUTPUT)
#define PS2_DATA_INPUT  gpio_set_direction(PS2_DATA_PIN, GPIO_MODE_INPUT)
#define PS2_DATA_LOW    gpio_set_level(PS2_DATA_PIN, 0)
#define PS2_DATA_HIGH   gpio_set_level(PS2_DATA_PIN, 1)

// #define CHARGING_STATE  gpio_get_level(CHARGING_PIN)

/****************************************************************
 * 
 *  Handy GPIO helpers
 * 
 ****************************************************************/

#define GPIO_INIT_IN_FLOATING(pinnum) \
  do { \
    gpio_reset_pin(pinnum); \
    gpio_set_direction(pinnum, GPIO_MODE_INPUT); \
    gpio_pullup_dis(pinnum); \
    gpio_pulldown_dis(pinnum); \
  } while(0)

#define GPIO_INIT_IN_PULLUP(pinnum) \
  do { \
    gpio_reset_pin(pinnum); \
    gpio_set_direction(pinnum, GPIO_MODE_INPUT); \
    gpio_pullup_en(pinnum); \
    gpio_pulldown_dis(pinnum); \
  } while(0)

#define GPIO_INIT_IN_PULLDOWN(pinnum) \
  do { \
    gpio_reset_pin(pinnum); \
    gpio_set_direction(pinnum, GPIO_MODE_INPUT); \
    gpio_pullup_dis(pinnum); \
    gpio_pulldown_en(pinnum); \
  } while(0)

#define GPIO_SET_PULLUP(pinnum) \
  do { \
    gpio_pullup_en(pinnum); \
    gpio_pulldown_dis(pinnum); \
  } while(0)

#define GPIO_SET_PULLDOWN(pinnum) \
  do { \
    gpio_pullup_dis(pinnum); \
    gpio_pulldown_en(pinnum); \
  } while(0)

#define GPIO_INIT_OUT_PULLUP(pinnum) \
  do { \
    gpio_reset_pin(pinnum); \
    gpio_set_direction(pinnum, GPIO_MODE_OUTPUT); \
    gpio_pullup_en(pinnum); \
  } while(0)

#define GPIO_INIT_OUT_PULLDOWN(pinnum) \
  do { \
    gpio_reset_pin(pinnum); \
    gpio_set_direction(pinnum, GPIO_MODE_OUTPUT); \
    gpio_pulldown_en(pinnum); \
  } while(0)

#endif
