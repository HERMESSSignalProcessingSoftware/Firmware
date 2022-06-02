/**
 * @mainpage HERMESS APB STAMP driver
 *
 * @section intro_sec Introduction
 * This driver implements the interface for communicating with the HERMESS
 * custom FPGA component "STAMP". The STAMP connects to in total three
 * TI ADS114X, whereas two measure the strain gauge rosettes and one measures
 * the temperature of a single STAMP.
 *
 * @section setup_sec Setup
 * 1. Initialize GPIO connected to ADC start with high
 * 2. Create a callback function which will be called by the interrupt
 *      service routine, when new measurements are available. Make
 *      sure to keep it short. Name the function
 *      "void FabricIrq<N>_IRQHandler ();"
 *      and replace <N> with the appropriate interrupt pin number.
 * 3. Create an empty apb_stamp_t
 * 4. Call APB_STAMP_init
 * 5. Call APB_STAMP_writeADC to configure the ADCs
 * 6. Call APB_STAMP_writeADC and APB_STAMP_readAdc to verify configuration
 * 7. Enable the the interrupts
 * 7. Pull the ADCs start signal low
 * 8. Enable Continuous mode via configuration command
 * 10. Enable the continuous signal
 *
 * @section cont_sec Continuous mode
 * STAMP components. If it is enabled, the STAMP
 * module will automatically read all incoming transmissions from the ADCs
 * and stores them in internal registers. Once both DMS sent their values,
 * the interrupt is triggered and the MCU can process the data.
 */


#ifndef APB_STAMP_H
#define APB_STAMP_H

#include <stdint.h>
#include "../apbUtilities.h"



namespace apb_stamp {


    struct StampConfig {
        bool reset; /**< resets VHDL internal status. (not ADCs) */
        bool continuous; /**< Enable continuous mode. */
        uint8_t pollingTimeout; /**< timeout waiting for SPI transfers
        after time specified here. Unit is 0.01s. */
        uint8_t id; /**< Internal STAMP ID appended to status status register */
        StampConfig (uint32_t bitfield = 0);
        uint32_t bitfield () const;
        bool operator== (const StampConfig &other) const;
    };



    struct StampStatus {
        bool newVal[3]; // SGR1, SGR2, RTD
        bool overwrittenVal[3];
        int8_t asyncTime;
        uint32_t id;
        StampStatus (uint16_t bitfield);
        uint16_t bitfield () const;
    };



    /** VHDL Stamp components dataframe
     *
     * A dataframe contains the last measured values of all 3 ADCs and
     * the contents of the status register.
     */
    struct StampDataframe {
        const uint16_t dataSgr1;
        const uint16_t dataSgr2;
        const uint16_t dataRtd;
        const StampStatus status;
        const uint64_t timestamp;
        StampDataframe (uint32_t Sgr12, uint32_t RtdStatus);
    };



    /** APB Stamp class
     *
     * Instances of this class can access the VHDL Stamp components.
     * For access to connected ADCs use the AdcCommand classes.
     */
    class Stamp {
    public:
        /** Adc enumeration
         *
         * Use this enumeration with bitwise "OR" operation to select
         * multiple ADCs when writing to them using Stamp::writeAdc.
         */
        enum Adc: uint8_t {SGR1 = 0x10U, SGR2 = 0x20U, RTD = 0x40U};

        /** Address modifier
         *
         * When interacting with the VHDL Stamp component, certain address
         * modifiers may be used. Most of them may be combined using the
         * bitwise "OR" operation.
         */
        enum Mod: uint16_t {
            ModNone = 0x000U, /**< As the name suggests, no address
            modifier will be applied to the operation. */
            ModAtomic = 0x800U, /**< An atomic operation does not permit
            STAMP component to read another value in continuous mode from the
            ADCs, even when the ADC signals that there are new data available.
            Furthermore when asserting this modifier, it will keep the chip
            select in its state even after finishing the write. Also note, that
            it is important to not assert this modifier on the last call of
            functions accessing this STAMP, as it will practically freeze the
            component until it finishes the next transaction. */
            ModStatusReset = 0x400U, /**< This resets the internal status
            register of the STAMP component. These are only the informations
            as stored in StampStatus. It does not reset the component. This
            modifier is useful when reading the status register, as it will
            only be reset after it has been read. */
            ModDataReady = 0x080U /**< Only usable with Stamp::writeAdc. This
            will prevent the microcontroller from continuing before the ADCs
            confirmed finishing their activity by pulling the DRDY signal low.
            Has a timeout of one second, after which the process will be
            aborted. */
        };

        const uint32_t baseAddr; /**< The base address of the VHDL
        stamp component on the APB */

        const IRQn interruptPin; /**< The Fabric to Microcontroller pin
        number signaling interrupts by the VHDL Stamp component */

        /** Constructor
         *
         * No further configuration or initialization is being done by the
         * constructor. For configuration run writeConfig. The MSS interrupt is
         * disabled by default.
         * @param baseAddr See Stamp::baseAddr.
         * @param interruptPin See Stamp::interruptPin.
         */
        Stamp (uint32_t baseAddr, IRQn interruptPin);

        /** Configure VHDL Stamp component
         *
         * Writes to the configuration register of the STAMP component. This
         * does not configure the ADCs. After writing it performs an automatic
         * check, that the written values are indeed set.
         * @param conf Instance of StampConfig to be used.
         * @returns true, if successful. False otherwise.
         */
        bool writeConfig (const StampConfig &conf) const;

        /** Reads VHDL Stamp component configuration
         *
         * @returns A StampConfig instance
         */
        StampConfig readConfig () const;

        /** Read a Dataframe
         *
         * @returns The most current dataframe
         */
        StampDataframe readDataframe () const;

        /** Write commands to the ADCs
         *
         * @param adcs One or more of Stamp::Adc. Can be combined using bitwise
         * or. If writing to one or more ADCs at a time you should make sure the
         * triggered commands do not write to their respective outputs. Failing
         * to do so could potentially damage the ADCs.
         * @param val the value to pass via SPI to the controller. Note, that
         * the SPI module always transfers 16 bit regardless of the operation.
         * If sending a command with only 8 bits it is recommended to fill the
         * higher 8 bits with an ADC NOP command.
         * @param mod One or more of ModNone, ModAtomic, ModStatusReset and
         * ModDataReady. Can be combined using bitwise or.
         */
        void writeAdc (uint8_t adcs, uint16_t val, uint16_t mod) const;

        /** Read latest SPI output from ADC
         *
         * Returns the last SPI output made by an ADC. This function does not
         * initiate a new communication with the ADC, it merely outputs the
         * previously recorded data.
         * @param mod One or more of ModNone, ModAtomic and
         * ModStatusReset. Can be combined using bitwise or.
         * @return The ADC out of the last two sent commands. Most significant
         * byte is the first transaction, least significant byte is second
         * transaction.
         */
        uint16_t readAdc (uint16_t mod) const;

        /** Enable the Stamp F2M interrupt
         *
         * Note, that the VHDL Stamp components interrupts are not part
         * of the usual productive procedure.
         */
        void enableInterrupt () const;

        /** Disable the Stamp F2M interrupt
         */
        void disableInterrupt () const;

        /** Clear interrupt flag
         *
         * Clears the interrupt flag and should be called at the beginning
         * of an ISR.
         */
        void clearInterrupt () const;
    };

}


#endif
