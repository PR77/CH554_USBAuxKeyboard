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
#include "system.h"
#include "menu.h"
#include "menu_cfg.h"
#include "menu_helper.h"

#define NUMBER_OF_MENU_ENTERIES     sizeof(menuEntries) / sizeof(menuEntry_s)

const menuEntry_s menuEntries[] = {
    {"h", "Display command overview\n", menu_printCommandOverview},
    {"d", "Dump ROM memory\n",          menu_dumpROMMemory},
    {"x", "Dump RAM memory\n",          menu_dumpRAMMemory},
    {"s", "Rotary step size\n",         menu_rotaryStepSize},
    {"r", "Reset\n",                    menu_coldReboot}
};

inline uint8_t menu_getNumberOfMenuEnteries(void) {
    return (NUMBER_OF_MENU_ENTERIES);
}

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

void menu_signalError(void) {
  
    serial_printCharacter('\a');
}

void menu_dumpROMMemory(char *argument) {

    static uint16_t baseAddress = 0;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: d|D <uint16_t> start address of 32 byte ROM memory block\n");
        return;
    }

    serial_printCharacter('\n');

    if (argument != NULL) {
        // Set parameter or option value         
        baseAddress = menu_parseNumericalString(argument);
    }

    menu_dumpHex((__code uint8_t *)baseAddress, (uint16_t)baseAddress, 32);
    baseAddress += 32;
}

void menu_dumpRAMMemory(char *argument) {

    static uint16_t baseAddress = 0;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: x|X <uint16_t> start address of 32 byte RAM memory block\n");
        return;
    }

    serial_printCharacter('\n');

    if (argument != NULL) {
        // Set parameter or option value         
        baseAddress = menu_parseNumericalString(argument);
    }

    menu_dumpHex((__xdata uint8_t *)baseAddress, (uint16_t)baseAddress, 32);
    baseAddress += 32;
}


void menu_rotaryStepSize(char *argument) {
    
    static uint16_t rotaryStepSize = 0;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: s|S <uint16_t> step size\n");
        return;
    }

    serial_printCharacter('\n');

    if (argument == NULL) {
        // Get parameter or option value
        serial_printHexWord(rotaryStepSize);
        serial_printCharacter('\n');
    } else {
        // Set parameter or option value
        rotaryStepSize = (uint16_t)menu_parseNumericalString(argument);
        serial_printHexWord(rotaryStepSize);
        serial_printCharacter('\n');        
    }
}

void menu_coldReboot(char *argument) {

    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: r|R Perform a cold reboot of device\n");
        return;
    }

    system_coldReboot();
}