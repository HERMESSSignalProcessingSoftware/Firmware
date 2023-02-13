/*
 * TelemetryData.cpp
 *
 *  Created on: 19.10.2022
 *      Author: RG
 */

#include "telemetry.h"
#include "../tools/crc.h"
#include "../tools/datapackage.h"
#include "../apbdrivers/STAMP/apb_stamp.h"
#include <string>
#include <cstring>

namespace HERMESS {

TelemetryData::TelemetryData() {
    watermark = 0;
    localFrameNumber = 0;
    tm_index = 0;
    metadata = 0;
    type = NOT_SET;
    for (int i = 0; i < 15; i++) {
        data[i] = 0;
    }

    for (int i = 0; i < 6; i++) {
        frame[i] = 0;
    }
}

TelemetryData::~TelemetryData() {
}

void TelemetryData::message(std::string s) {
    if (this->type == NOT_SET || this->type == MESSAGE) {
        this->type = MESSAGE;
        uint8_t *ptr8 = (uint8_t*)&data;
        if (s.length() < 59) {
            strncpy((char*)ptr8, s.c_str(), (uint32_t)s.length());
            this->data[s.length()] = 0; // Make sure its NULL - Terminted
        }
    }
}
void TelemetryData::addDataFromStamp(const apb_stamp::StampDataframe &df,
        uint32_t stampid) {
    if (this->type == DATA || this->type == NOT_SET) {
        this->type = DATA;
        this->metadata |= (1 << stampid);
        if (stampid == 0) {
            this->addData(df.timestamp);
        }
        this->addData(df.dataRtd | (df.dataSgr1 << 16));
        this->addData(df.dataSgr2 | (df.status.bitfield() << 16));
    }
}

void TelemetryData::addData(uint32_t data) {
    if (tm_index < 16)
        this->data[tm_index++] = data;
}

uint32_t* TelemetryData::getNextFrame(void) {
    if (localFrameNumber < 4) {
        frame[0] = (SYNC1WORD | SYNC2WORD |
                (uint8_t)(type) << 8);
        for (int i = 0; i < 4; i++) {
            if (watermark == 12 && i == 3 && type == DATA) {
                frame[4] = 0x0; //- No data here
                break;
            } else {
                frame[1 + i] = data[watermark + i];
            }
        }
        watermark += 4;
        uint16_t crc = CRC::Calculate(frame, 5, CRC::CRC_16_CCITTFALSE());
        frame[5] = crc; // End meta
        localFrameNumber++;
        return frame;
    }
    return NULL;
}

bool TelemetryData::hasNext(void) {
    return localFrameNumber < 4;
}

uint32_t TelemetryData::getIndex(void) {
    return tm_index;
}

uint32_t TelemetryData::getWatermark(void) {
    return watermark;
}
} /* namespace HERMESS */
