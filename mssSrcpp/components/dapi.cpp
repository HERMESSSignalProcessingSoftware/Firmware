#include <cstring>
#include "dapi.h"
#include "controller.h"
#include "../tools/msghandler.h"
#include "../tools/tools.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sb_hw_platform.h"



Dapi &Dapi::getInstance () {
    static Dapi instance;
    return instance;
}


void Dapi::worker () {
    // check if no outgoing transmission is in progress
    if (MSS_UART_tx_complete(&g_mss_uart0)) {

        // delete previous item
        if (transferInProgress) {
            msgQueue.pop();
            transferInProgress = false;
        }

        // transmit the next item
        if (!msgQueue.empty()) {
            Message &msg = msgQueue.front();
            MSS_UART_irq_tx(&g_mss_uart0, msg.ptr, msg.size);
            transferInProgress = true;
        }
    }

    // handle incoming transmission
    if (rxBufferIdx >= 3) {
        // check from back to front the end of transmission bytes
        bool transmissionReceived = (rxBuffer[rxBufferIdx-1] == 0xF0);
        // find the last index of the content bytes
        uint8_t endIdx = rxBufferIdx - 2;
        while (rxBuffer[endIdx] == 0x00 && endIdx >= 0)
            endIdx--;
        transmissionReceived &= (endIdx >= 1);
        // check content demarcation byte
        transmissionReceived &= (rxBuffer[endIdx] == 0x17);

        if (transmissionReceived) {
            // set the endIdx to the actual last byte of the content bytes
            endIdx--;
            // a seemingly valid command was received
            switch (rxBuffer[0]) {
            case 0x00:
                // echo command
                transmitRaw(rxBuffer, endIdx+1);
                (*this) << "\x0F\x17\xF0";
                break;
            case 0x03:
                // Start live data acquisition
                Controller::getInstance().setLiveDataAcquisition(true);
                break;
            case 0x04:
                // Stop live data acquisition
                Controller::getInstance().setLiveDataAcquisition(false);
                break;
            default:
                MsgHandler::getInstance().warning("Unknown command byte");
            }
            rxBufferIdx = 0;
        }

        else if (rxBufferIdx >= 64) {
            // reset the rxBuffer, as something terrible has happened
            rxBufferIdx = 0;
            MsgHandler::getInstance().warning("RX Buffer overflow");
        }
    }
}


Dapi &Dapi::transmitRaw (const uint8_t * const ptr, const uint32_t size) {
    msgQueue.push(Message(ptr, size));
    return *this;
}


Dapi &Dapi::operator<< (const std::string msg) {
    return this->transmitRaw((const uint8_t*)msg.c_str(), msg.size());
}


Dapi &Dapi::operator<< (const char * const msg) {
    // get message length
    uint32_t strLen = std::strlen(msg);

    // append to transmission queue
    msgQueue.push(Message((uint8_t*) msg, strLen));

    return *this;
}


Dapi &Dapi::sendLiveData (Measurement::Datapackage &dp) {
    const uint8_t transmissionSize = 13+(dp.numReceived * 8);
    uint8_t toTransmit[transmissionSize];
    toTransmit[0] = 0x03U;
    toTransmit[1] = dp.numReceived;
    toByteArray(&toTransmit[2], dp.timestamp);
    for (uint8_t i = 0; i < dp.numReceived; i++) {
        const uint8_t offset = i * 8;
        toTransmit[10 + offset] = dp.frameOrder[i];
        toTransmit[11 + offset] = dp.errors[i];
        toByteArray(&toTransmit[12 + offset], dp.values[i][0]);
        toByteArray(&toTransmit[14 + offset], dp.values[i][1]);
        toByteArray(&toTransmit[16 + offset], dp.values[i][2]);
    }
    toTransmit[transmissionSize - 3] = '\x0F';
    toTransmit[transmissionSize - 2] = '\x17';
    toTransmit[transmissionSize - 1] = '\xF0';
    return this->transmitRaw(toTransmit, transmissionSize);
}


Dapi::Message::Message (const uint8_t * const ptr, const uint32_t size):
        size{size}, ptr{nullptr} {
    // get appropriately sized array and make string copy
    uint8_t *copy = new uint8_t[size];
    std::memcpy(copy, ptr, size);
    this->ptr = copy;
}


Dapi::Message::Message (const Message &other):
        size{other.size}, ptr{nullptr} {
    // get appropriately sized array and make string copy
    uint8_t *copy = new uint8_t[size];
    std::memcpy(copy, other.ptr, size);
    this->ptr = copy;
}


Dapi::Message::~Message () {
    // delete array
    delete[] ptr;
}


void Dapi::rxHandler (mss_uart_instance_t *this_uart) {
    MSS_GPIO_set_output(OUT_DAPI_CTS, 1);
    Dapi &dapi = Dapi::getInstance();
    dapi.rxBufferIdx += MSS_UART_get_rx(this_uart,
            &dapi.rxBuffer[dapi.rxBufferIdx],
            64-dapi.rxBufferIdx);
    MSS_GPIO_set_output(OUT_DAPI_CTS, 0);
}


Dapi::Dapi () {
    MSS_GPIO_config(OUT_DAPI_CTS, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(OUT_DAPI_CTS, 0);

    MSS_UART_init(&g_mss_uart0, MSS_UART_115200_BAUD,
            MSS_UART_DATA_8_BITS
            | MSS_UART_EVEN_PARITY
            | MSS_UART_ONE_STOP_BIT);
    MSS_UART_set_tx_endian(&g_mss_uart0, MSS_UART_LITTLEEND);
    MSS_UART_set_rx_endian(&g_mss_uart0, MSS_UART_LITTLEEND);
    MSS_UART_set_rx_handler(&g_mss_uart0, &Dapi::rxHandler,
            MSS_UART_FIFO_EIGHT_BYTES);
}
