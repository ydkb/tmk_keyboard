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
const uint8_t PROGMEM unimap_trans[MATRIX_ROWS][MATRIX_COLS] = {
    { UM_ESC, UM_1,   UM_2,   UM_3,   UM_4,   UM_5,   UM_6,   UM_7,   UM_8,   UM_9,   UM_0,   UM_GRV, UM_NLCK,UM_PMNS,UM_PPLS },
    { UM_TAB, UM_Q,   UM_W,   UM_E,   UM_R,   UM_T,   UM_Y,   UM_U,   UM_I,   UM_O,   UM_P,   UM_BSPC,UM_P7,  UM_P8,  UM_P9   },
    { UM_CAPS,UM_A,   UM_S,   UM_D,   UM_F,   UM_G,   UM_H,   UM_J,   UM_K,   UM_L,   UM_ENT, UM_NO,  UM_P4,  UM_P5,  UM_P6   },
    { UM_LSFT,UM_Z,   UM_X,   UM_C,   UM_V,   UM_B,   UM_N,   UM_M,   UM_COMM,UM_RSFT,UM_NO,  UM_UP,  UM_P1,  UM_P2,  UM_P3   },
    { UM_LCTL,UM_LGUI,UM_LALT,UM_RALT,UM_NO,  UM_NO,  UM_SPC, UM_NO,  UM_RCTL,UM_LEFT,UM_NO,  UM_DOWN,UM_RGHT,UM_P0,  UM_PDOT }
};

/*
["Esc","!\n1","@\n2","#\n3","$\n4","%\n5","^\n6","&\n7","*\n8","(\n9",")\n0","~\n`","Num Lock","-","+"],
["Tab","Q","W","E","R","T","Y","U","I","O","P","Backspace","7\nHome","8\n¡ü","9\nPgUp"],
["Caps Lock","A","S","D","F","G","H","J","K","L","Enter",{a:7},"",{a:4},"4\n¡û","5","6\n¡ú"],
["Shift","Z","X","C","V","B","N","M","<\n,","RShift",{a:7},"",{a:4},"¡ü","1\nEnd","2\n¡ý","3\nPgDn"],
["Ctrl","Alt","Win","App",{a:7},"","",{a:4},"Space",{a:7},"",{a:4},"RCtrl","¡û",{a:7},"",{a:4},"¡ý","¡ú","0\nIns",".\nDel"]
*/
#endif
