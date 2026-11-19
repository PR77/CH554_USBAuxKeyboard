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
// Two LED timing profiles are supported, selected by #define WS2812_V6 (in
// ws2812_cfg.h). Leave WS2812_V6 undefined for older/classic WS2812B strips.
//
//                        classic WS2812B         WS2812B-V6
//   T0H (0-bit HIGH)     350ns +/-150ns          220ns - 380ns
//   T1H (1-bit HIGH)     900ns +/-150ns          750ns - 1.6us
//   T0L (0-bit LOW)      (period - T0H)          750ns - 1.6us
//   T1L (1-bit LOW)      (period - T1H)          220ns - 420ns
//   RES (reset/latch)    > 50us                  > 300us  (see WS2812_T_RESET_US)
//
// Per bit, at FREQ_SYS clock speed (1 clock = 1 machine cycle on CH55x):
//   T0H = (3 + TCT_DELAY_cycles) * cycle_time
//   T1H = (5 + TCT_DELAY_cycles + T1H_DELAY_cycles) * cycle_time
//   period = (11 + TCT_DELAY_cycles + T1H_DELAY_cycles) * cycle_time
//   T0L = period - T0H = (8 + T1H_DELAY_cycles) * cycle_time
//   T1L = period - T1H = 6 * cycle_time                      <- FIXED, not tunable
// where the fixed "11"/"3"/"5"/"6" come from the setb/rlc/mov/clr/nop/nop/djnz
// instructions in the bit-send loop below (each non-jump instruction takes as
// many cycles as it has bytes on this core).
//
// IMPORTANT: T1L depends only on FREQ_SYS, not on the delay macros below, because
// it's just the fixed clr+nop+nop+djnz tail. At 12MHz/6MHz that tail is already
// 500ns/1000ns - both exceed WS2812B-V6's 420ns T1L max - so WS2812_V6 cannot be
// satisfied at those clocks without restructuring the asm loop itself (removing a
// trailing nop, reordering instructions, etc.), which is beyond a macro change.
// WS2812_V6 is therefore only offered at 24MHz and 16MHz below; it's a compile
// error at 12MHz/6MHz so it fails loudly instead of quietly missing spec.
#if FREQ_SYS == 24000000        // 24 MHz system clock, cycle = 41.7ns
  #ifdef WS2812_V6
    // WS2812B-V6: T0H=292ns, T1H=833ns, T0L=792ns, T1L=250ns - all within spec
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
      nop              
      // 11 cycles
    #define TCT_DELAY \
      nop             \
      nop             \
      nop             \
      nop
      // 4 cycles
  #else
    // Classic WS2812B: T0H=417ns, T1H=958ns, TCT=1208ns - within old +/-150ns tolerance
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
      nop              
      // 11 cycles
    #define TCT_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 7 cycles
  #endif
 
#elif FREQ_SYS == 16000000      // 16 MHz system clock, cycle = 62.5ns
  #ifdef WS2812_V6
    // WS2812B-V6: T0H=313ns, T1H=813ns, T0L=875ns, T1L=375ns - all within spec
    #define T1H_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 6 cycles
    #define TCT_DELAY \
      nop             \
      nop
      // 2 cycles
  #else
    // Classic WS2812B: T0H=375ns, T1H=938ns, TCT=1313ns - within old +/-150ns tolerance
    #define T1H_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 7 cycles
    #define TCT_DELAY \
      nop             \
      nop             \
      nop
      // 3 cycles
  #endif
 
#elif FREQ_SYS == 12000000      // 12 MHz system clock, cycle = 83.3ns
  #ifdef WS2812_V6
    #error WS2812B-V6 needs T1L <= 420ns, at 6MHz the loop is 1000ns. Use FREQ_SYS 16MHz or 24MHz for WS2812B-V6.
  #else
    // Classic WS2812B: T0H=250ns, T1H=917ns, TCT=1417ns
    #define T1H_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 6 cycles
    #define TCT_DELAY                               // 0 cycles
  #endif
 
#elif FREQ_SYS == 6000000       // 6 MHz system clock, cycle = 166.7ns
  #ifdef WS2812_V6
    #error WS2812B-V6 needs T1L <= 420ns, at 6MHz the loop is 1000ns. Use FREQ_SYS 16MHz or 24MHz for WS2812B-V6.
  #else
    // Classic WS2812B: T0H=500ns (at tolerant ceiling), T1H=833ns, TCT=1833ns
    #define T1H_DELAY                                // 0 cycles
    #define TCT_DELAY                                // 0 cycles
  #endif
 
#else
  #error Unsupported system clock frequency for NeoPixels!
#endif

#ifdef WS2812_V6
  #define WS2812_T_RESET_US       300
#else
  #define WS2812_T_RESET_US       50
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
        targetColour.blue = 255 - (wheelPosition * 3);
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