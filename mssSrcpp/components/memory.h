#ifndef COMPONENTS_MEMORY_H_
#define COMPONENTS_MEMORY_H_

#include <stdint.h>
#include "../tools/datapackage.h"
#include "../hal.h"
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"

#define c_WRDI 0x04
#define c_READSTATUSREG1 0x05
#define c_READSTATUSREG2 0x07
#define c_READCONFIGREG1 0x35
#define c_CLEARSTATUSREG1 0x30
#define c_WREN 0x06
#define c_READSTATUSREG2 0x07
#define c_WRITEPAGE 0x12
#define c_READ 0x13
#define c_READID 0x9F

/*Warining: Chip Erease below!*/
#define c_CE 0xC7

#define PAGE_COUNT 125000

#define PAGEADDR(i) (i << 9)

#define PAGESIZE 512
#define START_OF_DATA_SEGMENT 0x200
/**
 * spihandle = hspin where n E {1,2,...,m}
 * CS_Pin = the Pin connected to n_chipselect of the corresponding flash memory
 * CS_Port = GPION N E {A,B,...,E}
 */
typedef struct {
    mss_spi_instance_t *spihandle;
    mss_gpio_id_t CS_Pin;
} SPI_Values;

class Memory {
public:
    static Memory& getInstance();

    // Maybe check for clearing memory progression here?
    void worker();

    /* Must be run async. Calling this function should be non-blocking.
     Also call Controller::clearMemFinished(), when finalized clearing
     the memory*/
    void clearMemory();

    /* Fast prepare to save some datapackages */
    void abortClearMemory();

    void saveDp(const Datapackage &dp);
private:
    Memory();
    mss_gpio_id_t CS_Pin;       /*< */
    mss_spi_instance_t *handle; /*< */
    uint8_t memory[PAGESIZE];   /*< */
    uint32_t memoryCounter;     /*< */
    uint32_t pageCounter;       /*< */
    bool write;                 /*< */
    /* Private functions */

    /**
     *
     * @param uint32_t* data
     * @param uint32_t address
     * @param int count
     * @param mss_gpio_id_t nCSPin
     * @return uint32_t 0 ok, 1 error count greater than size of data
     */
    uint32_t readBytes(uint8_t*, uint32_t, int, mss_gpio_id_t);

    /**
     * Function Read Status
     * reads the status byte from the memory unit
     * @param: mss_gpio_id_t associated memory controller FLASH_CS_1 / 2
     * @return 8 bit status register
     */
    uint8_t readStatus(mss_gpio_id_t);

    /**
     *  Function Write Byte
     *  @brief Writes one byte so SPI
     *  @param uint8_t data: the one byte
     *  @param mss_gpio_id_t: the specifier of the SPI nCS Pin
     *  @return 0: Ok.
     */
    int writeByte(uint8_t data, mss_gpio_id_t);

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
     * @param mss_gpio_id_t SPI_val
     * @return 0: ok
     */
    int writePage(uint8_t *data, uint32_t address, mss_gpio_id_t);

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
     * @param mss_gpio_id_t SPI_val: the corresponding SPI values
     * @return 0: ok
     */
    int readPage(uint8_t *data, uint32_t address, mss_gpio_id_t);

    /**
     * Function Chip Erase
     * Erases the whole chip
     * @param mss_gpio_id_t SPI_val: the corresponding memory ic
     * @param bool fastReturn, waits until write in progress flag disappears.
     */
    int chipErase(mss_gpio_id_t, bool fastReturn);

    /**
     * Waits for the bit WIP (Write in Progress) bit to toggle
     * Reads SR1 register from memory
     * @param mss_gpio_id_t device identifier
     */
    void writeReady(mss_gpio_id_t);


    /**
     * Writes a given number of bytes to the spi device; you may use this for partial page writing
     * @param uint32_t data : The content for the memory
     * @param uint32_t size : The number of bytes to be transfered
     * @param uint32_t address : The address of the page which have to be written
     * @param mss_gpio_id_t device : The device which will receive the data
     */
    void writeBytes(uint8_t *data, uint32_t size, uint32_t address,
            mss_gpio_id_t device);

    /**
     * @brief transmits a signal 32 bit value to the device by using partial page programming (Still 0x12 with less data)
     *
     * @param uint32_t value : The Value to be transmitted
     * @param uint32_t address : The address to be safe at
     * @param mss_gpio_id_t device : The memory device
     */
    void write32Bit(uint32_t value, uint32_t address, mss_gpio_id_t device);

    /**
     * FOR TESTING PURPOSE
     */
    uint32_t testMemory(void);

    /**
     *
     * @param uint32_t pageAddr: the address to be saved on page 0 - 0x199
     * @param uint32_t metaAddress the page  address of the current meta page
     * @param mss_gpio_id_t dev: the device to be safed on
     * @return uint32_t page address of the current page
     */
    uint32_t updateMetadata(uint32_t pageAddr, uint32_t metaAddress,
            mss_gpio_id_t dev);

};

#endif /* COMPONENTS_MEMORY_H_ */
