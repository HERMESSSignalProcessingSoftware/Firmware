#ifndef COMPONENTS_MEASUREMENT_H_
#define COMPONENTS_MEASUREMENT_H_

#include "../tools/tools.h"
#include "../tools/queue.h"
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
    struct Datapackage {
        enum errorCodes: uint8_t {
            AdcLagging = 0x01U,
            StampLagging = 0x02U,
            NoNew = 0x04U,
            Overwritten = 0x08U
        };

        uint8_t numReceived = 0;
        uint8_t frameOrder[6] = {0, 0, 0, 0, 0, 0};
        uint16_t values[6][3];
        uint8_t errors[6];
        uint64_t timestamp = 0;
        void readDf (apb_stamp::StampDataframe &df);
    };

    /** Singleton access method
     *
     * Use this method to get the Measurement instance
     * @return the instance
     */
    static Measurement &getInstance ();

    /** Sets the data storage
     *
     * Asserts the ADC_START line going to the ADCs enabling their continuous
     * mode. When they trigger the VHDL interrupts, the VHDL interrupts
     * will trigger the MSS interrupts. Any start will reset the timestamps
     * in the storage! USE THE CONTROLLER METHOD TO ACTUALLY TRIGGER THIS!
     *
     * @param start true for Starting, false for Stopping
     */
    void setDataAcquisition (bool start);

    /** Worker function
     *
     * Call this function in main loop.
     */
    void worker ();

    /** Timestamp getter
     *
     * @return the current timestamp in 250us increments after starting data
     * acquisition
     */
    uint64_t getTimestamp () const;

private:
    apb_stamp::Stamp stamps[6]; /**< contains all Stamp instances */

    volatile uint64_t timestamp; /**< Timestamp in units of 250us (=1/4kHz) */

    volatile uint8_t stampDataAvailable; /**< bitmask indicating, which
    stamps indicated that their data is available. */

    volatile apb_stamp::StampDataframe *dfs[6];

    Datapackage dp;

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

    /** Reset and start the timestamp generator
     *
     * After calling this action, the current timestamp will also be reset to 0.
     */
    void startTimestampGenerator ();

    /** Stop the timestamp generator
     *
     * After calling this action, the timestamp will retain its current value.
     * However there is no mechanism to continue the counting, thus a full
     * restart of the timestamp generator including the reset of
     * the current value must be made.
     */
    void stopTimestampGenerator ();

    // define friend functions
    friend void Timer2_IRQHandler ();
    friend void F2M_INT_STAMP1_HANDLER ();
    friend void F2M_INT_STAMP2_HANDLER ();
    friend void F2M_INT_STAMP3_HANDLER ();
    friend void F2M_INT_STAMP4_HANDLER ();
    friend void F2M_INT_STAMP5_HANDLER ();
    friend void F2M_INT_STAMP6_HANDLER ();
};



#endif
