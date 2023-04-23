/*
 * message.cpp
 *
 *  Created on: 23.04.2023
 *      Author: RG
 */

#include "message.h"

Message::Message(const uint8_t *const ptr, const uint32_t size) :
        size { size }, ptr { nullptr }, refCnt { nullptr } {
    // get appropriately sized array and make string copy
    uint8_t *copy = new uint8_t[size];
    std::memcpy(copy, ptr, size);
    this->ptr = copy;
    // initialize memory counter
    refCnt = new uint8_t;
    *refCnt = 1;
}



Message::Message(const Message &other) :
        size { other.size }, ptr { other.ptr }, refCnt {other.refCnt} {
    // increment the reference counter
    (*refCnt)++;
}



Message::~Message() {
    // decrement refCnt
    (*refCnt)--;
    if (*refCnt == 0) {
        // delete array
        delete[] ptr;
    }
}



