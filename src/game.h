#ifndef GAME_H_
#define GAME_H_

#include "types.h"
#include "memorystack.h"

#include <glm/vec3.hpp>

#if BUILD_INTERNAL
// Debug versions only!
void *ReadFile( const char* filename);
void FreeMemory( void *Memory);
#endif

struct Camera {
    glm::vec3 pos;
};


struct GameInput {
    real64 deltaTime;
    uint64 frame;
    bool32 KEY_W;
    bool32 KEY_A;
    bool32 KEY_S;
    bool32 KEY_D;
    bool32 KEY_UP;
    bool32 KEY_LEFT;
    bool32 KEY_DOWN;
    bool32 KEY_RIGHT;
    bool32 KEY_ESC;
    bool32 KEY_SPACE;

    /**
     * MB 0: LBUTTON
     * MB 1: RBUTTON
     * MB 2: MBUTTON
     * MB 3: XBUTTON1
     * MB 4: XBUTTON4
     */
    bool32 MouseButtons[5];
    int32 MouseX; 
    int32 MouseY; 
    int32 MouseZ;
};

void gameInit( MemoryStack *);
void gameUpdateAndRender( GameInput);
void resize( int w, int h);

#endif // GAME_H_
