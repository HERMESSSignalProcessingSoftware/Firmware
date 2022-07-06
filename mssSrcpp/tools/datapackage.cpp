#include "datapackage.h"
#include "../tools/tools.h"

#define ADC_LAG_THRESHOLD               2
#define TIMESTAMP_LAG_THRESHOLD         1



void Datapackage::toBytes (uint8_t arr[]) const {
    arr[0] = numReceived;
    toByteArray(&arr[1], timestamp);
    for (uint8_t i = 0; i < numReceived; i++) {
        const uint8_t offset = i * 8;
        arr[9 + offset] = frameOrder[i];
        arr[10 + offset] = errors[i];
        toByteArray(&arr[11 + offset], values[i][0]);
        toByteArray(&arr[13 + offset], values[i][1]);
        toByteArray(&arr[15 + offset], values[i][2]);
    }
}



void Datapackage::readDf (const apb_stamp::StampDataframe &df) {
    if (numReceived >= 6)
        return;

    // determine timestamp issues
    if (numReceived == 0)
        timestamp = df.timestamp;
    else
        errors[numReceived] = ((timestamp - df.timestamp)
                > TIMESTAMP_LAG_THRESHOLD ? StampLagging : 0);

    // copy values
    frameOrder[numReceived] = df.status.id;
    values[numReceived][0] = df.dataSgr1;
    values[numReceived][1] = df.dataSgr2;
    values[numReceived][2] = df.dataRtd;

    for (uint8_t i = 0; i < 3; i++) {
        // generate error values
        errors[numReceived] |= (AdcLagging ?
                ((df.status.asyncTime > ADC_LAG_THRESHOLD) && i == 0)
                || ((df.status.asyncTime < -ADC_LAG_THRESHOLD) && i == 1) : 0)
                | (NoNew ? !df.status.newVal[i] : 0)
                | (Overwritten ? df.status.overwrittenVal[i] : 0);
    }
    numReceived++;
}
