/*
 * TelemetryData.h
 *
 *  Created on: 19.10.2022
 *      Author: Robin Grimsmann
 */

#include <stdint.h>

#ifndef MODEL_TELEMETRYDATA_H_
#define MODEL_TELEMETRYDATA_H_

namespace HERMESS {

class TelemetryData {
public:
    TelemetryData();
    virtual ~TelemetryData();

    /**
     *
     * @param data
     */
    void addData(uint32_t data);

    /**
     *
     * @return
     */
    uint32_t* getNextFrame(void);

    /**
     * Used for filling the data struct of this object.
     * @return
     */
    uint32_t getIndex(void);

    /** Iteration methode
     *
     * one object contains 16 32 bit fields,
     * converted to frames there are 4 frames of 4 byte of
     * data each additional two 32 bits of meta data required by the  RXSM
     *
     * @return true if number of frames is below 4
     */
    bool hasNext(void);
    /**
     *
     * @return the watermark
     */
    uint32_t GetWatermark(void);
private:
    /**
     * timestamp, all stamp data (64 bit per stamp therefore 12 uint32_t fields), statusReg1 and statusReg2
     */
    uint32_t data[15];

    /**
     *
     */
    uint32_t frame[6];

    /**
     * Indicates the process of transmitting all data in steps of 4
     */
    uint32_t watermark;

    /**
     * shows how many items are in the array.
     */
    uint32_t tm_index;

    /**
     *
     */
    uint32_t frameNumber;

};

} /* namespace HERMESS */

#endif /* MODEL_TELEMETRYDATA_H_ */
