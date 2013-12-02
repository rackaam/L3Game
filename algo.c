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
    int  i, debut, fin, size = strlen(str);

    for( i = 0; i < size; i++)
    {
        debut = 0;
        fin = i;

        for(debut = 0; debut <= i; debut++, fin--)
        {
            char research[20] = {0};
            strncpy(research, &(str[debut]), size - i);

            //Teste si research est le mot recherché
            if ( g_hash_table_contains(hashTable, research))
            {
                char * buffer;
                buffer = malloc((size - i) * sizeof(char));
                strcpy(buffer, research);
                return buffer;
            }
        }
    }

    return NULL;
}

int getFirstUpgradableIdx(int tab[], int tabSize, int strLen)
{
    int i, j;
    for(i = tabSize - 1, j = 0; i > -1; i--, j++)
    {
        if(tab[i] < strLen - 1 - j)
        {
            return i;
        }
    }
    return -1; //None upgradable idx
}

void substr(char dest[], char src[], int tab[], int tabSize)
{
    //A IMPLEMENTER
    //Retourne str moins les lettres ayant leur index dans tab.
}

char* secondRule(char* str, GHashTable *hashTable)
{
//    char buff[30];
    int strLen = strlen(str);
    int i, j, idx;
    for(i = 0; i < strLen; i++)
    {
        int tab[i];
        for(j = 0; j < i; j++)
        {
            tab[j] = j;
        }

        /*
        substr(buff, str, tab, i);
        if (g_hash_table_contains(hashTable, buff))
        {
            char* buffer;
            buffer = malloc(strlen(buff) * sizeof(char));
            strcpy(buffer, buff);
            return buffer;
        }
        */
        while((idx = getFirstUpgradableIdx(tab, i, strLen)) != -1)
        {
            tab[idx]++;
            if(idx < i - 1)
            {
                idx++;
                int a = 1;
                for(; idx < i; idx++, a++)
                {
                    tab[idx] = tab[idx - a] + a;
                }
            }
            /*
            substr(buff, str, tab, i);
            if (g_hash_table_contains(hashTable, buff))
            {
                char* buffer;
                buffer = malloc(strlen(buff) * sizeof(char));
                strcpy(buffer, buff);
                return buffer;
            }
            */
        }
    }
    return NULL;
}

char* thirdRule(char* str, GHashTable *hashtable)
{
    return (char*)g_hash_table_lookup(hashtable, str);
}

