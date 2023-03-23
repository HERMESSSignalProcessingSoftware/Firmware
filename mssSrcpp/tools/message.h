#ifndef TOOLS_MESSAGE_H_
#define TOOLS_MESSAGE_H_

#include <stdint.h>
#include <cstring>

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

    uint8_t *refCnt; /**< a counter counting the number
    of references to the pointer object */

    const uint32_t size; /**< size of the array */
};

#endif /* TOOLS_MESSAGE_H_ */
