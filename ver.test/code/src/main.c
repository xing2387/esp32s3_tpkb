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

static const char *TAG = "kb-main";

volatile bool is_usb_connected = false;

static void init_usb(void) {
    ESP_LOGI(TAG, "USB initialization");

    // Setting of descriptor. You can use descriptor_tinyusb and
    // descriptor_str_tinyusb as a reference
    tusb_desc_device_t my_descriptor = {.bLength = sizeof(my_descriptor),
                                        .bDescriptorType = TUSB_DESC_DEVICE,
                                        .bcdUSB = 0x0200,  // USB version. 0x0200 means version 2.0
                                        .bDeviceClass = TUSB_CLASS_UNSPECIFIED,
                                        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

                                        .idVendor = 0x303A,
                                        .idProduct = 0x3000,
                                        .bcdDevice = 0x0101,  // Device FW version

                                        .iManufacturer = 0x01,  // see string_descriptor[1] bellow
                                        .iProduct = 0x02,       // see string_descriptor[2] bellow
                                        .iSerialNumber = 0x03,  // see string_descriptor[3] bellow

                                        .bNumConfigurations = 0x01};

    tusb_desc_strarray_device_t my_string_descriptor = {
        // array of pointer to string descriptors
        (char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
        "hhuysqt",             // 1: Manufacturer
        "Keyboard Hacker",     // 2: Product
        "012-345",             // 3: Serials, should use chip ID

        "my CDC",  // 4: CDC Interface
        "my MSC",  // 5: MSC Interface
        "my HID",  // 6: HID Interface
    };

    tinyusb_config_t tusb_cfg = {
        .descriptor = &my_descriptor,
        .string_descriptor = my_string_descriptor,
        .external_phy = false  // In the most cases you need to use a `false` value
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");
}

void init_log() {
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    esp_log_set_vprintf(esp_apptrace_vprintf);
    esp_log_set_vprintf(vprintf);
    esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, 100000 /*tmo in us*/);
    ESP_LOGI(TAG, "Tracing is finished.");
}

void app_main() {
    init_log();

    ESP_LOGI("app_main", "init_usb\n");
    init_usb();

    void keyboard_task(void *arg);
    xTaskCreate(&keyboard_task, "kb_task", 4096, NULL, configMAX_PRIORITIES, NULL);

    void trackpoint_task(void *arg);
    xTaskCreate(&trackpoint_task, "mouse_task", 4096, NULL, configMAX_PRIORITIES, NULL);
}

/****************************************************************
 *
 *  Callback override
 *
 ****************************************************************/

// tinyusb callbacks for connection
void tud_mount_cb(void) {
    is_usb_connected = true;
    printf("USB connected.\n");
}

// tinyusb callbacks for disconnection
void tud_umount_cb(void) {
    is_usb_connected = false;
    printf("USB disconnected\n");
}

void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    is_usb_connected = false;
    // printf("USB suspended, %s\n");
    printf("%s(%s)\n", __func__, remote_wakeup_en ? "true" : "false");
}

void tud_resume_cb(void) {
    is_usb_connected = true;
    printf("%s\n", __func__);
}
