#include "measurement.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../tools/msghandler.h"
#include "../tools/tools.h"
#include "controller.h"
#include "tm.h"


Measurement &Measurement::getInstance () {
    static Measurement instance;
    return instance;
}



void Measurement::worker () {
    if (stampDataAvailable) {
        // assemble data package
        psr_t isr = HAL_disable_interrupts();
        for (uint8_t i = 0; i < 6; i++) {
            if ((stampDataAvailable >> i) & 0x1U) {
                dp.readDf(const_cast<apb_stamp::StampDataframe&>(*dfs[i]));
                Tm::getInstance().addDataToQueue(const_cast<apb_stamp::StampDataframe&>(*dfs[i]), i);
                delete dfs[i];
                stampDataAvailable &= ~(1u << i);
            }
        }
        HAL_restore_interrupts(isr);
        if (dp.numReceived >= 6) {
            Controller::getInstance().datapackageAvailable(dp);
            dp.numReceived = 0;
        }
    }
}



Measurement::Measurement ():
    stamps{
        apb_stamp::Stamp(ADDR_STAMP1, F2M_INT_PIN(INT_STAMP1)),
        apb_stamp::Stamp(ADDR_STAMP2, F2M_INT_PIN(INT_STAMP2)),
        apb_stamp::Stamp(ADDR_STAMP3, F2M_INT_PIN(INT_STAMP3)),
        apb_stamp::Stamp(ADDR_STAMP4, F2M_INT_PIN(INT_STAMP4)),
        apb_stamp::Stamp(ADDR_STAMP5, F2M_INT_PIN(INT_STAMP5)),
        apb_stamp::Stamp(ADDR_STAMP6, F2M_INT_PIN(INT_STAMP6))
    } {
    // get the global SPU configuration
    const Configuration &spuConf = Controller::getInstance().configuration;

    // start ADC and let it settle
    MSS_GPIO_config(GPIO_PORT(OUT_ADC_START), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(OUT_ADC_START), 1);
    delay(20);

    // configure all 6 stamps
    apb_stamp::StampConfig conf;
    conf.reset = true;
    conf.continuous = false;
    conf.pollingTimeout = 100; // 1 second
    conf.id = 0;
    for (uint8_t i = 0; i < 6; i++)
        stamps[i].writeConfig(conf);

    // prepare all three ADCs
    uint8_t allAdcs = apb_stamp::Stamp::SGR1
            | apb_stamp::Stamp::SGR2
            | apb_stamp::Stamp::RTD;

    // prepare the ADC reset command
    apb_stamp::AdcCommandReset cmdReset;

    // prepare the ADC default configuration
    apb_stamp::AdcCommandConfigure cmdConf(
            [] (uint8_t attempt, std::string msg) -> bool {
        // enrich message
        msg.append(" (attempt ").append(std::to_string(attempt)).append(")");

        // in case of errors try three times and log the messages
        MsgHandler &msgHandler = MsgHandler::getInstance();

        if (attempt < 3) {
            msgHandler.warning(msg);
            return true; // retry
        }
        msgHandler.error(msg);
        return false;
    });
    cmdConf.sys0Pga[0] = spuConf.sgrPga;
    cmdConf.sys0Pga[1] = spuConf.sgrPga;
    cmdConf.sys0Pga[2] = spuConf.rtdPga;
    cmdConf.sys0Sps[0] = spuConf.sgrSps;
    cmdConf.sys0Sps[1] = spuConf.sgrSps;
    cmdConf.sys0Sps[2] = spuConf.rtdSps;

    // prepare ADC system offset calibration
    apb_stamp::AdcCommandCalibrate cmdCalSys(
            apb_stamp::AdcCommandCalibrate::sysocal);
    apb_stamp::AdcCommandCalibrate cmdCalSelf(
            apb_stamp::AdcCommandCalibrate::selfocal);

    for (uint8_t i = 0; i < 6; i++) {
        // reset ADCs
        cmdReset(stamps[i], allAdcs);

        // configure ADCs
        cmdConf(stamps[i], allAdcs);

        // calibrate ADCs
        cmdCalSys(stamps[i],
                (spuConf.sgrSystemOffsetCal ? apb_stamp::Stamp::SGR1
                        | apb_stamp::Stamp::SGR2 : 0)
                | (spuConf.rtdSystemOffsetCal ? apb_stamp::Stamp::RTD : 0));
        cmdCalSelf(stamps[i],
                (spuConf.sgrSelfOffsetCal ? apb_stamp::Stamp::SGR1
                        | apb_stamp::Stamp::SGR2 : 0)
                | (spuConf.rtdSelfOffsetCal ? apb_stamp::Stamp::RTD : 0));
    }

    // pull ADC start low to disable continuous mode
    MSS_GPIO_set_output(GPIO_PORT(OUT_ADC_START), 0);
    // enable continuous mode of the VHDL unit to prepare it
    conf.continuous = true;

    for (uint8_t i = 0; i < 6; i++) {
        // enable continuous mode and set proper id
        conf.id = i;
        stamps[i].writeConfig(conf);
    }
}



void Measurement::setDataAcquisition (bool start) {
    if (start) {
        for (uint8_t i = 0; i < 6; i++)
            stamps[i].enableInterrupt();
        MSS_GPIO_set_output(GPIO_PORT(OUT_ADC_START), 1);
    }
    else {
        for (uint8_t i = 0; i < 6; i++)
            stamps[i].disableInterrupt();
        MSS_GPIO_set_output(GPIO_PORT(OUT_ADC_START), 0);
    }
}



bool Measurement::isRunning () {
    return getGpioInput(OUT_ADC_START);
}



void Measurement::handleStampInterrupt (uint8_t stamp) {
    // reset the pending IRQ and allow further processing
    NVIC_ClearPendingIRQ(stamps[stamp].interruptPin);
    if (stampDataAvailable & (1u << stamp))
        stamps[stamp].resetInterrupt();
    else {
        dfs[stamp] = stamps[stamp].readDataframe();
        stampDataAvailable |= (1u << stamp);
    }
}
