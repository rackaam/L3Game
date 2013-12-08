#ifndef Ball_H_INCLUDED
#define Ball_H_INCLUDED

#define TO_DEGREES 57.295779513

#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#include <glib.h>
#include <time.h>
#include "chipmunk/chipmunk.h"

extern TTF_Font* font;

typedef struct Ball Ball;
struct Ball
{
    char c[2];
    int color;
    int affected;
    cpBody* body;
    cpShape* shape;
    SDL_Surface* surface;
};

void ballInit();
void ballQuit();
void initBall(Ball* ball, cpSpace* space, SDL_Surface* surface, int line
              , char spawnsTab[], int tabLen);
void freeBall(void* ball);
void renderBall(void* ball, void* surface);
void checkIfAffected(void* data, void* userData);
void resetAffected(void* data, void* userData);
void addIfAffected(void* data, void* userData);
void addChar(void* data, void* userData);
void removeBallFromSpace(cpSpace* space, Ball* ball);

#endif // Ball_H_INCLUDED
