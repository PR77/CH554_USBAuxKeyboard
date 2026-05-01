/********************************** (C) COPYRIGHT *******************************
* File Name     : NVM.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/26
* Description   : Non-volatile memory (Data Flash) handler
*******************************************************************************/

#ifndef __CH554_NVM_H__
#define __CH554_NVM_H__

#include <stdint.h>
#include "ch554.h"

#define DATA_FLASH_SIZE_MAX         128
#define DATA_FLASH_BLOCK_VALID      0xA5
#define DATA_FLASH_BLOCK_INVALID    0xFF

typedef enum {
    nvmOk = 0,
    nvmBusy,
    nvmGeneralError,
    nvmCommandError,
    nvmAddressError,
    nvmSizeError
} nvmStatus_e;

void nvm_initialise(void);
nvmStatus_e nvm_readBlock(uint8_t *data, uint8_t size);
nvmStatus_e nvm_writeBlock(const uint8_t *data, uint8_t size);

#endif // __CH554_NVM_H__