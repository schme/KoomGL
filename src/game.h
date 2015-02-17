#ifndef GAME_H_
#define GAME_H_

#include "types.h"
#include "memorystack.h"

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
};

void gameInit( MemoryStack *);
void gameUpdateAndRender( GameInput);
void resize( int w, int h);

#endif // GAME_H_
