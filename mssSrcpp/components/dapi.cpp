#include <cstring>
#include "dapi.h"
#include "../tools/msghandler.h"
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
        transmissionReceived &= (endIdx > 1);
        // check content demarcation byte
        transmissionReceived &= (rxBuffer[endIdx] == 0x17);

        if (transmissionReceived) {
            // set the endIdx to the actual last byte of the content bytes
            endIdx--;
            // a seemingly valid command was received
            switch (rxBuffer[0]) {
            case 0x00:
                handleCommandEcho(endIdx);
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
    return (*this) << msg.c_str();
}


Dapi &Dapi::operator<< (const char * const msg) {
    // get message length
    uint32_t strLen = std::strlen(msg);

    // append to transmission queue
    msgQueue.push(Message((uint8_t*) msg, strLen));

    return *this;
}


Dapi &Dapi::operator<< (char msg) {
    // append to transmission queue
    msgQueue.push(Message((uint8_t*) &msg, 1));
    return *this;
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


void Dapi::handleCommandEcho (uint8_t contentSize) {
    transmitRaw(rxBuffer, contentSize+1);
    operator <<("\x0F\x17\xF0");
}
