#include <stdint.h>

#include "sf2drivers/CMSIS/system_m2sxxx.h"
#include "sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "sf2drivers/drivers/mss_watchdog/mss_watchdog.h"
#include "components/measurement.h"
#include "components/dapi.h"


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
    measurement.setDataStorage(true);

    // get the DAPI
    Dapi &dapi = Dapi::getInstance();

    for (;;) {
        MSS_WD_reload();
        measurement.worker();
        dapi.worker();
    }
    return 0;
}
