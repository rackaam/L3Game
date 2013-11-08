#include <stdio.h>
#include <glib.h>
#include "circle.h"

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);

int main(void)
{
    cpVect mouseVect = cpv(-1, -1);
    cpVect mouseVect2;
    int drawLine = 0;
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

    cpFloat timeStep = 1.0 / 8.0;
    int run = 1;
    SDL_Event event;
    while (run)
    {
        cpSpaceStep(space, timeStep);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                run = 0;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseVect.x = event.button.x;
                mouseVect.y = event.button.y;
                printf("%f/%f\n", mouseVect.x, mouseVect.y);
                break;
            case SDL_MOUSEMOTION:
                if(((SDL_MouseMotionEvent*)&event)->state & SDL_BUTTON(1))
                {
                    drawLine = 1;
                    mouseVect2.x = event.button.x;
                    mouseVect2.y = event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                drawLine = 0;
                break;
            }
            if(event.key.keysym.sym == SDLK_ESCAPE)
            {
                run = 0;
            }
        }

        renderContainer(surface, container, 3);
        for(i = 0; i < 4; i++)
        {
            renderCircle(surface, &(circles[i]));
        }
        if(drawLine)
        {
            lineColor(surface, mouseVect.x, mouseVect.y,
                      mouseVect2.x, mouseVect2.y, 0x000000FF);
        }
        SDL_Flip(surface);
        SDL_Delay(1000.0 / 60.0);
    }

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
    int run = 1;
    SDL_Event event;

    while (run)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            run = 0;
        }
    }
}

