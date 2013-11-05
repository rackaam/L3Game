#include "circle.h"

void initCircle(Circle* circle, cpSpace* space, SDL_Surface* surface)
{
    circle->c = 'b';
    circle->color = SDL_MapRGB(surface->format, 170, 206, 112);
    cpFloat radius = 22;
    cpFloat mass = 5;

    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    circle->body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPos(circle->body, cpv(rand() % 500, 50));
    circle->shape = cpSpaceAddShape(space, cpCircleShapeNew(circle->body,
                                    radius, cpvzero));
    cpShapeSetFriction(circle->shape, 0);
}

void freeCircle(Circle* circle)
{
    cpShapeFree(circle->shape);
    cpBodyFree(circle->body);
}