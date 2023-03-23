#ifndef COMPONENTS_TM_H_
#define COMPONENTS_TM_H_

#include <stdint.h>
#include <string>
#include "../tools/queue.h"

// The maximum number of content bytes stored in the message buffer. Potentially
// exceeding this value on data storage will abort the addition of this data
// frame.
#define TM_MAX_BUFFER_SIZE 200



/** Telemetry class
 *
 * This class enqueues the messages for telemetry downlink and performs
 * the transmission
 */
class Tm {
public:
    /** Singleton access method
     *
     * Use this method to get the Dapi instance
     * @return the instance
     */
    static Tm &getInstance ();

    /** Worker method
     *
     * Run this method twice a second. This worker does
     * schedule a new TM transmission with the most current data.
     */
    void worker2Hz ();

    /** Text message transfer operator
     *
     * Enqueues a text message for asynchronous transmission via the TM. It
     * is highly recommended to command the transfer of one in itself
     * consistent unit and not many partial units.
     * @param msg The message string to be transmitted including the
     * trailing ASCII '0', '1' or '2' */
    Tm &operator<< (std::string msg);

private:
    uint8_t txBuffer[64]; /**< The transmission buffer handed over to the
    UART interface */

    uint8_t frameId = 0; /**< A wrapping id of the number of frames
    sent by the TM component for the GSS to detect missing packages */

    uint64_t sendingTimestamp = 0; /**< The timestamp at the beginning of
    the timestamp sending process */

    uint8_t sendingTimestampByte = 5; /**< Number of bytes of the timestamp
    already sent */

    Queue<std::string> msgQueue; /**< The queue containing all messages
    to be sent via TM. The front message is potentially partially
    transmitted already as indicated by sentBytes. */

    uint32_t queueSize = 0; /**< The number of content bytes in msgQueue */

    uint32_t sentMsgBytes = 0; /**< The number of bytes already sent from the
    front message */

    /** Telemetry class constructor
     *
     * Initializes UART 1 interface and sets the always common transmission
     * bytes
     */
    Tm ();
};

#endif
