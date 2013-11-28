#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

guint anagramHash(gconstpointer key);
gboolean anagramEqual(gconstpointer a, gconstpointer b);

char* firstRule(char* str, GHashTable *hashtable);
char* secondRule(char* str, GHashTable *hashtable);
char* thirdRule(char* str, GHashTable *hashtable);


#endif // ALGO_H_INCLUDED
