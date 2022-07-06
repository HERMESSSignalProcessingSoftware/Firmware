#ifndef COMPONENTS_CONFIGURATION_H_
#define COMPONENTS_CONFIGURATION_H_

#include <stdint.h>
#include "../apbdrivers/STAMP/adc_command.h"


/** Configuration data class
 *
 * A storage class for SPU configurations stored in the EEPROM. This
 * class requires initialized EEPROM NVM.
 */
struct Configuration {
public:
    char confName[17] = "SPU-DEFAULT\0\0\0\0\0"; /**< The SPU configuration
    name designator as ASCII encoded string. May not be NULL-terminated. */

    bool sgrSelfOffsetCal = false; /**< Run a self offset calibration
    on the SGR ADCs during initialization */

    bool sgrSystemOffsetCal = false; /**< Run a system offset calibration
    on the SGR ADCs during initialization */

    bool rtdSelfOffsetCal = false; /**< Run a self offset calibration
        on the RTD ADCs during initialization */

    bool rtdSystemOffsetCal = false; /**< Run a system offset calibration
        on the RTD ADCs during initialization */

    bool enableStorage = true; /**< Enable storage of measurements to flash
    on SODS signal asserted */

    bool enableClear = false; /**< Enable automatic clearing of measurements
    on SOE signal asserted */

    bool enableTm = true; /**< Enable the telemetry output stream */

    apb_stamp::AdcCommandConfigure::PGA sgrPga =
            apb_stamp::AdcCommandConfigure::PGA::PGA64; /**< The ADC PGA
    setting for the SGRs */

    apb_stamp::AdcCommandConfigure::SPS sgrSps =
            apb_stamp::AdcCommandConfigure::SPS::SPS1000; /**< The
    samplerate of the SGR ADCs */

    apb_stamp::AdcCommandConfigure::PGA rtdPga =
            apb_stamp::AdcCommandConfigure::PGA::PGA16; /**< The ADC PGA
    setting for the SGRs */

    apb_stamp::AdcCommandConfigure::SPS rtdSps =
            apb_stamp::AdcCommandConfigure::SPS::SPS10; /**< The
    samplerate of the RTD ADCs */

    uint64_t minTimestamp = 0; /**< Minimum value of the 250us incremented
    timestamp that has to be reached before shutting down the storage of
    measurements. */

    uint64_t maxTimestamp = 0xFFFFFFFFFFFFFFFF; /**< Maximum value of the
    250us incremented timestamp that may be reached before aborting storage
    of packages occurs. */

    /** Constructor from NVM data
     *
     * Reads the contents of the EEPROM and uses them for the EEPROM. If no
     * EEPROM content is set, the default values set in this header
     * are used.
     */
    Configuration ();

    /** Constructor from DAPI content bytes
     *
     * @param dapiFrame The content bytes as received via the DAPI
     */
    Configuration (const uint8_t dapiFrame[35]);

    /** Write to non-volatile memory
     *
     * Store this configuration in the EEPROM of the SPU.
     * This implies, that this configuration will be used after the
     * next SPU restart. Prints messages when successful or on failure to the
     * MsgHandler.
     */
    void writeToNVM () const;

    /** Convert to DAPI frame
     *
     * @param dapiFrame A 35 byte long array, where the
     * converted configuration can be stored
     */
    void toDapiFrame (uint8_t dapiFrame[35]) const;

private:
    /** Fill properties from DAPI frame
     *
     * @param dapiFrame The content bytes as received via the DAPI
     */
    void fromDapiFrame (const uint8_t dapiFrame[35]);
};

#endif /* COMPONENTS_CONFIGURATION_H_ */
