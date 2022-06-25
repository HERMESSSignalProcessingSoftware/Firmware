#include "configuration.h"
#include <cstring>
#include "../sb_hw_platform.h"
#include "../sf2drivers/drivers/mss_nvm/mss_nvm.h"
#include "tools.h"
#include "msghandler.h"



Configuration::Configuration () {
    uint8_t *memory = (uint8_t*) ADDR_NVM_CONF;
    if (memory[0] == 0xAAU)
        fromDapiFrame(&memory[1]);
}



Configuration::Configuration (const uint8_t dapiFrame[35]) {
    fromDapiFrame(dapiFrame);
}



void Configuration::writeToNVM () const {
    // set watermark and content to a temporary variable
    uint8_t frame[36];
    frame[0] = 0xAAU;
    toDapiFrame(&frame[1]);

    // write to nvm
    nvm_status_t status;
    status = NVM_unlock(ADDR_NVM_CONF, 36);

    if (status == NVM_SUCCESS)
        // try to write to NVM
        status = NVM_write(ADDR_NVM_CONF, frame, 36, NVM_LOCK_PAGE);
    else {
        MsgHandler::getInstance().error("Unknown error when trying "
                "to unlock eNVM: " + std::to_string(status));
        return;
    }

    // after writing compare the status
    if (status == NVM_SUCCESS)
        MsgHandler::getInstance().info("New configuration stored");
    else
        MsgHandler::getInstance().error("Unknown error when trying "
                "to write new configuration: " + std::to_string(status));
}



void Configuration::toDapiFrame (uint8_t dapiFrame[35]) const {
    std::memcpy(dapiFrame, confName, 16);
    dapiFrame[16] = (sgrSelfOffsetCal ? 0x80U : 0)
            | (sgrSystemOffsetCal ? 0x40U : 0)
            | (rtdSelfOffsetCal ? 0x20U : 0)
            | (rtdSystemOffsetCal ? 0x10U : 0)
            | (enableStorage ? 0x04U : 0)
            | (enableClear ? 0x02U : 0)
            | (enableTm ? 0x01U : 0);
    dapiFrame[17] = sgrPga | sgrSps;
    dapiFrame[18] = rtdPga | rtdSps;
    toByteArray(&dapiFrame[19], minTimestamp);
    toByteArray(&dapiFrame[27], maxTimestamp);
}



void Configuration::fromDapiFrame (const uint8_t dapiFrame[35]) {
    std::memcpy(confName, dapiFrame, 16);
    sgrSelfOffsetCal = dapiFrame[16] & 0x80U;
    sgrSystemOffsetCal = dapiFrame[16] & 0x40U;
    rtdSelfOffsetCal = dapiFrame[16] & 0x20U;
    rtdSystemOffsetCal = dapiFrame[16] & 0x10U;
    enableStorage = dapiFrame[16] & 0x04U;
    enableClear = dapiFrame[16] & 0x02U;
    enableTm = dapiFrame[16] & 0x01U;
    sgrPga = apb_stamp::AdcCommandConfigure::PGA(dapiFrame[17] & 0xF0U);
    sgrSps = apb_stamp::AdcCommandConfigure::SPS(dapiFrame[17] & 0x0FU);
    rtdPga = apb_stamp::AdcCommandConfigure::PGA(dapiFrame[18] & 0xF0U);
    rtdSps = apb_stamp::AdcCommandConfigure::SPS(dapiFrame[18] & 0x0FU);
    minTimestamp = 0;
    maxTimestamp = 0;
    for (uint8_t i = 0; i < 8; i++) {
        minTimestamp |= dapiFrame[19+i] << ((7-i) * 8);
        maxTimestamp |= dapiFrame[27+i] << ((7-i) * 8);
    }
}
