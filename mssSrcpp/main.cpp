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

    MSS_GPIO_init();
    MSS_GPIO_config(MSS_GPIO_0, MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_31, MSS_GPIO_OUTPUT_MODE);

    // initiate the measurement
    Measurement &measurement = Measurement::getInstance();

    // get the DAPI
    Dapi &dapi = Dapi::getInstance();

    for (;;) {
        MSS_WD_reload();
        MSS_GPIO_set_output(MSS_GPIO_31, MSS_GPIO_get_inputs() & 1U);
        dapi.worker();
    }
    return 0;
}
