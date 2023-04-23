#ifndef COMPONENTS_MEMORY_H_
#define COMPONENTS_MEMORY_H_

#include <stdint.h>
#include "../tools/datapackage.h"
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"

/** Memory Commands
 * typedef enum based implementation of commands for the S25FL512S
 */
typedef enum {
    c_WRDI = 0x04, /**< c_WRDI command write disable */
    c_READSTATUSREG1 = 0x05, /**< c_READSTATUSREG1 command read status register 1 */
    c_WREN = 0x06, /**< c_WREN command write enable*/
    c_READSTATUSREG2 = 0x07, /**< c_READSTATUSREG2 command read status register 2*/
    c_WRITEPAGE = 0x12, /**< c_WRITEPAGE command write page*/
    c_READ = 0x13, /**< c_READ command read page needs to be followed by 4 bytes of address and 512 bytes of nothing to transmit the data to the controller*/
    c_CLEARSTATUSREG1 = 0x30,/**< c_CLEARSTATUSREG1 command clear status register 1 */
    c_READCONFIGREG1 = 0x35, /**< c_READCONFIGREG1 command read config register 1*/
    c_READID = 0x9F, /**< c_READID command read the id of the device*/
    c_CE = 0xC7 /**< c_CE command chip erase. */
} SPI_MemoryCommand_t;

/**
 *
 */
typedef enum {
    InterfaceOne, /**< InterfaceOne */
    InterfaceTwo, /**< InterfaceTwo */
    InterfaceMeta /**< InterfaceMeta */
} ActiveInterface_t;


#define PAGEADDR(i)         (i << 9)
#define PAGESIZE            512
#define PAGE_COUNT          125000
#define DATASETS_PER_PAGE   8
#define METADATA_PAGEADDR   0x0

class MemorySPI {
public:
    /** MemorySPI default constructor
     *
     */
    MemorySPI();

    /** MemorySPI copy constructor
     *
     *  MemorySPI p = objectOld;
     * @param
     */
    MemorySPI(const MemorySPI&);

    /** MemorySPI parametersized constructor
     *
     * @param pin GPIO_PORT(PIN)
     * @param handle &g_mss_spiN
     * @param addr some unsigned 32 bit
     */
    MemorySPI(mss_gpio_id_t pin, mss_spi_instance_t *handle, uint32_t addr);

    /** Destructor
     *
     */
    ~MemorySPI();

    /** Set CS Pin function
     * sets the new chip select pin
     * @param pin GPIO_PORT(pin) of the mss_gpio_id_t
     */
    void setCSPin(mss_gpio_id_t pin);

    /** Set SPI handle function
     *
     * @param handle g_mss_spiN of mss_spi_instance_t
     */
    void setSPIHandle(mss_spi_instance_t *handle);

    /** Get SPI handle function
     *
     * @return used handle in this object
     */
    mss_spi_instance_t* getSPIHandle(void);

    /** Get CS Pin function
     *
     * @return mss_gpio_id_t of the used CS pin
     */
    mss_gpio_id_t getCSPin(void);

    /** Read status function
     *
     * @return uint8_t status register 1 of the device
     */
    uint8_t readStatus(void);

    /** Write Byte function
     * writes one byte to the device
     * @param data uint8_t the byte to be written to the device
     */
    void writeByte(uint8_t data);

    /** Write page function
     * writes a total amount of 512 bytes (PAGESIZE) to the device.
     * @param data the array to be written.
     * @return the number of bytes written to the device
     */
    uint32_t writePage(uint8_t *data);

    /** Read page function
     * reads a page from the device
     * @param data the array, of 512 (PAGESIZE) bytes, for the received data.
     * @param address uint32_t the address of the memory device.
     */
    void readPage(uint8_t *data, uint32_t addr);

    /** Chip Erase function
     *  Erases both memory controllers.
     */
    void chipErase(void);

    /** Write ready function
     * If blocking is set to true the function will not return until the SR1[1] bit returned to 0
     * @param blocking
     */
    bool writeReady(bool blocking);

    /** Get address function
     *
     * @return unsigned 32 bit value of the address used in this devive
     */
    uint32_t getAddress(void);

    /** Set address function
     *
     * @param addr uint32_t value of the address to be used in future
     */
    void setAddress(uint32_t addr);

    /** Increase address function
     *
     * performs this->address++;
     */
    void increaseAddress(void);

    /** Decreae address function
     *
     * performs this->address--;
     */
    void decreaseAddress(void);

private:
    mss_gpio_id_t CSPin;            /**<! The chip select pin set with GPIO_PORT(pin)*/
    mss_spi_instance_t *spihandle;  /**<! The spi instance used for memory functions*/
    uint32_t address;               /**<! The start address*/
    uint32_t pagesWritten;          /**<! Number of pages written to the device*/
};


class Memory {
public:
    /** Singleton object get instance function
     *
     * @return instance of this class
     */
    static Memory& getInstance();

    /** Worker function
     *
     * writes the measurment data to the memory
     */
    void worker();

    /** Clear memory function
     *
     *  calls Controller::clearMemFinished(), when finalized clearing the memory
     */
    void clearMemory();

    /** Abort clear memory
     *  Fast preparing to store some data packages.
     *
     *  NOT IMPLEMENTED
     */
    void abortClearMemory();

    /** Save datapackge function
     * Called from controller / measurment
     *
     * Adds the datapackages to the memory internal memory.
     * @param dp
     */
    void saveDp(const Datapackage &dp);

    /** Metadata highest address function
     * Searches the highest entry in the pages form 0 to 0x200
     *
     * At first it looks for the first unwritten entry (0xFFFFFFFF)
     * Second the index will be decreased by one to return the last saved address.
     * @return uint32_t address
     */
    uint32_t metaDataHighestAddress(void);

    /** Update metadata function
     *
     * Updates the value of interfaceOne to the flash device connected to flash_cs1
     */
    void updateMetadata(void);

    /** Recovery function
     *
     * Reads the metadata on the device to set the right address after power loss and reboot.
     */
    void recovery(void);

    /** Memory status function
     *
     * Reads the value of the status register 1 from both memory controllers.
     * FCS1_SR1 << 8 | FCS2_SR1
     *
     * @return 32 bit value of the status registers
     */
    uint32_t memoryStatus(void);

    /** Dump memory function
     *
     *
     *
     * @param startAddr 32bit unsigned value
     * @param endAddr   32bit unsigned value
     */
    void dumpMemory(uint32_t startAddr, uint32_t endAddr);

    /** Dump in progress function
     *
     * @return bool True: if dump is in progress.
     */
    bool dumpInProgress(void);
private:
    const uint32_t PageSize; /*!< The size of one page, usually 512 bytes*/
    const uint32_t PageCount; /*!< Total number of pages on one flash device*/
    const uint32_t PageAddressShift; /*!< Normally 1 << 9 is the address shift per page*/
    const uint32_t DatasetsPerPage; /*!< Datasets per page: 9.14 -> 9 */
    MemorySPI interfaceOne; /*!< Memory interface connected to the first memory device*/
    MemorySPI interfaceTwo; /*!< Memory interface connected to the second memory device*/
    MemorySPI metaInterface; /*!< Memory interface connected to the meta data device, in this case same device as interfaceOne*/
    ActiveInterface_t activeInterface; /*!< Indicating the active device to the worker function. Starting with interfaceOne, continuing with interfaceTwo*/
    uint8_t memory[PAGESIZE]; /*!< Buffer to store the data packages until the page is full*/
    uint32_t savedDataPoints; /*!< Counter for all datapoints*/
    bool dumpInProgressVar; /*!< True, if memory dump is requested*/
    uint32_t endAddr; /*!< Stores the end address of the memory dump*/
    Memory();
};

#endif /* COMPONENTS_MEMORY_H_ */
