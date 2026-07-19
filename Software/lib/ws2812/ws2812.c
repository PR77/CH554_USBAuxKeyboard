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
// - T0H (HIGH-time for "0"-bit) must be max.  350ns +/- 150ns
// - T1H (HIGH-time for "1"-bit) must be min.  900ns +/- 150ns
// - TCT (total clock time) must be      min. 1250ns +/- 150ns
//
// Per bit, at FREQ_SYS clock speed (1 clock = 1 machine cycle on CH55x):
//   T0H = (3 + TCT_DELAY_cycles) * cycle_time
//   T1H = (5 + TCT_DELAY_cycles + T1H_DELAY_cycles) * cycle_time
//   TCT = (11 + TCT_DELAY_cycles + T1H_DELAY_cycles) * cycle_time
// where the fixed "11" and "3"/"5" come from the setb/rlc/mov/clr/nop/nop/djnz
// instructions in the bit-send loop below (each non-jump instruction takes as
// many cycles as it has bytes on this core).
#if FREQ_SYS == 24000000        // 24 MHz system clock, cycle = 41.7ns
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
    nop                         // 11 cycles -> T1H = (5+7+11)*41.7ns = 958ns  (>= 900ns OK)
  #define TCT_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                         // 7 cycles  -> T0H = (3+7)*41.7ns   = 417ns  (<= 500ns tolerant max, over 350ns nominal)
                                //           -> TCT = (11+7+11)*41.7ns = 1208ns (< 1250ns nominal, > 1100ns tolerant min)
#elif FREQ_SYS == 16000000      // 16 MHz system clock, cycle = 62.5ns
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                         // 7 cycles  -> T1H = (5+3+7)*62.5ns = 938ns  (>= 900ns OK)
  #define TCT_DELAY \
    nop             \
    nop             \
    nop                         // 3 cycles  -> T0H = (3+3)*62.5ns  = 375ns  (<= 500ns tolerant max, over 350ns nominal)
                                //           -> TCT = (11+3+7)*62.5ns = 1313ns (>= 1250ns OK)
#elif FREQ_SYS == 12000000      // 12 MHz system clock, cycle = 83.3ns
  #define T1H_DELAY \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop                         // 6 cycles  -> T1H = (5+0+6)*83.3ns = 917ns  (>= 900ns OK)
  #define TCT_DELAY             // 0 cycles  -> T0H = 3*83.3ns       = 250ns  (well under 350ns)
                                //           -> TCT = (11+0+6)*83.3ns = 1417ns (>= 1250ns OK)
#elif FREQ_SYS == 6000000       // 6 MHz system clock, cycle = 166.7ns
#error Unsupported system clock frequency for NeoPixels!
#endif

SBIT(WS2812_LED, WS2812_PORT, WS2812_PIN);

static __xdata uint8_t ws2812_pixelBuffer[WS2812_PIXEL_COUNT * WS2812_BYTES_PER_PIXEL];

void ws2812_initialise(void) {

    WS2812_MOD_OC = WS2812_MOD_OC & ~(1 << WS2812_PIN);
    WS2812_DIR_PU = WS2812_DIR_PU | (1 << WS2812_PIN);

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

void ws2812_setPixelBrightness(uint8_t pixel, ws2812Brightness_e brightness) {

    if (pixel > (WS2812_PIXEL_COUNT - 1)) {
        return;
    }

    // Simple but effective...
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 0] >>= (uint8_t)brightness;
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 1] >>= (uint8_t)brightness;
    ws2812_pixelBuffer[(pixel * WS2812_BYTES_PER_PIXEL) + 2] >>= (uint8_t)brightness;
}

void ws2812_displayFullWheelColour(ws2812Brightness_e brightness) {
    uint8_t colourStep = 0;

    if (WS2812_PIXEL_COUNT > 1) {
        colourStep = UINT8_MAX / (WS2812_PIXEL_COUNT - 1);
    }
    
    for (uint8_t i = 0; i < WS2812_PIXEL_COUNT; i++) {
        ws2812_setPixelWheelColour(i, (i * colourStep));
        ws2812_setPixelBrightness(i, brightness);
    }
    ws2812_updatePixels();
}

void ws2812_updatePixels(void) {

    for (uint8_t i = 0; i < WS2812_PIXEL_COUNT * WS2812_BYTES_PER_PIXEL; i++) {
        system_disableGlobalInterupts();
        ws2812_sendByte(ws2812_pixelBuffer[i]);    
        system_restoreGlobalInterupts();

        // Updated to only disable and restore global interruupts when updating LED
        // pixel data. Performance improvement.
    }

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
        setb _WS2812_LED        ; 2 CLK - WS2812_LED pin HIGH
        TCT_DELAY               ; x CLK - TCT delay
        rlc  a                  ; 1 CLK - data bit -> carry (MSB first)
        mov  _WS2812_LED, c     ; 2 CLK - "0"-bit? -> WS2812_LED pin LOW now
        T1H_DELAY               ; x CLK - TH1 delay
        clr  _WS2812_LED        ; 2 CLK - "1"-bit? -> WS2812_LED pin LOW a little later
        nop                     ; 1 CLK - "1"-bit? -> keep pin low a little longer
        nop                     ; 1 CLK - "1"-bit? -> keep pin low a little longer
        djnz r7, 01$            ; 2/6 CLK - repeat for all bits
    __endasm;
}