#ifndef COMPONENTS_TM_H_
#define COMPONENTS_TM_H_

#include <stdint.h>
#include <string>
#include "../tools/queue.h"
#include "../model/telemetry.h"
// The maximum number of content bytes stored in the message buffer. Potentially
// exceeding this value on data storage will abort the addition of this data
// frame.
#define TM_MAX_BUFFER_SIZE 2000

#define TELEMETRY_STATUS_INTERRUPT_TX     (1 << 31)
#define TELEMETRY_STATUS_INTERRUPT_RX     (1 << 30)

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

    /** Interrupt based worker method
     *
     * Run this method every time the Tm fabric completed a transmission
     * load most current data to the fabric.
     */
    void worker_irq(void);

    /** Clear Interrupt
     *
     * Call this function to clear the interrupt pending bit of the status register
     *
     * @param interrupt
     */
    void clearInterrupt(uint32_t interrupt);

    /**
     * Sets the CONFIG_START (1 << 1) bit in the config register
     *
     * To automatically start a transistion, you need to enable CONFIG_AUTO_START (1 << 2) in the config register
     */
    void telemetryTransmissionStart(void);

    /**
     * Removes the CONFIG_START and the CONFIG_AUTO_START bit from the config register
     */
    void telemetryTransmissionStop(void);

    /** Text message transfer operator
     *
     * Enqueues a text message for asynchronous transmission via the TM. It
     * is highly recommended to command the transfer of one in itself
     * consistent unit and not many partial units.
     * @param msg The message string to be transmitted including the
     * trailing ASCII '0', '1' or '2' */
    Tm &operator<< (std::string msg);

private:
    uint8_t frameId = 0; /**< A wrapping id of the number of frames
    sent by the TM component for the GSS to detect missing packages */

    uint64_t sendingTimestamp = 0; /**< The timestamp at the beginning of
    the timestamp sending process */

    uint8_t sendingTimestampByte = 5; /**< Number of bytes of the timestamp
    already sent */

    Queue<std::string> msgQueue; /**< The queue containing all messages
    to be sent via TM. The front message is potentially partially
    transmitted already as indicated by sentBytes. */

    Queue<HERMESS::TelemetryData> dataQueue; /**< The queue containing all data messages send via TM. */

    uint32_t msgQueueSize = 0; /**< The number of content bytes in msgQueue */

    uint32_t dataQueueSize = 0; /**< The number of data bytes in the dataQueue*/

    uint32_t sentMsgBytes = 0; /**< The number of bytes already sent from the
    front message */

    /** Telemetry class constructor
     *
     * Initializes Telemetry fabric interface and sets the always common transmission
     * bytes
     */
    Tm ();
};

#endif
