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
 * Driver for Thinkpad keyboard & trackpad
 * USB & BLE interface.
 */

#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/unistd.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pin_cfg.h"
#include "sdkconfig.h"
#include "tinyusb.h"
#include "tusb.h"
#include "tusb_hid.h"

/****************************************************************
 *
 *  Private Definition
 *
 ****************************************************************/

// #define USE_FN_TRACKPOINT_PAN
#define SCALE_TRACKPOINT_SPEED
#define MOUSE_SCALE_MIN 1

/****************************************************************
 *
 *  Private Varibles
 *
 ****************************************************************/

// UART1 fd for select()
static int uart1_fd = -1;

static const char *TAG = "tp-task";

/****************************************************************
 *
 *  Public Varibles
 *
 ****************************************************************/

extern bool is_usb_connected;

/****************************************************************
 *
 *  Private function prototypes
 *
 ****************************************************************/

static void ps2_gpio_init(void);
static uint8_t ps2_read(void);
static void ps2_write_1(uint8_t ch);
static void ps2_write_2(uint8_t ch);

static void init_trackpoint(void);

static void poll_trackpoint(uint poll_ms);

/****************************************************************
 *
 *  Private functions
 *
 ****************************************************************/

/**
 * Initialize trackpad GPIO
 */
static void ps2_gpio_init(void) {
    GPIO_INIT_OUT_PULLUP(PS2_CLK_PIN);
    GPIO_INIT_OUT_PULLUP(PS2_DATA_PIN);
    PS2_CLK_HIGH;
    PS2_DATA_HIGH;
}

/**
 * Read one byte from PS2.
 * **Only for trackpoint initialization!**
 * @return result
 */
static uint8_t ps2_read(void) {
    while (PS2_CLK_STATE == 1)
        ;
    while (PS2_CLK_STATE == 0)
        ;

    uint8_t res = 0;
    for (int i = 0; i < 8; i++) {
        while (PS2_CLK_STATE == 1)
            ;
        if (PS2_DATA_STATE != 0) {
            res |= (1 << i);
        }
        while (PS2_CLK_STATE == 0)
            ;
    }
    while (PS2_CLK_STATE == 1)
        ;
    while (PS2_CLK_STATE == 0)
        ;
    while (PS2_CLK_STATE == 1)
        ;
    while (PS2_CLK_STATE == 0)
        ;

    printf("receive 0x%02x\n", res);
    return res;
}

/**
 * Write one byte to PS2.
 * **Only for trackpoint initialization!**
 * @param ch command
 */
static void ps2_write_1(uint8_t ch) {
    uint8_t op = ch ^ 0x1;
    op = op ^ (op >> 4);
    op = op ^ (op >> 2);
    op = op ^ (op >> 1);
    op &= 0x1;

    printf("0x%02x, parity %c\n", ch, op ? '1' : '0');

    PS2_CLK_OUTPUT;
    PS2_DATA_OUTPUT;

    // start
    PS2_CLK_LOW;
    usleep(50);
    PS2_DATA_LOW;
    usleep(50);

    PS2_CLK_HIGH;
    PS2_CLK_INPUT;

    // data
    while (PS2_CLK_STATE == 1)
        ;
    // printf("0x%02x,1 parity %c\n", ch, op ? '1' : '0');
    for (int i = 0; i < 8; i++) {
        while (PS2_CLK_STATE == 0)
            ;
        if (ch & 0x1) {
            PS2_DATA_HIGH;
        } else {
            PS2_DATA_LOW;
        }
        ch >>= 1;
        while (PS2_CLK_STATE == 1)
            ;
    }

    // odd parity
    while (PS2_CLK_STATE == 0)
        ;
    if (op) {
        PS2_DATA_HIGH;
    } else {
        PS2_DATA_LOW;
    }
    while (PS2_CLK_STATE == 1)
        ;

    // end
    while (PS2_CLK_STATE == 0)
        ;
    PS2_DATA_HIGH;
    PS2_DATA_INPUT;
    while (PS2_CLK_STATE == 1)
        ;

    // ack
    while (PS2_CLK_STATE == 0)
        ;
    while (PS2_CLK_STATE == 1)
        ;
}

/**
 * Write one byte to PS2, using yet another timing...
 * **Only for trackpoint initialization!**
 * @param ch command
 */
static void ps2_write_2(uint8_t ch) {
    uint8_t op = ch ^ 0x1;
    op = op ^ (op >> 4);
    op = op ^ (op >> 2);
    op = op ^ (op >> 1);
    op &= 0x1;

    printf("0x%02x, parity %c\n", ch, op ? '1' : '0');

    PS2_CLK_OUTPUT;
    PS2_DATA_OUTPUT;

    // start
    PS2_CLK_LOW;
    usleep(50);
    PS2_DATA_LOW;
    usleep(50);

    PS2_CLK_HIGH;
    PS2_CLK_INPUT;

    // data
    while (PS2_CLK_STATE == 1)
        ;
    for (int i = 0; i < 8; i++) {
        usleep(20);
        if (ch & 0x1) {
            PS2_DATA_HIGH;
        } else {
            PS2_DATA_LOW;
        }
        ch >>= 1;
        while (PS2_CLK_STATE == 0)
            ;
        while (PS2_CLK_STATE == 1)
            ;
    }

    // odd parity
    usleep(20);
    if (op) {
        PS2_DATA_HIGH;
    } else {
        PS2_DATA_LOW;
    }
    while (PS2_CLK_STATE == 0)
        ;
    while (PS2_CLK_STATE == 1)
        ;

    // end
    usleep(20);
    // PS2_DATA_HIGH;
    PS2_DATA_INPUT;
    while (PS2_DATA_STATE == 1)
        ;
    while (PS2_CLK_STATE == 1)
        ;

    // ack
    while (PS2_CLK_STATE == 0)
        ;
    while (PS2_CLK_STATE == 1)
        ;
}

static void init_trackpoint(void) {
    ps2_gpio_init();

    // reset mouse
    gpio_reset_pin(PS2_RESET_PIN);
    gpio_set_direction(PS2_RESET_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PS2_RESET_PIN, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(PS2_RESET_PIN, 0);
    vTaskDelay(70 / portTICK_PERIOD_MS);

    uint8_t ret = 0;
    void (*ps2_write)(uint8_t) = ps2_write_1;

    ps2_write(0xff);  // mouse reset
    ret = ps2_read();
    if (ret != 0xfa) {
        ESP_LOGI(TAG, "Use another timing...");
        ps2_write = ps2_write_2;
    }
    int nrtry = 0;
    for (nrtry = 0; nrtry < 5; nrtry++) {
        ESP_LOGI(TAG, "Init round %d", nrtry);
        vTaskDelay(70 / portTICK_PERIOD_MS);
        ps2_write(0xff);  // mouse reset
        if (ps2_read() != 0xfa) continue;
        vTaskDelay(70 / portTICK_PERIOD_MS);
        ps2_write(0xff);  // mouse reset
        if (ps2_read() != 0xfa) continue;
        vTaskDelay(70 / portTICK_PERIOD_MS);
        ps2_write(0xf3);  // set sample rate
        if (ps2_read() != 0xfa) continue;
        vTaskDelay(3 / portTICK_PERIOD_MS);
        ps2_write(0x50);  // set sample rate 80
        if (ps2_read() != 0xfa) continue;
        vTaskDelay(3 / portTICK_PERIOD_MS);
        // ps2_write(0xf2);  // mouse id
        // if (ps2_read() != 0xfa) continue;
        // vTaskDelay(50 / portTICK_PERIOD_MS);
        ps2_write(0xf4);  // enable data reporting
        if (ps2_read() != 0xfa)
            continue;
        else
            break;
    }

    if (nrtry < 5) {
        ESP_LOGI(TAG, "PS2 initialized.");

        /**
         * From now on, PS2 will only be used as a receiver, and the DATA line
         * has the identical timing to a UART...
         */
        const uart_config_t uart_config = {
            .baud_rate = 14465,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_ODD,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };
        uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0);
        uart_param_config(UART_NUM_1, &uart_config);
        uart_set_pin(UART_NUM_1, -1, PS2_DATA_PIN, -1, -1);

        uart1_fd = open("/dev/uart/1", O_RDWR);
        if (uart1_fd < 0) {
            printf("Failed to open uart1. Mouse task exit...\n");
        } else {
            printf("VFS open uart1\n");
        }
    } else {
        ESP_LOGI(TAG, "Failed to init trackpoint...");
    }
}

/**
 * Check the trackpoint PS2 input within a short time
 * @param poll_us poll time in microsecond
 */
static void poll_trackpoint(uint poll_us) {
    if (uart1_fd < 0) {
        vTaskDelay(poll_us / 1000 / portTICK_PERIOD_MS);
        return;
    }

    // expire time for select()
    struct timeval mouse_tv = {
        .tv_sec = 0,
        .tv_usec = poll_us,
    };

    static bool is_midkey = false, is_pan = true;

    int8_t buttons = 0, dx = 0, dy = 0;
    int8_t pan_x = 0, pan_y = 0;
    bool is_recv = false;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(uart1_fd, &rfds);

    // wait for PS2 input...
    int s = select(uart1_fd + 1, &rfds, NULL, NULL, &mouse_tv);

    if (s < 0) {
        ESP_LOGE(TAG, "Select failed: errno %d. Exit...", errno);
        close(uart1_fd);
        uart1_fd = -1;
    } else if (s != 0) {
        // parse all the PS2 packets
        while (1) {
            char mousebuf[3];
            int nrrd = uart_read_bytes(UART_NUM_1, mousebuf, 3, 5);
            if (nrrd > 0) {
                if (nrrd < 3) {
                    // read the remaining bytes
                    int nrrd2 = uart_read_bytes(UART_NUM_1, &mousebuf[nrrd], 3 - nrrd, 3);
                    nrrd += nrrd2;
                }
                if (nrrd == 3) {
                    // printf("recv: %02x %02x %02x\n", mousebuf[0], mousebuf[1], mousebuf[2]);
                    buttons |= mousebuf[0];
                    dx += mousebuf[1], dy -= mousebuf[2];
                    is_recv = true;
                } else {
                    // printf("Only receive %d chars: ", nrrd);
                    // for (int nr = 0; nr < nrrd; nr++) {
                    //   printf("%02x ", mousebuf[nr]);
                    // }
                    // printf("\n");

                    // discard the dirty data
                    buttons = dx = dy = 0;
                    is_recv = false;
                    uart_flush_input(UART_NUM_1);
                    break;
                }
            } else {
                break;
            }
        }
    }

    buttons &= 0b00000111;
    if (is_recv) {
        // mid key detection
        if (buttons & 0b00000100) {
            is_midkey = true;
            // printf("midkey press\n");
            if (dx != 0 || dy != 0) {
                // middle key for pan
                pan_x = dx > 0 ? 1 : dx < 0 ? -1 : 0;
                pan_y = dy < 0 ? 1 : dy > 0 ? -1 : 0;
                dx = dy = 0;
                is_pan = true;
                // printf("midkey pan\n");
            }
        } else {
            if (is_midkey && !is_pan) {
                // printf("send mid key\n");
                if (is_usb_connected) {
                    tinyusb_hid_mouse_report(0b00000100, 0, 0, 0, 0);
                    vTaskDelay(20);
                    tinyusb_hid_mouse_report(0, 0, 0, 0, 0);
                    vTaskDelay(20);
                }
            }
            is_midkey = is_pan = false;

#ifdef SCALE_TRACKPOINT_SPEED
            // Scale the trackpoint mouse since it may be too slow...
            if (dx > MOUSE_SCALE_MIN)
                dx += (dx - MOUSE_SCALE_MIN) * 2;
            else if (dx < -MOUSE_SCALE_MIN)
                dx += (dx + MOUSE_SCALE_MIN) * 2;
            if (dy > MOUSE_SCALE_MIN)
                dy += (dy - MOUSE_SCALE_MIN) * 2;
            else if (dy < -MOUSE_SCALE_MIN)
                dy += (dy + MOUSE_SCALE_MIN) * 2;
#endif
        }

        if (is_usb_connected) {
            tinyusb_hid_mouse_report(buttons & 0b00000011, dx, dy, pan_y, pan_x);
        }

        // printf("Mouse %3d, %3d; Pan %3d, %3d; Buttons 0x%02x\n", dx, dy, pan_x, pan_y, buttons);
    }
}

/****************************************************************
 *
 *  Public functions
 *
 ****************************************************************/

unsigned get_kb_scan_interval_us(void) { return 5000 * 5 / 6; }
/**
 * trackpoint task
 */
void trackpoint_task(void *arg) {
    (void)arg;

    ESP_LOGI(TAG, "START");
    init_trackpoint();
    ESP_LOGI(TAG, "Init finish");

    while (1) {
        // Poll here and do not bother using semaphores...
        if (!is_usb_connected) {
            vTaskDelay(2000);
            continue;
        }

        poll_trackpoint(get_kb_scan_interval_us());
    }
}
