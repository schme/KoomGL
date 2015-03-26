#ifndef MEMORYSTACK_H_
#define MEMORYSTACK_H_

#include <assert.h>
#include "types.h"

struct MemoryArena {
    b32 isInitialized;
    u64 stackSize;
    void *memoryPool;
    void *top;
};

/** initialises to zero before handing it out */
void* popMemoryArena(MemoryArena* memory, u64 bytes);
void pushMemoryArena(MemoryArena* memory, u64 bytes);

#endif // MEMORYSTACK_H_
