/********************************** (C) COPYRIGHT *******************************
* File Name     : ROTARY_CFG.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/01/25
* Description   : ROTARY Switch Driver
*******************************************************************************/

#ifndef __CH554_ROTARY_CFG_H__
#define __CH554_ROTARY_CFG_H__

#include <stdint.h>
#include "ch554.h"

#ifndef ROTARY_ENC_A_PORT
#define ROTARY_ENC_A_PORT           0x90
#endif

#ifndef ROTARY_ENC_A_MOD_OC
#define ROTARY_ENC_A_MOD_OC         P1_MOD_OC
#endif

#ifndef ROTARY_ENC_A_DIR_PU
#define ROTARY_ENC_A_DIR_PU         P1_DIR_PU
#endif

#ifndef ROTARY_ENC_A_PIN
#define ROTARY_ENC_A_PIN            1
#endif

#ifndef ROTARY_ENC_B_PORT
#define ROTARY_ENC_B_PORT           0x90
#endif

#ifndef ROTARY_ENC_B_MOD_OC
#define ROTARY_ENC_B_MOD_OC         P1_MOD_OC
#endif

#ifndef ROTARY_ENC_B_DIR_PU
#define ROTARY_ENC_B_DIR_PU         P1_DIR_PU
#endif

#ifndef ROTARY_ENC_B_PIN
#define ROTARY_ENC_B_PIN            0
#endif

#endif // __CH554_ROTARY_CFG_H__