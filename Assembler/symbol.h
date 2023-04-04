/**
 * @file symbol.h
 * @brief the header file of symbol.c that contains the function prototype of the symbol.c
 */

#ifndef SYMBOL_H
#define SYMBOL_H
#include "machineCode.h"

/**
 * @brief an enum that holds the different types of the symbol
 * 
 */
typedef enum {relocatable, external, entry, null} TYPE;

/**
 * @brief a struct that holds the symbol that is used as a node in the symbol table
 * 
 */
typedef struct symbol {
    char *name;
    int value;
    TYPE type;
    struct symbol* next;
}symbol_t;

/**
 * @brief Create a Symbol object
 * 
 * @param label the name of the symbol
 * @param value the value of the symbol
 * @param type the type of the symbol
 * @param head_ref the head of the symbol table
 */
void createSymbol(char* label, int value, TYPE type, symbol_t ** head_ref);

/**
 * @brief a function that search for a symbol in the symbol table
 * 
 * @param label the name of the symbol
 * @param head_ref the head of the symbol table
 * @param result the result of the search if found than result points to that symbol, if not than it points to NULL
 */
void searchSymbol(char* label, symbol_t** head_ref, symbol_t** result);

/**
 * @brief a function that modify the type of a symbol
 * 
 * @param label the name of the symbol
 * @param head_ref the head of the symbol table
 * @param type the new type of the symbol
 * @return int 1 if the symbol is not found, 0 if the symbol is found and the type is modified
 */
int modifySymbol(char *label, symbol_t ** head_ref, TYPE type);

/**
 * @brief a function that relocate the symbol in the symbol table
 * 
 * @param head_ref the head of the symbol table
 * @param map the machine code map
 * @param IC the instruction counter
 * @param exception the exception array that is used to store any kind of exception
 */
void relocateSymbol(symbol_t **head_ref, machine_code map[], int IC, int exception[]);

/**
 * @brief a function that update the symbol in the symbol table
 * 
 * @param label the name of the symbol
 * @param DC the data counter which we want to value of the symbol value to
 * @param head_ref the head of the symbol table
 */
void updateSymbol(char * label, int DC, symbol_t ** head_ref);
#endif