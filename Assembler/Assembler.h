/**
 * @file Assembler.h
 * @brief the header file of the assembler, contains the prototypes of the functions in the assembler.c file
 */
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include "symbol.h"
/**
 * @brief get the file name and the code of the file type and return the file name with the correct extension
 *
 * @param fileName the file name
 * @param file the file name with the correct extension
 * @param code the code of the file type
 * @return 1 if the file name is too long, 0 otherwise
 */
int getFile(char *fileName, char *file, int code);

/**
 * @brief check if the file name is too long
 *
 * @param fileName the file name
 * @return 1 if the file name is too long, 0 otherwise
 */
int checkEntExt(symbol_t **head_ref);

/**
 * @brief write the ob file
 *
 * @param fileName the file name
 * @param IC the instruction counter
 * @param map the map of the machine code
 */
void writeObjectFile(char *fileName, int IC, machine_code map[]);

/**
 * @brief write the entry file
 *
 * @param fileName the file name
 * @param head the head of the symbol table
 */
void writeEntryFile(char *fileName, symbol_t **head);

/**
 * @brief write the extern file
 *
 * @param fileName the file name
 * @param head the head of the symbol table
 * @param map the map of the machine code
 */
void writeExternFile(char *fileName, symbol_t **head, machine_code map[]);

/**
 * @brief reverse a string
 *
 * @param str the string
 */
void strrev(char *str);
#endif
