/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler
*******************************************************************************/

#ifndef __CH554_MENU_H__
#define __CH554_MENU_H__

#include <stdint.h>
#include "ch554.h"

#define MENU_COMMAND_BUFFER_SIZE        16

typedef struct {
    char *commandName;
    char *commandText;
    void (*commandHandler)(char *argument);
} menuEntry_s;

void menu_initialise(void);
void menu_processCommand(void);
void menu_cyclicHandler(char character);

#endif // __CH554_MENU_H__