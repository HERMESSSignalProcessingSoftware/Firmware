#include "../../tools/tools.h"
#include "adc_command.h"


using namespace apb_stamp;



#define ADS_CMD_WAKEUP              0xFF00U
#define ADS_CMD_SLEEP               0xFF02U
#define ADS_CMD_SYNC                0x0404U
#define ADS_CMD_RESET               0xFF06U
#define ADS_CMD_NOP                 0xFFFFU
#define ADS_CMD_RDATA               0xFF12U
#define ADS_CMD_RDATAC              0xFF14U
#define ADS_CMD_SDATAC              0xFF16U
#define ADS_CMD_RREG(reg, num)      0x2000U | (reg << 8) | (num - 1)
#define ADS_CMD_WREG(reg, num)      0x4000U | (reg << 8) | (num - 1)
#define ADS_CMD_SYSOCAL             0xFF60U
#define ADS_CMD_SYSGCAL             0xFF61U
#define ADS_CMD_SELFOCAL            0xFF62U

#define ADS_REG_MUX0                0x00U
#define ADS_REG_VBIAS               0x01U
#define ADS_REG_MUX1                0x02U
#define ADS_REG_SYS0                0x03U
#define ADS_REG_OFC0                0x04U
#define ADS_REG_FSC0                0x07U
#define ADS_REG_IDAC0               0x0AU
#define ADS_REG_IDAC1               0x0BU
#define ADS_REG_GPIOCFG             0x0CU
#define ADS_REG_GPIODIR             0x0DU
#define ADS_REG_GPIODAT             0x0EU


AdcCommand::AdcCommand (ErrorHandler eh): eh{eh} {}


AdcCommand::~AdcCommand () {}



bool AdcCommand::operator() (Stamp &stamp, uint8_t adcs) {
    uint8_t attempt = 1;
    std::string msg;
    do {
        // execute the action
        msg = execute(stamp, adcs);
    } while (!msg.empty() && eh && eh(attempt++, msg));

    return msg.empty();
}



AdcCommandReset::AdcCommandReset () {};


std::string AdcCommandReset::execute (Stamp &stamp, uint8_t adcs) {
    // issue the reset command
    stamp.writeAdc(adcs, ADS_CMD_RESET, Stamp::ModNone);
    // wait for 1ms to let them settle for further SPI communication
    delay(1);
    return "";
}



uint8_t AdcCommandReadReg::operator[] (uint8_t index) {
    if (index >= num || !readValues)
        return 0;

    // if odd index, take the most significant 8 bits
    if (index & 0x1U)
        return readValues[index>>1] >> 8;

    // if even index, take the least significant 8 bits
    return readValues[index>>1] & 0xFFU;
}


AdcCommandReadReg::AdcCommandReadReg (uint8_t offset, uint8_t num):
        offset{offset}, num{num} {}


AdcCommandReadReg::~AdcCommandReadReg () {
    // delete dynamic array if it is set
    if (readValues)
        delete[] readValues;
}


std::string AdcCommandReadReg::execute (Stamp &stamp, uint8_t adcs) {
    // divide num by two and round down
    uint8_t size = num >> 1;

    // create new results array
    if (readValues)
        delete[] readValues;
    readValues = new uint16_t[size];

    // read the values
    for (uint8_t i = 0; i < size; i++) {
        stamp.writeAdc(adcs, ADS_CMD_RREG(offset+i,2), Stamp::ModAtomic);
        stamp.writeAdc(adcs, ADS_CMD_NOP, Stamp::ModAtomic);
        readValues[i] = stamp.readAdc(Stamp::ModNone);
    }

    return "";
}



AdcCommandConfigure::AdcCommandConfigure (ErrorHandler eh): AdcCommand(eh) {};


std::string AdcCommandConfigure::execute (Stamp &stamp, uint8_t adcs) {
    std::string msg;

    // stop continuous data conversion mode
    stamp.writeAdc(adcs, ADS_CMD_SDATAC, Stamp::ModNone);

    for (uint8_t i = 0; i < 3; i++) {
        // continue work, if ADC is in adcs
        uint8_t adc = 1 << (4+i);
        if (adc & adcs) {

            // write to 12 registers
            stamp.writeAdc(adc, ADS_CMD_WREG(ADS_REG_MUX0, 12),
                    Stamp::ModAtomic);
            stamp.writeAdc(adc, (mux0[i] << 8) | vbias[i],
                    Stamp::ModAtomic);
            stamp.writeAdc(adc, (mux1[i] << 8) | sys0Pga[i] | sys0Sps[i],
                    Stamp::ModAtomic);
            stamp.writeAdc(adc, ((ofc[i]&0xFFU) << 8) | ((ofc[i]>>8) & 0xFFU),
                    Stamp::ModAtomic);
            stamp.writeAdc(adc, (ofc[i] >> 16) | (fsc[i] & 0xFFU),
                    Stamp::ModAtomic);
            stamp.writeAdc(adc, (fsc[i] & 0xFF00U) | (fsc[i] >> 16),
                    Stamp::ModAtomic);
            stamp.writeAdc(adc, (idac0[i] << 8) | idac1[i],
                    Stamp::ModNone);

            // read and compare the 12 register values
            AdcCommandReadReg reader(ADS_REG_MUX0, 12);
            reader(stamp, adc);
            uint16_t r0Mux0 = reader[0]; // !!!
            uint16_t r1Vbias = reader[1]; // !!!
            uint16_t r2Mux1 = reader[2]; // !!!
            uint16_t r3Sys0 = reader[3]; // !!!
            uint16_t r4Ofc0 = reader[4]; // !!!
            uint16_t r5Ofc1 = reader[5]; // !!!
            uint16_t r6Ofc2 = reader[6]; // !!!
            uint16_t r7Fsc0 = reader[7]; // !!!
            uint16_t r8Fsc1 = reader[8]; // !!!
            uint16_t r9Fsc2 = reader[9]; // !!!
            uint16_t r10Idac0 = reader[10]; // !!!
            uint16_t r11IDac1 = reader[11]; // !!!
            if (reader[0] != mux0[i] ||
                    reader[1] != vbias[i] ||
                    reader[2] != mux1[i] ||
                    reader[3] != (sys0Pga[i] | sys0Sps[i]) ||
                    reader[4] != (ofc[i] & 0xFFU) ||
                    reader[5] != ((ofc[i] >> 8) & 0xFFU) ||
                    reader[6] != (ofc[i] >> 16) ||
                    reader[7] != (fsc[i] & 0xFFU) ||
                    reader[8] != ((fsc[i] >> 8) & 0xFFU) ||
                    reader[9] != (fsc[i] >> 16) ||
                    ((reader[10] & 0x0FU) != idac0[i]) ||
                    reader[11] != idac1[i]) {
                // invalid configuration detected
                if (msg.empty()) {
                    msg += "Invalid configuration for STAMP with base address ";
                    msg += std::to_string(stamp.baseAddr);
                    msg += " ADC ";
                }
                else
                    msg += ", ";
                msg += std::to_string(i);
            }
        }
    };

    // re-enable continuous data conversion mode
    stamp.writeAdc(adcs, ADS_CMD_RDATAC, Stamp::ModNone);
    return msg;
}



AdcCommandCalibrate::AdcCommandCalibrate (CalibrationType type): type{type} {}


std::string AdcCommandCalibrate::execute (Stamp &stamp, uint8_t adcs) {
    // perform the calibration for each ADC
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t adc = i << (4+i);
        calValue[i] = 0;

        if (adcs & adc) {
            // perform the calibration for each ADC
            stamp.writeAdc(adc, type, Stamp::ModDataReady);

            // read the result and store it to the result register
            uint8_t reg = type == sysgcal ? ADS_REG_FSC0 : ADS_REG_OFC0;
            AdcCommandReadReg reader(reg, 4);
            reader(stamp, adc);
            calValue[i] = (reader[0] << 16) | (reader[1] << 8) | reader[2];
        }
    }

    return "";
}
