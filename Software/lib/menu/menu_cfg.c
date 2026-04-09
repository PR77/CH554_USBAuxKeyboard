/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU_CFG.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "serial.h"
#include "menu.h"
#include "menu_cfg.h"

const menuEntry_s menuEntries[] = {
    {"h", "Display command overview\n", menu_printCommandOverview},
    {"s", "Rotary step size\n", menu_rotaryStepSize},
    {"r", "Reset\n", menu_coldReboot}
};

static uint16_t rotaryStepSize = 0;

void menu_printCommandPrompt(void) {
 
    serial_printCharacter('\n');
    serial_printCharacter(MENU_COMMAND_PROMPT_CHARACTER);
}

void menu_printCommandOverview(char *argument) {

    (void)argument;
    
    serial_printString("\x1b[2J\x1b[H\n");
    serial_printString("Command Description ------------------------------------------------------------\n");
    
    for (uint8_t i = 0; i < NUMBER_OF_MENU_ENTERIES; i++) {
        serial_printString(menuEntries[i].commandName);
        serial_printCharacter('\t');
        serial_printString(menuEntries[i].commandText);
    }
}

void menu_rotaryStepSize(char *argument) {
    
    if (*argument == '?') {
        serial_printString("Usage: s|S <uint16_t> step size\n");
        return;
    }

    if (argument == NULL) {
        // Get parameter or option value
        serial_printHexWord(rotaryStepSize);
        serial_printCharacter('\n');
    } else {
        // Set parameter or option value
        rotaryStepSize = (uint16_t)strtoul(argument, NULL, 10);
        serial_printHexWord(rotaryStepSize);
        serial_printCharacter('\n');        
    }
}

void menu_coldReboot(char *argument) {

    (void)argument;

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bSW_RESET;
}