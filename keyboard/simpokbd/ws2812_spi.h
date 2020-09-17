#include "hal.h"
#include "light_apa102.h"

//struct cRGB { uint8_t g; uint8_t r; uint8_t b; };

void set_leds_color_rgb(struct cRGB color);
void set_led_color_rgb(struct cRGB color, int pos);
void leds_init(void);


 // This is what users will use to interface with this
void ws2812_setleds(struct cRGB *ledarray, uint16_t number_of_leds);
void ws2812_setleds_rgbw(struct cRGB *ledarray, uint16_t number_of_leds);


void WS2812_init(void);
void WS2812_set_color( uint8_t index, uint8_t red, uint8_t green, uint8_t blue );
void WS2812_set_color_all( uint8_t red, uint8_t green, uint8_t blue );
void WS2812_send_colors(void);
