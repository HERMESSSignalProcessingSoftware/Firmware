#ifndef COMPONENTS_MEASUREMENT_H_
#define COMPONENTS_MEASUREMENT_H_

#include "../tools/tools.h"
#include "../tools/queue.h"
#include "../tools/datapackage.h"
#include "../apbdrivers/STAMP/apb_stamp.h"
#include "../apbdrivers/STAMP/adc_command.h"
#include "../sb_hw_platform.h"


/**
 * Measurement Singleton Class
 *
 * Uses Timer2 as 32 bit timer for Timestamp generation.
 *
 * After initialization the ADCs are configured for continuous 1kSPS operation
 * and the VHDL Stamp components are ready to capture interrupts. The ADCs are
 * not calibrated by default. Also by default the measurement is turned off and
 * must be turned on through a call to setDataStorage(true).
 *
 * Call worker() in main loop for data processing.
 */
class Measurement {
public:
    /** Singleton access method
     *
     * Use this method to get the Measurement instance
     * @return the instance
     */
    static Measurement &getInstance ();

    /** Worker function
     *
     * Call this function in main loop. Copies acquired StampDataframes
     * and calls Controller::datapackageAvailable() when a full package
     * was assembled.
     */
    void worker ();

    /** Sets the data storage
     *
     * Asserts the ADC_START line going to the ADCs enabling their continuous
     * mode. When they trigger the VHDL interrupts, the VHDL interrupts
     * will trigger the MSS interrupts.
     * USE THE CONTROLLER METHOD TO ACTUALLY TRIGGER THIS!
     *
     * @param start true for Starting, false for Stopping
     */
    void setDataAcquisition (bool start);

    /** Is Running
     *
     * @returns true, if the measurement is currently running
     */
    bool isRunning ();

private:
    apb_stamp::Stamp stamps[6]; /**< contains all Stamp instances */

    volatile uint8_t stampDataAvailable; /**< bitmask indicating, which
    stamps indicated that their data is available. */

    volatile apb_stamp::StampDataframe *dfs[6]; /**< Intermediate dataframees
    read from the Stamps during execution handleStampInterrupt(). Those
    will be copied into dp and deleted. */

    Datapackage dp; /**< The most recent datapackage containing all
    dataframes */

    /** Constructor
     *
     * Initializes all components so that they are ready to operate.
     */
    Measurement ();

    /** Stamp ISR template
     *
     * This function is called by all six STAMP data_available interrupts.
     * @param stamp The calling STAMP component in range [0, 5]
     */
    void handleStampInterrupt (uint8_t stamp);

    // define friend functions
    friend void F2M_INT_HANDLER(INT_STAMP1) ();
    friend void F2M_INT_HANDLER(INT_STAMP2) ();
    friend void F2M_INT_HANDLER(INT_STAMP3) ();
    friend void F2M_INT_HANDLER(INT_STAMP4) ();
    friend void F2M_INT_HANDLER(INT_STAMP5) ();
    friend void F2M_INT_HANDLER(INT_STAMP6) ();
};



#endif
