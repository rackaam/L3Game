#include "algo.h"
#include <glib.h>


int ascending(void const *a, void const *b )
{
    return (*(char*)a - * (char*)b );
}

guint anagramHash(gconstpointer key)
{
    char* str = (char*)key;
    char tab[strlen(str) + 1];
    strcpy(tab, str);
    qsort(tab, strlen(tab), sizeof(char), ascending);
    int hash = 5381;
    int c, i = 0;

    while ((c = tab[i++]))
    {
        hash = ((hash << 5) + hash) + c;    /* hash * 33 + c */
    }

    return hash;

}

gboolean anagramEqual(gconstpointer a, gconstpointer b)
{
    char* str1 = (char*)a;
    char* str2 = (char*)b;
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if(len1 != len2)
    {
        return FALSE;
    }

    int count1[26] = {0};
    int count2[26] = {0};

    int i;
    for (i = 0; i < len1;  i++)
    {
        count1[(unsigned int)(str1[i] - 97)]++;
        count2[(unsigned int)(str2[i] - 97)]++;
    }

    for (i = 0; i < 26; i++)
    {
        if (count1[i] != count2[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

char* firstRule(char* str, GHashTable *hashTable)
{
    int g, i, j, size = strlen(str);

    char research[20], research2[20];
    for( i = 0; i < size; i++)
    {
        memset (research, 0, sizeof (research));
        memset (research2, 0, sizeof (research2));
        j = 0;
        g = i;

        while(g < size)
        {
            research[j] = str[g];

            j = j + 1;
            g = i + j;
        }

        //Teste si research est le mot recherché
        if ( g_hash_table_contains(hashTable, research))
        {
            char * buffer;
            buffer = malloc((j) * sizeof(char));
            strcpy(buffer, research);
            return buffer;
        }

        j = 0;
        g = size - i - 1;

        while(j <= g)
        {
            research2[g - j] = str[g - j];
            j = j + 1;
        }

        //Teste si research2 est le mot recherché
        if ( g_hash_table_contains(hashTable, research2))
        {
            char * buffer;
            buffer = malloc((j) * sizeof(char));
            strcpy(buffer, research2);
            return buffer;
        }
    }

    return NULL;
}

char* secondRule(char* str, GHashTable *hashtable)
{
    return "testSecondeRule";
}

char* thirdRule(char* str, GHashTable *hashtable)
{
    return (char*)g_hash_table_lookup(hashtable, str);
}
