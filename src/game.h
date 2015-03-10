#ifndef GAME_H_
#define GAME_H_

#include "ks_math.h"
#include "memorystack.h"

#if BUILD_INTERNAL
// Debug versions only!
void *ReadFile( const char* filename);
void FreeMemory( void *Memory);
#endif


struct Camera {
    vec3 pos;    
};


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

#endif // GAME_H_
