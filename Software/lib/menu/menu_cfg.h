/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU_CFG.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler
*******************************************************************************/

#ifndef __CH554_MENU_CFG_H__
#define __CH554_MENU_CFG_H__

#include <stdint.h>
#include "ch554.h"

#define MENU_CLEAR_SCREEN_STRING        "\x1b[2J\x1b[H"
#define MENU_COMMAND_PROMPT_CHARACTER   '>'

extern const menuEntry_s menuEntries[];

inline uint8_t menu_getNumberOfMenuEnteries(void);
void menu_printCommandPrompt(void);
void menu_printCommandOverview(char *argument);
void menu_signalError(void);
void menu_dumpROMMemory(char *argument);
void menu_dumpRAMMemory(char *argument);
void menu_rotaryStepSize(char *argument);
void menu_coldReboot(char *argument);

#endif // __CH554_MENU_CFG_H__