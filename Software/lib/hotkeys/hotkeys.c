/********************************** (C) COPYRIGHT *******************************
* File Name     : HOTKEYS.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/28
* Description   : Hotkeys mapping and handler
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "serial.h"
#include "device/usbhid.h"
#include "hotkeys.h"
#include "hotkeys_cfg.h"
#include "nvm.h"

#define PHYSICAL_HOTKEY_DESC_WIDTH  30
#define PHYSICAL_HOTKEY_LABEL_WIDTH 7

const physicalHotKeys_s physicalHotKeys[NUMBER_OF_PHYSICAL_HOTKEYS] = {
    {rotaryDialClockwise,           "rotaryDialClockwise",         NULL,       0, 0},
    {rotaryDialCounterClockwise,    "rotaryDialCounterClockwise",  NULL,       0, 0},
    {rotaryDialSwitchShort,         "rotaryDialSwitchShort",       NULL,       0, 0},
    {rotaryDialSwitchLong,          "rotaryDialSwitchLong",        NULL,       0, 0},
    {macroSwitch1Short,             "macroSwitch1Short",           "<< A >>",  0, 3},
    {macroSwitch1Long,              "macroSwitch1Long",            NULL,       0, 0},
    {macroSwitch2Short,             "macroSwitch2Short",           "<< B >>",  4, 2},
    {macroSwitch2Long,              "macroSwitch2Long",            NULL,       0, 0},
    {macroSwitch3Short,             "macroSwitch3Short",           "<< C >>",  8, 3},
    {macroSwitch3Long,              "macroSwitch3Long",            NULL,       0, 0},
};

#define HOTKEY_HANDLER_DESC_WIDTH   35

const hotKeyHandler_s hotKeyHandlers[NUMBER_OF_HOTKEY_HANDLERS] = {
    {mouseWheelRotateUp,        "usbhid_mouseWheelRotateUp",        usbhid_mouseWheelRotateUp},
    {mouseWheelRotateDown,      "usbhid_mouseWheelRotateDown",      usbhid_mouseWheelRotateDown},
    {consumerMediaMute,         "usbhid_consumerMediaMute",         usbhid_consumerMediaMute},
    {keyboardLockWorkstation,   "usbhid_keyboardLockWorkstation",   usbhid_keyboardLockWorkstation},
    {consumerMediaBrowser,      "usbhid_consumerMediaBrowser",      usbhid_consumerMediaBrowser},
    {consumerMediaEditor,       "usbhid_consumerMediaEditor",       usbhid_consumerMediaEditor},
    {consumerMediaCalculator,   "usbhid_consumerMediaCalculator",   usbhid_consumerMediaCalculator},
    {nullHandler,               "usbhid_nullHandler",               usbhid_nullHandler},
};

void hotkeys_initialise(void) {

    if (nvm_readBlock((uint8_t *)hotKeyMap, sizeof(hotKeyMap_s) * NUMBER_OF_PHYSICAL_HOTKEYS) != nvmOk) {

        serial_printString("\nhotKeyMapDefaults RESTORED\n");
        memcpy(hotKeyMap, hotKeyMapDefaults, sizeof(hotKeyMap_s) * NUMBER_OF_PHYSICAL_HOTKEYS);
    }
}

void hotkeys_updateHotKeyMapping(uint8_t hotKeyMapIndex, uint8_t hotKeyMapValue) {

    if (hotKeyMapIndex > NUMBER_OF_PHYSICAL_HOTKEYS) {
        return;
    }

    if (hotKeyMapValue > NUMBER_OF_HOTKEY_HANDLERS) {
        return;
    }

    hotKeyMap[hotKeyMapIndex].hotKeyHandler = (hotKeyHandlers_e)hotKeyMapValue;

    if (nvm_writeBlock((uint8_t *)hotKeyMap, sizeof(hotKeyMap_s) * NUMBER_OF_PHYSICAL_HOTKEYS) == nvmOk) {
        serial_printString("\nhotKeyMap UPDATED\n");
    }
}

void hotkeys_displayPhysicalHotKeys(void) {
   
    serial_printString("\nPhysical Hotkeys / OLED Label / Position ---------------------------------------\n");

    // Itterate through all of the hotkeys handlers
    for (uint8_t i = 0; i < NUMBER_OF_PHYSICAL_HOTKEYS; i++) {
        serial_printHexByte(i);
        serial_printString(":\t");

        if (i != (uint8_t)physicalHotKeys[i].physicalIndex) {
            serial_printString("Mapping ERROR detected");
        } else {        
            serial_printHexByte(physicalHotKeys[i].physicalIndex);
            serial_printString(":\t");
            serial_printStringPadded(physicalHotKeys[i].physicalText, PHYSICAL_HOTKEY_DESC_WIDTH);
            serial_printString(": ");
            serial_printStringPadded(physicalHotKeys[i].physicalLabel, PHYSICAL_HOTKEY_LABEL_WIDTH);
            serial_printString(" : ");
            serial_printHexByte(physicalHotKeys[i].xPositionLabel);
            serial_printCharacter(' ');
            serial_printHexByte(physicalHotKeys[i].yPositionLabel);
        }
        
        serial_printCharacter('\n');
    }
}

void hotkeys_displayHotKeyHandlers(void) {

    serial_printString("\nHotkey Handlers ----------------------------------------------------------------\n");

    // Itterate through all of the hotkeys handlers
    for (uint8_t i = 0; i < NUMBER_OF_HOTKEY_HANDLERS; i++) {
        serial_printHexByte(i);
        serial_printString(":\t");

        if (i != (uint8_t)hotKeyHandlers[i].handlerIndex) {
            serial_printString("Mapping ERROR detected");
        } else {        
            serial_printHexByte(hotKeyHandlers[i].handlerIndex);
            serial_printString(":\t");
            serial_printStringPadded(hotKeyHandlers[i].handlerText, HOTKEY_HANDLER_DESC_WIDTH);
        }
        
        serial_printCharacter('\n');
    }
}

void hotkeys_displayHotKeyMapping(void) {

    uint8_t mappedHotKeyHandlerIndex = 0;

    serial_printString("\nHotkey Mapping -----------------------------------------------------------------\n");

    // Itterate through all of the physical hotkeys
    for (uint8_t i = 0; i < NUMBER_OF_PHYSICAL_HOTKEYS; i++) {
        serial_printHexByte(i);
        serial_printString(":\t");

        if (i != (uint8_t)hotKeyMap[i].physicalHotKey) {
            serial_printString("Mapping ERROR detected");
        } else {
            mappedHotKeyHandlerIndex = hotKeyMap[i].hotKeyHandler;
            serial_printStringPadded(physicalHotKeys[i].physicalText, PHYSICAL_HOTKEY_DESC_WIDTH);
            serial_printString(" --> ");

            if (NUMBER_OF_HOTKEY_HANDLERS > mappedHotKeyHandlerIndex) {
                serial_printString(hotKeyHandlers[mappedHotKeyHandlerIndex].handlerText);
            } 
        }

        serial_printCharacter('\n');
    }
}

void hotkeys_triggerHotKeyHandler(physicalHotKey_e physicalHotKey) {

    hotKeyHandlers_e foundHotKeyHandlerIndex = nullHandler;
    
    // TODO: DATA-Flash table to translate the physicalHotKey number (i.e., enum rotary
    // encoder dial or switch, MACRO SW 1, 2 and 3, etc) to a mapped usbhid_xxx handler.
    // The hotKeyMap would be saved in DATA-Flash and modifable.
   
    for (uint8_t i = 0; i < NUMBER_OF_PHYSICAL_HOTKEYS; i++) {
        if (physicalHotKey == hotKeyMap[i].physicalHotKey) {
            foundHotKeyHandlerIndex = hotKeyMap[i].hotKeyHandler;

            // Quick sanity check to ensure hotKeyHandlerIndex in the mapping structure
            // actually matches handlerIndex in our support hotkey handlers.
            if (foundHotKeyHandlerIndex != hotKeyHandlers[foundHotKeyHandlerIndex].handlerIndex) {
                break;
            }

            if (hotKeyHandlers[(uint8_t)foundHotKeyHandlerIndex].commandHandler != NULL) {
                hotKeyHandlers[(uint8_t)foundHotKeyHandlerIndex].commandHandler();
            }
            break;
        }    
    }
}