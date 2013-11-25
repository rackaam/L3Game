#include <stdio.h>
#include "circle.h"
#include "algo.h"

#define CIRCLES_NUMBER 40
#define VIDEO_RECORDING 0

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);
cpBool preSolve(cpArbiter *arb, cpSpace *space, void *data);
GSList* selection(GSList* liste, cpVect* startPos, GHashTable *hashtable, cpSpace* space);
gint sortFunction(gconstpointer a, gconstpointer b, gpointer startPos);

int main(void)
{
    GHashTable *hashTable = g_hash_table_new(g_str_hash, g_str_equal);
    char s[30];
    FILE* file = fopen("dico", "r");
    while(fscanf(file, "%s", s) == 1)
    {
        char* keyTemp = g_strdup(s);
        char* valTemp = g_strdup(s);
        g_hash_table_insert(hashTable, keyTemp, valTemp);
    }
    fclose(file);

    TTF_Init();
    circleInit();
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
    container[0] = cpSegmentShapeNew(space->staticBody, cpv(20, 10),
                                     cpv(70, 450), 0);
    cpShapeSetFriction(container[0], 0.5);
    cpShapeSetElasticity(container[0], 0.5);
    container[1] = cpSegmentShapeNew(space->staticBody, cpv(70, 450),
                                     cpv(530, 450), 0);
    cpShapeSetFriction(container[1], 0.5);
    cpShapeSetElasticity(container[1], 0.5);
    container[2] = cpSegmentShapeNew(space->staticBody, cpv(530, 450),
                                     cpv(580, 10), 0);
    cpShapeSetFriction(container[2], 0.5);
    cpShapeSetElasticity(container[2], 0.5);
    cpSpaceAddShape(space, container[0]);
    cpSpaceAddShape(space, container[1]);
    cpSpaceAddShape(space, container[2]);

    GSList* liste = NULL;
    int i;
    for(i = 0; i < CIRCLES_NUMBER; i++)
    {
        Circle* circle = malloc(sizeof(Circle));
        initCircle(circle, space, surface, i % 10);
        liste = g_slist_append(liste, circle );
    }

    cpFloat timeStep = 1.0 / 8.0;
    int run = 1;
    SDL_Event event;
    cpSpaceAddCollisionHandler(space, 0, 1, NULL, preSolve, NULL, NULL, &liste);
    int frameCounter = 0;
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
                liste = selection(liste, &mouse1, hashTable, space);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    run = 0;
                    break;
                default:
                    break;
                }
                break;
            }
        }

        g_slist_foreach(liste, resetAffected, NULL);

        cpShape* mouseSeg = NULL;
        if(drawLine)
        {
            mouseSeg = cpSegmentShapeNew(space->staticBody,
                                         cpv(mouse1.x, mouse1.y),
                                         cpv(mouse2.x, mouse2.y), 0);
            mouseSeg->collision_type = 1;
            cpSpaceAddShape(space, mouseSeg);
        }
        cpSpaceStep(space, timeStep);
        renderContainer(surface, container, 3);

        g_slist_foreach(liste, renderCircle, surface);

        if(drawLine)
        {
            lineColor(surface, mouse1.x, mouse1.y,
                      mouse2.x, mouse2.y, 0x000000FF);
            cpSpaceRemoveShape(space, mouseSeg);
            cpShapeFree(mouseSeg);
        }
        if(VIDEO_RECORDING)
        {
            char buf[20];
            sprintf(buf, "img/frame_%04d.bmp", frameCounter);
            SDL_SaveBMP(surface, buf);
            frameCounter++;
        }
        SDL_Flip(surface);
        SDL_Delay(1000.0 / 60.0);
    }

    g_slist_free_full(liste, freeCircle);
    cpSpaceFree(space);

    circleQuit();
    TTF_Quit();
    SDL_Quit();
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
    g_slist_foreach((*(GSList**)data), checkIfAffected, a);
    return cpFalse; // Aucune collision avec ce segment
}

GSList* selection(GSList* liste, cpVect* startPos, GHashTable *hashtable, cpSpace* space)
{
    GSList* circles = NULL;
    g_slist_foreach(liste, addIfAffected, &circles);
    circles = g_slist_sort_with_data(circles, sortFunction, startPos);
    char str[20] = "";
    g_slist_foreach(circles, addChar, str);
    if(strlen(str))
    {
        char* wordFound = NULL;
        wordFound = firstRule(str, hashtable);
        printf("Selected letters : %s\n", str);
        if(wordFound)
        {
            printf("Mot : %s\n", wordFound);
            char c;
            int i = 0;
            Circle* circle;
            GSList* elem;
            while((c = wordFound[i++]) != '\0')
            {
                elem = circles;
                while(elem)
                {
                    circle = (Circle*)elem->data;
                    if(circle->affected && circle->c[0] == c)
                    {
                        liste = g_slist_remove(liste, circle);
                        removeCircleFromSpace(space, circle);
                        elem = NULL;
                    }
                    else
                    {
                        elem = elem->next;
                    }
                }
            }
            free(wordFound);
        }
    }
    return liste;
}

gint sortFunction(gconstpointer a, gconstpointer b, gpointer startPos)
{
    cpVect* start = (cpVect*) startPos;
    cpVect vA = cpBodyGetPos(((Circle*)a)->body);
    cpVect vB = cpBodyGetPos(((Circle*)b)->body);
    return (int)(cpvdistsq(*start, vA) - cpvdistsq(*start, vB));
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

