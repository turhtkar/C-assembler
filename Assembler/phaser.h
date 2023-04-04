#ifndef PHASER_H
#define PHASER_H
#include <stdio.h>
#include "symbol.h"
#include "machineCode.h"

/**
 * @brief a function that phrase an operation line of the file and check if it is a valid operation line
 * and also code it to machine code if it is valid and store it inside map
 *
 * @param line the rest of the operation line
 * @param opNum the number of operation in the line
 * @param head_ref the head of the symbol table
 * @param map the array of machine code
 * @param IC the instruction counter
 * @param exception an array that holds the exception of the line
 * @param lineNum the number of the line
 * @return IC the instruction counter
 */
int phraseOperation(char *line, int opNum, symbol_t **head_ref, machine_code map[], int IC, int exception[], int lineNum);

/**
 * @brief a function that phrase a data line of the file and check if it is a valid data line
 * and also code it to machine code if it is valid and store it inside map
 *
 * @param line the rest of the data line
 * @param head_ref the head of the symbol table
 * @param map the array of machine code
 * @param DC the data counter
 * @param exception an array that holds the exception of the line
 * @param lineNum the number of the line
 * @return DC the data counter
 */
int sortingValidation(char *token, int exception[]);

/**
 * @brief a function that phrase a string line of the file and check if it is a valid string line
 * and also code it to machine code if it is valid and store it inside map
 *
 * @param line the rest of the string line
 * @param head_ref the head of the symbol table
 * @param map the array of machine code
 * @param DC the data counter
 * @param exception an array that holds the exception of the line
 * @param lineNum the number of the line
 * @return DC the data counter
 */
int validateData(char *line, machine_code map[], int DC, int exception[]);

/**
 * @brief a function that phrase a string line of the file and check if it is a valid string line
 *
 * @param line the string line
 * @param map the array of machine code
 * @param DC the data counter
 * @param exception the exception array that is used to store any kind of exception
 * @return int
 */
int validateString(char *line, machine_code map[], int DC, int exception[]);

/**
 * @brief a function to validate a label
 *
 * @param label the label to validate
 * @param head the head of the symbol table
 * @param pass the pass number
 * @param exception the exception array that is used to store any kind of exception
 * @return the label if it is valid
 */
char *validateLabel(char *label, symbol_t **head, int pass, int exception[]);

/**
 * @brief a function to validate an entry line
 *
 * @param line the rest of the entry line
 * @param head_ref the head of the symbol table
 * @param exception the exception array that is used to store any kind of exception
 */
void validateEntry(char *line, symbol_t **head_ref, int exception[]);
/**
 * @brief a function to validate an extern line
 *
 * @param line the rest of the extern line
 * @param head_ref the head of the symbol table
 * @param exception the exception array that is used to store any kind of exception
 */
void phraseExtern(char *line, symbol_t **head_ref, int exception[]);
/**
 * @brief a function to pharse the file and check if it is a valid file
 * this function is also used to create the symbol table
 * and also code it to machine code if it is valid and store it inside the machine code map
 *
 * @param fptr the .am file pointer
 * @param pass the pass number
 * @param head_ref the head of the symbol table
 * @param map the array of machine code
 * @param IC the instruction counter
 * @param exception the exception array that is used to store any kind of exception
 * @return IC the instruction counter
 */
int parse_file(FILE *fptr, int pass, symbol_t **head_ref, machine_code map[], int IC, int exception[]);
int isascii(int c);
#endif
