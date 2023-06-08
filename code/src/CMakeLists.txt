# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# idf_component_register(SRCS ${app_sources}
#                         REQUIRES soc nvs_flash ulp driver tinyusb)

idf_component_register( SRCS "main.c"
                            "hid/esp_hidd_prf_api.c"
                            "hid/hid_device_le_prf.c"
                            "hid/hid_dev.c"
                            "keymap/keymap.c"
                            "keyboard.c"
                            "trackpoint.c"
                        INCLUDE_DIRS "."
                            "hid"
                            "keymap"
                        REQUIRES soc ulp driver tinyusb)