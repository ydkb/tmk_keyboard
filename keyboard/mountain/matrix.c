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
#include "lufa.h"
#include "unimap.h"
#include "wait.h"
#include "hook.h"
#include "switch_board.h"


static uint8_t debounce_mask = DEBOUNCE_DN_MASK;

extern debug_config_t debug_config;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS] = {0};


static uint8_t matrix_current_row = 0;
static uint16_t matrix_scan_timestamp = 0;
static uint8_t matrix_debouncing[MATRIX_ROWS][MATRIX_COLS] = {0};
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

static void get_key_ready(void) {
    DDRB  &= ~(1<<3);
    PORTB |=  (1<<3);
    _delay_us(5);
}

inline void select_key_ready(void) {
    DDRB |= (1<<3);
} 


void matrix_init(void)
{

    //debug_config.enable = true;


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
    DDRB  |=  (1<<3 | 1<<1);
    DDRB  &= ~(1<<2);
    PORTB |=  (1<<3 | 1<<2 | 1<<1);
}


static uint8_t get_col(uint8_t col) {
    if (col<8) return PINB&(1<<3) ? 0 : 0x80;
    else return PINB&(1<<2) ? 0 : 0x80;
}


void select_all_rows()
{
}

void unselect_rows(void)
{
}

void select_next_row(uint8_t row)
{
    select_key_ready();
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
    get_key_ready();
}


void hook_nkro_change(uint8_t kbd_nkro) {
    /* 6kro or nkro display */
    if (kbd_nkro) {
        debounce_mask = DEBOUNCE_NK_MASK;
    } else {
        debounce_mask = DEBOUNCE_DN_MASK;
    }
    type_num(kbd_nkro?0:6);
}


