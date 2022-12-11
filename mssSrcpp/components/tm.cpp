#include "tm.h"
#include "controller.h"
#include <cstring>
#include "../sf2drivers/drivers/mss_uart/mss_uart.h"
#define CRCPP_USE_CPP11
#include "../tools/crc.h"
#include "../apbdrivers/TELEMETRY/abp_telemetry.h"
#include "../model/telemetry.h"

Tm& Tm::getInstance() {
    static Tm instance;
    return instance;
}

void Tm::worker_irq(void) {
    // Gather information here
    if (interfaceFree) {
        uint32_t row = 0;
        if (!dataQueue.empty()) {
            HERMESS::TelemetryData &data = dataQueue.front();
            dataQueue.pop();
            dataQueueSize--;
            while (data.hasNext()) {
                uint32_t *frame = data.getNextFrame();
                for (int i = 0; i < 6; i++) {
                    APBTelemetyLoadTXMemory(frame[i], row + i);
                }
                row += 6;
            }
            APBTelemetryTransmissionStart();
            interfaceFree = false;
        }
    }
}

Tm& Tm::operator<<(std::string msg) {
    uint32_t size = msg.size();

    // do not push message, if not allowed by configuration
    if (Controller::getInstance().configuration.enableTm
            && (msgQueueSize + size) <= TM_MAX_BUFFER_SIZE) {
        msgQueue.push(msg);
        msgQueueSize += size;
    }
    return *this;
}

void Tm::enqueue(HERMESS::TelemetryData data) {
    if (dataQueueSize < 5) {
        this->dataQueue.push(data);
        dataQueueSize++;
    }
}

void Tm::addDataToQueue(const apb_stamp::StampDataframe &df, uint32_t stampid) {
//    if (allowNewData) {
//        if (stampid == 0) {
//            txBuffer = HERMESS::TelemetryData();
//            txBuffer.addData((uint32_t)df.timestamp);
//        } else if (stampid == 5) {
//            allowNewData = false;
//        }
//        txBuffer.addData((uint32_t)(df.dataRtd | df.dataSgr1 << 16));
//        txBuffer.addData((uint32_t)(df.dataSgr2 | df.status.bitfield() << 16));
//        if (stampid == 5) {
//            dataQueue.push(txBuffer);
//        }
//    }
}

Tm::Tm() {
    // initialize the telemetry fabric IP
    interfaceFree = true;
    //GAP_30kBits;
    APBInitTMDriver(BAUD_38400, GAP_3MS,
    CONFIG_INTERRPUT_ENA | CONFIG_GLOBAL_START);
    // set always common transmission bytes
//    txBuffer[62] = 0x17;
//    txBuffer[63] = 0xF0;
}

void Tm::enableInterrupt(void) {
    NVIC_EnableIRQ(F2M_INT_PIN(INT_TELEMETRY));
    NVIC_ClearPendingIRQ(F2M_INT_PIN(INT_TELEMETRY));
}

void Tm::clearInterrupt(uint32_t interrupt) {
    uint32_t statusReg = APBTelemetryStatusRegister_get();
    statusReg &= ~(interrupt);
    APBTelemetryStatusRegister_set(statusReg);
    if (interrupt & TELEMETRY_STATUS_INTERRUPT_TX) {
        interfaceFree = true;
    }
}

void Tm::telemetryTransmissionStart(void) {
    APBTelemetryTransmissionStart();
}

void Tm::telemetryTransmissionStop(void) {
    APBTelemetryTransmissionStop();
}
