/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU_CFG.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "serial.h"
#include "system.h"
#include "menu.h"
#include "menu_cfg.h"
#include "menu_helper.h"
#include "hotkeys.h"
#include "nvm.h"

#define NUMBER_OF_MENU_ENTERIES     (sizeof(menuEntries) / sizeof(menuEntry_s))

const menuEntry_s menuEntries[] = {
    {'h', "Display command overview",   menu_printCommandOverview},
    {'d', "Dump CODE memory",           menu_dumpCODEMemory},
    {'x', "Dump XDATA memory",          menu_dumpXDATAMemory},
    {'n', "Dump NVM memory",            menu_dumpDataFlashMemory},
    {'m', "Modify Hotkey map",          menu_modifyHotKeyMap},
    {'r', "Reset",                      menu_coldReboot}
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
    
    serial_printString(MENU_CLEAR_SCREEN_STRING);
    serial_printStringTitle("Command Description", SERIAL_CONSOLE_COLUMN_WIDTH, '-', true);
    
    for (uint8_t i = 0; i < NUMBER_OF_MENU_ENTERIES; i++) {
        serial_printCharacter(menuEntries[i].commandName);
        serial_printCharacter('\t');
        serial_printString(menuEntries[i].commandText);
        serial_printCharacter('\n');
    }
}

void menu_signalError(void) {
  
    serial_printCharacter('\a');
}

void menu_dumpCODEMemory(char *argument) {

    static uint16_t baseAddress = 0;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: d|D <uint16_t> start address of 32 byte CODE block\n");
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

void menu_dumpXDATAMemory(char *argument) {

    static uint16_t baseAddress = 0;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: x|X <uint16_t> start address of 32 byte XDATA block\n");
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

void menu_dumpDataFlashMemory(char *argument) {

    uint8_t *dataFlashMirror = NULL;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: n|N Display 128 byte NVM block\n");
        return;
    }

    serial_printCharacter('\n');

    dataFlashMirror = nvm_readDataFlashIntoMirror();
    menu_dumpHex(dataFlashMirror, (uint16_t)DATA_FLASH_ADDR, 128);
}

void menu_modifyHotKeyMap(char *argument) {
    
    uint8_t hotKeyMapIndex = 0, hotKeyMapValue = 0;
    
    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: m|M <uint16_t> [MSB] Physical Hotkey [LSB] Hotkey Handler\n");
        return;
    }

    serial_printCharacter('\n');        

    if (argument == NULL) {
        // Get parameter or option value
        hotkeys_displayHotKeyMapping();
    } else {
        // Set parameter or option value
        hotKeyMapIndex = menu_parseNumericalString(argument) >> 8;
        hotKeyMapValue = menu_parseNumericalString(argument) & 0xFF;
        hotkeys_updateHotKeyMapping(hotKeyMapIndex, hotKeyMapValue);
    }
}

void menu_coldReboot(char *argument) {

    if ((argument != NULL) && (*argument == '?')) {
        serial_printString("\nUsage: r|R Perform a cold reboot\n");
        return;
    }

    system_coldReboot();
}