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
    {rotaryDialClockwise,           nullHandler},
    {rotaryDialCounterClockwise,    nullHandler},
    {rotaryDialSwitchShort,         nullHandler},
    {rotaryDialSwitchLong,          nullHandler},
    {macroSwitch1Short,             nullHandler},
    {macroSwitch1Long,              nullHandler},
    {macroSwitch2Short,             nullHandler},
    {macroSwitch2Long,              nullHandler},
    {macroSwitch3Short,             nullHandler},
    {macroSwitch3Long,              nullHandler}         
};

__xdata hotKeyMap_s hotKeyMap[NUMBER_OF_PHYSICAL_HOTKEYS];
