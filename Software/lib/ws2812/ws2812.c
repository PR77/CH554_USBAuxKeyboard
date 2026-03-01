/********************************** (C) COPYRIGHT *******************************
* File Name     : WS2812.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/01/22
* Description   : WS2812(B) RGB LED Driver
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "system.h"
#include "ws2812.h"
#include "ws2812_cfg.h"
#include "ws2812_gamma.h"

// References - 2020 by Stefan Wagner 
// Project Files (EasyEDA): https://easyeda.com/wagiminator
// Project Files (Github):  https://github.com/wagiminator
// License: http://creativecommons.org/licenses/by-sa/3.0/

// ===================================================================================
// Protocol Delays
// ===================================================================================
// There are three essential conditions:
// - T0H (HIGH-time for "0"-bit) must be max.  500ns
// - T1H (HIGH-time for "1"-bit) must be min.  625ns
// - TCT (total clock time) must be      min. 1150ns
// The bit transmission loop takes 11 clock cycles.
#if FREQ_SYS == 24000000        // 24 MHz system clock
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                         // 15 - 4 = 11 clock cycles for min 625ns
  #define TCT_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                         // 28 - 11 - 11 = 6 clock cycles for min 1150ns
#elif FREQ_SYS == 16000000      // 16 MHz system clock
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                         // 10 - 4 = 6 clock cycles for min 625ns
  #define TCT_DELAY \
    nop             \
    nop                         // 19 - 6 - 11 = 2 clock cycles for min 1150ns
#elif FREQ_SYS == 12000000      // 12 MHz system clock
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop                         // 8 - 4 = 4 clock cycles for min 625ns
  #define TCT_DELAY             // 14 - 4 - 11 < 0 clock cycles for min 1150ns
#elif FREQ_SYS == 6000000       // 13 MHz system clock
  #define T1H_DELAY             // 4 - 4 = 0 clock cycles for min 625ns
  #define TCT_DELAY             // 7 - 0 - 11 < 0 clock cycles for min 1150ns
#else
  #error Unsupported system clock frequency for NeoPixels!
#endif

SBIT(WS2812_LED, WS2812_PORT, WS2812_PIN);

static __xdata uint8_t ws2812_pixelBuffer[WS2812_PIXEL_COUNT * WS2812_BYTES_PER_PIXEL];

void ws2812_initialise(void) {

    WS2812_MOD_OC = WS2812_MOD_OC & ~(1 << WS2812_MOD_OC);
    WS2812_DIR_PU = WS2812_DIR_PU | (1 << WS2812_DIR_PU);

    memset(&ws2812_pixelBuffer, 0, sizeof(ws2812_pixelBuffer));
    WS2812_LED = 0;
}

void ws2812_setPixelWheelColour(uint8_t pixel, uint8_t wheelPosition) {
    ws2812RGB_s targetColour;

    if (pixel > (WS2812_PIXEL_COUNT - 1)) {
        return;
    }

    if (wheelPosition < 85) {
        targetColour.red = (wheelPosition * 3);
        targetColour.blue = (255 - wheelPosition);
        targetColour.green = 0;
    }
    else if (wheelPosition < 170) {
        wheelPosition -= 85;

        targetColour.red = 255 - (wheelPosition * 3);
        targetColour.blue = 0;
        targetColour.green = (wheelPosition * 3);
    } 
    else {
        wheelPosition -= 170;

        targetColour.red = 0;
        targetColour.blue = (wheelPosition * 3);
        targetColour.green = 255 - (wheelPosition * 3);
    }

    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 0] = gamma_lut[targetColour.red];
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 1] = gamma_lut[targetColour.green];
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 2] = gamma_lut[targetColour.blue];
}

void ws2812_setPixelBrightess(uint8_t pixel, ws2812Brightness_e brightness) {

    if (pixel > (WS2812_PIXEL_COUNT - 1)) {
        return;
    }

    // Simple but effective...
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 0] >>= (uint8_t)brightness;
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 1] >>= (uint8_t)brightness;
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 2] >>= (uint8_t)brightness;
}

void ws2812_displayFullWheelColour(ws2812Brightness_e brightness) {

    for (uint8_t i = 0; i < WS2812_PIXEL_COUNT; i++) {
        ws2812_setPixelWheelColour(i, (i * (UINT8_MAX / (WS2812_PIXEL_COUNT - 1))));
        ws2812_setPixelBrightess(i, brightness);
    }
    ws2812_updatePixels();
}

void ws2812_updatePixels(void) {

    system_disableGlobalInterupts();

    for (uint8_t i = 0; i < WS2812_PIXEL_COUNT * WS2812_BYTES_PER_PIXEL; i++) {
        ws2812_sendByte(ws2812_pixelBuffer[i]);    
    }

    system_enableGlobalInterupts();
    system_mDelayuS(WS2812_T_RESET_US);
}

// ===================================================================================
// Send a Data Byte to the Pixels String
// ===================================================================================
// This is the most time sensitive part. Outside of the function, it must be 
// ensured that interrupts are disabled and that the time between the 
// transmission of the individual bytes is less than the pixel's latch time.
static void ws2812_sendByte(uint8_t data) { 
    data;                       // stop unreferenced argument warning
    __asm
        .even
        mov  r7, #8             ; 2 CLK - 8 bits to transfer
        xch  a, dpl             ; 2 CLK - data byte -> accu
        01$:
        rlc  a                  ; 1 CLK - data bit -> carry (MSB first)
        setb _WS2812_LED        ; 2 CLK - WS2812_LED pin HIGH
        mov  _WS2812_LED, c     ; 2 CLK - "0"-bit? -> WS2812_LED pin LOW now
        T1H_DELAY               ; x CLK - TH1 delay
        clr  _WS2812_LED        ; 2 CLK - "1"-bit? -> WS2812_LED pin LOW a little later
        TCT_DELAY               ; y CLK - TCT delay
        djnz r7, 01$            ; 2/4|5|6 CLK - repeat for all bits
    __endasm;
}