/********************************** (C) COPYRIGHT *******************************
* File Name     : WS2812.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/01/22
* Description   : WS2812(B) RGB LED Driver
*******************************************************************************/

#ifndef __CH554_WS2812_H__
#define __CH554_WS2812_H__

#include <stdint.h>
#include "ch554.h"
#include "ws2812_cfg.h"

#define WS2812_PIXEL_COUNT      12
#define WS2812_BYTES_PER_PIXEL  3
#define WS2812_T_RESET_US       65

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} ws2812RGB_s;

typedef enum {
    brightness100Percent = 0,
    brightness75Percent = 1,
    brightness50Percent = 2,
    brightness25Percent = 4,
    brightnessOff = 7
} ws2812Brightness_e;

void ws2812_initialise(void);
void ws2812_setPixelWheelColour(uint8_t pixel, uint8_t wheelPosition);
void ws2812_setPixelBrightess(uint8_t pixel, ws2812Brightness_e brightness);
void ws2812_displayFullWheelColour(ws2812Brightness_e brightness);
void ws2812_updatePixels(void);
static void ws2812_sendByte(uint8_t data);

#endif // __CH554_WS2812_H__