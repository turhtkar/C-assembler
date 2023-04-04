#include "macro.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "exception.h"

/* Function to allocate memory for a new macro*/
macro_t *create_macro(macro_t *next)
{
    macro_t *new_macro = (macro_t *)malloc(sizeof(macro_t));
    new_macro->text = malloc(sizeof(char *));
    new_macro->text[0] = NULL;
    new_macro->lines = 0;
    if (next == NULL)
    {
        new_macro->next = NULL;
    }
    else
    {
        new_macro->next = next;
    }
    return new_macro;
}
/* Function to search for a macro with a given name in the linked list*/
macro_t *search_macro(macro_t *head, char *name)
{
    macro_t *current = head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void add_line(macro_t *macro, char *text)
{
    macro->lines++;
    macro->text = (char **)realloc(macro->text, macro->lines * sizeof(char *));
    if (!(macro->text))
    {
        fprintf(stderr, "Memmory reallocation failed");
    }
    macro->text[macro->lines - 1] = tryMalloc((macro->text[macro->lines - 1]), (80 * sizeof(char)));
    strcpy(macro->text[macro->lines - 1], text);
}
