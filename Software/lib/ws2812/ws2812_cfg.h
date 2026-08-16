/********************************** (C) COPYRIGHT *******************************
* File Name     : WS2812_CFG.h
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/01/22
* Description   : WS2812(B) RGB LED Driver
*******************************************************************************/

#ifndef __CH554_WS2812_CFG_H__
#define __CH554_WS2812_CFG_H__

#include <stdint.h>
#include "ch554.h"

// Define WS2812_V6 to tune bit timing for WS2812B-V6 LEDs, which have tighter
// T0H/T1H/T0L/T1L windows than older/classic WS2812B strips. Leave this
// undefined to use classic WS2812B timing. Only supported at FREQ_SYS 16MHz
// and 24MHz - see the timing derivation comment in ws2812.c for why.
#define WS2812_V6

#ifndef WS2812_PIXEL_COUNT
#define WS2812_PIXEL_COUNT  12
#endif

#ifndef WS2812_PORT
#define WS2812_PORT         0xB0
#endif

#ifndef WS2812_MOD_OC
#define WS2812_MOD_OC       P3_MOD_OC
#endif

#ifndef WS2812_DIR_PU
#define WS2812_DIR_PU       P3_DIR_PU
#endif

#ifndef WS2812_PIN
#define WS2812_PIN          2
#endif

#endif // __CH554_WS2812_CFG_H__