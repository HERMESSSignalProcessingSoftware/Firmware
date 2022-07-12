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

    /** Worker function
     *
     * Call this function in main loop. Evaluates RXSM signal lines (LO, SOE,
     * SODS). Toggles the MSS HB LED, toggles write protection LED
     * (Memsync HB LED). Calls the methods requiring a specific clocking (TM).
     */
    void worker ();

    /** Data package handler
     *
     * Transfers any assembled data package consisting of six STAMP data frames
     * @param dp
     */
    void datapackageAvailable (const Datapackage &dp);

    /** Start / Stop data measurement for storage
     *
     * If running == true, it will reset the timestamp to 0, regardless if
     * a live data acquisition is currently running. In this case, a warning
     * will be issued. If necessary it will start and stop the measurement
     * component.
     * @param running true, if the data acquisition shall be started
     */
    void setStoredDataAcquisition (bool running);

    /** Start / Stop Live Data Measurement and Sending to GSS via DAPI
     *
     * Logs event to DAPI and enables the transportation of live measurement
     * values to the GSS via DAPI. Is safe for calling multiple times. If
     * necessary it will start and stop the measurement component.
     * @param running true, if the data acquisition shall be started
     */
    void setLiveDataAcquisition (bool running);

    /** Timestamp getter
     *
     * @return the current timestamp in 250us increments after starting data
     * acquisition
     */
    uint64_t getTimestamp () const;

    /** Get the state byte
     *
     * @return The state byte according to TM spec
     */
    uint8_t getStateByte () const;

    /** Clearing of memory finished
     *
     * Call this function to inform the controller, that the clearing
     * of the memory has finished
     */
    void clearMemFinished ();

    /** WD triggered handler
     *
     * Call this function when detecting a MCU restart after the watchdog
     * triggered a full reset. This function will issue an error message
     * and try to recover the state prior to reset.
     */
    void wdTriggered ();

private:
    volatile uint64_t timestamp = 0; /**< Timestamp in units of
    250us (=1/4kHz) */

    bool liveAcquisition = false; /**< Indicates a live acquisition feeding
    live data via DAPI to the Ground Station Software */

    bool storedAcquisition = false; /**< Indicates, if the measurements
    currently being performed should be stored to memory */

    bool clearingMemory = false; /**< Indicates, if the measurement storage
    is currently being cleared. This state must not be active, when stored
    acquisition is active. */

    bool clearedMemoryBeforeSods = false; /**< Indicates, that the memory
    was cleared before the SODS signal came in and demanded a start
    of stored acquisition. This value is reset to false when SODS becomes
    unset again. */

    uint64_t storedAcquisitionStarted = 0; /**< The timestamp when the
    stored acquisition has started */

    uint16_t recCnt = 0; /**< Counts the number of datapackages
    in between a toggle of the recording heartbeat LED. */

    uint16_t recCntThr = 0; /**< The value of recCnt after which a recording
    heartbeat impulse will be made. This value is dependent on
    the SGR samplerate, such that a frequency of ~2Hz can be achieved. */

    uint64_t last2HzTimestamp = 0; /**< The last time the 2Hz handler was
    executed. Comparing with this value is also safe after a timestamp
    reset or overflow. */

    bool hbLedOutputState[2] = {false, false}; /**< The current state
    of the RECORDING and MSS HB LED in that order */

    bool rxsmSignal[3] = {false, false, false}; /**< The current state
    of the RXSM signal (LO, SOE, SODS) */

    bool wdTriggeredFlag = false; /**< The MCU was restarted after a
    WD reset */

    /** Private Controller instance
     *
     * Initializes globally used GPIOs
     */
    Controller ();

    /** Reset and start the timestamp generator
     *
     * After calling this action, the current timestamp will also be reset to 0.
     */
    void resetTimestampGenerator ();

    // define friend function for accessing timestamp
    friend void Timer2_IRQHandler ();
};


#endif /* COMPONENTS_CONTROLLER_H_ */
