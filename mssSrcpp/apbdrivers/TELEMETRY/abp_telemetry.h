/*
 * abp_telemetry.h
 *
 *  Created on: 02.10.2022
 *      Author: Robin Grimsmann, M.Eng.
 */

#ifndef APBDRIVERS_TELEMETRY_ABP_TELEMETRY_H_
#define APBDRIVERS_TELEMETRY_ABP_TELEMETRY_H_

#include "../apbUtilities.h"
#include "../../sb_hw_platform.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
    uint32_t data4;
    uint32_t data5;
    uint32_t data6;
} TelemetryRXFrame_t;

#define MemoryAddressModifier   (1 << 8)
#define MemoryRXAddresModifier  (1 << 9)
#define MemoryAddressShift(i)   (i << 2)
#define StartMemoryAddress      0x00
#define EndMemoryAddress        23
/*  */
#define RegisterTransmitCounter 0x000
#define RegisterConfig          0x004
#define RegisterStatus          0x008
#define RegisterConfigBaud      0x00C
#define RegisterConfigGap       0x010
#define RegisterRX              0x014
/* Just Reading access! */
#define READRegisterZero        0x064

#define TELEMETRY_ADDR(i) (ADDR_TEMELEMTRY | i)


#define BAUD_9600   5208
#define BAUD_19200  2604
#define BAUD_38400  1302

#define GAP_3MS     150000
#define GAP_30kBits 87500

#define CONFIG_RESET_TX_BUFFER  (1 << 31)
#define CONFIG_RESET_RX_BUFFER  (1 << 30)
#define CONFIG_START            (1 << 1)
#define CONFIG_GLOBAL_START     (1 << 2)
#define CONFIG_INTERRPUT_ENA    (1 << 0)

/**
 *
 * @param baud
 * @param gap
 */
void InitTMDriver(uint32_t baud, uint32_t gap, uint32_t config);

void TelemetyLoadTXMemory(uint32_t data, uint32_t row);

/**
 * @param row: int \in [0:5]
 * @return content of the register
 */
uint32_t TelemetryGetRXMemoryLine(uint32_t row);

/**
 *
 */
void TelemetryTransmissionStart(void);

/**
 *
 */
void TelemetryTransmissionStop(void);

/**
 *
 * @return returns the content of the status register
 */
uint32_t TelemetryStatusRegister_get(void);

/**
 *Sets the value in the status register
 * @param value
 */
void TelemetryStatusRegister_set(uint32_t value);

/**
 * Returns the content of the  Config Reigster
 * @return int
 */
uint32_t TelemetryGetConfigReg(void);

/**
 *Sets the value of the content register
 * @param value int
 */
void TelemetryWriteConfigReg(uint32_t value);

void EnableSoftwareAutoStart(void);

#ifdef __cplusplus
}
#endif
#endif /* APBDRIVERS_TELEMETRY_ABP_TELEMETRY_H_ */
