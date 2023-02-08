#include "memory.h"
#include <stdint.h>
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sb_hw_platform.h"

Memory& Memory::getInstance() {
    static Memory instance;
    return instance;
}

void Memory::worker() {
}

void Memory::clearMemory() {
    this->interfaceOne.chipErase();
    this->interfaceTwo.chipErase();
}

void Memory::abortClearMemory() {

}

void Memory::saveDp(const Datapackage &dp) {

}

Memory::Memory() :
        PageSize(512), PageCount(125000), PageAddressShift(1 << 9), DatasetsPerPage(
                8) {
    interfaceOne = MemorySPI(GPIO_PORT(FLASH_CS1), &g_mss_spi0);
    interfaceTwo = MemorySPI(GPIO_PORT(FLASH_CS2), &g_mss_spi0);
    activeInterface = InterfaceOne;

    MSS_GPIO_config(GPIO_PORT(FLASH_CS1), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config(GPIO_PORT(FLASH_CS2), MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(GPIO_PORT(FLASH_CS1), 1);
    MSS_GPIO_set_output(GPIO_PORT(FLASH_CS2), 1);

    MSS_SPI_init(&g_mss_spi0);
    MSS_SPI_configure_master_mode(&g_mss_spi0, MSS_SPI_SLAVE_0, MSS_SPI_MODE0,
            4u, MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);
    MSS_SPI_set_slave_select(&g_mss_spi0, MSS_SPI_SLAVE_0);

    for (int i = 0; i < 128; i++) {
        memory[i] = 0;
    }
}

MemorySPI::MemorySPI() :
        CSPin(GPIO_PORT(FLASH_CS1)), spihandle(&g_mss_spi0) {
}

MemorySPI::MemorySPI(const MemorySPI &old) :
        CSPin(old.CSPin), spihandle(old.spihandle) {

}

MemorySPI::MemorySPI(mss_gpio_id_t pin, mss_spi_instance_t *handle) :
        CSPin(pin), spihandle(handle) {
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

uint32_t MemorySPI::writePage(uint8_t *data, uint32_t address) {
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
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
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

void MemorySPI::readPage(uint8_t *data, uint32_t address) {
    SPI_MemoryCommand_t command = c_READ;
    uint8_t tmp_add;
    //CS low
    MSS_GPIO_set_output(CSPin, 0);

    //commando schicken
    //  writeByte(c_READ, SPI_val);
    MSS_SPI_transfer_frame(spihandle, (uint8_t) command);

    //4 Byte Addressse schicken MSB to LSB
    //  HAL_SPI_Transmit(SPI_val.spihandle, (uint8_t*) (&address), 4, 40);
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(spihandle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
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
        while(status == 0) {
            StatusReg1 = readStatus();
            if ((StatusReg1 & 0x01) == 0)
                status = 1;
        }
    } else {
        StatusReg1 = readStatus();
    }
    return (StatusReg1 & 0x01) == 0;
}
