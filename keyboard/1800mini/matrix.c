/*
Copyright 2013,2014 Kai Ryu <kai1103@gmail.com>

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
#include <avr/io.h>
#include <util/delay.h>
#include "lufa.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "ble51.h"
#include "ble51_task.h"
#include "rgblight.h"
#include "TWIlib.h"

#include "timer.h"
#include "wait.h"

#ifndef DEBOUNCE
#define DEBOUNCE 5
#endif
extern debug_config_t debug_config;
extern rgblight_config_t rgblight_config;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS] = {0};

#define DEBOUNCE_MASK ((1 << DEBOUNCE) - 1)
static uint8_t matrix_current_row = 0;
static uint16_t matrix_row_timestamp = 0;
static uint8_t matrix_debouncing[MATRIX_ROWS][MATRIX_COLS] = {0};


static void init_cols(void);
static void select_row(uint8_t row);
static uint8_t get_col(uint8_t col);
void unselect_rows(void);
void select_all_rows(void);

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

void matrix_init(void)
{
    TWIInit();
    PORTF |=  (1<<PF7);
    DDRF  |=  (1<<PF7);
    IS31FL3733_init(0x50);
    IS31FL3733_init(0x5F);
    
    rgblight_init();
    
    timer_init();
    _delay_ms(1);


    unselect_rows();
    init_cols();
}

uint8_t matrix_scan(void)
{
    static uint16_t elapsed;
    if (matrix_current_row == 0) {
        elapsed = timer_elapsed(matrix_row_timestamp);
    }
    if (elapsed >= 1) {
        if (matrix_current_row == 0) {
            matrix_row_timestamp = timer_read();
        }
        select_row(matrix_current_row);
        for (uint8_t i = 0; i < matrix_cols(); i++) {
            uint8_t *debounce = &matrix_debouncing[matrix_current_row][i];
            uint8_t col = get_col(i);
            uint8_t count = elapsed;
            do {
                *debounce <<= 1;
                *debounce |= col;
            } while (--count);
            matrix_row_t *row = &matrix[matrix_current_row];
            matrix_row_t mask = ((matrix_row_t)1 << i);
            switch (*debounce & DEBOUNCE_MASK) {
                case DEBOUNCE_MASK:
#if DEBOUNCE > 1
                case (DEBOUNCE_MASK >> 1):
#if DEBOUNCE > 2
                case (DEBOUNCE_MASK >> 2):
#if DEBOUNCE > 3
                case (DEBOUNCE_MASK >> 3):
#if DEBOUNCE > 4
                case (DEBOUNCE_MASK >> 4):
#if DEBOUNCE > 5
                case (DEBOUNCE_MASK >> 5):
#if DEBOUNCE > 6
                case (DEBOUNCE_MASK >> 6):
#if DEBOUNCE > 7
                case (DEBOUNCE_MASK >> 7):
#if DEBOUNCE > 8
                case (DEBOUNCE_MASK >> 8):
#if DEBOUNCE > 9
                case (DEBOUNCE_MASK >> 9):
#if DEBOUNCE > 10
                case (DEBOUNCE_MASK >> 10):
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                    if ((*row & mask) == 0) {
                        *row |= mask;
                    }
                    break;
#if DEBOUNCE > 1
                case ((DEBOUNCE_MASK << 1) & DEBOUNCE_MASK):
#if DEBOUNCE > 2
                case ((DEBOUNCE_MASK << 2) & DEBOUNCE_MASK):
#if DEBOUNCE > 3
                case ((DEBOUNCE_MASK << 3) & DEBOUNCE_MASK):
#if DEBOUNCE > 4
                case ((DEBOUNCE_MASK << 4) & DEBOUNCE_MASK):
#if DEBOUNCE > 5
                case ((DEBOUNCE_MASK << 5) & DEBOUNCE_MASK):
#if DEBOUNCE > 6
                case ((DEBOUNCE_MASK << 6) & DEBOUNCE_MASK):
#if DEBOUNCE > 7
                case ((DEBOUNCE_MASK << 7) & DEBOUNCE_MASK):
#if DEBOUNCE > 8
                case ((DEBOUNCE_MASK << 8) & DEBOUNCE_MASK):
#if DEBOUNCE > 9
                case ((DEBOUNCE_MASK << 9) & DEBOUNCE_MASK):
#if DEBOUNCE > 10
                case ((DEBOUNCE_MASK << 10) & DEBOUNCE_MASK):
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                    break;
                case 0:
                    if ((*row & mask) != 0) {
                        *row &= ~mask;
                    }
                    break;
                default:
                    print("bounce!: ");
                    pbin(*debounce & DEBOUNCE_MASK);
                    print("\n");
                    break;
            }
        }
        unselect_rows();
    }
    matrix_current_row++;
    if (matrix_current_row >= matrix_rows()) {
        matrix_current_row = 0;
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
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < matrix_rows(); i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

/* Column pin configuration
 *  col: 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14
 *  pin: A5 A6 A7 C7 E2 C6 C5 F0 B4 B5 B6 B0 E3 E7 E6
 */
static void  init_cols(void)
{
    // Input with pull-up(DDR:0, PORT:1)
    DDRF  &= ~(1<<PF0);
    PORTF |=  (1<<PF0);
    PORTE |=  (1<<PE7 | 1<<PE6 | 1<<PE3 | 1<<PE2);
    DDRE  &= ~(1<<PE7 | 1<<PE6 | 1<<PE3 | 1<<PE2);
    DDRC  &= ~(1<<PC7 | 1<<PC6 | 1<<PC5);
    PORTC |=  (1<<PC7 | 1<<PC6 | 1<<PC5);
    DDRB  &= ~(1<<PB6 | 1<<PB5 | 1<<PB4 | 1<<PB0);
    PORTB |=  (1<<PB6 | 1<<PB5 | 1<<PB4 | 1<<PB0);
    DDRA  &= ~(1<<PA7);
    PORTA |=  (1<<PA7);
    DDRA  &= ~(1<<PA6);
    PORTA |=  (1<<PA6);
    DDRA  &= ~(1<<PA5);
    PORTA |=  (1<<PA5);
}
/* Column pin configuration
 *  col: 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14
 *  pin: A5 A6 A7 C7 E2 C6 C5 F0 B4 B5 B6 B0 E3 E7 E6
 */
 
static uint8_t get_col(uint8_t col)
{

        switch (col) {
            case 0: return PINA&(1<<PA5) ? 0 : 1;
            case 1: return PINA&(1<<PA6) ? 0 : 1;
            case 2: return PINA&(1<<PA7) ? 0 : 1;
            case 3: return PINC&(1<<PC7) ? 0 : 1;
            case 4: return PINE&(1<<PE2) ? 0 : 1;
            case 5: return PINC&(1<<PC6) ? 0 : 1;
            case 6: return PINC&(1<<PC5) ? 0 : 1;
            case 7: return PINF&(1<<PF0) ? 0 : 1;
            case 8: return PINB&(1<<PB4) ? 0 : 1;
            case 9: return PINB&(1<<PB5) ? 0 : 1;
            case 10: return PINB&(1<<PB6) ? 0 : 1;
            case 11: return PINB&(1<<PB0) ? 0 : 1;
            case 12: return PINE&(1<<PE3) ? 0 : 1;
            case 13: return PINE&(1<<PE7) ? 0 : 1;
            case 14: return PINE&(1<<PE6) ? 0 : 1;
            default: return 0;
        }
}


/* Row pin configuration
 * row: 0   1   2   3   4
 * pin: F1  F2  F3  F4  B7
 */
void unselect_rows(void)
{
    // Hi-Z(DDR:0, PORT:0) to unselect
    DDRF &=~ 0b00011110;
    DDRB &=~ (1<<PB7);
}

static void select_row(uint8_t row)
{
    // Output low(DDR:1, PORT:0) to select
    if (row <4 ) DDRF |= (1<<(row+1));
    else if (row = 4) DDRB |=(1<<PB7);
}

void select_all_rows(void)
{
    DDRF |= 0b00011110;
    DDRB |= (1<<PB7);
}

bool suspend_wakeup_condition(void)
{
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        if (get_col(i)) return true;
    }
    return false;
}