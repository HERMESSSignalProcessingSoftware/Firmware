#ifndef COMPONENTS_DAPI_H_
#define COMPONENTS_DAPI_H_

#include <string>
#include "../sf2drivers/drivers/mss_uart/mss_uart.h"
#include "../tools/queue.h"
#include "../tools/datapackage.h"

// The maximum number of content bytes stored in the message buffer. Potentially
// exceeding this value on data storage will abort the addition of this data
// frame.
#define DAPI_MAX_BUFFER_SIZE 1000


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

private:
    /** Message container class
     *
     * An internal class for RAII management of message arrays
     */
    struct Message {
        /** Message object constructor
         *
         * Copies the entire message to be transmitted into a self
         * managed memory area
         * @param ptr the starting address
         * @param size the size to be transmitted in bytes
         */
        Message (const uint8_t * const ptr, const uint32_t size);

        /** Copy constructor
         *
         * Also copies the underlying string
         * @param other The object to copy
         */
        Message (const Message &other);

        /** Message object destructor
         *
         * Deletes the array behind ptr
         */
        ~Message ();

        const uint8_t *ptr; /**< the start address of the array */

        const uint32_t size; /**< size of the array */
    };

    /**
     * UART RX receiver function
     *
     * Reads UART inputs into rxBuffer.
     * @param this_uart Ignored by this callback
     */
    static void rxHandler (mss_uart_instance_t *this_uart);

    /** Constructor
     *
     * initializes the UART0 interface for DAPI usage
     */
    Dapi ();

    bool transferInProgress = false; /**< Indicates, if a transfer is
    in progress and if the next msgQueue item should be retained. */

    Queue<Message> msgQueue; /**< A list of C style strings to be
    transmitted */

    uint32_t queueSize = 0; /**< Number of content bytes in the msgQueue */

    uint8_t rxBuffer[64]; /**< The contents read via UART */

    uint8_t rxBufferIdx = 0; /**< The index to start writing next incoming
    UART rx data into the rxBuffer */
};


#endif /* COMPONENTS_DAPI_H_ */
