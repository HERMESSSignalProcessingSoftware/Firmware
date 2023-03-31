#include "controller.h"
#include "dapi.h"
#include "memory.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sf2drivers/drivers/mss_timer/mss_timer.h"
#include "../tools/msghandler.h"
#include "../tools/tools.h"

Controller& Controller::getInstance() {
    static Controller instance;
    return instance;
}

void Controller::worker() {
    // run this every 500ms
    if ((timestamp - last2HzTimestamp) >= 2000) {
        // toggle the MSS LED
        last2HzTimestamp = timestamp;
        hbLedOutputState[1] = !hbLedOutputState[1];
        MSS_GPIO_set_output(GPIO_PORT(LED_HB_MSS), hbLedOutputState[1]);
        // run the TM worker
    }
}

void Controller::increaseTimestamp(void) {
    timestamp++;
}

uint64_t Controller::getTimestamp() const {
    return timestamp;
}

uint8_t Controller::getStateByte() const {
    uint8_t returner = (wdTriggeredFlag ? 0x80 : 0) | (rxsmSignal[0] ? 0x40 : 0)
            | (rxsmSignal[1] ? 0x20 : 0) | (rxsmSignal[2] ? 0x10 : 0)
            | (clearingMemory ? 0x08 : 0) | (storedAcquisition ? 0x04 : 0)
            | (!getGpioInput(IN_WP) ? 0x02 : 0)
            | (clearedMemoryBeforeSods ? 0x01 : 0);
    return returner;
}

void Controller::clearMemFinished() {

}

void Controller::wdTriggered() {
    wdTriggeredFlag = true;
    MsgHandler::getInstance().error("Watchdog triggered");
    /* TODO !!! reload status (current timestamp, current address,
     currently measuring values?) */
}

Controller::Controller() {
    // initialize LEDs
    MSS_GPIO_config(GPIO_PORT(LED_HB_MEMSYNC), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(LED_HB_MEMSYNC), 0);
    MSS_GPIO_config(GPIO_PORT(LED_HB_MSS), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(LED_HB_MSS), 0);
    MSS_GPIO_config(GPIO_PORT(LED_RECORDING), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(LED_RECORDING), 0);
}

void Controller::resetTimestampGenerator() {
    timestamp = 0;
    MSS_TIM2_load_immediate(25000);
}
