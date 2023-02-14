#include "memory.h"
#include <stdint.h>
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sb_hw_platform.h"
#include <cstring>
#include "../tools/msghandler.h"

Memory& Memory::getInstance() {
    static Memory instance;
    return instance;
}

void Memory::worker() {
    bool errorMemoryFull = false;
    if (savedDataPoints >= 9) {
        savedDataPoints = 0;
        if (this->activeInterface == InterfaceOne) {
            if (interfaceOne.getAddress() < PageCount) {
                interfaceOne.writePage((uint8_t*) this->memory);
                interfaceOne.increaseAddress();
                activeInterface = InterfaceTwo;
            } else {
                errorMemoryFull = true;
            }
        } else if (this->activeInterface == InterfaceTwo) {
            if (interfaceTwo.getAddress() < PageCount) {
                interfaceTwo.writePage((uint8_t*) this->memory);
                interfaceTwo.increaseAddress();
                activeInterface = InterfaceOne;
            } else {
                errorMemoryFull = true;
            }
        } else {
            // Meta interface read here
        }
    }
    if (errorMemoryFull)
        MsgHandler::getInstance().error("Memory full!");
}

uint32_t Memory::metaDataHighestAddress(void) {
    uint8_t memory[PAGESIZE];
    uint32_t *ptr = (uint32_t*) &memory;
    uint32_t addr = 0x0;
    int32_t index = -2;
    bool possibleFoundOnLastPage = false;
    MemorySPI tmpMetaDevive = metaInterface;
    tmpMetaDevive.setAddress(0x0);
    while (tmpMetaDevive.getAddress() < 0x200) {
        tmpMetaDevive.readPage(memory, PAGEADDR(tmpMetaDevive.getAddress()));
        if (possibleFoundOnLastPage) {
            index = 127;
        } else {
            for (int i = 0; i < 128; i++) {
                if (ptr[i] == 0xFFFFFFFF) {
                    index = i - 1;
                    break;
                }
            }
        }

        if (index == -1) { // found proceed to previous page
            tmpMetaDevive.decreaseAddress();
            possibleFoundOnLastPage = true;
        } else if (index >= 0) {
            addr = ptr[index];
            break;
        } else {
            tmpMetaDevive.increaseAddress();
            // Index < -1 -> means -2 nothing found
        }
    }
    delete &tmpMetaDevive;
    return addr;
}

void Memory::recovery(void) {
    uint32_t addr = metaDataHighestAddress();
    if (addr + 0x200 < PAGE_COUNT) {
        interfaceOne = MemorySPI(GPIO_PORT(FLASH_CS1), &g_mss_spi0,
                addr + 0x200);
        interfaceTwo = MemorySPI(GPIO_PORT(FLASH_CS2), &g_mss_spi0,
                addr + 0x200);
    }
}

uint32_t Memory::memoryStatus(void) {
    return (interfaceOne.readStatus() << 8) | (interfaceTwo.readStatus());
}

void Memory::updateMetadata(void) {
    uint8_t metaData[PageSize];
    uint32_t *ptrMetadata = (uint32_t*) &metaData;
    int32_t index = -1;
    bool found = false;
    if (savedDataPoints <= 7) {
        while (!found && metaInterface.getAddress() < 0x200) {
            metaInterface.readPage(metaData,
                    PAGEADDR(metaInterface.getAddress()));
            for (int i = 0; i < 128; i++) {
                if (ptrMetadata[i] == 0xFFFFFFFF) {
                    index = i;
                    break;
                }
            }
            if (index == -1) {
                metaInterface.increaseAddress();
            } else {
                found = true;
                if ((index > 0
                        && interfaceOne.getAddress() != ptrMetadata[index - 1])
                        || (index == 0)) {
                    ptrMetadata[index] = interfaceOne.getAddress();
                }
            }
        }
        if (metaInterface.getAddress() < 0x200)
            metaInterface.writePage(metaData);
    }
}

void Memory::clearMemory() {
    this->interfaceOne.chipErase();
    this->interfaceTwo.chipErase();
}

void Memory::abortClearMemory() {

}

void Memory::saveDp(const Datapackage &dp) {
    if (savedDataPoints < DatasetsPerPage) {
        uint8_t array[56];
        dp.toBytes((unsigned char*) &array);
        memcpy(this->memory + (56 * this->savedDataPoints++), array, 56);
    }
}

Memory::Memory() :
        PageSize(PAGESIZE), PageCount(PAGE_COUNT), PageAddressShift(1 << 9), DatasetsPerPage(
                9), savedDataPoints(0) {
    interfaceOne = MemorySPI(GPIO_PORT(FLASH_CS1), &g_mss_spi0, 0x200);
    interfaceTwo = MemorySPI(GPIO_PORT(FLASH_CS2), &g_mss_spi0, 0x200);
    metaInterface = MemorySPI(GPIO_PORT(FLASH_CS1), &g_mss_spi0, 0x00);
    activeInterface = InterfaceOne;

    MSS_GPIO_config(interfaceOne.getCSPin(), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config(interfaceTwo.getCSPin(), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(interfaceOne.getCSPin(), 1);
    MSS_GPIO_set_output(interfaceTwo.getCSPin(), 1);

    MSS_SPI_init(&g_mss_spi0);
    MSS_SPI_configure_master_mode(&g_mss_spi0, MSS_SPI_SLAVE_0, MSS_SPI_MODE0,
            4u, MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);
    MSS_SPI_set_slave_select(&g_mss_spi0, MSS_SPI_SLAVE_0);

    uint32_t *memoryPtr = (uint32_t*) &this->memory;
    for (int i = 0; i < 128; i++) {
        memoryPtr[i] = 0;
    }
}

MemorySPI::MemorySPI() :
        CSPin(GPIO_PORT(FLASH_CS1)), spihandle(&g_mss_spi0), address(0), pagesWritten(
                0) {
}

MemorySPI::MemorySPI(const MemorySPI &old) :
        CSPin(old.CSPin), spihandle(old.spihandle), address(old.address), pagesWritten(
                old.pagesWritten) {

}

MemorySPI::MemorySPI(mss_gpio_id_t pin, mss_spi_instance_t *handle,
        uint32_t addr) :
        CSPin(pin), spihandle(handle), address(addr), pagesWritten(0) {
}

MemorySPI::~MemorySPI() {

}

void MemorySPI::setCSPin(mss_gpio_id_t pin) {
    this->CSPin = pin;
}

void MemorySPI::setSPIHandle(mss_spi_instance_t *handle) {
    this->spihandle = handle;
}

mss_spi_instance_t* MemorySPI::getSPIHandle(void) {
    return this->spihandle;
}

mss_gpio_id_t MemorySPI::getCSPin(void) {
    return this->CSPin;
}

uint8_t MemorySPI::readStatus(void) {
    uint8_t data;
    SPI_MemoryCommand_t cmd = c_READSTATUSREG1;
    MSS_GPIO_set_output(CSPin, 0);
    MSS_SPI_transfer_frame(spihandle, (uint8_t) cmd);
    data = MSS_SPI_transfer_frame(spihandle, 0x00);
    MSS_GPIO_set_output(CSPin, 1);
    return data;
}

void MemorySPI::writeByte(uint8_t data) {
    MSS_GPIO_set_output(CSPin, 0);
    MSS_SPI_transfer_frame(spihandle, data);
    MSS_GPIO_set_output(CSPin, 1);
}

uint32_t MemorySPI::writePage(uint8_t *data) {
    SPI_MemoryCommand_t command = c_WRITEPAGE;
    uint8_t tmp_add;
    uint32_t i = 0;
    //Write enable
    writeByte((uint8_t) c_WREN);

    MSS_GPIO_set_output(CSPin, 0);
    //commando schicken
    MSS_SPI_transfer_frame(spihandle, (uint8_t) command);
    //delay(1); /* !!! Do not use this in productiv application, build a function which is able to wait just a few us*/

    /* end of waiting */
    //Addressse schicken MSB to LSB
    //address = 0x11223344;
    //MSS_SPI_transfer_block(&g_mss_spi0, &address, 4, recBuffer, 0); /* Reihnfolge der bytes ist nicht richtig */
    tmp_add = (uint8_t) ((PAGEADDR(address) >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((PAGEADDR(address) >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((PAGEADDR(address) >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) (PAGEADDR(address) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);
    //Daten schicken
    for (i = 0; i < PAGESIZE; i++) {
        MSS_SPI_transfer_frame(spihandle, data[i]);

    }

    MSS_GPIO_set_output(CSPin, 1);

    //Write disable
    writeByte((uint8_t) c_WRDI);
    return i;
}

void MemorySPI::readPage(uint8_t *data, uint32_t addr) {
    SPI_MemoryCommand_t command = c_READ;
    uint8_t tmp_add;
    //CS low
    MSS_GPIO_set_output(CSPin, 0);

    //commando schicken
    //  writeByte(c_READ, SPI_val);
    MSS_SPI_transfer_frame(spihandle, (uint8_t) command);

    //4 Byte Addressse schicken MSB to LSB
    //  HAL_SPI_Transmit(SPI_val.spihandle, (uint8_t*) (&address), 4, 40);
    tmp_add = (uint8_t) ((addr >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((addr >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((addr >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) (addr & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    //Daten lesen+
    for (uint32_t i = 0; i < PAGESIZE; i++) {
        data[i] = MSS_SPI_transfer_frame(spihandle, 0x00);
    }

    //CS high
    MSS_GPIO_set_output(CSPin, 1);

}

void MemorySPI::chipErase(void) {
    writeByte((uint8_t) c_WREN);
    writeByte((uint8_t) c_CE);
    writeByte((uint8_t) c_WRDI);
    writeReady(false);
}

bool MemorySPI::writeReady(bool blocking) {
    uint32_t status = 0;
    uint8_t StatusReg1 = 0;
    if (blocking) {
        while (status == 0) {
            StatusReg1 = readStatus();
            if ((StatusReg1 & 0x01) == 0)
                status = 1;
        }
    } else {
        StatusReg1 = readStatus();
    }
    return (StatusReg1 & 0x01) == 0;
}

uint32_t MemorySPI::getAddress(void) {
    return this->address;
}

void MemorySPI::setAddress(uint32_t addr) {
    this->address = addr;
}

void MemorySPI::increaseAddress(void) {
    this->pagesWritten++;
    this->address++;
}

void MemorySPI::decreaseAddress(void) {
    this->address--;
}
