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
    SPI_Values spiVal;
    mss_gpio_id_t CS_Pin; /*< */
    mss_spi_instance_t *handle; /*< */
    uint8_t memory[PAGESIZE]; /*< */
    uint32_t memoryCounter;
    bool write;
};

#endif /* COMPONENTS_MEMORY_H_ */
