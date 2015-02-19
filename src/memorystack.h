#ifndef MEMORYSTACK_H_
#define MEMORYSTACK_H_

#include <assert.h>
#include "types.h"

struct MemoryStack {
    bool32 isInitialized;
    uint64 stackSize;
    void *memoryPool;
    void *top;
};

/** initialises to zero before handing it out */
void* popMemoryStack(MemoryStack* memory, uint64 bytes);
void pushMemoryStack(MemoryStack* memory, uint64 bytes);

#endif // MEMORYSTACK_H_
