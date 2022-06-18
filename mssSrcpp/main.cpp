#include <stdint.h>

#include "sf2drivers/CMSIS/system_m2sxxx.h"
#include "sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "sf2drivers/drivers/mss_watchdog/mss_watchdog.h"
#include "components/measurement.h"
#include "components/dapi.h"
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

    // initiate the measurement
    Measurement &measurement = Measurement::getInstance();

    // get the DAPI
    Dapi &dapi = Dapi::getInstance();

    // send error message if this is a restart after a triggered watchdog
    if (MSS_WD_timeout_occured()) {
        MsgHandler::getInstance().error("Watchdog triggered");
        // !!! reload status configuration
        MSS_WD_clear_timeout_event();
    }

    for (;;) {
        MSS_WD_reload();
        measurement.worker();
        dapi.worker();
    }
    return 0;
}
