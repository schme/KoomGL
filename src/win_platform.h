#ifndef WIN_PLATFORM_H
#define WIN_PLATFORM_H

#include "types.h"

// DEBUG ONLY
void FreeMemory( void *memory);
void* ReadFile(const char *filename, uint32* content_size);

#endif // WIN_PLATFORM_H
