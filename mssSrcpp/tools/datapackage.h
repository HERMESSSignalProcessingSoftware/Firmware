#ifndef TOOLS_DATAPACKAGE_H_
#define TOOLS_DATAPACKAGE_H_

#include <stdint.h>
#include "../apbdrivers/STAMP/apb_stamp.h"



/** Datapackage struct
 *
 * This container class contains up to six STAMP dataframes with each
 * 3 ADC readings. This package does not necessarily hold values from
 * six different STAMPs, but may contain values from different points in time
 * from the same STAMP. The entries may also not be in any numerical STAMP
 * order. The order of the values can only be derived from the frameOrder
 * member referencing to the numerical STAMP origin of the first-level-index
 * values member.
 *
 * This struct is designed to read dataframes after initialization and
 * aggregate some information for more compact storage and transmission
 * of members of this struct.
 */
struct Datapackage {
    /** The error codes here form the basis for the errors bitmask of the
     * Datapackage.
     */
    enum ErrorCodes: uint8_t {
        AdcLagging = 0x01U,  /**< Either one of the SGR ADCs exceeded an
        interdependent threshold time for finishing their sampling */
        StampLagging = 0x02U,/**< The entire STAMPs timestamp exceeded
        a threshold time compared to the first STAMPs timestamp */
        NoNew = 0x04U,       /**< The STAMP was read out despite it having
        not collected any new samples */
        Overwritten = 0x08U  /**< The STAMP was not read out fast enough
        resulting in loss of at least one dataframe */
    };

    uint8_t numReceived = 0; /**< The number of stored dataframes in this
    datapackage */

    uint8_t frameOrder[6] = {0, 0, 0, 0, 0, 0}; /**< The value of each cell
    of this array is the STAMP id of the measurements stored in the values
    array with the same index */

    uint16_t values[6][3]; /**< The measurement values from the STAMPs defined
    in frameOrder. The secondary level is always SGR1, SGR2, RTD. */

    uint8_t errors[6]; /**< The errors bitmask defined by ErrorCodes for each
    read dataframe */

    uint64_t timestamp = 0; /**< The system timestamp of the first read
    dataframe */

    /** Convert to byte array
     *
     * Convert this object to a byte array for storage or transmission into the
     * format described by the DAPI spec for sending live data.
     * @param arr A byte array buffer to store the results to. The array must
     * be at least 9+(8*numReceived) long.
     */
    void toBytes (uint8_t arr[]) const;

    /** Read dataframe
     *
     * Extends this datapackage by the contents of the provided dataframe. Must
     * not be called, when numReceived is >= 6.
     * @param df The dataframe to read
     */
    void readDf (const apb_stamp::StampDataframe &df);
};

#endif /* TOOLS_DATAPACKAGE_H_ */
