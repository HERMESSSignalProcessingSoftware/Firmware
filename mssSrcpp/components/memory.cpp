#include "memory.h"
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
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
        if (this->pageCounter <= 125000) {
            /*most important: Write data to flash*/
            writePage(this->memory, PAGEADDR(this->pageCounter), this->CS_Pin);
            /* Toggle CS Pin */
            if (this->CS_Pin == FLASH_CS_2) {
                this->CS_Pin = FLASH_CS_1;
                this->pageCounter++;
            } else {
                this->CS_Pin = FLASH_CS_2;
            }
            /*Set internal values properly*/
            this->memoryCounter = 0;
        }
        this->write = false;
    }
}

void Memory::clearMemory() {
    mss_gpio_id_t currentSelectedPin = this->CS_Pin;
    this->CS_Pin = FLASH_CS_1;
    writeReady(this->CS_Pin);
    chipErase(this->CS_Pin, false);
    this->CS_Pin = FLASH_CS_2;
    writeReady(this->CS_Pin);
    chipErase(this->CS_Pin, true);
    this->CS_Pin = currentSelectedPin;
}

void Memory::abortClearMemory() {

}

void Memory::saveDp(const Datapackage &dp) {
    if (this->memoryCounter <= 18) {
        uint8_t array[56];
        dp.toBytes((unsigned char*) &array);
        /*TODO: Pointer arithmetic!*/

        /*for (int i = (28 * this->memoryCounter++); i < 28; i++) {
         this->memory[i] = array[i % 28];
         }*/
        memcpy(this->memory + (56 * this->memoryCounter++), array, 56);
        if (this->memoryCounter == 9) {
            this->write = true;
        }
    }
}

void Memory::readMemory(uint8_t *data, uint32_t from, uint32_t to) {
    if (data) {
        for (uint32_t i = from; i < to; i++) {
            this->readPage(data, PAGEADDR(i), FLASH_CS_1);
            this->readPage(data + PAGESIZE, PAGEADDR(i), FLASH_CS_2);
        }
    }
}

Memory::Memory() {
    this->handle = &g_mss_spi0;
    this->CS_Pin = FLASH_CS_1;
    this->memoryCounter = 0;
    this->write = false;
    this->pageCounter = 0;
    for (int i = 0; i < PAGESIZE; i++)
        this->memory[i] = 0;
    MSS_SPI_init(this->handle);
    MSS_SPI_configure_master_mode(this->handle, MSS_SPI_SLAVE_0, MSS_SPI_MODE0, 16u, MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);
    /* Do set the nCSx signals to high */
    MSS_GPIO_set_output(FLASH_CS_1, 1);
    MSS_GPIO_set_output(FLASH_CS_2, 1);
    MSS_SPI_set_slave_select(this->handle, MSS_SPI_SLAVE_0);
}

uint8_t Memory::readStatus(mss_gpio_id_t nCSPin) {
    uint8_t data;
    uint8_t command = c_READSTATUSREG1;
//CS low
    MSS_GPIO_set_output(nCSPin, 0);

//delay(1);

//send command and read answer into data
    MSS_SPI_transfer_frame(this->handle, command);
    data = MSS_SPI_transfer_frame(this->handle, 0x00);

//CS high
    MSS_GPIO_set_output(nCSPin, 1);
    return data;
}

/**
 *  Function Write Byte
 *  @brief Writes one byte so SPI
 *  @param uint8_t data: the one byte
 *  @param SPI_Val: the specifier of the SPI
 *  @return 0: Ok.
 */
int Memory::writeByte(uint8_t data, mss_gpio_id_t nCSPin) {
    MSS_GPIO_set_output(nCSPin, 0);
//delay(1);

//send command and read answer into data
    MSS_SPI_transfer_frame(this->handle, data);

//CS1 high
    MSS_GPIO_set_output(nCSPin, 1);
    return 0;
}

/**
 * Function Write Page
 * @brief Transmits command C_WREN (write enable) to the SPI
 * C_WRITEPAGE. Transfert address now, after that you have to transfer one page (256 byte) of data.
 *
 * Data overview
 *
 * Byte :   1       2           3-6     7 - 263     264
 * Ddata:   c_WREN  c_WRITEPAGE ADDR    DATA        c_WRDI
 *
 * @param uint8_t * data: Pointer to the data field
 * @param uint32_t address: the address on the memory chip
 * @param SPI_Values SPI_val
 * @return 0: ok
 */
int Memory::writePage(uint8_t *data, uint32_t address, mss_gpio_id_t nCSPin) {
    uint8_t command = c_WRITEPAGE;
    uint8_t tmp_add;
    uint32_t i = 0;
//Write enable
    writeByte(c_WREN, nCSPin);

    MSS_GPIO_set_output(nCSPin, 0);
//commando schicken
    MSS_SPI_transfer_frame(this->handle, command);
//delay(1); /* !!! Do not use this in productiv application, build a function which is able to wait just a few us*/

    /* end of waiting */
//Addressse schicken MSB to LSB
//address = 0x11223344;
//MSS_SPI_transfer_block(this->handle, &address, 4, recBuffer, 0); /* Reihnfolge der bytes ist nicht richtig */
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);
//Daten schicken
    for (i = 0; i < PAGESIZE; i++) {
        MSS_SPI_transfer_frame(this->handle, data[i]);

    }

    MSS_GPIO_set_output(nCSPin, 1);

//Write disable
    writeByte(c_WRDI, nCSPin);
    return i;
}

/**
 * Function Read Page
 * @brief
 * Reads one page from the memory unit. The address determines which page is going to be read.
 *
 * Data overview:
 * To Memory IC:
 * Byte:    1       2 - 5
 * Data:    c_READ  Address
 *
 * From Memory IC:
 * Byte:    0 - 512
 * Data:    Data
 *
 * @param uint8_t  * data: Pointer to the data array
 * @param uint32_t address: Address of the momory unit to be read
 * @param SPI_Values SPI_val: the corresponding SPI values
 * @return 0: ok
 */
int Memory::readPage(uint8_t *data, uint32_t address, mss_gpio_id_t nCSPin) {

    uint8_t command = c_READ;
    uint8_t tmp_add;
//CS low
    MSS_GPIO_set_output(nCSPin, 0);

//commando schicken
//  writeByte(c_READ, SPI_val);
    MSS_SPI_transfer_frame(this->handle, command);

//4 Byte Addressse schicken MSB to LSB
//  HAL_SPI_Transmit(SPI_val.spihandle, (uint8_t*) (&address), 4, 40);
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

//Daten lesen+
    for (uint32_t i = 0; i < PAGESIZE; i++) {
        data[i] = MSS_SPI_transfer_frame(this->handle, 0x00);
    }

//CS high
    MSS_GPIO_set_output(nCSPin, 1);

    return 0;
}

/**
 * Function Chip Erase
 * Erases the whole chip
 * @param SPI_Values SPI_val: the corresponding memory ic
 * @param bool fastReturn, waits until write in progress flag disappears.
 */
int Memory::chipErase(mss_gpio_id_t nCSPin, bool fastReturn) {
//Write enable
    writeByte(c_WREN, nCSPin);
//erase chip
    writeByte(c_CE, nCSPin);
//Write Disable
    writeByte(c_WRDI, nCSPin);
//warte bis Schreiben beendet ist
    if (fastReturn == false)
        writeReady(nCSPin);

    return 0;
}

uint32_t Memory::readBytes(uint8_t *data, uint32_t address, int count,
        mss_gpio_id_t nCSPin) {
    if (count > sizeof(data)) {
        return 1;
    }

    uint8_t command = c_READ;
    uint8_t tmp_add;
//CS low
    MSS_GPIO_set_output(nCSPin, 0);
//commando schicken
    MSS_SPI_transfer_frame(this->handle, command);

//4 Byte Addressse schicken MSB to LSB
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

//Daten lesen
    for (uint32_t i = 0; i < count; i++) {
        data[i] = MSS_SPI_transfer_frame(this->handle, 0x00);
    }

//CS1 high
    MSS_GPIO_set_output(nCSPin, 1);

    return 0;
}

uint32_t Memory::updateMetadata(uint32_t pageAddr, uint32_t metaAddress,
        mss_gpio_id_t dev) {
    uint8_t buffer[PAGESIZE] = { 0 };
    uint32_t *ptr = (uint32_t*) buffer;
    uint32_t value = 0;
    uint32_t offset = 0;
    uint32_t page;
    /*Iterate over all pages*/
    for (uint32_t i = metaAddress; i < START_OF_DATA_SEGMENT; i++) {
        readPage(buffer, PAGEADDR(i), dev);
        ptr = (uint32_t*) (buffer); //Reset the ptr to the start of the buffer
        for (uint32_t index = 0; index < 128; index++) {
            value = *(ptr);
            if (value == 0xFFFFFFFF) {
                offset = index;
                //system.metaAddressOffset = index;
                break;
            } else {
                ptr++;
            }

        }
        if (value == 0xFFFFFFFF) {
            *ptr = pageAddr; //Pointer was added due to search function implemented above, remove one
            write32Bit(pageAddr, PAGEADDR(i) + offset * 4, dev);
            //writePage(buffer, i, dev);
            page = i;
            break;
        }
    }
    writeReady(dev);
    return page;
}

/**
 * Waits for the bit WIP (Write in Progress) bit to toggle
 * Reads SR1 register from memory
 */
void Memory::writeReady(mss_gpio_id_t nCSPin) {
    uint32_t status = 0;
    uint8_t SR1 = 0xF;
    while (status == 0) {
        SR1 = readStatus(nCSPin);
        if ((SR1 & 0x1) == 0)
            status = 1;
    }
}

void Memory::write32Bit(uint32_t value, uint32_t address,
        mss_gpio_id_t nCSPin) {
    writeByte(c_WREN, nCSPin);

    MSS_GPIO_set_output(nCSPin, 0);

    MSS_SPI_transfer_frame(this->handle, c_WRITEPAGE);

//Transfer address MSB to LSB
    uint8_t tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(this->handle, tmp_add);

    uint8_t *ptr = (uint8_t*) &value;
    for (uint32_t i = 0; i < 4; i++) {
        MSS_SPI_transfer_frame(this->handle, ptr[i]);
    }

    MSS_GPIO_set_output(nCSPin, 1);
//Write disable
    writeByte(c_WRDI, nCSPin);
}

void Memory::writeBytes(uint8_t *data, uint32_t size, uint32_t address,
        mss_gpio_id_t nCSPin) {
    if (data) {
        writeByte(c_WREN, nCSPin);

        MSS_GPIO_set_output(nCSPin, 0);

        MSS_SPI_transfer_frame(this->handle, c_WRITEPAGE);

        //Transfer address MSB to LSB
        uint8_t tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
        MSS_SPI_transfer_frame(this->handle, tmp_add);

        tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
        MSS_SPI_transfer_frame(this->handle, tmp_add);

        tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
        MSS_SPI_transfer_frame(this->handle, tmp_add);

        tmp_add = (uint8_t) (address & 0x000000FF);
        MSS_SPI_transfer_frame(this->handle, tmp_add);

        for (uint32_t i = 0; i < size; i++) {
            MSS_SPI_transfer_frame(this->handle, data[i]);

        }
        MSS_GPIO_set_output(nCSPin, 1);
        //Write disable
        writeByte(c_WRDI, nCSPin);
    }
}

