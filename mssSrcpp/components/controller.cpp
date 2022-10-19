#include "controller.h"
#include "dapi.h"
#include "memory.h"
#include "tm.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sf2drivers/drivers/mss_timer/mss_timer.h"
#include "../tools/msghandler.h"
#include "../tools/tools.h"



Controller &Controller::getInstance () {
    static Controller instance;
    return instance;
}



void Controller::worker () {
    // run this every 500ms
    if ((timestamp - last2HzTimestamp) >= 2000) {
        // toggle the MSS LED
        last2HzTimestamp = timestamp;
        hbLedOutputState[1] = !hbLedOutputState[1];
        MSS_GPIO_set_output(GPIO_PORT(LED_HB_MSS), hbLedOutputState[1]);
        // run the TM worker
    }

    if (telemetryInterruptEOTX) {
        Tm::getInstance().worker_irq();
    }

    if (telemetryInterruptRXDR) {
        // Read data via Tm::getInstance.xxxx() to get the message and parse it.
    }
    // toggle write protection indicator LED
    MSS_GPIO_set_output(GPIO_PORT(LED_HB_MEMSYNC), getGpioInput(IN_WP));

    // run this, if the LO occurred
    if (getGpioInput(IN_RXSM_LO) == rxsmSignal[0]) {
        rxsmSignal[0] = !rxsmSignal[0];
        if (rxsmSignal[0])
            MsgHandler::getInstance().info("LO asserted");
        else
            MsgHandler::getInstance().info("LO released");
    }

    // run this, if SOE change occurred
    if (getGpioInput(IN_RXSM_SOE) == rxsmSignal[1]) {
        rxsmSignal[1] = !rxsmSignal[1];

        std::string msg = std::string("SOE ") +
                (rxsmSignal[1] ? "asserted" : "released");
        bool warning = false;
        if (!rxsmSignal[1]) {
            if (clearingMemory) {
                // abort clearing memory
                Memory::getInstance().abortClearMemory();
                clearingMemory = false;
                msg.append(": Aborted clearing memory");
            }
        }
        else if (rxsmSignal[1] && !rxsmSignal[1] && !storedAcquisition
                && configuration.enableClear && getGpioInput(IN_WP)) {
            // clear the memory on SOE, NOT LO and no current measurements
            clearingMemory = true;
            Memory::getInstance().clearMemory();
            msg.append(": Clearing memory");
        }
        else {
            // do not clear memory
            msg.append(": Not clearing memory because measurements are "
                    "running or LO is asserted or configuration "
                    "prohibition or write protection");
            warning = true;
        }

        if (warning)
            MsgHandler::getInstance().warning(msg);
        else
            MsgHandler::getInstance().info(msg);
    }

    // run this, if SODS change occurred
    if (getGpioInput(IN_RXSM_SODS) == rxsmSignal[2]) {
        rxsmSignal[2] = !rxsmSignal[2];

        std::string msg = std::string("SODS ") +
                (rxsmSignal[2] ? "asserted" : "released");
        uint8_t type = 0;

        // SODS was set
        if (rxsmSignal[2]) {
            // write protection was set
            if (!getGpioInput(IN_WP)) {
                msg.append(": Write protection set");
                type = 2;
            }
            else if (!configuration.enableStorage) {
                msg.append(": Configuration prohibits storage");
                type = 1;
            }
            else if (!storedAcquisition) {
                msg.append(": Stored acquisition already running");
                type = 1;
            }
            else {
                setStoredDataAcquisition(true);
                msg.append(": Measurement started");
            }
        }
        else
            clearedMemoryBeforeSods = false;

        // send message
        if (type == 0)
            MsgHandler::getInstance().info(msg);
        else if (type == 1)
            MsgHandler::getInstance().warning(msg);
        else
            MsgHandler::getInstance().error(msg);
    }

    // check, if conditions are met to shut down the stored measurement
    if (storedAcquisition
            && ((rxsmSignal[2] && timestamp >= configuration.minTimestamp)
            || timestamp >= configuration.maxTimestamp)) {
        setStoredDataAcquisition(false);
        MsgHandler::getInstance().info("Stored measurement stopped");
    }
}



void Controller::datapackageAvailable (const Datapackage &dp) {
    // forward to memory
    if (storedAcquisition)
        Memory::getInstance().saveDp(dp);

    // enter this if clause with 2Hz
    if (++recCnt >= recCntThr) {
        recCnt = 0;
        hbLedOutputState[0] = !hbLedOutputState[0];
        MSS_GPIO_set_output(GPIO_PORT(LED_RECORDING),
                hbLedOutputState[0] ? 1 : 0);

        if (liveAcquisition)
            Dapi::getInstance().sendLiveData(dp);
    }
}



void Controller::setStoredDataAcquisition (bool running) {
    // abort, if stored acquisition already the same state
    if (running == storedAcquisition)
        return;
    storedAcquisition = running;

    if (running && liveAcquisition) {
        resetTimestampGenerator();
        MsgHandler::getInstance().warning("Live data timestamp reset to 0");
    }
    else if (running && !liveAcquisition) {
        resetTimestampGenerator();
        Measurement::getInstance().setDataAcquisition(true);
    }
    else if (!running && !liveAcquisition)
        Measurement::getInstance().setDataAcquisition(false);
}



void Controller::setLiveDataAcquisition (bool running) {
    std::string msg = "Live data acquisition ";
    // abort, if live data acquisition is already running
    if (running == liveAcquisition) {
        MsgHandler::getInstance().warning(msg +
                (running ? "already running" : "already stopped"));
        return;
    }
    liveAcquisition = running;

    // start continuous measurements
    if (running && !storedAcquisition) {
        resetTimestampGenerator();
        Measurement::getInstance().setDataAcquisition(true);
        MsgHandler::getInstance().info(msg + "started");
    }
    else if (running && storedAcquisition) {
        MsgHandler::getInstance().warning(msg + "started, but timestamps "
                "will be off");
    }
    else if (!running && !storedAcquisition) {
        Measurement::getInstance().setDataAcquisition(false);
        MsgHandler::getInstance().info(msg + "stopped");
    }
    else {
        MsgHandler::getInstance().info(msg + "stopped");
    }
}



uint64_t Controller::getTimestamp () const {
    return timestamp;
}



uint8_t Controller::getStateByte () const {
    uint8_t returner = (wdTriggeredFlag ? 0x80 : 0)
            | (rxsmSignal[0] ? 0x40 : 0)
            | (rxsmSignal[1] ? 0x20 : 0)
            | (rxsmSignal[2] ? 0x10 : 0)
            | (clearingMemory ? 0x08 : 0)
            | (storedAcquisition ? 0x04 : 0)
            | (!getGpioInput(IN_WP) ? 0x02 : 0)
            | (clearedMemoryBeforeSods ? 0x01 : 0);
    return returner;
}



void Controller::clearMemFinished () {
    clearingMemory = false;
    // set the cleared memory before sods flag to true, if the stored stored
    // acquisition hasn't started yet
    if (!storedAcquisition)
        clearedMemoryBeforeSods = true;
    MsgHandler::getInstance().info("Memory finished clearing");
}



void Controller::wdTriggered () {
    wdTriggeredFlag = true;
    MsgHandler::getInstance().error("Watchdog triggered");
    /* TODO !!! reload status (current timestamp, current address,
    currently measuring values?) */
}



Controller::Controller () {
    // initialize LEDs
    MSS_GPIO_config(GPIO_PORT(LED_HB_MEMSYNC), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(LED_HB_MEMSYNC), 0);
    MSS_GPIO_config(GPIO_PORT(LED_HB_MSS), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(LED_HB_MSS), 0);
    MSS_GPIO_config(GPIO_PORT(LED_RECORDING), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(LED_RECORDING), 0);

    // initialize inputs
    MSS_GPIO_config(GPIO_PORT(IN_RXSM_LO), MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(GPIO_PORT(IN_RXSM_SOE), MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(GPIO_PORT(IN_RXSM_SODS), MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(GPIO_PORT(IN_WP), MSS_GPIO_INPUT_MODE);

    // set the heartbeatCounterThreshold depending on the SGR samplerate
    switch (configuration.sgrSps) {
    case apb_stamp::AdcCommandConfigure::SPS5:
        recCntThr = 2;
        break;
    case apb_stamp::AdcCommandConfigure::SPS10:
        recCntThr = 5;
        break;
    case apb_stamp::AdcCommandConfigure::SPS20:
        recCntThr = 10;
        break;
    case apb_stamp::AdcCommandConfigure::SPS40:
        recCntThr = 20;
        break;
    case apb_stamp::AdcCommandConfigure::SPS160:
        recCntThr = 80;
        break;
    case apb_stamp::AdcCommandConfigure::SPS320:
        recCntThr = 160;
        break;
    case apb_stamp::AdcCommandConfigure::SPS640:
        recCntThr = 320;
        break;
    case apb_stamp::AdcCommandConfigure::SPS1000:
        recCntThr = 500;
        break;
    default:
        recCntThr = 1000;
        break;
    }

    // enable MSS timer 2 for the timestamp generator
    MSS_TIM2_init(MSS_TIMER_PERIODIC_MODE);
    MSS_TIM2_load_background(25000);
    MSS_TIM2_enable_irq();
    MSS_TIM2_start();

    // start timestamp generator
    resetTimestampGenerator();
}

void Controller::resetTimestampGenerator () {
    timestamp = 0;
    MSS_TIM2_load_immediate(25000);
}

void Controller::telemetryRXInterrupt() {
    telemetryInterruptRXDR = true;
    Tm::getInstance().clearInterrupt(TELEMETRY_STATUS_INTERRUPT_RX);
}

void Controller::telemetryTXInterrupt() {
    telemetryInterruptEOTX = true;
    Tm::getInstance().clearInterrupt(TELEMETRY_STATUS_INTERRUPT_TX);
}
