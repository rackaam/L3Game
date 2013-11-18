#ifndef CIRCLE_H_INCLUDED
#define CIRCLE_H_INCLUDED

#define TO_DEGREES 57.295779513

#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#include <glib.h>
#include <time.h>
#include "chipmunk/chipmunk.h"

typedef struct Circle Circle;
struct Circle
{
    char c[2];
    int color;
    int affected;
    cpBody* body;
    cpShape* shape;
    SDL_Surface* surface;
};

void circleInit();
void circleQuit();
void initCircle(Circle* circle, cpSpace* space, SDL_Surface* surface);
void freeCircle(void* circle);
void renderCircle(void* circle, void* surface);
void checkIfAffected(void* data, void* userData);
void resetAffected(void* data, void* userData);
void addIfAffected(void* data, void* userData);
void addChar(void* data, void* userData);

#endif // CIRCLE_H_INCLUDED
