/*
 * TelemetryData.cpp
 *
 *  Created on: 19.10.2022
 *      Author: RG
 */

#include "telemetry.h"
#include "../tools/crc.h"

namespace HERMESS {

TelemetryData::TelemetryData() {
   watermark = 0;
   frameNumber = 0;
   tm_index = 0;
   for (int i = 0; i < 15; i++) {
       data[i] = 0;
   }

  for (int i = 0; i < 6; i++) {
      data[i] = 0;
  }

}

TelemetryData::~TelemetryData() {
}

void TelemetryData::addData(uint32_t data) {
    this->data[tm_index++] = data;
}

uint32_t* TelemetryData::getNextFrame(void) {
   if (frameNumber < 4) {
       frame[0] = 0x0; // Meta data
       for (int i = 0; i < 4; i++) {
           if (watermark == 12 && i == 3) {
               frame[4] = 0x0; //- No data here
               break;
           } else {
               frame[1 + i] = data[watermark + i];
           }
       }
       watermark += 4;
       uint16_t crc = CRC::Calculate(frame, 5, CRC::CRC_16_CCITTFALSE());
       frame[5] = crc; // End meta
       frameNumber++;
       return frame;
   }
   return NULL;
}

bool TelemetryData::hasNext(void) {
    return frameNumber < 4;
}

uint32_t TelemetryData::getIndex(void) {
    return tm_index;
}

uint32_t TelemetryData::getWatermark(void) {
    return watermark;
}
} /* namespace HERMESS */
