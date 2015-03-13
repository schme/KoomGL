#include "memorystack.h"

#include <stdio.h>
#include <string.h>

void increaseStackPointer(MemoryStack* memory, u64 bytes) {
    memory->top = (char*)memory->top + bytes;
}


void decreaseStackPointer(MemoryStack* memory, u64 bytes) {
    if( (char*)memory->top - bytes < memory->memoryPool) {
        memory->top = memory->memoryPool;
        return;
    }
    memory->top = (char*)memory->top - bytes;
}


void* popMemoryStack(MemoryStack *memory, u64 bytes) {
    if( (char*)memory->top + bytes >=
        (char*)memory->memoryPool + memory->stackSize) {
        return NULL;
    }
    void* p = memory->top;
    memset( p, 0, bytes);
    printf( "MemoryStack: used %llu bytes\n", bytes);

    increaseStackPointer( memory, bytes);
    return p;
}


void pushMemoryStack(MemoryStack* memory, u64 bytes) {
    assert( (char*)memory->top - bytes >= memory->memoryPool && "Invalid MemoryStack free\n");

    memory->top = (char*)memory->top - bytes;

    printf( "MemoryStack: returned %llu bytes\n", bytes);
}
