#include "algo.h"
#include <glib.h>



char* firstRule(char* string, GHashTable *hashTable)
{
    int g,i,j,size=strlen(string);

    char research[20], research2[20];
    for( i=0; i<size; i++)
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
        if ( g_hash_table_contains(hashTable, research))
            {
            printf("Yes\n Research a trouvé %s\n", research);
            char * buffer;
            buffer = malloc((j) * sizeof(char));
            strcpy(buffer, research);
            return buffer;
            }


        j=0;
        g=size-i-1;

        while(j<=g)
        {
            research2[g-j]=string[g-j];
            j=j+1;
        }

        //Teste si research2 est le mot recherché
   if ( g_hash_table_contains(hashTable, research2))
            {
            printf("Yes\n Research2 a trouvé %s\n", research2);
            char * buffer;
            buffer = malloc((j) * sizeof(char));
            strcpy(buffer, research2);
            return buffer;
            }


    }

    return "testRule1";
}

char* secondRule(char* str)
{
    return "testSecondeRule";
}

char* thirdRule(char* str)
{
    return "testThirdRule";
}
