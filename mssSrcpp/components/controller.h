#ifndef COMPONENTS_CONTROLLER_H_
#define COMPONENTS_CONTROLLER_H_

#include "measurement.h"



/** The Controller Class
 *
 * A singleton class managing major aspects of the SPU, like its different
 * modes.
 */
class Controller {
public:
    /** Singleton access method
     *
     * Use this method to get the Controller instance
     * @return the instance
     */
    static Controller &getInstance ();

    void datapackageAvailable (Measurement::Datapackage &dp);

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

    uint16_t heartbeatCounter = 0;

    bool hbLedOutputState = false;

    /** Private Controller instance
     */
    Controller ();
};


#endif /* COMPONENTS_CONTROLLER_H_ */
