#ifndef TOOLS_QUEUE_H_
#define TOOLS_QUEUE_H_

#include <stdint.h>


/** Small FIFO Queue
 *
 * Not more than 255 enqueued items allowed. This is an alternative to
 * the not functional std::queue STL.
 * @tparam T The type to use
 */
template <typename T> class Queue {
public:
    void push (T item);

    /** Remove first element
     *
     * Only call this function, when it is certain that the queue contains
     * at least one item!
     */
    void pop ();

    /** Returns reference to first element
     *
     * Only call this function, when it is certain that the queue contains
     * at least one item!
     */
    T &front () const;

    /** Get the number of elements
     *
     * @return The number of enqueued elements
     */
    uint8_t size () const;

    /** Check, if queue is empty
     *
     * @return True, if no enqueued elements. False otherwise.
     */
    bool empty () const;

private:
    /** Internal class for storing elements
     *
     * @tparam T See Queue<T>
     */
    struct QueueElement {
        QueueElement *next = nullptr;
        T content;
        QueueElement (T item);
    };

    QueueElement *head = nullptr; /**< The head of the list and first item
    to output */
};


#include "queue.hpp"

#endif
