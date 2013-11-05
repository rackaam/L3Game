#include <stdio.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "circle.h"

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);

int main(void)
{
    SDL_Init( SDL_INIT_VIDEO );

    SDL_Surface* surface = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE |
                                            SDL_DOUBLEBUF);
    if(surface == NULL)
    {
        fprintf(stderr, "Failed to load the video mode");
        exit(EXIT_FAILURE);
    }

    cpSpace* space = getSpace();

    cpShape* container[3];
    container[0] = cpSegmentShapeNew(space->staticBody, cpv(100, 150),
                                     cpv(200, 300), 0);
    container[1] = cpSegmentShapeNew(space->staticBody, cpv(200, 300),
                                     cpv(400, 300), 0);
    container[2] = cpSegmentShapeNew(space->staticBody, cpv(400, 300),
                                     cpv(500, 150), 0);
    cpSpaceAddShape(space, container[0]);
    cpSpaceAddShape(space, container[1]);
    cpSpaceAddShape(space, container[2]);

    Circle circle;
    initCircle(&circle, space, surface);
    // Now that it's all set up, we simulate all the objects in the space by
    // stepping forward through time in small increments called steps.
    // It is *highly* recommended to use a fixed size time step.
    cpFloat timeStep = 1.0 / 40.0;
    cpFloat time;
    for(time = 0; time < 22; time += timeStep)
    {
        cpVect pos = cpBodyGetPos(circle.body);
        cpVect vel = cpBodyGetVel(circle.body);
        printf(
            "Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is \
            (%5.2f, %5.2f)\n",
            time, pos.x, pos.y, vel.x, vel.y
        );
        cpSpaceStep(space, timeStep);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 170, 206, 152));
        renderContainer(surface, container, 3);
        filledCircleColor(surface, pos.x, pos.y, 22, circle.color);
        SDL_Flip(surface);
    }
    printf("%f", cpSegmentShapeGetA(container[0]).x);
    pause();

    // Clean up our objects and exit!
    freeCircle(&circle);
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
        thickLineColor(surface, a.x, a.y, b.x, b.y, 2,
                       SDL_MapRGB(surface->format, 17, 206, 112));
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

