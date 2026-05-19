/********************************** (C) COPYRIGHT *******************************
* File Name     : NVM.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/26
* Description   : Non-volatile memory (Data flash) handler
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "nvm.h"

__xdata int8_t lastValidBlock, nextFreeBlock;
__xdata uint8_t dataFlashMirror[DATA_FLASH_SIZE_MAX];

// IMPORTANT: readBlock and writeBlock block size parameter defines the data only portion
// size. 1 additional byte is appended to determine if a block is valid. This is part of
// the block-wise level wearing algorithum.

void nvm_initialise(void) {

    nextFreeBlock = -1;
    lastValidBlock = 0;
}

__xdata uint8_t * nvm_readDataFlashIntoMirror(void) {
    
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    
    for (uint8_t i = 0; i < DATA_FLASH_SIZE_MAX; i++) {
        ROM_ADDR_L = (i << 1);
        ROM_CTRL = ROM_CMD_READ;

        if ((ROM_STATUS & bROM_CMD_ERR) == 0) {
	        dataFlashMirror[i] = ROM_DATA_L;
        }
    }

    return (dataFlashMirror);
}

nvmStatus_e nvm_writeByte(uint8_t addressOffset, uint8_t data) {

    ROM_ADDR_L = (addressOffset << 1);
    ROM_DATA_L = data;

    // Checking bROM_ADDR_OK can be done before triggering the ROM_CMD_WRITE request.
    if ((ROM_STATUS & bROM_ADDR_OK) == bROM_ADDR_OK) {
        ROM_CTRL = ROM_CMD_WRITE;
    } 

    // Check ROM_STATUS after writing has occurred - Core is stopped during writing.
    // ... check for bROM_CMD_ERR
    if ((ROM_STATUS & bROM_CMD_ERR) == bROM_CMD_ERR) {
        return (nvmCommandError);
    } 

    // Check ROM_STATUS after writing has occurred - Core is stopped during writing.
    // ... check for bROM_ADDR_OK
    if ((ROM_STATUS & bROM_ADDR_OK) == 0) {
        return (nvmAddressError);
    }

    return (nvmOk);
}

nvmStatus_e nvm_readBlock(uint8_t *data, uint8_t size) {

    nvmStatus_e result = nvmOk;

    if (data == NULL) {
        return (nvmDataPointerError);
    }
    
    if (size > DATA_FLASH_SIZE_MAX) {
        return (nvmSizeError);
    }

    nextFreeBlock = nvm_findLastValidBlock(size);

    if (nextFreeBlock < 0) {
        // If there was an error finding the next free block, then return error.
        return (nvmNoValidBlockFound);
    }

    // Data flash base address is DATA_FLASH_ADDR and reads must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    
    for (uint8_t i = 0; i < size; i++) {
        ROM_ADDR_L = (((nextFreeBlock * (size + 1)) + i) << 1);
        ROM_CTRL = ROM_CMD_READ;

        if ((ROM_STATUS & bROM_CMD_ERR) == 0) {
	        data[i] = ROM_DATA_L;
        } else {
            result = nvmCommandError;
            break;    
        }
    }

    return (result);
}

nvmStatus_e nvm_writeBlock(const uint8_t *data, uint8_t size) {

    nvmStatus_e result = nvmOk;
    
    if (data == NULL) {
        return (nvmDataPointerError);
    }

    if (size > DATA_FLASH_SIZE_MAX) {
        return (nvmSizeError);
    }

    lastValidBlock = nvm_findLastValidBlock(size);

    if (lastValidBlock < 0) {
        // If there was no free valid block found, then start from block 0 again and
        // lastValidBlock is the last possible block.
        nextFreeBlock = 0;
        lastValidBlock = ((DATA_FLASH_SIZE_MAX / (size + 1)) - 1);
    } else {
        // Otherwise set nextFreeBlock to the next block in the chain based on the
        // value of lastValidBlock.
        nextFreeBlock = lastValidBlock + 1;

        if (nextFreeBlock > ((DATA_FLASH_SIZE_MAX / (size + 1)) - 1)) {
            nextFreeBlock = 0;
        }
    }
    
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bDATA_WE;
    SAFE_MOD = 0x00;

    // Data flash base address is DATA_FLASH_ADDR and writes are 16 bits. Only the lower
    // 8 bites contains the data to write so all writes must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;

    // Wrap the Data flash update in a do ... while loop to allow for easy clean-up where
    // Data flash accesses do not return nvmOk. This avoids gotos and labels.
    
    do {
        // Write data payload to the start of the next free block.
        for (uint8_t i = 0; i < size; i++) {
            result = nvm_writeByte(((nextFreeBlock * (size + 1)) + i), data[i]);
            
            if (result != nvmOk) {
                break;
            }
        }

        // Write DATA_FLASH_BLOCK_VALID to the end of the next free block.
        result = nvm_writeByte(((nextFreeBlock * (size + 1)) + size), DATA_FLASH_BLOCK_VALID);

        if (result != nvmOk) {
            break;
        }

        // Write DATA_FLASH_BLOCK_INVALID to the end of the previous block.
        result = nvm_writeByte(((lastValidBlock * (size + 1)) + size), DATA_FLASH_BLOCK_INVALID);
    } while (0);

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bDATA_WE;
    SAFE_MOD = 0x00;

    return (result);
}

int8_t nvm_findLastValidBlock(uint8_t size) {
    
    int8_t blockFound = -1;
    uint8_t numberOfBlocks = 0;

    // When checking size here, ensure an addition byte is added for the Valid Flag.
    if ((size + 1) > DATA_FLASH_SIZE_MAX) {
        return (blockFound);   
    } else {
        numberOfBlocks = DATA_FLASH_SIZE_MAX / (size + 1);
    }

    // Data flash base address is DATA_FLASH_ADDR and reads must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;

    for (uint8_t i = 0; i < numberOfBlocks; i++) {
        ROM_ADDR_L = (((i * (size + 1)) + size) << 1);
        // The position of the valid / invalid flasg is the last byte of the block
        // whos size is defined by the size parameter. Example where size is 20;
        // Block 0, byte number --> 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 [14-VALID_FLAG]
        // Block 1, byte number --> 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 [29-VALID_FLAG]
        // Block 2, byte number --> 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D [3E-VALID_FLAG]
        // Block 3, byte number --> 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 [53-VALID_FLAG]
        // Block 4, byte number --> 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67 [68-VALID_FLAG]
        // Block 5, byte number --> 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C [7D-VALID_FLAG]
        ROM_CTRL = ROM_CMD_READ;

        if ((ROM_STATUS & bROM_CMD_ERR) == 0) {
            if (ROM_DATA_L == DATA_FLASH_BLOCK_VALID) {
                blockFound = i;
                break;
            }
        } else {
            break;    
        }
    }

    return (blockFound);
}
