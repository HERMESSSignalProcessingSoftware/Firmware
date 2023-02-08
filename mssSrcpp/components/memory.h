#ifndef COMPONENTS_MEMORY_H_
#define COMPONENTS_MEMORY_H_

#include <stdint.h>
#include "../tools/datapackage.h"
#include "../sf2drivers/drivers/mss_spi/mss_spi.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"

typedef enum {
    c_WRDI = 0x04,
    c_READSTATUSREG1 = 0x05,
    c_WREN = 0x06,
    c_READSTATUSREG2 = 0x07,
    c_WRITEPAGE = 0x12,
    c_READ = 0x13,
    c_CLEARSTATUSREG1 = 0x30,
    c_READCONFIGREG1 = 0x35,
    c_READID = 0x9F,
    c_CE = 0xC7
} SPI_MemoryCommand_t;

typedef enum { InterfaceOne, InterfaceTwo} ActiveInterface_t;

#define PAGEADDR(i)         (i << 9)
#define PAGESIZE            512
#define PAGE_COUNT          125000
#define DATASETS_PER_PAGE   8
#define METADATA_PAGEADDR   0x0

class MemorySPI {
public:
    /**
     *
     */
    MemorySPI();

    /**
     *
     * @param
     */
    MemorySPI(const MemorySPI&);
    /**
     *
     * @param pin
     * @param handle
     */
    MemorySPI(mss_gpio_id_t pin, mss_spi_instance_t *handle);

    /**
     *
     */
    ~MemorySPI();

    /**
     *
     * @param pin
     */
    void setCSPin(mss_gpio_id_t pin);

    /**
     *
     * @param handle
     */
    void setSPIHandle(mss_spi_instance_t *handle);
    /**
     *
     * @return
     */
    mss_spi_instance_t* getSPIHandle(void);

    /**
     *
     * @return
     */
    mss_gpio_id_t getCSPin(void);

    /**
     *
     * @return
     */
    uint8_t readStatus(void);

    /**
     *
     * @param data
     * @return bool
     */
    void writeByte(uint8_t data);

    /**
     *
     * @param data
     * @param address
     * @return
     */
    uint32_t writePage(uint8_t *data, uint32_t address);

    /**
     *
     * @param data
     * @param address
     */
    void readPage(uint8_t *data, uint32_t address);

    /**
     *
     */
    void chipErase(void);

    /**
     *
     * @param blocking
     */
    bool writeReady(bool blocking);
private:
    mss_gpio_id_t CSPin;
    mss_spi_instance_t *spihandle;
};

// Remove after finishing implementation of memory in SPI version 131
typedef struct {
    mss_spi_instance_t *spihandle;
    uint32_t CS_Pin;
} SPI_Values;
/*END*/

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
    const uint32_t PageSize;
    const uint32_t PageCount;
    const uint32_t PageAddressShift;
    const uint32_t DatasetsPerPage;
    MemorySPI interfaceOne;
    MemorySPI interfaceTwo;
    ActiveInterface_t activeInterface;
    Memory();
};

#endif /* COMPONENTS_MEMORY_H_ */
