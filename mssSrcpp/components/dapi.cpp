#include <cstring>
#include "dapi.h"
#include "controller.h"
#include "../tools/msghandler.h"
#include "../tools/configuration.h"
#include "../tools/tools.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sb_hw_platform.h"
#include "memory.h"

Dapi& Dapi::getInstance() {
    static Dapi instance;
    return instance;
}

void Dapi::readAndTransmitMemory(uint32_t highAddress) {
    uint8_t frame[PAGESIZE + 7] = { 0 };
    uint32_t numberOfPages = (highAddress - 0x200) << 1;
    uint32_t pageCounter = 0;
    frame[0] = (numberOfPages >> 16) & 0xFF;
    frame[1] = (numberOfPages >> 8) & 0xFF;
    frame[3] = (numberOfPages & 0xFF);
    frame[PAGESIZE + 5] = 0x17;
    frame[PAGESIZE + 6] = 0xF0;
    MemorySPI readOutDevice = MemorySPI(GPIO_PORT(FLASH_CS1), &g_mss_spi0, 0x200);
    for (uint32_t address = 0x200; address <= highAddress; address++) {
        readOutDevice.readPage(&frame[4], address);
        transmitRaw(frame, PAGESIZE + 7);
        readOutDevice.setCSPin(GPIO_PORT(FLASH_CS2));
        readOutDevice.readPage(&frame[4], address);
        transmitRaw(frame, PAGESIZE + 7);
        readOutDevice.setCSPin(GPIO_PORT(FLASH_CS1));
    }
    delete &readOutDevice;
}

void Dapi::worker() {
    // check if no outgoing transmission is in progress
    if (MSS_UART_tx_complete(&g_mss_uart0)) {

        // delete previous item
        if (transferInProgress) {
            queueSize -= msgQueue.front().size;
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
        bool transmissionReceived = (rxBuffer[rxBufferIdx - 1] == 0xF0);
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
                transmitRaw(rxBuffer, endIdx + 1);
                (*this) << "\x0F\x17\xF0";
                break;
            case 0x02:
                readAndTransmitMemory(
                        Memory::getInstance().metaDataHighestAddress());

                break;
            case 0x03:
                // Start live data acquisition
                Controller::getInstance().setLiveDataAcquisition(true);
                break;
            case 0x04:
                // Stop live data acquisition
                Controller::getInstance().setLiveDataAcquisition(false);
                break;
            case 0x05:
                // Read SPU configuration data
                uint8_t txBuffer[39];
                txBuffer[0] = 0x05U;
                Controller::getInstance().configuration.toDapiFrame(
                        &txBuffer[1]);
                txBuffer[36] = 0x0FU;
                txBuffer[37] = 0x17U;
                txBuffer[38] = 0xF0U;
                transmitRaw(txBuffer, 39);
                break;
            case 0x06:
                // Write SPU configuration data
                if (endIdx != 35)
                    MsgHandler::getInstance().warning("Malformed SPU config "
                            "frame received");
                else
                    Configuration(&rxBuffer[1]).writeToNVM();
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

Dapi& Dapi::transmitRaw(const uint8_t *const ptr, const uint32_t size) {
    if ((queueSize + size) <= DAPI_MAX_BUFFER_SIZE) {
        msgQueue.push(Message(ptr, size));
        queueSize += size;
    }
    return *this;
}

Dapi& Dapi::operator<<(const std::string msg) {
    return this->transmitRaw((const uint8_t*) msg.c_str(), msg.size());
}

Dapi& Dapi::operator<<(const char *const msg) {
    // get message length
    uint32_t strLen = std::strlen(msg);

    // append to transmission queue
    if ((queueSize + strLen) <= DAPI_MAX_BUFFER_SIZE) {
        msgQueue.push(Message((uint8_t*) msg, strLen));
        queueSize += strLen;
    }

    return *this;
}

Dapi& Dapi::sendLiveData(const Datapackage &dp) {
    const uint8_t transmissionSize = 13 + (dp.numReceived * 8);
    uint8_t toTransmit[transmissionSize];
    toTransmit[0] = 0x03U;
    dp.toBytes(&toTransmit[1]);
    toTransmit[transmissionSize - 3] = '\x0F';
    toTransmit[transmissionSize - 2] = '\x17';
    toTransmit[transmissionSize - 1] = '\xF0';
    return this->transmitRaw(toTransmit, transmissionSize);
}

Dapi::Message::Message(const uint8_t *const ptr, const uint32_t size) :
        size { size }, ptr { nullptr } {
    // get appropriately sized array and make string copy
    uint8_t *copy = new uint8_t[size];
    std::memcpy(copy, ptr, size);
    this->ptr = copy;
}

Dapi::Message::Message(const Message &other) :
        size { other.size }, ptr { nullptr } {
    // get appropriately sized array and make string copy
    uint8_t *copy = new uint8_t[size];
    std::memcpy(copy, other.ptr, size);
    this->ptr = copy;
}

Dapi::Message::~Message() {
    // delete array
    delete[] ptr;
}

void Dapi::rxHandler(mss_uart_instance_t *this_uart) {
    MSS_GPIO_set_output(GPIO_PORT(OUT_DAPI_CTS), 1);
    Dapi &dapi = Dapi::getInstance();
    dapi.rxBufferIdx += MSS_UART_get_rx(this_uart,
            &dapi.rxBuffer[dapi.rxBufferIdx], 64 - dapi.rxBufferIdx);
    MSS_GPIO_set_output(GPIO_PORT(OUT_DAPI_CTS), 0);
}

Dapi::Dapi() {
    MSS_GPIO_config(GPIO_PORT(OUT_DAPI_CTS), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(OUT_DAPI_CTS), 0);

    MSS_UART_init(&g_mss_uart0, MSS_UART_115200_BAUD,
    MSS_UART_DATA_8_BITS | MSS_UART_EVEN_PARITY | MSS_UART_ONE_STOP_BIT);
    MSS_UART_set_tx_endian(&g_mss_uart0, MSS_UART_LITTLEEND);
    MSS_UART_set_rx_endian(&g_mss_uart0, MSS_UART_LITTLEEND);
    MSS_UART_set_rx_handler(&g_mss_uart0, &Dapi::rxHandler,
            MSS_UART_FIFO_EIGHT_BYTES);
}
