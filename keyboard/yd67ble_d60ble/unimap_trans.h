/*
Copyright 2016 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef UNIMAP_COMMON_H
#define UNIMAP_COMMON_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include "unimap.h"


/* Mapping to Universal keyboard layout
 *
 * Universal keyboard layout
 *         ,-----------------------------------------------.
 *         |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
 * |Esc|   |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|     |PrS|ScL|Pau|     |VDn|VUp|Mut|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|  #|Retn|               |  4|  5|  6|KP,|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * |Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /| RO|Shift |     |Up |     |  1|  2|  3|KP=|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * |Ctl|Gui|Alt|MHEN|     Space      |HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  0    |  .|Ent|
 * `-----------------------------------------------------------' `-----------' `---------------'
 */

#if (PRODUCT_ID == 0x2167) // YD67BLE
const uint8_t PROGMEM unimap_trans[5][16] = {
    { UM_ESC, UM_1,   UM_2,   UM_3,   UM_4,   UM_5,   UM_6,   UM_7,        UM_8,   UM_9,   UM_0,   UM_MINS,UM_EQL, UM_BSPC,UM_HOME,UM_NO,  },
    { UM_TAB, UM_Q,   UM_W,   UM_E,   UM_R,   UM_T,   UM_Y,   UM_U,        UM_I,   UM_O,   UM_P,   UM_LBRC,UM_RBRC,UM_BSLS,UM_PGUP,UM_NO,  },
    { UM_CAPS,UM_A,   UM_S,   UM_D,   UM_F,   UM_G,   UM_H,   UM_NO,       UM_J,   UM_K,   UM_L,   UM_SCLN,UM_QUOT,UM_ENT, UM_PGDN,UM_END, },
    { UM_LSFT,UM_Z,   UM_X,   UM_C,   UM_V,   UM_LCTL,UM_LGUI,UM_LALT,     UM_M,   UM_COMM,UM_DOT, UM_N,   UM_B,   UM_SLSH,UM_RSFT,UM_UP,  },
    { UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,   UM_NO, UM_NO,       UM_SPC, UM_NO,  UM_RALT,UM_RCTL,UM_NO,  UM_LEFT,UM_DOWN,UM_RGHT,}
};
#elif (PRODUCT_ID == 0x2161) //D60BLE
const uint8_t PROGMEM unimap_trans[5][16] = {
    { UM_ESC, UM_2,   UM_1,   UM_3,   UM_4,   UM_5,   UM_6,   UM_7,        UM_8,   UM_9,   UM_0,   UM_MINS,UM_EQL, UM_HOME,UM_BSPC,UM_NO,  },
    { UM_TAB, UM_Q,   UM_W,   UM_E,   UM_R,   UM_T,   UM_Y,   UM_U,        UM_I,   UM_O,   UM_P,   UM_LBRC,UM_RBRC,UM_BSLS,UM_PGUP,UM_NO,  },
    { UM_CAPS,UM_A,   UM_S,   UM_D,   UM_F,   UM_G,   UM_H,   UM_NO,       UM_J,   UM_K,   UM_L,   UM_SCLN,UM_QUOT,UM_ENT, UM_PGDN,UM_NO, },
    { UM_LSFT,UM_Z,   UM_X,   UM_C,   UM_V,   UM_LCTL,UM_LGUI,UM_LALT,     UM_M,   UM_COMM,UM_DOT, UM_N,   UM_B,   UM_SLSH,UM_RSFT,UM_UP,  },
    { UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,   UM_NO, UM_NO,       UM_SPC, UM_NO,  UM_RALT,UM_RGUI,UM_NO,  UM_NO,  UM_RCTL,UM_NO,}
};
#endif

#endif
