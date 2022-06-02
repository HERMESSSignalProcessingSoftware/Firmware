#include "apb_stamp.h"
#include "../../components/measurement.h"

using namespace apb_stamp;


// address modifier only for use with writeAdc
#define STAMP_MOD_NO_DR             0xF7FU


// usable addresses
#define STAMP_REG_NOP               0x000U
#define STAMP_REG_WRITE_DMS1        0x010U
#define STAMP_REG_WRITE_DMS2        0x020U
#define STAMP_REG_WRITE_TEMP        0x040U
#define STAMP_REG_READ_SPI_IN       0x080U
#define STAMP_REG_READ_SGR12        0x100U
#define STAMP_REG_READ_RTDSR        0x180U
#define STAMP_REG_CONF              0x200U
#define STAMP_REG_DUMMY             0x380U



StampConfig::StampConfig (uint32_t bitfield) {
    this->reset = bitfield >> 31;
    this->continuous = (bitfield >> 30) & 0x01U;
    this->pollingTimeout = (bitfield >> 22) & 0xFFU;
    this->id = bitfield & 0x07U;
}


uint32_t StampConfig::bitfield () const {
    return (uint32_t) this->id
            | ((uint32_t) this->pollingTimeout << 22)
            | ((uint32_t) (this->continuous ? 1 : 0) << 30)
            | ((uint32_t) (this->reset ? 1 : 0) << 31);
}


bool StampConfig::operator== (const StampConfig &other) const {
    return (this->id == other.id)
            && (this->pollingTimeout == other.pollingTimeout)
            && (this->continuous == other.continuous)
            && (this->reset == other.reset);
}



StampStatus::StampStatus (uint16_t bitfield) {
    for (uint8_t i = 0; i < 3; i++) {
        this->newVal[i] = (bitfield >> (15 - i)) & 0x01U;
        this->overwrittenVal[i] = (bitfield >> (12 - i)) & 0x01U;
    }
    this->asyncTime = (bitfield >> 3) & 0x7FU;
    this->id = bitfield & 0x07U;
}


uint16_t StampStatus::bitfield () const {
    uint16_t returner = (uint16_t) this->id
            | ((uint16_t) this->asyncTime << 3);
    for (uint8_t i = 0; i < 3; i++) {
        returner |= ((uint16_t) (this->overwrittenVal[i] ? 1 : 0) << (12-i))
                | ((uint16_t) (this->newVal[i] ? 1 : 0) << (15-i));
    }
    return returner;
}



StampDataframe::StampDataframe (uint32_t dataSgr12, uint32_t dataRtdStatus):
        dataSgr1{(uint16_t) (dataSgr12 >> 16)},
        dataSgr2{(uint16_t) (dataSgr12 & 0xFFFFU)},
        dataRtd{(uint16_t) (dataRtdStatus >> 16)},
        status(dataRtdStatus & 0xFFFFU),
        timestamp{Measurement::getInstance().getTimestamp()} {}



Stamp::Stamp (uint32_t baseAddr, IRQn interruptPin):
        baseAddr{baseAddr}, interruptPin{interruptPin} {}


bool Stamp::writeConfig (const StampConfig &conf) const {
    // write config to STAMP
    HW_set_32bit_reg(baseAddr | STAMP_REG_CONF, conf.bitfield());
    // read back config and compare
    return readConfig() == conf;
}


StampConfig Stamp::readConfig () const {
    return StampConfig(HW_get_32bit_reg(baseAddr | STAMP_REG_CONF));
}


StampDataframe Stamp::readDataframe () const {
    return StampDataframe(
            HW_get_32bit_reg(baseAddr | STAMP_REG_READ_SGR12 | ModAtomic),
            HW_get_32bit_reg(baseAddr | STAMP_REG_READ_RTDSR | ModStatusReset)
    );
}


void Stamp::writeAdc (uint8_t adcs, uint16_t val, uint16_t mod) const {
    HW_set_32bit_reg(baseAddr | adcs | mod, val);
}


uint16_t Stamp::readAdc (uint16_t mod) const {
    addr_t addr = baseAddr | STAMP_REG_READ_SPI_IN | (mod & STAMP_MOD_NO_DR);
    return HW_get_32bit_reg(addr);
}


void Stamp::enableInterrupt () const {
    NVIC_ClearPendingIRQ(interruptPin);
    NVIC_EnableIRQ(interruptPin);
}


void Stamp::disableInterrupt () const {
    NVIC_DisableIRQ(interruptPin);
    NVIC_ClearPendingIRQ(interruptPin);
}


void Stamp::clearInterrupt () const {
    NVIC_ClearPendingIRQ(interruptPin);
}
