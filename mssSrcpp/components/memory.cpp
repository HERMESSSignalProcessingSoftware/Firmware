#include "memory.h"



Memory &Memory::getInstance () {
    static Memory instance;
    return instance;
}



void Memory::worker () {}



void Memory::clearMemory () {}



void Memory::abortClearMemory () {}



void Memory::saveDp (const Datapackage &dp) {}



Memory::Memory() {
}

