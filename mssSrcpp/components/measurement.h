#ifndef COMPONENTS_MEASUREMENT_H_
#define COMPONENTS_MEASUREMENT_H_

#include "../apbdrivers/STAMP/apb_stamp.h"
#include "../apbdrivers/STAMP/adc_command.h"
#include "../sb_hw_platform.h"


/**
 * Measurement Singleton Class
 *
 * Uses Timer2 as 32 bit timer for Timestamp generation
 */
class Measurement {
public:
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
     * in the storage!
     * @param start true for Starting, false for Stopping
     */
    void setDataStorage (bool start);

    // !!! calibrate and get values, push calibration values

private:
    apb_stamp::Stamp stamps[6]; /**< contains all Stamp instances */

    volatile uint64_t timestamp; /**< Timestamp in units of 250us (=1/4kHz) */

    /** Stamp ISR template
     *
     * This function is called by all six STAMP data_available interrupts.
     * @param stamp The calling STAMP component in range [0, 5]
     */
    static inline void handleStampInterrupt (uint8_t stamp);

    /** Constructor
     *
     * Initializes all components so that they are ready to operate.
     */
    Measurement ();

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

    /** Timer 2 ISR
     *
     * Allow the Timer2 ISR to access private members
     */
    friend void Timer2_IRQHandler ();

    friend void F2M_INT_STAMP1_HANDLER ();
    friend void F2M_INT_STAMP2_HANDLER ();
    friend void F2M_INT_STAMP3_HANDLER ();
    friend void F2M_INT_STAMP4_HANDLER ();
    friend void F2M_INT_STAMP5_HANDLER ();
    friend void F2M_INT_STAMP6_HANDLER ();
};



#endif
