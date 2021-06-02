/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

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

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "command.h"
#include "timer.h"
#include "matrix.h"
#include "debounce_pk.h"
#include "suspend.h"
#include "lufa.h"
#include "unimap.h"
#include "rgblight.h"
#include "ble51.h"
#include "ble51_task.h"
#include "wait.h"
#include "hook.h"
#include "switch_board.h"


static uint8_t debounce_mask = DEBOUNCE_DN_MASK;

extern debug_config_t debug_config;
extern uint32_t kb_idle_timer; 

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS] = {0};


static uint8_t matrix_current_row = 0;
static uint16_t matrix_scan_timestamp = 0;
static uint8_t matrix_debouncing[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint8_t encoder_state_prev[1][2] = {0}; 
static uint8_t get_col(uint8_t col);

static void init_cols(void);

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void hook_early_init()
{
    DDRD  &= ~(1<<1);
    PORTD |=  (1<<1);
}

void matrix_init(void)
{

    //debug_config.enable = true;


    rgblight_init();
    // initialize row and col
    init_cols();
    // initialize matrix state: all keys off
}

uint8_t matrix_scan(void)
{
    uint16_t time_check = timer_read();
    if (matrix_scan_timestamp == time_check) return 1;
    matrix_scan_timestamp = time_check;

    select_next_row(0);
    uint8_t *debounce = &matrix_debouncing[0][0];
    for (uint8_t row=0; row<matrix_rows(); row++) {
        matrix_current_row = row;
        for (uint8_t col=0; col<matrix_cols(); col++, *debounce++) {
            uint8_t real_col = col/2;
            if (col & 1) real_col += 8; 

            uint8_t key = get_col(real_col);
            if (real_col >= 8) select_next_row(1);
            *debounce = (*debounce >> 1) | key;
            if ((*debounce > 0) && (*debounce < 255)) {
                matrix_row_t row_prev = matrix[row]; 
                matrix_row_t *p_row = &matrix[row];
                matrix_row_t col_mask = ((matrix_row_t)1 << real_col);
                if        (*debounce >= debounce_mask) {
                    *p_row |=  col_mask;
                } else if (*debounce <= DEBOUNCE_UP_MASK) {
                    *p_row &= ~col_mask;
                } 

            }
        }
        if (matrix[row] > 0) {
            kb_idle_timer = timer_read32();
        }
    }
    return 1;
}


inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        phex(row); print(": ");
        print_bin_reverse8(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop32(matrix[i]);
    }
    return count;
}

void init_cols(void)
{
    DDRF  |=  (1<<6 | 1<<4);
    DDRF  &= ~(1<<5 | 1<<1);
    PORTF |=  (1<<6 | 1<<5 | 1<<4 | 1<<1);
}


static uint8_t get_col_f(uint8_t col) {
    if (col<8) return PINF&(1<<5) ? 0 : 0x80;
    else return PINF&(1<<1) ? 0 : 0x80;

}

uint8_t get_col(uint8_t col)
{
    uint8_t value = get_col_f(col);
    if (matrix_current_row < 2 && col == 15) {
        static uint8_t encoder_debounce = 0;
        static uint16_t encoder_idle_timer;
        uint8_t encoder_state_new = 0;
        uint8_t direction = matrix_current_row & 0b1 ; 

        if (timer_elapsed(encoder_idle_timer) > 400) encoder_debounce = direction?0b111:0;
        encoder_state_new = value? 1 : 0;
        if (encoder_state_new != encoder_state_prev[0][direction]) {
            encoder_state_prev[0][direction] = encoder_state_new;
            if (encoder_state_new == 0) {
                if (encoder_state_prev[0][direction?0:1] == 0) {
                    encoder_debounce = (encoder_debounce<<1) + direction; 
                    if ((encoder_debounce & 0b111) == (direction?0b111:0)) {
                        encoder_idle_timer = timer_read();
                        return DEBOUNCE_DN_MASK;
                    }
                } 
            } 
        }
        return 0;
    }
    return value; 
}

void select_all_rows()
{
}

void unselect_rows(void)
{
}

void select_next_row(uint8_t row)
{
    if (row == 0) {
        for (uint8_t i = 0; i < 40; i++) {
            DS_PL_HI();
            CLOCK_PULSE();
        }
        DS_PL_LO();
        CLOCK_PULSE();
    } else {
        DS_PL_HI();
        CLOCK_PULSE();
    }
    _delay_us(5);
}



bool suspend_wakeup_condition(void)
{
    DS_PL_LO();
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            CLOCK_PULSE();
            DS_PL_HI();
        }
        _delay_us(5);
        uint8_t key1 = PINF&(1<<5);
        uint8_t encoder_state = PINF&(1<<1) ? 0 : 1;
        if (encoder_state != encoder_state_prev[0][i] || key1 == 0) {
            return true;
        }
    }
    for (uint8_t i = 0; i < 5; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            if (i >= 3 && j == 0) DS_PL_HI();
            else DS_PL_LO();
            CLOCK_PULSE();
        }
    }
    _delay_us(5);
    if ( (PINF&0b100010) < 0b100010) { 
        kb_idle_timer = timer_read32();
        return true;
    }
    return false;
}

