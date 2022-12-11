#include "memory.h"
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sb_hw_platform.h"
#include "../tools/tools.h"
#include <cstring>

Memory& Memory::getInstance() {
    static Memory instance;
    return instance;
}

void Memory::worker() {
    if (this->write) {
        //TODO: Write function to safe data.
        writePage(&(this->memory), PAGEADDRESS(XX), this->spiVal);
        this->write = false;
        this->memoryCounter = 0;
    }
}

void Memory::clearMemory() {
}

void Memory::abortClearMemory() {
}

void Memory::saveDp(const Datapackage &dp) {
    if (memoryCounter <= 18) {
        uint8_t array[28];
        dp.toBytes((unsigned char*)&array);
        /*TODO: Pointer arithmetic!*/
        memcpy(this->memory + (28 * this->memoryCounter), array, 28);
        if (this->memoryCounter == 19) {
            this->write = true;
        }
    }
}

Memory::Memory() {
    this->handle = &g_mss_spi0;
    this->CS_Pin = FLASH_CS_1;
    this->memoryCounter = 0;
    this->write = false;
    this->spiVal = {&g_mss_spi0, FLASH_CS_1};
    MSS_SPI_init(this->handle);
    MSS_SPI_configure_master_mode(this->handle, MSS_SPI_SLAVE_0, MSS_SPI_MODE0, 4u, MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);
    /* Do set the nCSx signals to high */
    MSS_GPIO_set_output(FLASH_CS_1, 1);
    MSS_GPIO_set_output(FLASH_CS_2, 1);
    MSS_SPI_set_slave_select(this->handle, MSS_SPI_SLAVE_0);

    //TODO: Where is the data stored at?
    //TODO: Interrupts?
    //TODO: Implementation?
}

