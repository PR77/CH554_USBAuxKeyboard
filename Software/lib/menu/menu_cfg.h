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

#define MENU_COMMAND_PROMPT_CHARACTER   '>'
#define NUMBER_OF_MENU_ENTERIES         2   //sizeof(menuEntries) / sizeof(menuEntry_s)

extern const menuEntry_s menuEntries[];

void menu_printCommandPrompt(void);
void menu_printCommandOverview(char *argument);
void menu_rotaryStepSize(char *argument);

#endif // __CH554_MENU_CFG_H__