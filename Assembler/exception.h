/**
 * @file exception.h
 * @brief the header file of the exception, contains the prototypes of the functions in the exception.c file
 */
#ifndef EXCEPTION_H
#define EXCEPTION_H

/**
 * @brief transform a number to a string
 *
 * @param n the number
 * @param s the string
 */
void num2Char(int n, char *s);

/**
 * @brief print an exception
 *
 * @param message the message of the exception
 */
void printException(char *message);

/**
 * @brief a function that try to allocate memory for a given pointer, and if it fails it prints an exception
 *
 * @param ptr the pointer
 * @param size the size of the memory to allocate
 */
void *tryMalloc(void *ptr, size_t size);

/**
 * @brief print an exception to stderr
 *
 * @param errMessage the message of the exception
 */
void exitERR(char *errMessage);
/**
 * @brief print an exception to stderr by a given error code and line number of the exception
 *
 * @param errCode the code of the exception
 * @param lineNum the line number of the exception
 */
void getException(int errCode, int lineNum);

#endif
