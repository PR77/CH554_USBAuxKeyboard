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

#ifndef WS2812_PORT
#define WS2812_PORT          0x90
#endif

#ifndef WS2812_MOD_OC
#define WS2812_MOD_OC        P1_MOD_OC
#endif

#ifndef WS2812_DIR_PU
#define WS2812_DIR_PU        P1_DIR_PU
#endif

#ifndef WS2812_PIN
#define WS2812_PIN           3
#endif

#endif // __CH554_WS2812_CFG_H__