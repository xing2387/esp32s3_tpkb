#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_app_trace.h"
#include "esp_chip_info.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "keymap/keymap.h"
#include "pin_cfg.h"
#include "sdkconfig.h"
#include "tinyusb.h"
#include "tusb.h"
#include "tusb_hid.h"

static const char *TAG = "kb-task";

volatile bool is_caplk_on = false;
static bool is_fn_locked = 0;

#define COL_NUM 8
#define ROW_NUM 16
// keyboard pin array
static uint rowscan_pins[16] = {KB_ROW_0,  KB_ROW_1,  KB_ROW_2,  KB_ROW_3, KB_ROW_4,  KB_ROW_5,
                                KB_ROW_6,  KB_ROW_7,  KB_ROW_8,  KB_ROW_9, KB_ROW_10, KB_ROW_11,
                                KB_ROW_12, KB_ROW_13, KB_ROW_14, KB_ROW_15};
static uint colscan_pins[8] = {KB_COL_0, KB_COL_1, KB_COL_2, KB_COL_3,
                               KB_COL_4, KB_COL_5, KB_COL_6, KB_COL_7};

void init_kb_matrix() {
    for (int i = 0; i < 8; i++) {
        GPIO_INIT_OUT_PULLUP(colscan_pins[i]);
    }
    for (int i = 0; i < 16; i++) {
        GPIO_INIT_IN_PULLUP(rowscan_pins[i]);
    }
    GPIO_INIT_IN_PULLUP(BUTTON_FN);
    is_caplk_on = false;
}
static void kb_set_column_scan(int n) {
    for (int i = 0; i < COL_NUM; i++) {
        gpio_set_level(colscan_pins[i], i != n);
    }
}
static void do_fnfunc(fn_function_t fncode) {}
void keyboard_task(void *arg) {
    extern bool is_usb_connected;
    init_kb_matrix();
    bool last_is_key_pressed = false;
    uint64_t lasthid = 0;
    uint16_t lasthotkey = 0;
    fn_function_t lastfnfunc = FN_NOP;
    while (1) {
        if (!is_usb_connected) {
            vTaskDelay(2000);
            ESP_LOGI(TAG, "Waiting usb connect...");
            continue;
        }
        bool is_key_pressed = false;
        uint64_t hid = 0;
        uint8_t *hidbuf = (uint8_t *)&hid;
        int nr_hidkey = 0;
        uint16_t hotkey = 0;
        fn_function_t fnfunc = FN_NOP;
        bool has_phantom_key = false;
        uint32_t rows_connected = 0;

        vTaskDelay(pdMS_TO_TICKS(10));

        for (int col = 0; col < COL_NUM; col++) {
            kb_set_column_scan(col);
            uint32_t rows_cur_col = 0;  // rows connected with the current col
            for (int row = 0; row < ROW_NUM; row++) {
                if (gpio_get_level(rowscan_pins[row]) == 0) {
                    rows_cur_col |= 1 << row;
                    int hidkey = search_hid_key(col, row);
                    if (hidkey > 0) {
                        if (BUTTON_FN_STATE != 0) {
                            // if (true) {
                            // normal keyboard usage
                            if (hidkey >= KEY_LEFTCTRL && hidkey <= KEY_RIGHTMETA) {
                                hidbuf[0] |= 1u << (hidkey & 0x07);
                            } else if (is_fn_locked && hidkey >= KEY_F1 && hidkey <= KEY_F12) {
                                fn_keytable_t *fnitem = search_fn(col, row);
                                if (fnitem != NULL) {
                                    is_key_pressed = true;
                                    hotkey = fnitem->hidcode;
                                    fnfunc = fnitem->fncode;
                                    hid = 0;  // clear keyboard key
                                }
                            } else if (nr_hidkey < 6) {
                                hidbuf[2 + nr_hidkey] = hidkey;
                                nr_hidkey++;
                                is_key_pressed = true;
                                hotkey = 0;  // clear hotkey
                            }
                        } else {
                            if (is_fn_locked && hidkey >= KEY_F1 && hidkey <= KEY_F12) {
                                if (!is_key_pressed) {
                                    hidbuf[2] = hidkey;
                                    is_key_pressed = true;
                                    hotkey = 0;
                                }
                            } else {
                                // hotkey
                                fn_keytable_t *fnitem = search_fn(col, row);
                                if (fnitem != NULL && nr_hidkey < 6) {
                                    // is_key_pressed = true;
                                    // hotkey = fnitem->hidcode;
                                    // fnfunc = fnitem->fncode;
                                    // hid = 0;  // clear keyboard key
                                    hidbuf[2 + nr_hidkey] = fnitem->fncode;
                                    nr_hidkey++;
                                    is_key_pressed = true;
                                    hotkey = 0;  // clear hotkey
                                }
                            }
                        }
                    }
                }
            }
            // rows that are connected by both current col and previous cols, which
            // lead to "phantom keys" if more than one.
            uint32_t rows_connected_again = rows_cur_col & rows_connected;
            // If and only if less than two bits is 1, the following expr will be 0
            if (rows_connected_again & (rows_connected_again - 1)) has_phantom_key = true;
            rows_connected |= rows_cur_col;
            // poll_trackpoint(get_kb_scan_interval_us());
        }
        if (has_phantom_key) {
            hotkey = lasthotkey;
            fnfunc = lastfnfunc;
            hid = lasthid;
            is_key_pressed = last_is_key_pressed;
        }
        last_is_key_pressed = is_key_pressed;

        if (hid != lasthid) {
            // printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", hidbuf[0], hidbuf[1], hidbuf[2],
            //        hidbuf[3], hidbuf[4], hidbuf[5], hidbuf[6], hidbuf[7]);
            if (is_usb_connected) {
                tinyusb_hid_keyboard_report(hidbuf);
            }
        }
        lasthid = hid;

        if (hotkey != lasthotkey) {
            // printf("%04x\n", hotkey);
            if (is_usb_connected) {
                tinyusb_hid_consumer_report(hotkey);
            }
        }
        lasthotkey = hotkey;

        if (fnfunc != lastfnfunc) {
            do_fnfunc(fnfunc);
        }
        lastfnfunc = fnfunc;
    }
}

/*
1
2
42
41
33
39
38
37
36
35
45
48
47
21
14
13      -----R
12
11
10
9
3
8
18
17      -----C
16      TPDATA
15      TPCLK
7       TPRESET
6
5
4
*/