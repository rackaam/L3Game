#include "ball.h"

int colors[3];
int darkLayer;
SDL_Color fontColor = {255, 255, 255};

void initBall(Ball* ball, cpSpace* space, SDL_Surface* surface, int line
              , char spawnsTab[], int tabLen)
{
    ball->c[0] = spawnsTab[rand() % tabLen];
    ball->c[1] = '\0';

    cpFloat radius = 22;
    cpFloat mass = 2;
    ball->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, radius * 2 + 1,
                                         radius * 2, 32, 0xFF000000,
                                         0x00FF0000, 0x0000FF00, 0x000000FF);
    ball->color = colors[rand() % 3];

    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    ball->body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPos(ball->body, cpv(rand() % 360 + 120, 50 - line * 24));
    ball->shape = cpSpaceAddShape(space, cpCircleShapeNew(ball->body,
                                  radius, cpvzero));
    cpShapeSetFriction(ball->shape, 0.8);
    cpShapeSetElasticity(ball->shape, 0.9);
    ball->shape->collision_type = 0;
    ball->affected = 0;
}

void freeBall(void* data)
{
    Ball* ball = (Ball*)data;
    cpShapeFree(ball->shape);
    cpBodyFree(ball->body);
    free(ball);
}

void renderBall(void* data, void* surface)
{
    Ball* ball = (Ball*)data;
    cpFloat radius = cpCircleShapeGetRadius(ball->shape);
    SDL_Rect rect;
    cpVect pos = cpBodyGetPos(ball->body);

    filledCircleColor(ball->surface, radius, radius, radius, ball->color);

    if(ball->affected)
    {
        filledCircleColor(ball->surface, radius, radius, radius,
                          darkLayer);
    }
    SDL_Surface* c = TTF_RenderText_Blended(font, ball->c, fontColor);
    if(ball->c[0] == 'b' || ball->c[0] == 'd' || ball->c[0] == 'p' ||
            ball->c[0] == 'q' || ball->c[0] == 'u' || ball->c[0] == 'n' ||
            ball->c[0] == 'w' || ball->c[0] == 'm')
    {
        boxColor(c, 0, c->h - 2, c->w, c->h, 0xFFFFFFFF);
    }
    cpFloat angle = -cpBodyGetAngle(ball->body);
    SDL_Surface* rotatedSurface = rotozoomSurface(c,
                                  angle * TO_DEGREES, 1, SMOOTHING_ON);
    rect.x = (2 * radius - rotatedSurface->w) / 2;
    rect.y = (2 * radius - rotatedSurface->h) / 2;
    SDL_BlitSurface(rotatedSurface, NULL, ball->surface, &rect);
    rect.x = pos.x - radius;
    rect.y = pos.y - radius;
    SDL_BlitSurface(ball->surface, NULL, (SDL_Surface*)surface, &rect);
    SDL_FreeSurface(c);
    SDL_FreeSurface(rotatedSurface);
}

void checkIfAffected(void* data, void* userData)
{
    if(((Ball*)data)->shape == (cpShape*)userData)
    {
        ((Ball*)data)->affected = 1;
    }
}

void resetAffected(void* data, void* ignored)
{
    ((Ball*)data)->affected = 0;
}

void addIfAffected(void* data, void* userData)
{
    GSList** pp = (GSList**)userData;
    if(((Ball*)data)->affected)
    {
        *pp = g_slist_append((GSList*)*pp, (Ball*)data);
    }
}

void addChar(void* data, void* userData)
{
    strncat((char*)userData, ((Ball*)data)->c, 1);
}

void removeBallFromSpace(cpSpace* space, Ball* ball)
{
    cpSpaceRemoveShape(space, ball->shape);
    cpSpaceRemoveBody(space, ball->body);
    freeBall(ball);
}

void ballInit()
{
    font = TTF_OpenFont("UbuntuMono-R.ttf", 32);
    if(font == NULL)
    {
        fprintf (stderr, "Erreur : Fichier de police introuvable (fichier \"UbuntuMono-R.ttf\")\n");
        exit(-3);
    }
    SDL_Surface* s = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0xFF000000,
                                          0x00FF0000, 0x0000FF00, 0x000000FF);
    colors[0] = SDL_MapRGB(s->format, 255, 0, 0);
    colors[1] = SDL_MapRGB(s->format, 0, 255, 0);
    colors[2] = SDL_MapRGB(s->format, 0, 0, 255);
    darkLayer = SDL_MapRGBA(s->format, 0, 0, 0, 200);
    SDL_FreeSurface(s);
}

void ballQuit()
{
    TTF_CloseFont(font);
}
