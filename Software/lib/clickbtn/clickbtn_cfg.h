/********************************** (C) COPYRIGHT *******************************
* File Name     : CLICKBTN_CFG.h
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/10
* Description   : Click Button Driver for debounce and encoding
*******************************************************************************/

#ifndef __CH554_CLICKBTN_CFG_H__
#define __CH554_CLICKBTN_CFG_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"

#define NUMBER_OF_BUTTONS           3

#define ROTARY_ENC_SW_INDEX         0
#define MACRO_1_SW_INDEX            1
#define MACRO_2_SW_INDEX            2

#define DEBOUNCE_TIME_MS            20
#define MULTI_CLICK_TIME_MS         250
#define LONG_CLICK_TIME_MS          1000

#ifndef ROTARY_ENC_SW_PORT
#define ROTARY_ENC_SW_PORT          P1
#endif

#ifndef ROTARY_ENC_SW_MOD_OC
#define ROTARY_ENC_SW_MOD_OC        P1_MOD_OC
#endif

#ifndef ROTARY_ENC_SW_DIR_PU
#define ROTARY_ENC_SW_DIR_PU        P1_DIR_PU
#endif

#ifndef ROTARY_ENC_SW_PIN
#define ROTARY_ENC_SW_PIN           2
#endif

#ifndef MACRO_1_SW_PORT
#define MACRO_1_SW_PORT             P3
#endif

#ifndef MACRO_1_SW_MOD_OC
#define MACRO_1_SW_MOD_OC           P3_MOD_OC
#endif

#ifndef MACRO_1_SW_DIR_PU
#define MACRO_1_SW_DIR_PU           P3_DIR_PU
#endif

#ifndef MACRO_1_SW_PIN
#define MACRO_1_SW_PIN              2
#endif

#ifndef MACRO_2_SW_PORT
#define MACRO_2_SW_PORT             P1
#endif

#ifndef MACRO_2_SW_MOD_OC
#define MACRO_2_SW_MOD_OC           P1_MOD_OC
#endif

#ifndef MACRO_2_SW_DIR_PU
#define MACRO_2_SW_DIR_PU           P1_DIR_PU
#endif

#ifndef MACRO_2_SW_PIN
#define MACRO_2_SW_PIN              4
#endif

#endif // __CH554_CLICKBTN_CFG_H__