#ifndef CIRCLE_H_INCLUDED
#define CIRCLE_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <time.h>
#include "chipmunk/chipmunk.h"

typedef struct Circle Circle;
struct Circle
{
    char c;
    int color;
    cpBody* body;
    cpShape* shape;
};

void initCircle(Circle* circle, cpSpace* space, SDL_Surface* surface);
void freeCircle(Circle* circle);
void renderCircle(SDL_Surface* surface, Circle* circle);

#endif // CIRCLE_H_INCLUDED
