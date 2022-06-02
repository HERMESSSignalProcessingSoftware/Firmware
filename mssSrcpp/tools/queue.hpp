#ifndef TOOLS_QUEUE_HPP_
#define TOOLS_QUEUE_HPP_

#include "queue.h"

template <typename T> void Queue<T>::push (T item) {
    QueueElement *newPtr = new QueueElement(item);

    // append to existing list
    if (head) {
        QueueElement *curPtr = head;
        while (curPtr->next)
            curPtr = curPtr->next;
        curPtr->next = newPtr;
    }
    // start a new list
    else
        head = newPtr;
}



template <typename T> void Queue<T>::pop () {
    QueueElement *toDelete = head;
    head = head->next;
    delete toDelete;
}



template <typename T> T &Queue<T>::front () const {
    return head->content;
}



template <typename T> uint8_t Queue<T>::size () const {
    uint8_t counter = 0;
    QueueElement *curPtr = head;
    while (curPtr) {
        counter++;
        curPtr = curPtr->next;
    }
    return counter;
}



template <typename T> bool Queue<T>::empty () const {
    return !head;
}



template <typename T> Queue<T>::QueueElement::QueueElement (T item):
    content(item) {}

#endif
