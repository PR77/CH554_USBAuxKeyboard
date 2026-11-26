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
//   T0L (0-bit LOW)      800ns +/-150ns          580ns - 1.0us
//   T1H (1-bit HIGH)     800ns +/-150ns          580ns - 1.0us
//   T1L (1-bit LOW)      500ns +/-150ns          220ns - 420ns
//   RES (reset/latch)    > 50us                  > 300us  (see WS2812_T_RESET_US)
//
#if FREQ_SYS == 24000000        // 24 MHz system clock, cycle = 41.7ns
  #ifdef WS2812_V6
    // WS2812B-V6: T0H=333ns, T0L=833ns, T1H=792ns, T1L=417ns, CT=1166ns-1209ns
    #define TCT_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 5 cycles
    #define T1H_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop              
      // 9 cycles
    #define T1L_DELAY \
      nop             \
      nop 
      // 2 cycles
  #else
    // Classic WS2812B @24MHz: T0H=375, T0L=875, T1H=792, T1L=458, period=1250 ns
    #define TCT_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 6 cycles
    #define T1H_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 8 cycles
    #define T1L_DELAY \
      nop             \
      nop             \
      nop             \
      nop             \
      nop
      // 5 cycles
  #endif
 
#else
  #error Unsupported FREQ_SYS for NeoPixels! Only 24MHz is supported!
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

    for (uint16_t i = 0; i < WS2812_PIXEL_COUNT * WS2812_BYTES_PER_PIXEL; i++) {
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
        T1L_DELAY               ; x CLK - TL1 delay
        djnz r7, 01$            ; 6 CLK - repeat for all bits
    __endasm;
}