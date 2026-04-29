/********************************** (C) COPYRIGHT *******************************
* File Name     : HOTKEYS.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/28
* Description   : Hotkeys mapping and handler
*******************************************************************************/

#ifndef __CH554_HOTKEYS_H__
#define __CH554_HOTKEYS_H__

#include <stdint.h>
#include "ch554.h"
#include "hotkeys_cfg.h"

typedef struct {
    physicalHotKey_e physicalIndex;
    char *physicalText;
    char *physicalLabel;
    uint8_t xPositionLabel;
    uint8_t yPositionLabel;
} physicalHotKeys_s;

typedef struct {
    hotKeyHandlers_e handlerIndex;
    char *handlerText;
    void (*commandHandler)(void);
} hotKeyHandler_s;

extern const physicalHotKeys_s physicalHotKeys[];
extern const hotKeyHandler_s hotKeyHandlers[];

void hotkeys_initialise(void);
void hotkeys_displayPhysicalHotKeys(void);
void hotkeys_displayHotKeyHandlers(void);
void hotkeys_displayHotKeyMapping(void);
void hotkeys_triggerHotKeyHandler(physicalHotKey_e physicalHotKey);

#endif // __CH554_HOTKEYS_H__