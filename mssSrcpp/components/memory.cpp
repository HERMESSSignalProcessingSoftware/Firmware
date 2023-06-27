#include "memory.h"
#include <stdint.h>
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sb_hw_platform.h"
#include <cstring>
#include "../tools/msghandler.h"
#include "dapi.h"

Memory& Memory::getInstance() {
    static Memory instance;
    return instance;
}

bool Memory::dumpInProgress(void) {
    return dumpInProgressVar;
}

void Memory::convertAndTransmit(uint8_t data[512], uint32_t currentPage,
        uint32_t totalNumberOfPages) {
    uint8_t frame[67] = { 0 };
    uint8_t *framePtr = &frame[8];
    frame[0] = cmd;
    // current page number
    frame[1] = (currentPage >> 16) & 0xFF;
    frame[2] = (currentPage >> 8) & 0xFF;
    frame[3] = currentPage & 0xFF;
    // total pages
    frame[4] = (totalNumberOfPages >> 16) & 0xFF;
    frame[5] = (totalNumberOfPages >> 8) & 0xFF;
    frame[6] = totalNumberOfPages & 0xFF;
    // End
    frame[64] = 0x0F;
    frame[65] = 0x17;
    frame[66] = 0xF0;
    if (data) {
        for (int i = 0; i < 9; i++) {
            frame[7] = i & 0xFF;
            memcpy(framePtr, &data[i * 56], 56U);
            Dapi::getInstance().transmitRawPufferd(frame, 67);
        }
    }
}

void Memory::resetLocalInRAMMemory(void) {
    for (int i = 0; i < 128; i++)
        this->memory[i] = 0;
}

void Memory::dumpMemory(uint32_t startAddr, uint32_t endAddr, uint8_t cmd) {
    resetLocalInRAMMemory();
    uint32_t address;
    if (dumpInProgressVar == false) {
        dumpInProgressVar = true;
        interfaceOne.setAddress(startAddr);
        interfaceOne.setCSPin(GPIO_PORT(FLASH_CS1));
        interfaceOne.setSPIHandle(&g_mss_spi0);
        this->endAddr = endAddr;
        this->cmd = cmd;
    } else {
        address = interfaceOne.getAddress();
        if (interfaceOne.getCSPin() == GPIO_PORT(FLASH_CS1)) {
            interfaceOne.readPage((uint8_t*) this->memory, PAGEADDR(address));
            if (Dapi::getInstance().transmitPufferEmpty()) {
                convertAndTransmit(this->memory, address, this->endAddr);
                interfaceOne.setCSPin(GPIO_PORT(FLASH_CS2));
            }
        } else {
            interfaceOne.readPage((uint8_t*) this->memory, PAGEADDR(address));
            if (Dapi::getInstance().transmitPufferEmpty()) {
                convertAndTransmit(this->memory, (1 << 22) | address, this->endAddr);
                interfaceOne.setCSPin(GPIO_PORT(FLASH_CS1));
                interfaceOne.increaseAddress();
            }
        }
        if (interfaceOne.getAddress() >= this->endAddr) {
            dumpInProgressVar = false;
        }
    }
}

void Memory::worker() {
    if (dumpInProgressVar) {
        /*dump values, will not be used. Real address will be set during call from dapi*/
        dumpMemory(0x0, 0xFFFFFFFF, cmd);
    }
}

uint32_t Memory::metaDataHighestAddress(void) {
    resetLocalInRAMMemory();
    uint32_t *ptr = (uint32_t*) &this->memory;
    uint32_t addr = 0x0;
    int32_t index = -2;
    bool possibleFoundOnLastPage = false;
    MemorySPI tmpMetaDevive = metaInterface;
    tmpMetaDevive.setAddress(0x0);
    while (tmpMetaDevive.getAddress() < 0x200) {
        tmpMetaDevive.readPage(this->memory, PAGEADDR(tmpMetaDevive.getAddress()));
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
}

void Memory::clearMemory() {
}

void Memory::abortClearMemory() {

}

void Memory::saveDp(const Datapackage &dp) {
    if (savedDataPoints < DatasetsPerPage) {
    }
}

Memory::Memory() :
        PageSize(PAGESIZE), PageCount(PAGE_COUNT), PageAddressShift(1 << 9), DatasetsPerPage(
                9), savedDataPoints(0), dumpInProgressVar(false), endAddr(
                0xFFFFFFFF), cmd(0xff) {
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
    resetLocalInRAMMemory();
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
    return 0;
}

void MemorySPI::readPage(uint8_t *data, uint32_t addr) {
    SPI_MemoryCommand_t command = c_READ;
    uint8_t tmp_add;
    //CS low
    psr_t isr = HAL_disable_interrupts();
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
    HAL_restore_interrupts(isr);
}

void MemorySPI::chipErase(void) {

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
