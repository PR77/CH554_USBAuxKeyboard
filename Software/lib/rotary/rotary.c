/********************************** (C) COPYRIGHT *******************************
* File Name     : ROTARY.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/01/25
* Description   : ROTARY Switch Driver
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "system.h"
#include "rotary.h"
#include "rotary_cfg.h"

SBIT(ROTARY_ENC_A, ROTARY_ENC_A_PORT, ROTARY_ENC_A_PIN);
SBIT(ROTARY_ENC_B, ROTARY_ENC_B_PORT, ROTARY_ENC_B_PIN);

static rotaryDirection_e rotaryDirection = noRotation;
static uint8_t previousPostion = 0;
static int8_t rotationDelta = 0;

void rotary_initialise(void) {

    ROTARY_ENC_A_MOD_OC = ROTARY_ENC_A_MOD_OC & ~(1 << ROTARY_ENC_A_PIN);
    ROTARY_ENC_A_DIR_PU = ROTARY_ENC_A_DIR_PU | (1 << ROTARY_ENC_A_PIN);

    ROTARY_ENC_B_MOD_OC = ROTARY_ENC_B_MOD_OC & ~(1 << ROTARY_ENC_B_PIN);
    ROTARY_ENC_B_DIR_PU = ROTARY_ENC_B_DIR_PU | (1 << ROTARY_ENC_B_PIN);

    rotaryDirection = noRotation;
    previousPostion = 0;
    rotationDelta = 0;
}

rotaryDirection_e rotary_getDirection(void) {
    return (rotaryDirection);
}

void rotary_cyclicHanlder(void) {

    uint8_t currentPosition = 0;
    int8_t transition = 0;

    // ROTARY_ENC_A and ROTARY_ENC_B inputs are active LOW
    currentPosition = (!ROTARY_ENC_A << 1) | (!ROTARY_ENC_B);

    switch ((previousPostion << 2) | currentPosition) {
        // Clockwise transitions
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            transition += 1;
            break;

        // Counter-clockwise transitions
        case 0b0010:
        case 0b1011:
        case 0b1101:
        case 0b0100:
            transition -= 1;
            break;

        default:
            transition = 0;
    }

    previousPostion = currentPosition;
    rotationDelta += transition;

    if (rotationDelta >= TRANSITIONS_PER_DETENT) {
        rotaryDirection = clockwiseRotation;
        rotationDelta = 0;
    } else if (rotationDelta <= -TRANSITIONS_PER_DETENT) {
        rotaryDirection = counterClockwiseRotation;
        rotationDelta = 0;
    } else {
        rotaryDirection = noRotation;
    }
}