/********************************** (C) COPYRIGHT *******************************
* File Name     : HOTKEYS_CFG.C
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
#include "hotkeys_cfg.h"

const hotKeyMap_s hotKeyMapDefaults[NUMBER_OF_PHYSICAL_HOTKEYS] = {
    {rotaryDialClockwise,                       7},
    {rotaryDialCounterClockwise,                7},
    {rotaryDialSwitchShort,                     7},
    {rotaryDialSwitchLong,                      7},
    {macroSwitch1Short,                         7},
    {macroSwitch1Long,                          7},
    {macroSwitch2Short,                         7},
    {macroSwitch2Long,                          7},
    {macroSwitch3Short,                         7},
    {macroSwitch3Long,                          7}         
};

__xdata hotKeyMap_s hotKeyMap[NUMBER_OF_PHYSICAL_HOTKEYS];
