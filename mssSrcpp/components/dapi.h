#ifndef COMPONENTS_DAPI_H_
#define COMPONENTS_DAPI_H_

#include <string>
#include <queue>


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
     * @param msg A null terminated C style string. Can be freed after this
     * function returns.
     * @return A Dapi instance for further use
     */
    Dapi &operator<< (const char * const msg);

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
        Message(uint8_t const * const ptr, const uint32_t size);

        /** Message object destructor
         *
         * Deletes the array behind ptr
         */
        ~Message();

        const uint8_t * ptr; /**< the start address of the array */

        const uint32_t size; /**< size of the array */
    };

    /** Constructor
     *
     * initializes the UART0 interface for DAPI usage
     */
    Dapi ();

    bool transferInProgress; /**< Indicates, if a transfer is
    in progress and if the next msgQueue item should be retained. */

    std::queue<Message> msgQueue; /**< A list of C style strings to be
    transmitted */
};


#endif /* COMPONENTS_DAPI_H_ */
