/*
 * storage.cpp
 *
 *  Created on: 09.12.2022
 *      Author: RG
 */

#include "storage.h"
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"


/**
 * Function Read Status
 * reads the status byte from the memory unit
 * @param: SPI_val
 * @return 8 bit status register
 */
uint8_t readStatus(SPI_Values spi_val) {
    uint8_t data;
    uint8_t command = c_READSTATUSREG1;
    //CS low
    MSS_GPIO_set_output(spi_val.CS_Pin, 0);

    //delay(1);

    //send command and read answer into data
    MSS_SPI_transfer_frame(&g_mss_spi0, command);
    data = MSS_SPI_transfer_frame(&g_mss_spi0, 0x00);

    //CS high
    MSS_GPIO_set_output(spi_val.CS_Pin, 1);
    return data;
}

/**
 *  Function Write Byte
 *  @brief Writes one byte so SPI
 *  @param uint8_t data: the one byte
 *  @param SPI_Val: the specifier of the SPI
 *  @return 0: Ok.
 */
int writeByte(uint8_t data, SPI_Values spi_val) {
    MSS_GPIO_set_output(spi_val.CS_Pin, 0);
    //delay(1);

    //send command and read answer into data
    MSS_SPI_transfer_frame(&g_mss_spi0, data);

    //CS1 high
    MSS_GPIO_set_output(spi_val.CS_Pin, 1);
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
int writePage(uint8_t *data, uint32_t address, SPI_Values spi_val) {
    uint8_t command = c_WRITEPAGE;
    uint8_t tmp_add;
    uint32_t i = 0;
//Write enable
    writeByte(c_WREN, spi_val);

    MSS_GPIO_set_output(spi_val.CS_Pin, 0);
    //commando schicken
    MSS_SPI_transfer_frame(&g_mss_spi0, command);
    //delay(1); /* !!! Do not use this in productiv application, build a function which is able to wait just a few us*/

    /* end of waiting */
    //Addressse schicken MSB to LSB
    //address = 0x11223344;
    //MSS_SPI_transfer_block(&g_mss_spi0, &address, 4, recBuffer, 0); /* Reihnfolge der bytes ist nicht richtig */
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);
    //Daten schicken
    for (i = 0; i < PAGESIZE; i++) {
        MSS_SPI_transfer_frame(&g_mss_spi0, data[i]);

    }

    MSS_GPIO_set_output(spi_val.CS_Pin, 1);

    //Write disable
    writeByte(c_WRDI, spi_val);
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
int readPage(uint8_t *data, uint32_t address, SPI_Values spi_val) {

    uint8_t command = c_READ;
    uint8_t tmp_add;
    //CS low
    MSS_GPIO_set_output(spi_val.CS_Pin, 0);

    //commando schicken
//  writeByte(c_READ, SPI_val);
    MSS_SPI_transfer_frame(&g_mss_spi0, command);

//4 Byte Addressse schicken MSB to LSB
//  HAL_SPI_Transmit(SPI_val.spihandle, (uint8_t*) (&address), 4, 40);
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

//Daten lesen+
    for (uint32_t i = 0; i < PAGESIZE; i++) {
        data[i] = MSS_SPI_transfer_frame(&g_mss_spi0,  0x00);
    }

//CS high
    MSS_GPIO_set_output(spi_val.CS_Pin, 1);

    return 0;
}

/**
 * Function Chip Erase
 * Erases the whole chip
 * @param SPI_Values SPI_val: the corresponding memory ic
 */
int chipErase(SPI_Values SPI_val) {
    //Write enable
    writeByte(c_WREN, SPI_val);
    //erase chip
    writeByte(c_CE, SPI_val);
    //Write Disable
    writeByte(c_WRDI, SPI_val);
    //warte bis Schreiben beendet ist
    writeReady(SPI_val);

    return 0;
}


int readBytes(uint8_t *data, uint32_t address, int count, SPI_Values spi_val) {
    if (count > sizeof(data)) {
        return 1;
    }

    uint8_t command = c_READ;
    uint8_t tmp_add;
    //CS low
    MSS_GPIO_set_output(spi_val.CS_Pin, 0);
    //commando schicken
    MSS_SPI_transfer_frame(&g_mss_spi0, command);

    //4 Byte Addressse schicken MSB to LSB
    tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(&g_mss_spi0, tmp_add);

    //Daten lesen
    for (uint32_t i = 0; i < count; i++) {
        data[i] = MSS_SPI_transfer_frame(&g_mss_spi0, 0x00);
    }

    //CS1 high
    MSS_GPIO_set_output(spi_val.CS_Pin, 1);

    return 0;
}

uint32_t UpdateMetadata(uint32_t pageAddr, uint32_t metaAddress, SPI_Values dev) {
    uint8_t buffer[PAGESIZE] = { 0 };
    uint32_t *ptr = (uint32_t*)buffer;
    uint32_t value = 0;
    uint32_t offset = 0;
    uint32_t page;
    /*Iterate over all pages*/
    for (uint32_t i = metaAddress; i < START_OF_DATA_SEGMENT; i++) {
        readPage(buffer, PAGEADDR(i), dev);
        ptr = (uint32_t*)(buffer ); //Reset the ptr to the start of the buffer
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
            Write32Bit(pageAddr, PAGEADDR(i) + offset*4 , dev);
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
void writeReady(SPI_Values SPI_val) {
    uint32_t status = 0;
    uint8_t SR1 = 0xF;
    while (status == 0) {
        SR1 = readStatus(SPI_val);
        if ((SR1 & 0x1) == 0)
            status = 1;
    }
}

void Write32Bit(uint32_t value, uint32_t address, SPI_Values device) {
    writeByte(c_WREN, device);

    MSS_GPIO_set_output(device.CS_Pin, 0);

    MSS_SPI_transfer_frame(device.spihandle, c_WRITEPAGE);

    //Transfer address MSB to LSB
    uint8_t tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
    MSS_SPI_transfer_frame(device.spihandle, tmp_add);

    tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
    MSS_SPI_transfer_frame(device.spihandle, tmp_add);

    tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
    MSS_SPI_transfer_frame(device.spihandle, tmp_add);

    tmp_add = (uint8_t) (address & 0x000000FF);
    MSS_SPI_transfer_frame(device.spihandle, tmp_add);

    uint8_t *ptr= (uint8_t*)&value;
    for (uint32_t i = 0; i < 4; i++) {
        MSS_SPI_transfer_frame(device.spihandle, ptr[i]);
    }

    MSS_GPIO_set_output(device.CS_Pin, 1);
    //Write disable
    writeByte(c_WRDI, device);
}

void WriteBytes(uint8_t *data, uint32_t size, uint32_t address,
        SPI_Values device) {
    if (data) {
        writeByte(c_WREN, device);

        MSS_GPIO_set_output(device.CS_Pin, 0);

        MSS_SPI_transfer_frame(device.spihandle, c_WRITEPAGE);

        //Transfer address MSB to LSB
        uint8_t tmp_add = (uint8_t) ((address >> 24) & 0x000000FF);
        MSS_SPI_transfer_frame(device.spihandle, tmp_add);

        tmp_add = (uint8_t) ((address >> 16) & 0x000000FF);
        MSS_SPI_transfer_frame(device.spihandle, tmp_add);

        tmp_add = (uint8_t) ((address >> 8) & 0x000000FF);
        MSS_SPI_transfer_frame(device.spihandle, tmp_add);

        tmp_add = (uint8_t) (address & 0x000000FF);
        MSS_SPI_transfer_frame(device.spihandle, tmp_add);

        for (uint32_t i = 0; i < size; i++) {
            MSS_SPI_transfer_frame(device.spihandle, data[i]);

        }
        MSS_GPIO_set_output(device.CS_Pin, 1);
        //Write disable
        writeByte(c_WRDI, device);
    }
}
