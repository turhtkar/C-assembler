#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbol.h"
#include "exception.h"

void createSymbol(char *label, int value, TYPE type, symbol_t **head_ref)
{
    symbol_t *new_symbol = (symbol_t *)tryMalloc(NULL, sizeof(symbol_t));
    new_symbol->name = (char *)malloc(sizeof(char) * strlen(label) + 1);
    if (new_symbol->name == NULL)
    {
        exitERR("Failed to allocate space for new symbol name");
    }
    strcpy(new_symbol->name, label);
    new_symbol->value = value;
    new_symbol->type = type;
    new_symbol->next = *head_ref;
    *head_ref = new_symbol;
}

void searchSymbol(char *label, symbol_t **head_ref, symbol_t **result_ref)
{
    symbol_t *current = *head_ref;
    while (current != NULL)
    {
        if (strcmp(current->name, label) == 0)
        {
            *result_ref = current;
            return;
        }
        current = current->next;
    }
    *result_ref = NULL;
}

int modifySymbol(char *label, symbol_t **head_ref, TYPE type)
{
    symbol_t *current = *head_ref;
    while (current != NULL)
    {
        if (strcmp(current->name, label) == 0)
        {
            current->type = type;
            return 0;
        }
        current = current->next;
    }
    return 1;
}

void relocateSymbol(symbol_t **head_ref, machine_code map[], int IC, int exception[])
{
    symbol_t *found = NULL;
    int i = IC;
    while ((--i) >= 0)
    {
        if (map[i].bin_code == 2)
        {
            searchSymbol(map[i].word, head_ref, &found);
            if (found != NULL)
            {
                /*we change the first bit to 1 to set to external and only then we add the line number of .extern label*/
                map[i].bin_code = 1;
                map[i].bin_code |= (found->value << 2);
                if (found->type == external)
                {
                    createSymbol(map[i].word, i, IC, head_ref);
                }
            }
            else
            {
                /*throw exception, label not found*/
                exception[0] = 1;
                fprintf(stderr, "%s : label was not found in symbol table", map[i].word);
                break;
            }
        }
    }
}

void updateSymbol(char *label, int DC, symbol_t **head_ref)
{
    symbol_t *current = *head_ref;
    while (current != NULL)
    {
        if (strcmp(current->name, label) == 0)
        {
            current->value = DC;
            return;
        }
        current = current->next;
    }
}
