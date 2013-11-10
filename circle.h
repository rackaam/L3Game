#ifndef CIRCLE_H_INCLUDED
#define CIRCLE_H_INCLUDED

#define TO_DEGREES 57.295779513

#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include "chipmunk/chipmunk.h"

extern int circlesNumber;

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
void freeCircle(Circle* circle);
void renderCircle(SDL_Surface* surface, Circle* circle);

#endif // CIRCLE_H_INCLUDED
