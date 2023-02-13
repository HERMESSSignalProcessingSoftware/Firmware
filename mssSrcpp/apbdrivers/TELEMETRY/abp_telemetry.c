/*
 * abp_telemetry.c
 *
 *  Created on: 02.10.2022
 *      Author: Robin Grimsmann, M.Eng.
 */


#include "abp_telemetry.h"
#include "../apbUtilities.h"
#include "../../hal/hal.h"

void APBInitTMDriver(uint32_t baud, uint32_t gap, uint32_t config) {
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfigBaud), baud);
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfigGap), gap);
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfig), config);
}


void APBTelemetyLoadTXMemory(uint32_t data, uint32_t row) {
    uint32_t address = TELEMETRY_ADDR(MemoryAddressShift(row)) | MemoryAddressModifier;
    HW_set_32bit_reg(address , data);
}


uint32_t ABPTelemetryGetRXMemoryLine(uint32_t row){
    return HW_get_32bit_reg(TELEMETRY_ADDR(MemoryAddressShift(row)) | MemoryRXAddresModifier);
}


void APBTelemetryTransmissionStart(void) {
    uint32_t configRegister = HW_get_32bit_reg(TELEMETRY_ADDR(RegisterConfig));
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfig), configRegister | CONFIG_START);
}

void APBTelemetryTransmissionStop(void) {
    uint32_t configRegister = HW_get_32bit_reg(TELEMETRY_ADDR(RegisterConfig));
    configRegister &= ~(CONFIG_START);
    configRegister &= ~(CONFIG_GLOBAL_START);
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfig), configRegister);
}

uint32_t APBTelemetryGetConfigReg(void) {
    return HW_get_32bit_reg(TELEMETRY_ADDR(RegisterConfig));
}

void APBTelemetryWriteConfigReg(uint32_t value) {
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfig), value);
}

uint32_t APBTelemetryStatusRegister_get(void) {
    return HW_get_32bit_reg(TELEMETRY_ADDR(RegisterStatus));
}

void __inline__ APBTelemetryStatusRegister_set(uint32_t value) {
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterStatus), value);
}

void APBEnableSoftwareAutoStart(void) {
    uint32_t config_reg = HW_get_32bit_reg(TELEMETRY_ADDR(RegisterConfig));
    HW_set_32bit_reg(TELEMETRY_ADDR(RegisterConfig), config_reg | CONFIG_GLOBAL_START);
}
