/********************************** (C) COPYRIGHT *******************************
* File Name     : SYSTEM.C
* Author        : Zhiyuan Wan
* License       : MIT
* Version       : V1.0
* Date          : 2018/03/17
* Description   : 8051 System Functions
********************************************************************************
* Modified      : Paul Raspa (PR77)
* License       : MIT
* Version       : V2.0
* Date          : 2024/12/17
*******************************************************************************/

#ifndef __CH554_SYSTEM_H__
#define __CH554_SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>
#include "ch554.h"

typedef struct {
    uint32_t cycleCount;
    uint32_t startTime;
    uint32_t currentTime;
    uint32_t maximumTime;
    uint32_t totalTime;
} runTimeMeasurement_t;

void system_CfgFsys(void);
inline void system_restoreGlobalInterupts(void);
inline void system_enableGlobalInterupts(void);
inline void system_disableGlobalInterupts(void);
void system_initialiseRunTimeMeasurement(runTimeMeasurement_t *runTimeData);
void system_runTimeMeasurementStart(runTimeMeasurement_t *runTimeData, uint32_t value);
void system_runTimeMeasurementStop(runTimeMeasurement_t *runTimeData, uint32_t value);
void system_mDelayuS(uint16_t n);
void system_mDelaymS(uint16_t n);
inline void system_WDTModeSelect(uint8_t mode);
inline void system_WDTFeed(uint8_t timerTime);
inline void system_coldReboot(void);

#endif // __CH554_SYSTEM_H__
