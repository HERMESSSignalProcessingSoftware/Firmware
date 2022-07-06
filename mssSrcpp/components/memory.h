#ifndef COMPONENTS_MEMORY_H_
#define COMPONENTS_MEMORY_H_

#include <stdint.h>
#include "../tools/datapackage.h"



class Memory {
public:
    static Memory &getInstance ();

    // Maybe check for clearing memory progression here?
    void worker ();

    /* Must be run async. Calling this function should be non-blocking.
    Also call Controller::clearMemFinished(), when finalized clearing
    the memory*/
    void clearMemory ();

    /* Fast prepare to save some datapackages */
    void abortClearMemory ();

    void saveDp (const Datapackage &dp);
private:
    Memory();
};

#endif /* COMPONENTS_MEMORY_H_ */
