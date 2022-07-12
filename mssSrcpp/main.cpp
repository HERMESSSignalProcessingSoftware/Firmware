#include <stdint.h>

#include "sf2drivers/CMSIS/system_m2sxxx.h"
#include "sf2drivers/drivers/mss_watchdog/mss_watchdog.h"
#include "components/controller.h"
#include "components/measurement.h"
#include "components/dapi.h"
#include "components/memory.h"
#include "components/controller.h"
#include "tools/msghandler.h"


int main () {
    // perform general register startup
    SystemInit();

    // initialize the watchdog
    // hardware configuration dictates a full reset after 1 second of
    // no refreshing
    MSS_WD_init();

    // initialize GPIOs
    MSS_GPIO_init();

    // initiate the base components
    Controller &controller = Controller::getInstance();
    Measurement &measurement = Measurement::getInstance();
    Memory &memory = Memory::getInstance();
    Dapi &dapi = Dapi::getInstance();

    // send error message if this is a restart after a triggered watchdog
    if (MSS_WD_timeout_occured()) {
        controller.wdTriggered();
        MSS_WD_clear_timeout_event();
    }

    // inform about current configuration
    MsgHandler::getInstance().info(std::string("Configuration loaded: \"")
            + std::string(controller.configuration.confName)
            + "\" running SPU version " + SPU_VERSION);

    for (;;) {
        MSS_WD_reload();
        controller.worker();
        measurement.worker();
        memory.worker();
        dapi.worker();
    }
    return 0;
}
