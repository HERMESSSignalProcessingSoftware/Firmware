/*
 * storage.h
 *
 *  Created on: 09.12.2022
 *      Author: RG
 */

#ifndef TOOLS_STORAGE_H_
#define TOOLS_STORAGE_H_
#include "../components/memory.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define START_OF_DATA_SEGMENT 0x200

/**
 * Function Read Status
 * reads the status byte from the memory unit
 * @param: SPI_val
 * @return 8 bit status register
 */
uint8_t readStatus(SPI_Values);

/**
 *  Function Write Byte
 *  @brief Writes one byte so SPI
 *  @param uint8_t data: the one byte
 *  @param SPI_Val: the specifier of the SPI
 *  @return 0: Ok.
 */
int writeByte(uint8_t data, SPI_Values);

/**
 * Function Write Page
 * @brief Transmits command C_WREN (write enable) to the SPI
 * C_WRITEPAGE. Transfered address now, after that you have to transfer one page (256 byte) of data.
 *
 * Data overview
 *
 * Byte :   1       2           3-6     7 - 263     264
 * Data:   c_WREN  c_WRITEPAGE ADDR    DATA        c_WRDI
 *
 * @param uint8_t * data: Pointer to the data field
 * @param uint32_t address: the address on the memory chip
 * @param SPI_Values SPI_val
 * @return 0: ok
 */
int writePage(uint8_t *data, uint32_t address, SPI_Values);

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
 * Byte:    0 - 256
 * Data:    Data
 *
 * @param uint8_t  * data: Pointer to the data array
 * @param uint32_t address: Address of the momory unit to be read
 * @param SPI_Values SPI_val: the corresponding SPI values
 * @return 0: ok
 */
int readPage(uint8_t *data, uint32_t address, SPI_Values);

/**
 * Function Chip Erase
 * Erases the whole chip
 * @param SPI_Values SPI_val: the corresponding memory ic
 */
int chipErase(SPI_Values);

/**
 * Waits for the bit WIP (Write in Progress) bit to toggle
 * Reads SR1 register from memory
 */
void writeReady(SPI_Values);


/**
 * Writes a given number of bytes to the spi device; you may use this for partial page writing
 * @param data : The content for the memory
 * @param size : The number of bytes to be transfered
 * @param address : The address of the page which have to be written
 * @param device : The device which will receive the data
 */
void WriteBytes(uint8_t *data, uint32_t size, uint32_t address,
        SPI_Values device);

/**
 * @brief transmits a signal 32 bit value to the device by using partial page programming (Still 0x12 with less data)
 *
 * @param value : The Value to be transmitted
 * @param address : The address to be safed at
 * @param device : The memory device
 */
void Write32Bit(uint32_t value, uint32_t address, SPI_Values device);
/**
 * FOR TESTING PURPOSE
 */
uint32_t testMemory(void);

/**
 *
 * @param pageAddr the address to be saved on page 0 - 0x199
 * @param metaAddress the page  address of the current meta page
 * @param dev the device to be safed on
 * @return uint32_t page address of the current page
 */
uint32_t UpdateMetadata(uint32_t pageAddr, uint32_t metaAddress,
        SPI_Values dev);

#ifdef __cplusplus
}
#endif

#endif /* TOOLS_STORAGE_H_ */
