/* Copyright 2017 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "is31fl3733.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "TWIlib.h"
#include "progmem.h"

#define ISSI_COMMAND_REG 0xFD
#define ISSI_COMMAND_REG_LOCK 0xFE
#define ISSI_COMMAND_REG_WRITE_ENABLE 0xC5
#define ISSI_CONTROL_PAGE 0x00 // LED Control Register
#define ISSI_PWM_PAGE 0x01 // PWM Register
// 0x00 - 0xBF set PWM duty for LED (w)

#define ISSI_ABM_PAGE 0x02 // Auto Breath Mode Register 

#define ISSI_FUNCTION_PAGE 0x03 // Function Register

uint8_t g_twi_transfer_buffer[TXMAXBUFLEN];
bool g_pwm_buffer_update_required = false;
uint8_t g_pwm_buffer[1][0xC0] = {0};

#define RGB_3733_ADDR_OR 0x20
#define RGB_3733_ADDR_OG 0x10
#define RGB_3733_ADDR_OB 0x00

typedef union {
  uint8_t raw;
  struct {
    uint8_t  offset :4;
    uint8_t  sw_lr  :2;
    uint8_t  num    :2;
  };
} rgb_index;

void IS31FL3733_set_color(uint8_t data, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_index index = (rgb_index)data;

    g_pwm_buffer[0][RGB_3733_ADDR_OR+index.sw_lr*0x30 + index.offset ] = red;
    g_pwm_buffer[0][RGB_3733_ADDR_OG+index.sw_lr*0x30 + index.offset ] = green;
    g_pwm_buffer[0][RGB_3733_ADDR_OB+index.sw_lr*0x30 + index.offset ] = blue;
    g_pwm_buffer_update_required = true;
}

void IS31FL3733_write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    g_twi_transfer_buffer[0] = (addr << 1) | 0x00;
    g_twi_transfer_buffer[1] = reg;
    g_twi_transfer_buffer[2] = data;

    TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
    while (TWIInfo.errorCode != 0xFF) {
        TWITransmitData(g_twi_transfer_buffer, 3, 0);
    }
}

void IS31FL3733_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer)
{
    IS31FL3733_select_page(addr, ISSI_PWM_PAGE);
    g_twi_transfer_buffer[0] = (addr << 1) | 0x00;
    for (uint8_t i = 0; i< 0xC0; i += 16) {
        g_twi_transfer_buffer[1] = i;
        for (uint8_t j = 0; j < 16; j++) {
            g_twi_transfer_buffer[j+2] = pwm_buffer[i+j];
        }
        TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
        while (TWIInfo.errorCode != 0xFF) {
            TWITransmitData(g_twi_transfer_buffer, 16+2, 0);
        }
    }
}

void IS31FL3733_select_page(uint8_t addr, uint8_t page)
{
    IS31FL3733_write_register(addr, ISSI_COMMAND_REG_LOCK, ISSI_COMMAND_REG_WRITE_ENABLE);
    IS31FL3733_write_register(addr, ISSI_COMMAND_REG, page);
}

void IS31FL3733_init(uint8_t addr)
{

    IS31FL3733_select_page(addr, ISSI_FUNCTION_PAGE);
    IS31FL3733_write_register(addr, 0x0, 0);
    IS31FL3733_write_register(addr, 0x0, 0b00000001);
    IS31FL3733_write_register(addr, 0x1, 128); 
    _delay_ms(10);
    IS31FL3733_select_page(addr, ISSI_CONTROL_PAGE);
    for (int i = 0x00; i <= 24; i++)
    {
        IS31FL3733_write_register(addr, i, 0b11111111);
    }
}

inline
void IS31FL3733_update_pwm_buffers_all()
{
    if (g_pwm_buffer_update_required) {
        g_pwm_buffer_update_required = false;
        IS31FL3733_write_pwm_buffer(0x50, g_pwm_buffer[0]);
    }
}