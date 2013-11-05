#include <stdio.h>
#include "circle.h"

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);

int main(void)
{
    SDL_Init( SDL_INIT_VIDEO );

    SDL_Surface* surface = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE |
                                            SDL_DOUBLEBUF);
    if(surface == NULL)
    {
        fprintf(stderr, "Failed to load the video mode");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    cpSpace* space = getSpace();

    cpShape* container[3];
    container[0] = cpSegmentShapeNew(space->staticBody, cpv(100, 150),
                                     cpv(200, 300), 0);
    cpShapeSetFriction(container[0], 0.5);
    cpShapeSetElasticity(container[0], 0.5);
    container[1] = cpSegmentShapeNew(space->staticBody, cpv(200, 300),
                                     cpv(400, 300), 0);
    cpShapeSetFriction(container[1], 0.5);
    cpShapeSetElasticity(container[1], 0.5);
    container[2] = cpSegmentShapeNew(space->staticBody, cpv(400, 300),
                                     cpv(500, 150), 0);
    cpShapeSetFriction(container[2], 0.5);
    cpShapeSetElasticity(container[2], 0.5);
    cpSpaceAddShape(space, container[0]);
    cpSpaceAddShape(space, container[1]);
    cpSpaceAddShape(space, container[2]);

    Circle circles[4];
    int i;
    for(i = 0; i < 4; i++)
    {
        initCircle(&(circles[i]), space, surface);
    }

    cpFloat timeStep = 1.0 / 40.0;
    cpFloat time;
    for(time = 0; time < 22; time += timeStep)
    {
        cpSpaceStep(space, timeStep);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
        renderContainer(surface, container, 3);
        for(i = 0; i < 4; i++)
        {
            renderCircle(surface, &(circles[i]));
        }
        SDL_Flip(surface);
    }
    pause();

    // Clean up our objects and exit!
    for(i = 0; i < 4; i++)
    {
        freeCircle(&(circles[i]));
    }
    cpSpaceFree(space);

    return 0;
}

cpSpace* getSpace(void)
{
    cpSpace *space = cpSpaceNew();
    cpVect gravity = cpv(0, 10);
    cpSpaceSetGravity(space, gravity);
    return space;
}

void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape)
{
    int i;
    for(i = 0; i < nbShape; i++)
    {
        cpVect a = cpSegmentShapeGetA(container[i]);
        cpVect b = cpSegmentShapeGetB(container[i]);
        thickLineColor(surface, a.x, a.y, b.x, b.y, 2, 0x000099FF);
    }
}

void pause()
{
    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            continuer = 0;
        }
    }
}

