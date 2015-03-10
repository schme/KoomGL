#ifndef MEMORYSTACK_H_
#define MEMORYSTACK_H_

#include <assert.h>
#include "types.h"

struct MemoryStack {
    b32 isInitialized;
    u64 stackSize;
    void *memoryPool;
    void *top;
};

/** initialises to zero before handing it out */
void* popMemoryStack(MemoryStack* memory, u64 bytes);
void pushMemoryStack(MemoryStack* memory, u64 bytes);

#endif // MEMORYSTACK_H_
