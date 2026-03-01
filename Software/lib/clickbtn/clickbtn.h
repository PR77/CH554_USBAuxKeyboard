/********************************** (C) COPYRIGHT *******************************
* File Name     : CLICKBTN.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/10
* Description   : Click Button Driver for debounce and encoding
*******************************************************************************/

#ifndef __CH554_CLICKBTN_H__
#define __CH554_CLICKBTN_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "clickbtn_cfg.h"

typedef enum {
    buttonReleased = 0,
    buttonPressed
} buttonState_e;

typedef struct {
    uint8_t (*inputPort)(void);
    uint8_t inputMask;
} inputPort_s;

typedef struct {
    int8_t clicks;
    int8_t clickCounts;
    buttonState_e debouncedState;
    buttonState_e previousState;
    uint32_t previousDebounceTime;
} clickBtn_s;

void clickbtn_initialise(void);
static uint8_t clickbtn_readRotaryEncSwPort(void);
static uint8_t clickbtn_readMacroSw1Port(void);
static uint8_t clickbtn_readMacroSw2Port(void);
__xdata clickBtn_s* clickbtn_getButtonState(uint8_t buttonObjectIndex);
void clickbtn_cyclicHandler(void);

#endif // __CH554_CLICKBTN_H__