#ifndef COMPONENTS_DAPI_H_
#define COMPONENTS_DAPI_H_

#include <string>
#include "../sf2drivers/drivers/mss_uart/mss_uart.h"
#include "../tools/message.h"
#include "../tools/queue.h"
#include "../tools/datapackage.h"

// The maximum number of content bytes stored in the message buffer. Potentially
// exceeding this value on data storage will abort the addition of this data
// frame.
#define DAPI_MAX_BUFFER_SIZE 1024


class Dapi {
public:
    /** Singleton access method
     *
     * Use this method to get the Dapi instance
     * @return the instance
     */
    static Dapi &getInstance ();

    /** Worker function
     *
     * Must be called continuously in the main program loop to ensure all
     * transfers are sent and received.
     */
    void worker ();

    /** Raw data transfer function
     *
     * Enqueues a message for asynchronous transmission via the DAPI. Note that
     * infos, warnings and errors may creep into the transmission stream. It
     * is therefore highly recommended to command the transfer of one in itself
     * consistent unit and not many partial units.
     * @param msg A null terminated C style string. Can be freed after this
     * function returns.
     * @return A Dapi instance for further use
     */
    Dapi &transmitRaw (const uint8_t * const ptr, const uint32_t size);

    /**
     *
     * @param ptr
     * @param size
     * @return
     */
    Dapi &transmitRawPufferd (const uint8_t * const ptr, uint32_t size);

    /**
     *
     * @return
     */
    bool transmitPufferEmpty(void);

    /** String transfer operator
     *
     * Enqueues a message for asynchronous transmission via the DAPI. Note that
     * infos, warnings and errors may creep into the transmission stream. It
     * is therefore highly recommended to command the transfer of one in itself
     * consistent unit and not many partial units.
     * The null termination will not be qeued for transmission.
     * @param msg A null terminated C style string. Can be freed after this
     * function returns.
     * @return A Dapi instance for further use
     */
    Dapi &operator<< (const std::string msg);

    /** String transfer operator
     *
     * Enqueues a message for asynchronous transmission via the DAPI. Note that
     * infos, warnings and errors may creep into the transmission stream. It
     * is therefore highly recommended to command the transfer of one in itself
     * consistent unit and not many partial units.
     * The null termination will not be qeued for transmission.
     * @param msg A null terminated C style string. Can be freed after this
     * function returns.
     * @return A Dapi instance for further use
     */
    Dapi &operator<< (const char * const msg);

    /** Sends a live data acquisition frame
     *
     * Transmits a SICD frame to the groundstation software.
     * @param dp the datapackage to transmit
     * @return A Dapi instance for further use
     */
    Dapi &sendLiveData (const Datapackage &dp);

    /**
     *
     * @return boolean true: is empty
     */
    bool queueIsEmpty(void);
private:
    /**
     * UART RX receiver function
     *
     * Reads UART inputs into rxBuffer.
     * @param this_uart Ignored by this callback
     */
    static void rxHandler (mss_uart_instance_t *this_uart);

    /**
     * Read the memory until highAddress is reached.
     *
     * Starts with FLASH_CS1, continues with FLASH_CS2 with the same address
     * increases address after reading of FLASH_CS2
     *
     * Wraps the data with some meta data and transmits it via DAPI.
     *
     * @param highAddress uint32_t
     */
    void readAndTransmitMemory(uint32_t highAddress);
    /** Constructor
     *
     * initializes the UART0 interface for DAPI usage
     */
    Dapi ();

    bool transferInProgress = false; /**< Indicates, if a transfer is
    in progress and if the next msgQueue item should be retained. */

    Queue<Message> msgQueue; /**< A list of C style strings to be
    transmitted */

    uint8_t puffer[DAPI_MAX_BUFFER_SIZE]; /**< */
    uint32_t pufferSize = 0; /**< */

    uint32_t queueSize = 0; /**< Number of content bytes in the msgQueue */

    uint8_t rxBuffer[64]; /**< The contents read via UART */

    uint8_t rxBufferIdx = 0; /**< The index to start writing next incoming
    UART rx data into the rxBuffer */
};


#endif /* COMPONENTS_DAPI_H_ */
