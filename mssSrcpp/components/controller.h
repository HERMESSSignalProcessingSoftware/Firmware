#ifndef COMPONENTS_CONTROLLER_H_
#define COMPONENTS_CONTROLLER_H_

#include "measurement.h"
#include "../tools/configuration.h"



/** The Controller Class
 *
 * A singleton class managing major aspects of the SPU, like its different
 * modes. You must call these functions prior to getting the first instance:
 * MSS_GPIO_init()
 */
class Controller {
public:
    const Configuration configuration; /**< The SPU configuration as
    read from EEPROM NVM during SPU initialization. */

    /** Singleton access method
     *
     * Use this method to get the Controller instance
     * @return the instance
     */
    static Controller &getInstance ();

    /** Data package handler
     *
     * Transfers any assembled data package consisting of six STAMP data frames
     * @param dp
     */
    void datapackageAvailable (const Measurement::Datapackage &dp);

    /** Start / Stop Live Data Measurement and Sending to GSS via DAPI
     *
     * Logs event to DAPI and enables the transportation of live measurement
     * values to the GSS via DAPI. Is safe for calling multiple times.
     * @param running
     */
    void setLiveDataAcquisition (bool running);

private:
    bool liveAcquisition = false; /**< Indicates a live acquisition feeding
    live data via DAPI to the Ground Station Software */

    bool measurementRunning = false; /**< Indicates, if the measurement
    is currently running in continuous mode */

    uint16_t heartbeatCounter = 0; /**< Counts the number of datapackages
    in between a toggle of the heartbeat LED. */

    uint16_t heartbeatCounterThreshold = 0; /**< The value of heartbeatCounter
    after which a heartbeat impulse will be made. This value is dependent on
    the SGR samplerate, such that a frequency of ~2Hz can be achieved. */

    bool hbLedOutputState = false;

    /** Private Controller instance
     *
     * Initializes globally used GPIOs
     */
    Controller ();
};


#endif /* COMPONENTS_CONTROLLER_H_ */
