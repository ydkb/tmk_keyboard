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
#include <avr/pgmspace.h>
#include "unimap_trans.h"
#include "bootloader.h"
#include "wait.h"
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "keymap.h"
#include "led.h"



enum function_id {
    TRICKY_ESC,
    LALT_F4,
    RESET,
    FN_WIN_LOCK,
    SPACE2,
};

#define AC_FN0 ACTION_LAYER_MOMENTARY(1)
#define AC_FN1 ACTION_FUNCTION(RESET)
#ifdef KEYMAP_SECTION_ENABLE
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] __attribute__ ((section (".keymap.keymaps"))) = {
#else
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] PROGMEM = {
#endif
    UNIMAP(
        BTN1,BTN2,BTN3,BTN4,BTN5,WH_U,WH_D, F20, F21, F22, F23, F24,
        ESC,      F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,           PSCR,SLCK,PAUS,         VOLD,VOLU,MUTE,
        GRV, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, JYEN,BSPC,     INS, HOME,PGUP,    NLCK,PSLS,PAST,PMNS,
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,     BSLS,     DEL, END, PGDN,    P7,  P8,  P9,  PPLS,
        CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     NUHS,ENT,                         P4,  P5,  P6,  PCMM,
        LSFT,NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RO,  RSFT,          UP,           P1,  P2,  P3,  PENT,
        LCTL,LGUI,LALT,MHEN,          SPC,           HENK,KANA,RALT,RGUI,APP, RCTL,     LEFT,DOWN,RGHT,    P0,       PDOT,PEQL
        ),
    UNIMAP(
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        GRV,      TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,         TRNS,TRNS,TRNS,
        ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL,      TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  TRNS,   TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,CALC,TRNS,TRNS,HOME,PGUP,LEFT,RGHT,     TRNS,TRNS,                        TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,FN1,TRNS,TRNS,END, PGDN,DOWN,     TRNS,TRNS,          TRNS,         TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,          TRNS,          TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     MUTE,VOLD,VOLU,    TRNS,     TRNS,TRNS
        ),
    UNIMAP_LED(TRNS,TRNS,TRNS),
    UNIMAP_LED(TRNS,TRNS,TRNS),
    UNIMAP_LED(TRNS,TRNS,TRNS),
    UNIMAP_LED(TRNS,TRNS,TRNS),
    UNIMAP_LED(TRNS,TRNS,TRNS),
    UNIMAP_LED(  NO,TRNS,TRNS),
};



#define MODS_SHIFT_MASK (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    static uint8_t mod_keys_registered;
    uint8_t pressed_mods = get_mods();
    switch (id) {
        case TRICKY_ESC:
            if (record->event.pressed) {
                uint8_t esc_shift_mods = opt&0b10 | (opt&0b1000)<<2;
                uint8_t esc_lalt = opt&0b100;
                if ((pressed_mods & esc_shift_mods) && (~pressed_mods & MOD_BIT(KC_LCTRL))) {
                    mod_keys_registered = KC_GRV;
                } else if (pressed_mods & esc_lalt) {
                    mod_keys_registered = KC_F4;
                } else {
                    mod_keys_registered = KC_ESC;
                }
                register_code(mod_keys_registered);
                send_keyboard_report();
            } else {
                unregister_code(mod_keys_registered);
                send_keyboard_report();
            }
            break;
        case LALT_F4:
            if (record->event.pressed) {
                if (pressed_mods & MOD_BIT(KC_LALT)) { 
                    mod_keys_registered = KC_F4;
                } else {
                    mod_keys_registered = KC_4;
                }
                register_code(mod_keys_registered);
                send_keyboard_report();
            } else {
                unregister_code(mod_keys_registered);
                send_keyboard_report();
            }
            break;
        case SPACE2:
            if (record->event.pressed) {
                if (pressed_mods & (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT))) {
                    break;
                } else {
                    register_code(KC_SPACE);
                    send_keyboard_report();
                }
            } else {
                unregister_code(KC_SPACE);
                send_keyboard_report();
            }
            break;
    }
    if (record->event.pressed) {
        switch (id) {
            case RESET:
                clear_keyboard(); 
                if (pressed_mods == MOD_BIT(KC_LCTRL)) {
                    uint16_t *const bootKeyPtr = (uint16_t *)0x0800;
                    *bootKeyPtr = 0xFC2B;
                    for (;;);
                }
                register_mods(0b00110011);
                unregister_mods(0b00110011);
                register_mods(0b11001100);
                unregister_mods(0b11001100);
                //clear_keyboard(); 
                break;
        }
    }
}

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    switch(id) {
        case 0: 
            if (record->event.pressed) {
                return 0x5F00;
            }
    }
    return MACRO_NONE;
};
