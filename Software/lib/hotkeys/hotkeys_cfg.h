/********************************** (C) COPYRIGHT *******************************
* File Name     : HOTKEYS_CFG.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/28
* Description   : Hotkeys mapping and handler
*******************************************************************************/

#ifndef __CH554_HOTKEYS_CFG_H__
#define __CH554_HOTKEYS_CFG_H__

#include <stdint.h>
#include "ch554.h"

typedef enum {
    rotaryDialClockwise = 0,
    rotaryDialCounterClockwise,
    rotaryDialSwitchShort,
    rotaryDialSwitchLong,
    macroSwitch1Short,
    macroSwitch1Long,
    macroSwitch2Short,
    macroSwitch2Long,
    macroSwitch3Short,
    macroSwitch3Long,

    NUMBER_OF_PHYSICAL_HOTKEYS
} physicalHotKey_e;

typedef enum {
    mouseWheelRotateUp = 0,
    mouseWheelRotateDown,
    consumerMediaMute,
    keyboardLockWorkstation,
    consumerMediaBrowser,
    consumerMediaEditor,
    consumerMediaCalculator,
    nullHandler,

    NUMBER_OF_HOTKEY_HANDLERS
} hotKeyHandlers_e;

typedef struct {
    physicalHotKey_e physicalHotKey;
    hotKeyHandlers_e hotKeyHandler;     
} hotKeyMap_s;

extern const hotKeyMap_s hotKeyMapDefaults[];
extern __xdata hotKeyMap_s hotKeyMap[];

#endif // __CH554_HOTKEYS_CFG_H__