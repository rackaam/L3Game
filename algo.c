#include "algo.h"


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
    int  i, debut, strLen = strlen(str);

    for( i = 0; i < strLen; i++)
    {
        debut = 0;
        for(debut = 0; debut <= i; debut++)
        {
            char research[20] = {0};
            strncpy(research, &(str[debut]), strLen - i);

            //Teste si research est le mot recherché
            if ( g_hash_table_contains(hashTable, research))
            {
                char * buffer;
                buffer = malloc((strLen - i) * sizeof(char));
                strcpy(buffer, research);
                return buffer;
            }
        }
    }
    return NULL;
}

int getFirstIncrementableIdx(int tab[], int tabSize, int strLen)
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
    int srcSize = strlen(src);
    if(tabSize == 0)
    {
        strcpy(dest, src);
    }
    else
    {
        int destIdx = 0; // Utilisé pour savoir ou écrire dans dest

        /* copie du début de src jusqu'au premier caractère a enlever */
        strncpy(dest, src, tab[0]);
        destIdx += tab[0];

        /* copie des caractères présent entre 2 caractères à enlever */
        int i, diff;
        for(i = 0; i < (tabSize - 1); i++)
        {
            diff = tab[i + 1] - tab[i];
            if(diff > 1)
            {
                strncpy(&(dest[destIdx]), &(src[tab[i] + 1]), diff - 1);
                destIdx += (diff - 1);
            }
        }

        /* copie du dernier caractère à enlever jusqu'a la fin de src */
        diff = srcSize - 1 - tab[tabSize - 1];
        strncpy(&(dest[destIdx]), &(src[tab[tabSize - 1] + 1]), diff);
        destIdx += diff;

        /* rajout d'un caractère de fin de chaine à la fin du mot car strncpy n'en rajoute pas */
        strncpy(&(dest[destIdx]), "\0", 1);
    }
}

char* secondRule(char* str, GHashTable *hashTable)
{
    char buff[30];
    int strLen = strlen(str);
    int i, j, idx;
    for(i = 0; i < strLen; i++)
    {
        int tab[i];
        for(j = 0; j < i; j++)
        {
            tab[j] = j;
        }

        substr(buff, str, tab, i);
        if (g_hash_table_contains(hashTable, buff))
        {
            char* buffer;
            buffer = malloc(strlen(buff) * sizeof(char));
            strcpy(buffer, buff);
            return buffer;
        }

        while((idx = getFirstIncrementableIdx(tab, i, strLen)) != -1)
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

            substr(buff, str, tab, i);
            if (g_hash_table_contains(hashTable, buff))
            {
                char* buffer;
                buffer = malloc(strlen(buff) * sizeof(char));
                strcpy(buffer, buff);
                return buffer;
            }
        }
    }
    return NULL;
}

char* thirdRule(char* str, GHashTable *hashTable)
{
    char buff[30];
    gpointer value = NULL;
    int strLen = strlen(str);
    int i, j, idx;
    for(i = 0; i < strLen; i++)
    {
        int tab[i];
        for(j = 0; j < i; j++)
        {
            tab[j] = j;
        }

        substr(buff, str, tab, i);
        if ((value = g_hash_table_lookup(hashTable, buff)))
        {
            char* buffer;
            buffer = malloc(strlen(buff) * sizeof(char));
            strcpy(buffer, (char*)value);
            return buffer;
        }

        while((idx = getFirstIncrementableIdx(tab, i, strLen)) != -1)
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

            substr(buff, str, tab, i);
            if ((value = g_hash_table_lookup(hashTable, buff)))
            {
                char* buffer;
                buffer = malloc(strlen(buff) * sizeof(char));
                strcpy(buffer, (char*)value);
                return buffer;
            }
        }
    }
    return NULL;
}

