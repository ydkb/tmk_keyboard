#ifndef DEBOUNCE_PK_H
#define DEBOUNCE_PK_H

#include <stdbool.h>
#include "print.h"
#ifdef __AVR__
#include "avr/avr_config.h"
#endif

#ifndef DEBOUNCE_DN
#define DEBOUNCE_DN 5
#endif

#ifndef DEBOUNCE_NK
#define DEBOUNCE_NK 1
#endif

#ifndef DEBOUNCE_UP
#define DEBOUNCE_UP 5
#endif

#if (DEBOUNCE_DN < 7) && (DEBOUNCE_NK < 7) && (DEBOUNCE_UP < 7)
#define DEBOUNCE_DN_MASK (uint8_t)(~(0x80 >> DEBOUNCE_DN))
#define DEBOUNCE_NK_MASK (uint8_t)(~(0x80 >> DEBOUNCE_NK))
#define DEBOUNCE_UP_MASK (uint8_t)(0x80 >> DEBOUNCE_UP)
#else
#error "DEBOUNCE VALUE must not exceed 6"
#endif



#endif
