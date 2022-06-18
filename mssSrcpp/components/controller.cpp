#include "controller.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../tools/msghandler.h"
#include "dapi.h"


Controller &Controller::getInstance () {
    static Controller instance;
    return instance;
}



void Controller::datapackageAvailable (Measurement::Datapackage &dp) {
    // enter this if clause with 2Hz
    if (++heartbeatCounter >= 5) {
        heartbeatCounter = 0;
        hbLedOutputState = !hbLedOutputState;
        MSS_GPIO_set_output(LED_HB_MEMSYNC, hbLedOutputState ? 1 : 0);

        if (liveAcquisition)
            Dapi::getInstance().sendLiveData(dp);
    }
}



void Controller::setLiveDataAcquisition (bool running) {
    std::string msg = "Live data acquisition ";
    // abort, if live data acquisition is already running
    if (running == liveAcquisition) {
        MsgHandler::getInstance().warning(msg +
                (running ? "already running" : "already stopped"));
        return;
    }

    // start continuous measurements
    if (running != measurementRunning)
        Measurement::getInstance().setDataAcquisition(running);
    liveAcquisition = running;
    measurementRunning = running;

    // inform the DAPI
    MsgHandler::getInstance().info(msg + (running ? "started" : "stopped"));
}



Controller::Controller () {
    // initialize LEDs
    MSS_GPIO_config(LED_HB_MEMSYNC, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(LED_HB_MEMSYNC, 0);
    MSS_GPIO_config(LED_RECORDING, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(LED_RECORDING, 0);
}
