#include "measurement.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sf2drivers/drivers/mss_timer/mss_timer.h"
#include "msghandler.h"
#include "tools.h"



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

        // in case of errors try three times and log the messages
        MsgHandler &msgHandler = MsgHandler::getInstance();
        if (attempt < 3) {
            msgHandler.warning(msg);
            return true; // retry
        }
        msgHandler.error(msg);
        return false;

    });

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

        // enable STAMP interrupts
        stamps[i].enableInterrupt();
    }

    // enable MSS timer 2 for the timestamp generator
    MSS_TIM2_init(MSS_TIMER_PERIODIC_MODE);
    MSS_TIM2_enable_irq();
    MSS_TIM2_load_background(25000);
}



void Measurement::setDataStorage (bool start) {
    if (start) {
        MSS_GPIO_set_output(OUT_ADC_START, 1);
        startTimestampGenerator();
    }
    else {
        MSS_GPIO_set_output(OUT_ADC_START, 0);
        stopTimestampGenerator();
    }
}



void Measurement::handleStampInterrupt (uint8_t stamp) {
    Measurement &instance = Measurement::getInstance();
    // reset the pending IRQ and allow further processing
    NVIC_ClearPendingIRQ(instance.stamps[stamp].interruptPin);
}



void Measurement::startTimestampGenerator () {
    timestamp = 0;
    MSS_TIM2_load_immediate(25000);
    MSS_TIM2_start();
}



void Measurement::stopTimestampGenerator () {
    MSS_TIM2_stop();
}



void Timer2_IRQHandler () {
    Measurement::getInstance().timestamp++;
    MSS_TIM2_clear_irq();
}



void F2M_INT_STAMP1_HANDLER () {
    Measurement::handleStampInterrupt(0);
}
void F2M_INT_STAMP2_HANDLER () {
    Measurement::handleStampInterrupt(1);
}
void F2M_INT_STAMP3_HANDLER () {
    Measurement::handleStampInterrupt(2);
}
void F2M_INT_STAMP4_HANDLER () {
    Measurement::handleStampInterrupt(3);
}
void F2M_INT_STAMP5_HANDLER () {
    Measurement::handleStampInterrupt(4);
}
void F2M_INT_STAMP6_HANDLER () {
    Measurement::handleStampInterrupt(5);
}
