/********************************** (C) COPYRIGHT *******************************
* File Name     : CLICKBTN.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/10
* Description   : Click Button Driver for debounce and encoding (8051 Port)
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "tick.h"
#include "clickbtn.h"
#include "clickbtn_cfg.h"

// References - 2016 by Paul Kourany 
// Project Files (Github):  https://github.com/pkourany/clickButton
// License: http://creativecommons.org/licenses/by-sa/3.0/

static __xdata clickBtn_s buttonObjects[NUMBER_OF_BUTTONS];
static const inputPort_s buttonPorts[NUMBER_OF_BUTTONS] = {
    {clickbtn_readRotaryEncSwPort,  (1 << ROTARY_ENC_SW_PIN)},
    {clickbtn_readMacroSw1Port,     (1 << MACRO_1_SW_PIN)},
    {clickbtn_readMacroSw2Port,     (1 << MACRO_2_SW_PIN)}
};

void clickbtn_initialise(void) {
    
    ROTARY_ENC_SW_MOD_OC = ROTARY_ENC_SW_MOD_OC & ~(1 << ROTARY_ENC_SW_PIN);
    ROTARY_ENC_SW_DIR_PU = ROTARY_ENC_SW_DIR_PU | (1 << ROTARY_ENC_SW_PIN);

    MACRO_1_SW_MOD_OC = MACRO_1_SW_MOD_OC & ~(1 << MACRO_1_SW_PIN);
    MACRO_1_SW_DIR_PU = MACRO_1_SW_DIR_PU | (1 << MACRO_1_SW_PIN);

    MACRO_2_SW_MOD_OC = MACRO_2_SW_MOD_OC & ~(1 << MACRO_2_SW_PIN);
    MACRO_2_SW_DIR_PU = MACRO_2_SW_DIR_PU | (1 << MACRO_2_SW_PIN);

    memset(buttonObjects, 0, (sizeof(clickBtn_s) * NUMBER_OF_BUTTONS));
}

static uint8_t clickbtn_readRotaryEncSwPort(void) {
    return (ROTARY_ENC_SW_PORT);
}

static uint8_t clickbtn_readMacroSw1Port(void) {
    return (MACRO_1_SW_PORT);
}

static uint8_t clickbtn_readMacroSw2Port(void) {
    return (MACRO_2_SW_PORT);
}

__xdata clickBtn_s* clickbtn_getButtonState(uint8_t buttonObjectIndex) {

    if (buttonObjectIndex >= NUMBER_OF_BUTTONS) {
        return (NULL);
    }

    return (&buttonObjects[buttonObjectIndex]);
}

void clickbtn_cyclicHandler(void) {

    uint32_t currentTickCount = tick_get1msTimerCount();
    
    for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++) {
        // Inputs are active LOW, so the actual state gets inverted
        buttonState_e _actualState = (buttonPorts[i].inputPort() & buttonPorts[i].inputMask) ? buttonReleased : buttonPressed;

        if (_actualState != buttonObjects[i].previousState) {
            buttonObjects[i].previousDebounceTime = currentTickCount;
        }

        if (((currentTickCount - buttonObjects[i].previousDebounceTime) > DEBOUNCE_TIME_MS) && (_actualState != buttonObjects[i].debouncedState)) {
            buttonObjects[i].debouncedState = _actualState;

            if (buttonObjects[i].debouncedState == buttonPressed) {
                buttonObjects[i].clickCounts++;  
            }
        }
        
        // Count short clicks - clicks will be positive
        if ((buttonObjects[i].debouncedState == buttonReleased) && ((currentTickCount - buttonObjects[i].previousDebounceTime) > MULTI_CLICK_TIME_MS)) {
            buttonObjects[i].clicks = buttonObjects[i].clickCounts;
            buttonObjects[i].clickCounts = 0;
        }

        // Count long clicks - clicks will be negative
        if ((buttonObjects[i].debouncedState == buttonPressed) && ((currentTickCount - buttonObjects[i].previousDebounceTime) > LONG_CLICK_TIME_MS)) {
            buttonObjects[i].clicks = 0 - buttonObjects[i].clickCounts;
            buttonObjects[i].clickCounts = 0;
        }

        buttonObjects[i].previousState = _actualState;
    }
}
