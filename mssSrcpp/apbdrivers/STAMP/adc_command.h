#ifndef APBDRIVERS_STAMP_ADC_COMMAND_H_
#define APBDRIVERS_STAMP_ADC_COMMAND_H_

#include <string>
#include "apb_stamp.h"


namespace apb_stamp {

    typedef bool (*const ErrorHandler)(uint8_t attempt, std::string msg);



    /** AdcCommand class
     *
     * The virtual base class for commands to the analog to digital converters
     * via a VHDL Stamp unit. Inherit this class to implement the individual
     * ADC commands.
     */
    class AdcCommand {
    public:
        /** Destructor
         *
         * Destructor for AdcCommand objects must be virtual to prevent
         * slicing of inherited objects upon destruction.
         */
        virtual ~AdcCommand ();

        /** Invoke functor
         *
         * A wrapper function for the execute method including a basic error
         * handling routine, which calls the ErrorHandler object on errors.
         * Another attempt will be made, when the ErrorHandler object returns
         * true. Otherwise this function will return false on the first failed
         * attempt.
         * @param stamp The Stamp object to be accessed
         * @param adcs The ADCs to be accessed
         * @returns true, if operation was successful. False otherwise.
         */
        bool operator() (Stamp &stamp,
                uint8_t adcs = (Stamp::SGR1 | Stamp::SGR2 | Stamp::RTD));

    protected:
        ErrorHandler eh = nullptr; /**< A function returning true, if another
        attempt shall be made */

        /** Constructor
         *
         * @param eh See AdcCommand::eh
         */
        AdcCommand (ErrorHandler eh = nullptr);

        /** Virtual base execute method
         *
         * Override this method to interact with the ADC.
         * @param stamp The Stamp object connecting the ADCs
         * @param adcs The ADCs to be accessed
         * @returns empty string, if operation was successful according to the
         * specialized method. Otherwise the string contains the error message.
         */
        virtual std::string execute (Stamp &stamp, uint8_t adcs) = 0;
    };



    /** AdcCommandReset class
     *
     * Resets all connected ADC registers for a clean configuration.
     * This class does not make use of error handlers, as there
     * is no error detection mechanism included.
     */
    struct AdcCommandReset: public AdcCommand {
        /** Constructor
         */
        AdcCommandReset ();

    private:
        /** Reset method
         *
         * Resets the ADCs registers by issuing the SPI command
         * and waiting for 1ms to let it settle.
         * @param stamp See AdcCommand::execute
         * @param adcs See AdcCommand::execute
         * @returns See AdcCommand::execute
         */
        std::string execute (Stamp &stamp, uint8_t adcs);
    };



    /** AdcCommandReadReg class
     *
     * Reads an even amount of bytes from the ADCs register.
     * This class does not make use of error handlers, as there
     * is no error detection mechanism included.
     */
    struct AdcCommandReadReg: public AdcCommand {
        uint8_t offset; /**< The start register offset */

        uint8_t num; /**< The number of registers to be read */

        /** Accessing operator
         *
         * After calling the invoke function, this function can be
         * used to access the results with an index range 0 to num-1.
         * @param idx Index in range 0 to num-1
         * @return the value of the selected register. Always returns 0,
         * if index out of range or no prior invocation occurred.
         */
        uint8_t operator[] (uint8_t idx);

        /** Constructor
         *
         * @param offset The offset of the first register as described in
         * the ADS 114x datasheet
         * @param num The number of registers to be read. Must be an even
         * number.
         */
        AdcCommandReadReg (uint8_t offset, uint8_t num);

        /** Destructor
         */
        ~AdcCommandReadReg ();

    private:
        uint16_t *readValues = nullptr; /**< The array containing the
        specified number of bytes after the read operation */

        /** Read method
         *
         * Read from the selected ADC and deletes prior value.
         * @param stamp See AdcCommand::execute
         * @param adcs See AdcCommand::execute. Only one ADC is allowed.
         * @returns Always returns empty string
         */
        std::string execute (Stamp &stamp, uint8_t adcs);
    };



    /** Class for ADC configuration
     *
     * This class contains common configurations for the ADCs. Constant members
     * should only be modified using the ADS114x datasheet. Generally,
     * this class can and should be used in production without modifying the
     * member parameters. The array members generally refer to the ADCs in the
     * order SGR1, SGR2, RTD.
     *
     * An exception are the calibration registers, which should be set
     * programmatically to a predetermined value.
     *
     * GPIO configurations are not implemented.
     */
    struct AdcCommandConfigure: public AdcCommand {
        enum PGA: uint8_t {PGA1=0x00U, PGA2=0x10U, PGA4=0x20U, PGA8=0x30U,
            PGA16=0x40U, PGA32=0x50U, PGA64=0x60U, PGA128=0x70U};

        enum SPS: uint8_t {SPS5=0, SPS10=1, SPS20=2, SPS40=3, SPS80=4,
            SPS160=5, SPS320=6, SPS640=7, SPS1000=8, SPS2000=9};

        const uint8_t mux0[3] = {0x01U, 0x01U, 0x0AU}; /**<
        No burn-out detection,
        SGR ADCs input AIN0 and AIN1,
        RTD ADC inputs AIN1 and AIN2 */

        const uint8_t vbias[3] = {0x00U, 0x00U, 0x00U}; /**< No Bias voltage */

        const uint8_t mux1[3] = {0xB0U, 0xB0U, 0x20U}; /**<
        Internal oscillator (read only),
        Internal reference voltage always on,
        SGR ADCs internal reference is measurement reference,
        RTD ADC REFP0 and REFP1 are measurement reference,
        No internal system monitoring mode */

        PGA sys0Pga[3] = {PGA64, PGA64, PGA16}; /**< The programmable gain
        amplifier (PGA) values defines the voltage measurement resolution */

        SPS sys0Sps[3] = {SPS10, SPS10, SPS5}; /**< The samples per second
        (SPS), also known as samplerate, defines the temporal resolution */

        uint32_t ofc[3] = {0, 0, 0}; /**< Offset calibration coefficient. Only
        the least significant 24 bits are used. */

        uint32_t fsc[3] = {0x400000U, 0x400000U, 0x400000U}; /**< Full scale
        calibration coefficient. Only the least significant 24 bits are used. */

        const uint8_t idac0[3] = {0x00U, 0x00U, 0x06U}; /**<
        Revision identification read only,
        DOUT/~DRDY pin mode normal,
        SGR ADCs IDACs off,
        RTD ADC IDAC set to 1mA */

        const uint8_t idac1[3] = {0xFFU, 0xFFU, 0x03U}; /**<
        SGR ADCs IDACs disconnected,
        RTD ADC IDAC output set to AIN0 and AIN3 */

        /** Constructor
         *
         * @param eh See AdcCommand::eh
         */
        AdcCommandConfigure (ErrorHandler eh = nullptr);

    private:
        /** The configuration execution function
         *
         * Configures all except the GPIO registers of the selected ADCs. Note,
         * that not all three ADCs are written to concurrently (even though
         * technically possible), but sequentially to allow for pinpointing
         * failed writing attempts.
         * The ErrorHandler will be utilized as described by AdcCommand::eh.
         * @param stamp See AdcCommand::execute
         * @param adcs See AdcCommand::execute
         * @returns See AdcCommand::execute
         */
        std::string execute (Stamp &stamp, uint8_t adcs);
    };



    /** Class for performing ADC calibrations
     *
     * This class enables the use of the ADS114x calibration
     * mechanisms. Make sure to follow the instructions for system offset and
     * system gain calibrations.
     */
    struct AdcCommandCalibrate: public AdcCommand {
    public:
        /** Calibration Type enumeration
         */
        enum CalibrationType: uint16_t {
            sysocal = 0xFF60U, /**< System offset calibration. The inputs must
            be externally shorted to a voltage within the input common-mode
            range. The inputs must be near the mid-supply voltage. */
            selfocal = 0xFF62U, /**< Self offset calibration. The device
            internally shorts the inputs to mid-supply and performs the
            calibration. */
            sysgcal = 0xFF61U /**< System gain calibration. The input must be
            set to full-scale. */
        };

        uint32_t calValue[3] = {0, 0, 0}; /**< The resulting calibration values
        for all three ADCs. Only the least significant 24 bits are used. */

        /** Constructor
         *
         * @param type The type of calibration to be performed.
         */
        AdcCommandCalibrate (CalibrationType type);

    private:
        const CalibrationType type;

        /** Performs the calibration
         *
         * The result will be stored in calValue and the function returns
         * once the calibration succeeded or a timeout occurred.
         * @param stamp See AdcCommand::execute
         * @param adcs See AdcCommand::execute
         * @returns Always returns empty string
         */
        std::string execute (Stamp &stamp, uint8_t adcs);
    };

}

#endif
