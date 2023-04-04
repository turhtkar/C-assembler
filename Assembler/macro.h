/**
 * @file macro.h
 * @brief the header file of the macro, contains the prototypes of the functions in the macro.c file
 */

#ifndef MACRO_H
#define MACRO_H

/**
 * @brief a struct of a macro that contains the name of the macro,
 * the text of the macro and the number of lines in the macro and used to create a linked list of macros
 */
typedef struct macro {
    char name[256];
    char** text;
    int lines;
    struct macro* next;
} macro_t;

/**
 * @brief Create a macro object
 * 
 * @param next the next macro in the linked list
 * @return the new head macro
 */
macro_t* create_macro(macro_t* next);

/**
 * @brief Search for a macro with a given name in the linked list
 * 
 * @param head the head of the linked list
 * @param name the name of the macro to search for
 * @return the new head macro
 */
macro_t* search_macro(macro_t* head, char* name);

/**
 * @brief Add a new line of text to the macro
 * 
 * @param macro the macro to add the line to
 * @param text the text of the line
 */
void add_line(macro_t* macro, char* text);/* Function to add a new line of text to the macro*/
#endif
