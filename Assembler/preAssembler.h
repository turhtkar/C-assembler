/**
 * @file preAssembler.h
 * @brief the header file of pre-assembler.c that contains the function prototype of the pre-assembler.c
 */

#ifndef PREASEMBLER_H
#define PREASEMBLER_H
#include <stdio.h>

/**
 * @brief a function that pre-assemble the file and store it in a new file
 * 
 * @param input_file_name the name of the input file
 * @param output_file_name the name of the output file
 * @param exception the exception array that is used to store any kind of exception
 */
void preAssmble(char* input_file_name, char * output_file_name, int exception[]);

/**
 * @brief a function that read a line from a file
 * 
 * @param fp the file pointer
 * @param buffer the buffer that will hold the line
 * @return char* the line
 */
char *readline (FILE *fp, char **buffer);
#endif
