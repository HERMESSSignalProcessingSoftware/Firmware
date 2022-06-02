#include <cstring>
#include "dapi.h"
#include "../sf2drivers/drivers/mss_uart/mss_uart.h"



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
}


Dapi &Dapi::transmitRaw (const uint8_t * const ptr, const uint32_t size) {
    msgQueue.push(Message(ptr, size));
    return *this;
}


Dapi &Dapi::operator<< (const std::string msg) {
    return (*this) << msg.c_str();
}


Dapi &Dapi::operator<< (const char * const msg) {
    // get message length and add null terminator
    uint32_t strLen = std::strlen(msg) + 1;

    // append to transmission queue
    msgQueue.push(Message((uint8_t*) msg, strLen));

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


Dapi::Dapi (): transferInProgress{false} {
    MSS_UART_init(&g_mss_uart0, MSS_UART_115200_BAUD,
            MSS_UART_DATA_8_BITS
            | MSS_UART_EVEN_PARITY
            | MSS_UART_ONE_STOP_BIT);
    MSS_UART_set_tx_endian(&g_mss_uart0, MSS_UART_LITTLEEND);
    MSS_UART_set_rx_endian(&g_mss_uart0, MSS_UART_LITTLEEND);
}
