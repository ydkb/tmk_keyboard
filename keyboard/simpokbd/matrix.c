/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

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

#include "ch.h"
#include "hal.h"

/*
 * scan matrix
 */
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "wait.h"
#include "backlight.h"
#include "rgblight.h"

#ifndef DEBOUNCE
#define DEBOUNCE 5
#endif

/* debounce for both key up and down */
#define DEBOUNCE_UP 7
#define DEBOUNCE_6 7

#define DEBOUNCE_MASK_UP (1 << DEBOUNCE_UP) - 1
#define DEBOUNCE_MASK_6 (1 << DEBOUNCE_6) - 1



extern debug_config_t debug_config;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS] = {0};

#define DEBOUNCE_MASK ((1 << DEBOUNCE) - 1)
static uint8_t matrix_current_row = 0;
static uint16_t matrix_scan_timestamp = 0;
static uint8_t matrix_debouncing[MATRIX_ROWS][MATRIX_COLS] = {0};


static void init_cols(void);
static void select_row(uint8_t row);
static uint8_t get_col(uint8_t col);
static void unselect_rows(void);

bool has_ws2812 = 0;

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
    //JTAG-DP Disabled and SW-DP Enabled..  need this to use PB3
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    palSetPadMode(GPIOB, 2, PAL_MODE_INPUT_PULLUP);
    wait_ms(5);
    if (palReadPad(GPIOB, 2) == PAL_HIGH) {
        has_ws2812 = 1;
        ws2812_spi_init();
    }

    backlight_init();
    rgblight_init();

    // initialize row and col
    unselect_rows();
    init_cols();
}

uint8_t matrix_scan(void)
{
    if (matrix_current_row == 0) { 
        uint16_t time_check = timer_read();
        if (matrix_scan_timestamp == time_check) return 1;
        matrix_scan_timestamp = time_check;
    }
    if (1) {
        select_row(matrix_current_row);
        wait_us(30);  // without this wait read unstable value.
        for (uint8_t i = 0; i < matrix_cols(); i++) {
            uint8_t *debounce = &matrix_debouncing[matrix_current_row][i];
            uint8_t col = get_col(i);
            *debounce = (*debounce << 1) + col;
            if ((*debounce > 0) && (*debounce < 255)) {
                matrix_row_t *row = &matrix[matrix_current_row];
                matrix_row_t mask = ((matrix_row_t)1 << i);
                if ((*debounce & DEBOUNCE_MASK_6) == DEBOUNCE_MASK_6) {  
                    *row |= mask;
                } else if ((*debounce & DEBOUNCE_MASK_UP) == 0) {
                    *row &= ~mask;
                } 
            }
        }

        unselect_rows();

        matrix_current_row++;
        if (matrix_current_row >= matrix_rows()) {
            matrix_current_row = 0;
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
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

/* Column pin configuration
 *  col: 0   1   2   3   4   5   6   7   8   9   10  11  12  13 14
 *  pin: B15 A2  B0  B1  B10 B11 B12 B13 B14 B9  B8  B3  B4  B5 A15
 */
static void  init_cols(void)
{
  palSetGroupMode(GPIOA, (1<<15|1<<2), 0, PAL_MODE_INPUT_PULLUP);
  palSetGroupMode(GPIOB, 0b1111111100111011, 0, PAL_MODE_INPUT_PULLUP);
}

/* Returns status of switches(1:on, 0:off) */
/* Column pin configuration
 *  col: 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14
 *  pin: B15 A2  B0  B1  B10 B11 B12 B13 B14 B9  B8  B3  B4  B5  15
 */
 
static uint8_t get_col(uint8_t col)
{
    switch (col) {
        case 0: return (palReadPad(GPIOB, 15)==PAL_HIGH) ? 0 : 1;
        case 1: return (palReadPad(GPIOA,  2)==PAL_HIGH) ? 0 : 1;
        case 2: return (palReadPad(GPIOB,  0)==PAL_HIGH) ? 0 : 1;
        case 3: return (palReadPad(GPIOB,  1)==PAL_HIGH) ? 0 : 1;
        case 4 ... 8: return (palReadPad(GPIOB, col+6)==PAL_HIGH) ? 0 : 1;
        case 9 ... 10: return (palReadPad(GPIOB, 18-col)==PAL_HIGH) ? 0 : 1;
        case 11 ... 13: return (palReadPad(GPIOB, col-8)==PAL_HIGH) ? 0 : 1;
        case 14: return (palReadPad(GPIOA, 15)==PAL_HIGH) ? 0 : 1;
        default: return 0;
    }
}

/* Row pin configuration
 *  row: 0   1  2  3  4  5 
 *  pin: A3 A4 A5 A6 A7 A8
 */
static void unselect_rows(void)
{
    if (has_ws2812) { 
        palSetGroupMode(GPIOA,  0b01111000, 0, PAL_MODE_INPUT);
        palSetPadMode(GPIOB,  2, PAL_MODE_INPUT);
    } else {
        palSetGroupMode(GPIOA,  0b11111000, 0, PAL_MODE_INPUT);
    }
}

static void select_row(uint8_t row)
{
    if (row == 4 && has_ws2812) {
        palSetPadMode(GPIOB, 2, PAL_MODE_OUTPUT_PUSHPULL);
        palClearPad(GPIOB, 2);
    } else {
        palSetPadMode(GPIOA, row+3, PAL_MODE_OUTPUT_PUSHPULL);
        palClearPad(GPIOA, row+3);
    }
}

