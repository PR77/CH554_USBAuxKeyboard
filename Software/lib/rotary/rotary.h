/********************************** (C) COPYRIGHT *******************************
* File Name     : ROTARY.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/01/25
* Description   : ROTARY Switch Driver
*******************************************************************************/

#ifndef __CH554_ROTARY_H__
#define __CH554_ROTARY_H__

#include <stdint.h>
#include "ch554.h"
#include "rotary_cfg.h"

#define TRANSITIONS_PER_DETENT      4

typedef enum {
    noRotation = 0,
    clockwiseRotation,
    counterClockwiseRotation
} rotaryDirection_e;

void rotary_initialise(void);
rotaryDirection_e rotary_getDirection(void);
void rotary_cyclicHanlder(void);

#endif // __CH554_ROTARY_H__