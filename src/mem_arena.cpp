#include "mem_arena.h"

#include <stdio.h>
#include <string.h>

void increaseStackPointer(MemoryArena* memory, u64 bytes) {
    memory->top = (char*)memory->top + bytes;
}


void decreaseStackPointer(MemoryArena* memory, u64 bytes) {
    if( (char*)memory->top - bytes < memory->memoryPool) {
        memory->top = memory->memoryPool;
        return;
    }
    memory->top = (char*)memory->top - bytes;
}


void* popMemoryArena(MemoryArena *memory, u64 bytes) {
    if( (char*)memory->top + bytes >=
        (char*)memory->memoryPool + memory->stackSize) {
        return NULL;
    }
    void* p = memory->top;
    memset( p, 0, bytes);
    printf( "MemoryArena: used %llu bytes\n", bytes);

    increaseStackPointer( memory, bytes);
    return p;
}


void pushMemoryArena(MemoryArena* memory, u64 bytes) {
    assert( (char*)memory->top - bytes >= memory->memoryPool && "Invalid MemoryArena free\n");

    memory->top = (char*)memory->top - bytes;

    printf( "MemoryArena: returned %llu bytes\n", bytes);
}
