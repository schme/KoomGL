#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef float r64;

typedef uint32_t b32;

#include "memorystack.h"


#define ArrCount(a) sizeof(a) / sizeof(*(a))

#define Kilobytes(x) (x) * 1024LL
#define Megabytes(x) (Kilobytes(x)*1024LL)
#define Gigabytes(x) (Megabytes(x)*1024LL)


const i32 windowWidth = 1280;
const i32 windowHeight = 720;

struct GameInput {
    r64 deltaTime;
    u64 frame;
    b32 KEY_W;
    b32 KEY_A;
    b32 KEY_S;
    b32 KEY_D;
    b32 KEY_UP;
    b32 KEY_LEFT;
    b32 KEY_DOWN;
    b32 KEY_RIGHT;
    b32 KEY_ESC;
    b32 KEY_SPACE;

    /**
     * MB 0: LBUTTON
     * MB 1: RBUTTON
     * MB 2: MBUTTON
     * MB 3: XBUTTON1
     * MB 4: XBUTTON4
     */
    b32 MouseButtons[5];
    i32 MouseX;
    i32 MouseY;
    i32 MouseZ;
};


void gameInit( MemoryStack *);
void gameUpdateAndRender( GameInput);
void resize( int w, int h);




// DEBUG ONLY
void* Read_File(const char *filename, u32* content_size);
void  Write_File( const char *filename, void* buffer, u32 buffer_size);
void  Free_Memory( void *memory);

#endif // PLATFORM_H_
