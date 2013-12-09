#include <stdio.h>
#include <limits.h>
#include "ball.h"
#include "algo.h"

void pause();
cpSpace* getSpace(void);
void renderContainer(SDL_Surface* surface, cpShape** container, int nbShape);
void renderScore(SDL_Surface* surface);
cpBool preSolve(cpArbiter *arb, cpSpace *space, void *data);
GSList* checkSelection(GSList* liste, cpVect* startPos, char * (*ruleFunction)(char*, GHashTable*), GHashTable *hashtable, cpSpace* space);
gint sortFunction(gconstpointer a, gconstpointer b, gpointer startPos);
void checkCharsDistribution(int count[], char* fileName);

/*Global----------*/
int score = 0;
char displayedWord[36] = {0};
unsigned int displayedWordTime = UINT_MAX;
// extern ball.h
TTF_Font* font;
/*----------------*/

int main(int argc, char* argv[])
{
    int i, j, k = 0;
    char rule = 1;
    int videoRecording = 0;
    int ballsNb = 40;
    char* (*ruleFunction)(char*, GHashTable*);

    /* Gestion des arguments */
    for(i = 1; i < argc; i++)
    {
        char *key, *value;
        key = strtok(argv[i], "=");
        value = strtok (NULL, " ,.-");
        if(!strcmp(key, "rule"))
        {
            rule = atoi(value);
        }
        else if(!strcmp(key, "record"))
        {
            videoRecording = atoi(value);
        }
        else if(!strcmp(key, "balls"))
        {
            ballsNb = atoi(value);
        }
    }

    printf("video: %d\n", videoRecording);
    printf("balls: %d\n", ballsNb);
    printf("rule: %d\n", rule);
    printf("-----------\n");

    /* Proportion de chaque lettre dans le dico */
    int count[26] = {0};
    checkCharsDistribution(count, "dico");
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

    /* Création et remplissage de la hashtable */
    GHashTable *hashTable = NULL;
    if(rule == 1)
    {
        hashTable = g_hash_table_new(g_str_hash, g_str_equal);
        ruleFunction = firstRule;
    }
    else if(rule == 2)
    {
        hashTable = g_hash_table_new(g_str_hash, g_str_equal);
        ruleFunction = secondRule;
    }
    else if(rule == 3)
    {
        hashTable = g_hash_table_new(anagramHash, anagramEqual);
        ruleFunction = thirdRule;
    }
    else
    {
        // Règle 1 par défaut
        hashTable = g_hash_table_new(g_str_hash, g_str_equal);
        ruleFunction = firstRule;
    }

    char s[30];
    FILE* file = fopen("dico", "r");
    while(fscanf(file, "%s", s) == 1)
    {
        char* keyTemp = g_strdup(s);
        char* valTemp = g_strdup(s);
        g_hash_table_insert(hashTable, keyTemp, valTemp);
    }
    fclose(file);

    /* Initialisation de la partie graphique */
    TTF_Init();
    ballInit();
    cpVect mouse1 = cpv(-1, -1);
    cpVect mouse2 = cpv(-1, -1);
    int mouseSeg = 0;
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

    /* Création du recipient */
    cpShape* container[3];
    container[0] = cpSegmentShapeNew(space->staticBody, cpv(20, 40),
                                     cpv(70, 450), 0);
    cpShapeSetFriction(container[0], 0.5);
    cpShapeSetElasticity(container[0], 0.5);
    container[1] = cpSegmentShapeNew(space->staticBody, cpv(70, 450),
                                     cpv(570, 450), 0);
    cpShapeSetFriction(container[1], 0.5);
    cpShapeSetElasticity(container[1], 0.5);
    container[2] = cpSegmentShapeNew(space->staticBody, cpv(570, 450),
                                     cpv(620, 40), 0);
    cpShapeSetFriction(container[2], 0.5);
    cpShapeSetElasticity(container[2], 0.5);
    cpSpaceAddShape(space, container[0]);
    cpSpaceAddShape(space, container[1]);
    cpSpaceAddShape(space, container[2]);

    /* Création des balles */
    GSList* liste = NULL;
    for(i = 0; i < ballsNb; i++)
    {
        Ball* ball = malloc(sizeof(Ball));
        initBall(ball, space, surface, i % 10, spawnsTab, spawnTabLen);
        liste = g_slist_append(liste, ball );
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
        /* RAZ de la surface */
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));

        /* Traitement des évenements (non bloquant) */
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
                    mouseSeg = 1;
                    mouse2.x = event.button.x;
                    mouse2.y = event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                mouseSeg = 0;
                liste = checkSelection(liste, &mouse1, ruleFunction, hashTable, space);
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

        cpShape* mouseSegShape = NULL;

        /* Création du segment pour les collisions */
        if(mouseSeg)
        {
            mouseSegShape = cpSegmentShapeNew(space->staticBody,
                                              cpv(mouse1.x, mouse1.y),
                                              cpv(mouse2.x, mouse2.y), 0);
            mouseSegShape->collision_type = 1;
            cpSpaceAddShape(space, mouseSegShape);
        }

        // Simulation
        cpSpaceStep(space, timeStep);

        // Dessin du récipient
        renderContainer(surface, container, 3);

        // Dessin de chaque cercle
        g_slist_foreach(liste, renderBall, surface);

        // Dessin du segment
        if(mouseSeg)
        {
            lineColor(surface, mouse1.x, mouse1.y,
                      mouse2.x, mouse2.y, 0x000000FF);
            cpSpaceRemoveShape(space, mouseSegShape);
            cpShapeFree(mouseSegShape);
        }

        // Affichage du score
        char buff[15];
        sprintf(buff, "Score : %d", score);
        SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, buff, scoreTextColor);
        SDL_BlitSurface(scoreSurface, NULL, surface, &rect0);

        // Affichage du dernier mot trouvé
        if(time(NULL) - displayedWordTime < 5)
        {
            SDL_Surface* wordSurface = TTF_RenderText_Blended(font, displayedWord, scoreTextColor);
            SDL_BlitSurface(wordSurface, NULL, surface, &rect1);
        }

        if(videoRecording)
        {
            char buf[20];
            sprintf(buf, "img/frame_%04d.bmp", frameCounter);
            SDL_SaveBMP(surface, buf);
            frameCounter++;
        }
        SDL_Flip(surface);
        // Mise en pause du programme pour réduire l'occupation du processeur
        SDL_Delay(1000.0 / 60.0);
    }

    g_slist_free_full(liste, freeBall);
    cpSpaceFree(space);

    ballQuit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

/* Fonction de collision entre le segment et une boule */
cpBool preSolve(cpArbiter *arb, cpSpace *space, void *data)
{
    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);
    g_slist_foreach((*(GSList**)data), checkIfAffected, a);
    return cpFalse; // Aucune collision avec ce segment
}

/* Recherche un mot dans le dictionnaire et suprime les boules utilisées */
GSList* checkSelection(GSList* liste, cpVect* startPos, char * (*ruleFunction)(char*,
                       GHashTable*), GHashTable *hashtable, cpSpace* space)
{
    GSList* balls = NULL;
    g_slist_foreach(liste, addIfAffected, &balls);
    balls = g_slist_sort_with_data(balls, sortFunction, startPos);
    char str[30] = "";
    g_slist_foreach(balls, addChar, str);
    if(strlen(str))
    {
        printf("Selected letters : %s\n", str);
        char* wordFound = NULL;
        wordFound = ruleFunction(str, hashtable);
        if(wordFound)
        {
            strcpy(displayedWord, wordFound);
            displayedWordTime = time(NULL);
            score += strlen(wordFound);
            printf("Score: %d\n", score);
            printf("Word : %s\n", wordFound);
            char c;
            int i = 0;
            Ball* ball;
            GSList* elem;
            while((c = wordFound[i++]) != '\0')
            {
                elem = balls;
                while(elem)
                {
                    ball = (Ball*)elem->data;
                    if(ball->affected && ball->c[0] == c)
                    {
                        liste = g_slist_remove(liste, ball);
                        removeBallFromSpace(space, ball);
                        break;
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

/* Retourne un cpSpace configuré pour le jeu */
cpSpace* getSpace(void)
{
    cpSpace *space = cpSpaceNew();
    cpVect gravity = cpv(0, 10);
    cpSpaceSetGravity(space, gravity);
    return space;
}

/* Affiche les nbShape segments du récipient */
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

/* Fonction de tri. Place la boule la plus proche du début du segment en première position */
gint sortFunction(gconstpointer a, gconstpointer b, gpointer startPos)
{
    cpVect* start = (cpVect*) startPos;
    cpVect vA = cpBodyGetPos(((Ball*)a)->body);
    cpVect vB = cpBodyGetPos(((Ball*)b)->body);
    return (int)(cpvdistsq(*start, vA) - cpvdistsq(*start, vB));
}

/* Calcul la répartition de chaque lettre dans un fichier */
void checkCharsDistribution(int count[], char* fileName)
{
    FILE* file = fopen(fileName, "r");
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
