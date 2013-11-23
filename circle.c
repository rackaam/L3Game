#include "circle.h"

TTF_Font* font = NULL;
int colors[3];
int darkLayer;
SDL_Color fontColor = {255, 255, 255};

void initCircle(Circle* circle, cpSpace* space, SDL_Surface* surface)
{
    circle->c[0] = rand() % 26 + 97;
    circle->c[1] = '\0';

    cpFloat radius = 22;
    cpFloat mass = 2;
    circle->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, radius * 2 + 1,
                                           radius * 2, 32, 0xFF000000,
                                           0x00FF0000, 0x0000FF00, 0x000000FF);
    circle->color = colors[rand() % 3];

    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    circle->body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPos(circle->body, cpv(rand() % 360 + 120, 50));
    circle->shape = cpSpaceAddShape(space, cpCircleShapeNew(circle->body,
                                    radius, cpvzero));
    cpShapeSetFriction(circle->shape, 0.8);
    cpShapeSetElasticity(circle->shape, 0.9);
    circle->shape->collision_type = 0;
    circle->affected = 0;
}

void freeCircle(void* data)
{
    Circle* circle = (Circle*)data;
    cpShapeFree(circle->shape);
    cpBodyFree(circle->body);
    free(circle);
}

void renderCircle(void* data, void* surface)
{
    Circle* circle = (Circle*)data;
    cpFloat radius = cpCircleShapeGetRadius(circle->shape);
    SDL_Rect rect;
    cpVect pos = cpBodyGetPos(circle->body);

    filledCircleColor(circle->surface, radius, radius, radius, circle->color);

    if(circle->affected)
    {
        filledCircleColor(circle->surface, radius, radius, radius,
                          darkLayer);
    }
    SDL_Surface* c = TTF_RenderText_Blended(font, circle->c, fontColor);
    cpFloat angle = -cpBodyGetAngle(circle->body);
    SDL_Surface* rotatedSurface = rotozoomSurface(c,
                                  angle * TO_DEGREES, 1, SMOOTHING_ON);
    rect.x = (2 * radius - rotatedSurface->w) / 2;
    rect.y = (2 * radius - rotatedSurface->h) / 2;
    SDL_BlitSurface(rotatedSurface, NULL, circle->surface, &rect);
    rect.x = pos.x - radius;
    rect.y = pos.y - radius;
    SDL_BlitSurface(circle->surface, NULL, (SDL_Surface*)surface, &rect);
    SDL_FreeSurface(c);
    SDL_FreeSurface(rotatedSurface);
}

void checkIfAffected(void* data, void* userData)
{
    if(((Circle*)data)->shape == (cpShape*)userData)
    {
        ((Circle*)data)->affected = 1;
    }
}

void resetAffected(void* data, void* ignored)
{
    ((Circle*)data)->affected = 0;
}

void addIfAffected(void* data, void* userData)
{
    GSList** pp = (GSList**)userData;
    if(((Circle*)data)->affected)
    {
        *pp = g_slist_append((GSList*)*pp, (Circle*)data);
    }
}

void addChar(void* data, void* userData)
{
    strncat((char*)userData, ((Circle*)data)->c, 1);
}

void removeCircleFromSpace(cpSpace* space, Circle* circle)
{
    cpSpaceRemoveShape(space, circle->shape);
    cpSpaceRemoveBody(space, circle->body);
    //freeCircle(circle);
}

void circleInit()
{
    font = TTF_OpenFont("UbuntuMono-R.ttf", 32);
    SDL_Surface* s = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0xFF000000,
                                          0x00FF0000, 0x0000FF00, 0x000000FF);
    colors[0] = SDL_MapRGB(s->format, 255, 0, 0);
    colors[1] = SDL_MapRGB(s->format, 0, 255, 0);
    colors[2] = SDL_MapRGB(s->format, 0, 0, 255);
    darkLayer = SDL_MapRGBA(s->format, 0, 0, 0, 200);
    SDL_FreeSurface(s);
}

void circleQuit()
{
    TTF_CloseFont(font);
}
