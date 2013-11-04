#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "chipmunk/chipmunk.h"

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

    // Now let's make a ball that falls onto the line and rolls off.
    // First we need to make a cpBody to hold the physical properties of the object.
    // These include the mass, position, velocity, angle, etc. of the object.
    // Then we attach collision shapes to the cpBody to give it a size and shape.

    cpFloat radius = 10;
    cpFloat mass = 5;

    // The moment of inertia is like mass for rotation
    // Use the cpMomentFor*() functions to help you approximate it.
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // The cpSpaceAdd*() functions return the thing that you are adding.
    // It's convenient to create and add an object in one line.
    cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPos(ballBody, cpv(150, 50));

    // Now we create the collision shape for the ball.
    // You can create multiple collision shapes that point to the same body.
    // They will all be attached to the body and move around to follow it.
    cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody,
                                         radius, cpvzero));
    cpShapeSetFriction(ballShape, 0.7);

    // Now that it's all set up, we simulate all the objects in the space by
    // stepping forward through time in small increments called steps.
    // It is *highly* recommended to use a fixed size time step.
    cpFloat timeStep = 1.0 / 40.0;
    cpFloat time;
    for(time = 0; time < 22; time += timeStep)
    {
        cpVect pos = cpBodyGetPos(ballBody);
        cpVect vel = cpBodyGetVel(ballBody);
        printf(
            "Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is \
            (%5.2f, %5.2f)\n",
            time, pos.x, pos.y, vel.x, vel.y
        );

        cpSpaceStep(space, timeStep);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 170, 206, 152));
        renderContainer(surface, container, 3);
        filledCircleColor(surface, pos.x, pos.y, 10, SDL_MapRGB(surface->format,
                          17, 206, 112));
        SDL_Flip(surface);
    }
    printf("%f", cpSegmentShapeGetA(container[0]).x);
    pause();

    // Clean up our objects and exit!
    cpShapeFree(ballShape);
    cpBodyFree(ballBody);
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

