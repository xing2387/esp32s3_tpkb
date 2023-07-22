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
 * Keymap for Thinkpad E580/T470 etc.
 */

#include "keymap.h"

keytable_t kbtbl[] = {
    {0, 10, 0, KEY_ESC},
    {1, 8, 0, KEY_F1},
    {1, 6, 0, KEY_F2},
    {2, 6, 0, KEY_F3},
    {0, 6, 0, KEY_F4},
    {0, 2, 0, KEY_F5},
    {0, 1, 0, KEY_F6},
    {2, 3, 0, KEY_F7},
    {1, 3, 0, KEY_F8},
    {1, 2, 0, KEY_F9},
    {5, 2, 0, KEY_F10},
    {5, 7, 0, KEY_F11},
    {5, 9, 0, KEY_F12},
    {1, 13, 0, KEY_HOME},
    {5, 13, 0, KEY_END},
    {1, 9, 0, KEY_INSERT},
    {1, 7, 0, KEY_DELETE},
    {1, 10, '~', KEY_GRAVE},
    {5, 10, '1', KEY_1},
    {5, 8, '2', KEY_2},
    {5, 6, '3', KEY_3},
    {5, 4, '4', KEY_4},
    {1, 4, '5', KEY_5},
    {1, 0, '6', KEY_6},
    {5, 0, '7', KEY_7},
    {5, 1, '8', KEY_8},
    {5, 3, '9', KEY_9},
    {5, 5, '0', KEY_0},
    {1, 5, '-', KEY_MINUS},
    {1, 1, '=', KEY_EQUAL},
    {2, 2, 0, KEY_BACKSPACE},
    {2, 10, 0, KEY_TAB},
    {3, 10, 'q', KEY_Q},
    {3, 8, 'w', KEY_W},
    {3, 6, 'e', KEY_E},
    {3, 4, 'r', KEY_R},
    {2, 4, 't', KEY_T},
    {2, 0, 'y', KEY_Y},
    {3, 0, 'u', KEY_U},
    {3, 1, 'i', KEY_I},
    {3, 3, 'o', KEY_O},
    {3, 5, 'p', KEY_P},
    {2, 5, '[', KEY_LEFTBRACE},
    {2, 1, ']', KEY_RIGHTBRACE},
    {4, 2, '\\', KEY_BACKSLASH},
    {2, 8, 0, KEY_CAPSLOCK},
    {4, 10, 'a', KEY_A},
    {4, 8, 's', KEY_S},
    {4, 6, 'd', KEY_D},
    {4, 4, 'f', KEY_F},
    {0, 4, 'g', KEY_G},
    {0, 0, 'h', KEY_H},
    {4, 0, 'j', KEY_J},
    {4, 1, 'k', KEY_K},
    {4, 3, 'l', KEY_L},
    {4, 5, ';', KEY_SEMICOLON},
    {0, 5, '\'', KEY_APOSTROPHE},
    {6, 2, 0, KEY_ENTER},
    {2, 12, 0, KEY_LEFTSHIFT},
    {6, 10, 'z', KEY_Z},
    {6, 8, 'x', KEY_X},
    {6, 6, 'c', KEY_C},
    {6, 4, 'v', KEY_V},
    {7, 4, 'b', KEY_B},
    {7, 0, 'n', KEY_N},
    {6, 0, 'm', KEY_M},
    {6, 1, ',', KEY_COMMA},
    {6, 3, '.', KEY_DOT},
    {7, 5, '/', KEY_SLASH},
    {6, 12, 0, KEY_RIGHTSHIFT},

    // { 23, 24, 0, 0 }, // Fn key
    {1, 14, 0, KEY_LEFTCTRL},
    {3, 11, 0, KEY_LEFTMETA},  // win key
    {0, 15, 0, KEY_LEFTALT},
    {7, 2, ' ', KEY_SPACE},
    {7, 15, 0, KEY_RIGHTALT},
    {5, 15, 0, KEY_PRTSC},
    {6, 14, 0, KEY_RIGHTCTRL},
    {1, 11, 0, KEY_PAGEUP},
    {6, 11, 0, KEY_PAGEUP},
    {5, 11, 0, KEY_PAGEDOWN},
    {7, 11, 0, KEY_PAGEDOWN},
    {7, 13, 0, KEY_LEFT},
    {0, 13, 0, KEY_UP},
    {7, 9, 0, KEY_RIGHT},
    {7, 7, 0, KEY_DOWN},
    {0, 7, 0, KEY_POWER},
    {2, 7, 0, KEY_VOLUMEUP},
    {3, 7, 0, KEY_VOLUMEDOWN},
    {4, 7, 0, KEY_MUTE},

    // { 1, 12, 0, KEY_MEDIA_CALC },
    // { 6, 12, '(', KEY_KPLEFTPAREN },
    // { 1, 15, ')', KEY_KPRIGHTPAREN },
    {2, 2, 0, KEY_BACKSPACE},
    // { 0, 16, 0, KEY_NUMLOCK },
    // { 1, 16, '/', KEY_KPSLASH },
    // { 6, 16, '*', KEY_KPASTERISK },
    // { 7, 16, '-', KEY_KPMINUS },
    // { 4, 16, '7', KEY_KP7 },
    // { 5, 16, '8', KEY_KP8 },
    // { 3, 16, '9', KEY_KP9 },
    // { 2, 16, '+', KEY_KPPLUS },
    // { 0, 17, '4', KEY_KP4 },
    // { 1, 17, '5', KEY_KP5 },
    // { 6, 17, '6', KEY_KP6 },
    // { 7, 17, '1', KEY_KP1 },
    // { 4, 17, '2', KEY_KP2 },
    // { 5, 17, '3', KEY_KP3 },
    // { 3, 17, 0, KEY_KPENTER },
    // { 2, 17, '0', KEY_KP0 },
    // { 6, 11, '.', KEY_KPDOT },
};

fn_keytable_t fntbl[] = {
    {0, 1, 0, FN_FNLOCK},
    // { 7,  4, KEY_CONSUMER_MUTE, FN_NOP  },
    // { 7,  3, KEY_CONSUMER_VOLUME_DECREMENT, FN_NOP  },
    // { 1,  3, KEY_CONSUMER_VOLUME_INCREMENT, FN_NOP  },
    // { 0,  3, , 0 },
    // { 0, 14, KEY_CONSUMER_BRIGHTNESS_DECREMENT, FN_NOP },
    // { 0,  8, KEY_CONSUMER_BRIGHTNESS_INCREMENT, FN_NOP },
    // { 1,  6, , 0 },
    // { 7,  6, , 0 },
    // { 7, 14, , 0 },
    // { 5, 14, , 0 },
    // { 5, 13, , 0 },
    // { 5, 11, , 0 },
    // {2, 14, 0, FN_BACKLIGHT},
     {4, 4, 0, KEY_LEFTMETA},
};
