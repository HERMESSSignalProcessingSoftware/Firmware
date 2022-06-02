#include "measurement.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sf2drivers/drivers/mss_timer/mss_timer.h"
#include "../tools/msghandler.h"
#include "../tools/tools.h"
#include "dapi.h"



Measurement &Measurement::getInstance () {
    static Measurement instance;
    return instance;
}


Measurement::Measurement ():
    stamps{
        apb_stamp::Stamp(ADDR_STAMP1, F2M_INT_STAMP1_PIN),
        apb_stamp::Stamp(ADDR_STAMP2, F2M_INT_STAMP2_PIN),
        apb_stamp::Stamp(ADDR_STAMP3, F2M_INT_STAMP3_PIN),
        apb_stamp::Stamp(ADDR_STAMP4, F2M_INT_STAMP4_PIN),
        apb_stamp::Stamp(ADDR_STAMP5, F2M_INT_STAMP5_PIN),
        apb_stamp::Stamp(ADDR_STAMP6, F2M_INT_STAMP6_PIN)
    }, timestamp{0} {
    // initialize recording LED
    MSS_GPIO_config(LED_FPGA_LOADED, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(LED_FPGA_LOADED, 0);

    // start ADC and let it settle
    MSS_GPIO_config(OUT_ADC_START, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(OUT_ADC_START, 1);
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

    // prepare ADC system offset calibration
    apb_stamp::AdcCommandCalibrate cmdCal(
            apb_stamp::AdcCommandCalibrate::sysocal);


    for (uint8_t i = 0; i < 6; i++) {
        // reset ADCs
        cmdReset(stamps[i], allAdcs);

        // configure ADCs
        cmdConf(stamps[i], allAdcs);


    }

    // pull ADC start low to disable continuous mode
    MSS_GPIO_set_output(OUT_ADC_START, 0);
    // enable continuous mode of the VHDL unit to prepare it
    conf.continuous = true;

    for (uint8_t i = 0; i < 6; i++) {
        // enable continuous mode and set proper id
        conf.id = i;
        stamps[i].writeConfig(conf);

        // calibrate ADCs
        cmdCal(stamps[i], allAdcs);
    }

    // enable MSS timer 2 for the timestamp generator
    MSS_TIM2_init(MSS_TIMER_PERIODIC_MODE);
    MSS_TIM2_load_background(25000);

    // push info about finishing stamp initialization
    MsgHandler::getInstance().info("STAMP init complete");
}



void Measurement::setDataStorage (bool start) {
    if (start) {
        startTimestampGenerator();
        for (uint8_t i = 0; i < 6; i++)
            stamps[i].enableInterrupt();
        MSS_GPIO_set_output(OUT_ADC_START, 1);
        MsgHandler::getInstance().info("Measurement started");
    }
    else {
        for (uint8_t i = 0; i < 6; i++)
            stamps[i].disableInterrupt();
        MSS_GPIO_set_output(OUT_ADC_START, 0);
        stopTimestampGenerator();
        MsgHandler::getInstance().info("Measurement stopped");
    }
}



uint64_t Measurement::getTimestamp () const {
    return timestamp;
}



void Measurement::worker () {
    if (dfQueue.size() >= 6) {
        if (++heartbeatCounter >= 500) {
            heartbeatCounter = 0;
            ledOutputState = !ledOutputState;
            MSS_GPIO_set_output(LED_FPGA_LOADED, ledOutputState ? 1 : 0);
        }

        while (!dfQueue.empty())
            dfQueue.pop();
        /*for (uint8_t i = 0; i < 6; i++)
            */

        // pop first six elements and check, if all elements are within a
        // time limit
        /*uint64_t firstTime = dfQueue.front().timestamp;
        bool withinTime[6];
        for (uint8_t i = 0; i < 6; i++) {
            int8_t diff = dfQueue.front().timestamp - firstTime;
            withinTime[dfQueue.front().status.id] = (bool) ((diff & 0x7F) <= 1);
            dfQueue.pop();
        }

        for (uint8_t i = 0; i < 6; i++) {
            if (!withinTime[i])
                MsgHandler::getInstance().error("NE");
        }*/
    }
}



void Measurement::handleStampInterrupt (uint8_t stamp) {
    // do not allow nested stamp retrieval interrupts
    psr_t enabledInterrupts = HAL_disable_interrupts();
    // reset the pending IRQ and allow further processing
    NVIC_ClearPendingIRQ(stamps[stamp].interruptPin);
    // read ADCs data and enqueue
    dfQueue.push(stamps[stamp].readDataframe());
    // allow further interrupts
    HAL_restore_interrupts(enabledInterrupts);
}



void Measurement::startTimestampGenerator () {
    timestamp = 0;
    MSS_TIM2_enable_irq();
    MSS_TIM2_load_immediate(25000);
    MSS_TIM2_start();
}



void Measurement::stopTimestampGenerator () {
    MSS_TIM2_stop();
    MSS_TIM2_disable_irq();
}
