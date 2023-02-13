#include <stdint.h>

#include "sf2drivers/CMSIS/system_m2sxxx.h"
#include "sf2drivers/drivers/mss_watchdog/mss_watchdog.h"
#include "components/controller.h"
#include "components/measurement.h"
#include "components/tm.h"
#include "components/dapi.h"
#include "components/memory.h"
#include "components/controller.h"
#include "tools/msghandler.h"
<<<<<<< HEAD
=======
#include "model/telemetry.h"
>>>>>>> e34d31ef438c63e3bfd3979803f72878cb389376

int main () {
    // perform general register startup
    SystemInit();

    // initialize the watchdog
    // hardware configuration dictates a full reset after 1 second of
    // no refreshing
    MSS_WD_init();

    // initialize GPIOs
    MSS_GPIO_init();
    Tm &tm = Tm::getInstance();
    // initiate the base components
    Controller &controller = Controller::getInstance();
    Measurement &measurement = Measurement::getInstance();
    Memory &memory = Memory::getInstance();
    Dapi &dapi = Dapi::getInstance();
    tm.enableInterrupt();
    // send error message if this is a restart after a triggered watchdog
    if (MSS_WD_timeout_occured()) {
        controller.wdTriggered();
        MSS_WD_clear_timeout_event();
    }

    // inform about current configuration
    MsgHandler::getInstance().info(std::string("Configuration loaded: \"")
            + std::string(controller.configuration.confName)
            + "\" running SPU version " + SPU_VERSION);
<<<<<<< HEAD
    Memory::getInstance().recovery();
=======
    HERMESS::TelemetryData msg;
    msg.message(std::string("TM started running SPU version: ") + SPU_VERSION);
    Tm::getInstance().enqueue(msg);
    msg.message(std::string("1111111111111111111111111111111111111111111111111111111111"));
    Tm::getInstance().enqueue(msg);
    uint8_t array[1024] = {0};
>>>>>>> e34d31ef438c63e3bfd3979803f72878cb389376
    for (;;) {
        MSS_WD_reload();
        controller.worker();
        measurement.worker();
        memory.worker();
        dapi.worker();
        /*Testing purpos only*/
        //memory.readMemory(array, 0x200, 0x201);
    }
    return 0;
}
