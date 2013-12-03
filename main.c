#include <stdio.h>
#include <limits.h>
#include "circle.h"
#include "algo.h"

#define CIRCLES_NUMBER 40
#define VIDEO_RECORDING 0

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);
void renderScore(SDL_Surface* surface);
cpBool preSolve(cpArbiter *arb, cpSpace *space, void *data);
GSList* selection(GSList* liste, cpVect* startPos, GHashTable *hashtable, cpSpace* space);
gint sortFunction(gconstpointer a, gconstpointer b, gpointer startPos);
void checkCharsDistribution(int count[]);

/*Global----------*/
int score = 0;
char displayedWord[30] = {0};
unsigned int displayedWordTime = UINT_MAX;
// extern circle.h
TTF_Font* font;
/*----------------*/

int main(void)
{
    int count[26] = {0};
    checkCharsDistribution(count);
    int i, j, k = 0;
    int spawnTabLen = 0;
    for(i = 0; i < 26; i++)
    {
        spawnTabLen += count[i];
    }
    char* spawnsTab = malloc(spawnTabLen * sizeof(char));
    for(i = 0; i < 26; i++)
    {
        for(j = 0; j < count[i]; j++)
        {
            spawnsTab[k++] = i + 'a';
        }
    }

    GHashTable *hashTable = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *anagramHashTable = g_hash_table_new(anagramHash, anagramEqual);
    char s[30];
    FILE* file = fopen("dico", "r");
    while(fscanf(file, "%s", s) == 1)
    {
        char* keyTemp = g_strdup(s);
        char* valTemp = g_strdup(s);
        g_hash_table_insert(hashTable, keyTemp, valTemp);
        g_hash_table_insert(anagramHashTable, keyTemp, valTemp);
    }
    fclose(file);

    TTF_Init();
    circleInit();
    cpVect mouse1 = cpv(-1, -1);
    cpVect mouse2 = cpv(-1, -1);
    int drawLine = 0;
    SDL_Color scoreTextColor = {0, 0, 0};
    SDL_Init( SDL_INIT_VIDEO );

    SDL_Surface* surface = SDL_SetVideoMode(640, 520, 32, SDL_HWSURFACE |
                                            SDL_DOUBLEBUF);
    if(surface == NULL)
    {
        fprintf(stderr, "Failed to load the video mode");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    cpSpace* space = getSpace();

    cpShape* container[3];
    container[0] = cpSegmentShapeNew(space->staticBody, cpv(20, 40),
                                     cpv(70, 450), 0);
    cpShapeSetFriction(container[0], 0.5);
    cpShapeSetElasticity(container[0], 0.5);
    container[1] = cpSegmentShapeNew(space->staticBody, cpv(70, 450),
                                     cpv(530, 450), 0);
    cpShapeSetFriction(container[1], 0.5);
    cpShapeSetElasticity(container[1], 0.5);
    container[2] = cpSegmentShapeNew(space->staticBody, cpv(530, 450),
                                     cpv(580, 40), 0);
    cpShapeSetFriction(container[2], 0.5);
    cpShapeSetElasticity(container[2], 0.5);
    cpSpaceAddShape(space, container[0]);
    cpSpaceAddShape(space, container[1]);
    cpSpaceAddShape(space, container[2]);

    GSList* liste = NULL;
    for(i = 0; i < CIRCLES_NUMBER; i++)
    {
        Circle* circle = malloc(sizeof(Circle));
        initCircle(circle, space, surface, i % 10, spawnsTab, spawnTabLen);
        liste = g_slist_append(liste, circle );
    }

    SDL_Rect rect0;
    SDL_Rect rect1;
    rect0.x = 0;
    rect0.y = 0;
    rect1.x = 0;
    rect1.y = 480;
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
                //liste = selection(liste, &mouse1, anagramHashTable, space);
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
        char buff[15];
        sprintf(buff, "Score : %d", score);
        SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, buff, scoreTextColor);
        SDL_BlitSurface(scoreSurface, NULL, surface, &rect0);
        if(time(NULL) - displayedWordTime < 5)
        {
            SDL_Surface* wordSurface = TTF_RenderText_Blended(font, displayedWord, scoreTextColor);
            SDL_BlitSurface(wordSurface, NULL, surface, &rect1);
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
        printf("Selected letters : %s\n", str);
        char* wordFound = NULL;
        wordFound = firstRule(str, hashtable);
        //wordFound = secondRule(str, hashtable);
        //wordFound = thirdRule(str, hashtable);
        if(wordFound)
        {
            strcpy(displayedWord, wordFound);
            displayedWordTime = time(NULL);
            score += strlen(wordFound);
            printf("Score: %d\n", score);
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

void checkCharsDistribution(int count[])
{
    FILE* file = fopen("dico", "r");
    char c;
    unsigned int total = 0;
    while((c = fgetc(file)) != EOF)
    {
        if(c > 96 && c < 123)
        {
            count[c - 'a']++;
            total++;
        }
    }
    fclose(file);
    total /= 100;
    int i;
    for(i = 0; i < 26; i++)
    {
        count[i] /= total;
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

