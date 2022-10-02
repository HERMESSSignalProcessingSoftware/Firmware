#include "tm.h"
#include "controller.h"
#include <cstring>
#include "../sf2drivers/drivers/mss_uart/mss_uart.h"
#define CRCPP_USE_CPP11
#include "../tools/crc.h"
#include "../apbdrivers/TELEMETRY/abp_telemetry.h"

Tm &Tm::getInstance () {
    static Tm instance;
    return instance;
}



void Tm::worker2Hz () {
    // this check should honestly never fail
    if (MSS_UART_tx_complete(&g_mss_uart1)) {
        // reset the timestamp byte counter and the timestamp itself
        if (sendingTimestampByte >= 4) {
            sendingTimestampByte = 0;
            sendingTimestamp = Controller::getInstance().getTimestamp();
        }

        // fill the txBuffer
        txBuffer[0] = frameId++;
        txBuffer[1] = Controller::getInstance().getStateByte();
        txBuffer[2] = sendingTimestampByte == 0 ? 0x01 : 0;
        txBuffer[3] = (sendingTimestamp>>((3-sendingTimestampByte++)*8))&0xFF;

        // empty initialize text part
        for (uint8_t i = 4; i < 60; i++)
            txBuffer[i] = 0;

        // fill text part
        if (!msgQueue.empty()) {
            std::string &text = msgQueue.front();

            // calculate the number of characters to be transmitted, including
            // one terminating null character
            uint8_t copyLength = text.size() + 1 - sentMsgBytes;
            if (copyLength > 56)
                copyLength = 56;

            // copy the characters to the tx buffer
            std::memcpy(&txBuffer[4], &(text.c_str()[sentMsgBytes]),
                    copyLength);
            sentMsgBytes += copyLength;

            // pop last message, if it has been fully copied already
            if (sentMsgBytes >= (text.size() + 1)) {
                queueSize -= msgQueue.front().size();
                msgQueue.pop();
                sentMsgBytes = 0;
            }
        }

        // calculate and add checksum
        uint16_t crc = CRC::Calculate(txBuffer, 60, CRC::CRC_16_CCITTFALSE());
        txBuffer[60] = (crc >> 8) & 0xFF;
        txBuffer[61] = crc & 0xFF;

        // run the transmission progress
        MSS_UART_irq_tx(&g_mss_uart1, txBuffer, 64);
    }
}



Tm &Tm::operator<< (std::string msg) {
    uint32_t size = msg.size();

    // do not push message, if not allowed by configuration
    if (Controller::getInstance().configuration.enableTm
            && (queueSize + size) <= TM_MAX_BUFFER_SIZE) {
        msgQueue.push(msg);
        queueSize += size;
    }
    return *this;
}



Tm::Tm () {
    // initialize the UART interface
    MSS_UART_init(&g_mss_uart1, MSS_UART_38400_BAUD,
            MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
    MSS_UART_set_tx_endian(&g_mss_uart1, MSS_UART_LITTLEEND);

    // set always common transmission bytes
    txBuffer[62] = 0x17;
    txBuffer[63] = 0xF0;
}

void Tm::clearInterrupt(uint32_t interrupt) {
    uint32_t statusReg = TelemetryStatusRegister_get();
    statusReg &= ~(interrupt);
    TelemetryStatusRegister_set(statusReg);
}

