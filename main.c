#include <stdio.h>
#include <glib.h>
#include "circle.h"

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);
cpBool preSolve(cpArbiter *arb, cpSpace *space, void *data);
void selection(Circle* circles);

int circlesNumber = 0;

int main(void)
{
    cpVect mouse1 = cpv(-1, -1);
    cpVect mouse2 = cpv(-1, -1);
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

    Circle circles[10];
    int i;
    for(i = 0; i < 4; i++)
    {
        initCircle(&(circles[i]), space, surface);
    }

    cpFloat timeStep = 1.0 / 8.0;
    int run = 1;
    SDL_Event event;
    cpSpaceAddCollisionHandler(space, 0, 1, NULL, preSolve, NULL, NULL, circles);

    while (run)
    {
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                run = 0;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse1.x = event.button.x;
                mouse1.y = event.button.y;
                break;
            case SDL_MOUSEMOTION:
                if(((SDL_MouseMotionEvent*)&event)->state & SDL_BUTTON(1))
                {
                    drawLine = 1;
                    mouse2.x = event.button.x;
                    mouse2.y = event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                drawLine = 0;
                selection(circles);
                break;
            }
            if(event.key.keysym.sym == SDLK_ESCAPE)
            {
                run = 0;
            }
        }

        for(i = 0; i < circlesNumber; i++)
        {
            circles[i].affected = 0;
        }
        cpShape* mouseSeg = cpSegmentShapeNew(space->staticBody,
                                              cpv(mouse1.x, mouse1.y),
                                              cpv(mouse2.x, mouse2.y), 0);
        if(drawLine)
        {
            mouseSeg->collision_type = 1;
            cpSpaceAddShape(space, mouseSeg);
        }
        cpSpaceStep(space, timeStep);
        renderContainer(surface, container, 3);
        for(i = 0; i < circlesNumber; i++)
        {
            renderCircle(surface, &(circles[i]));
        }
        if(drawLine)
        {
            lineColor(surface, mouse1.x, mouse1.y,
                      mouse2.x, mouse2.y, 0x000000FF);
            cpSpaceRemoveShape(space, mouseSeg);
        }
        cpShapeFree(mouseSeg);
        SDL_Flip(surface);
        SDL_Delay(1000.0 / 60.0);
    }

    while(circlesNumber)
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

cpBool preSolve(cpArbiter *arb, cpSpace *space, void *data)
{
    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);
    int i;
    Circle* circles = (Circle*)data;
    for(i = 0; i < circlesNumber; i++)
    {
        if(circles[i].shape == a)
        {
            circles[i].affected = 1;
        }
    }

    return cpFalse; // Aucune collision avec ce segment
}

char firstRule(char string)
{
    int g,i,j,size=strlen(string);

    char research[20], research2[20];
    for( i=0;i<size;i++)
    {
        memset (research, 0, sizeof (research));
        memset (research2, 0, sizeof (research2));
        j=0;
        g=i;

        while(g<size)
        {
            research[j]=string[g];

            j=j+1;
            g=i+j;
        }

    //Teste si research est le mot recherché
        //if ( ->Fonction pour vérifier si le mot est dans le dico<-)
        //    return research;


        j=0;
        g=size-i-1;

        while(j<=g)
        {
            research2[g-j]=string[g-j];
            j=j+1;
        }

    //Teste si research2 est le mot recherché
        //if ( ->Fonction pour vérifier si le mot est dans le dico<-)
        //    return research2;


    }

}

void selection(Circle* circles)
{
    int i;
    char str[20];
    int charNB = 0;
    for(i = 0; i < circlesNumber; i++)
    {
        if(circles[i].affected)
        {
            str[charNB++] = circles[i].c;
        }
    }
    if(charNB)
    {
        // Call to function Rules1 !!!
        printf("Selected letters : %s\n", str);
        char wordFound[20]=firstRule(str);
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

