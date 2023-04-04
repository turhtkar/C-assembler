#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include "exception.h"

void num2Char(int n, char *s)
{
    int i, len;
    char tmp = 0;
    i = 0;
    do
    {                                  /* generate digits in reverse order */
        s[i++] = (char)(n % 10 + '0'); /* get next digit */
    } while ((n /= 10) > 0);           /* delete it */
    s[i] = '\0';

    /* reverse the string */
    len = strlen(s);
    for (i = 0; i < len / 2; i++)
    {
        tmp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = tmp;
    }
}

void printException(char *message)
{
    fprintf(stderr, "%s", message);
}

void *tryMalloc(void *ptr, size_t size)
{
    if ((ptr = malloc(size)) == NULL)
    {
        exitERR("Memory allocation Failed");
    }
    return ptr;
}

void exitERR(char *errMessage)
{
    printException(errMessage);
}

void getException(int errCode, int lineNum)
{
    char errMessage[80] = "\nError in line ";
    char message[130];
    char number[5];
    int end = strlen(errMessage);
    num2Char(lineNum, number);
    memcpy(errMessage + end, number, end);
    end = strlen(errMessage);
    errMessage[end] = ':';
    errMessage[end + 1] = '\n';
    errMessage[end + 2] = '\0';

    switch (errCode)
    {
    case 1:
        strcpy(message, "Comma is not between operands\n");
        break;
    case 2:
        strcpy(message, "More than one comma between operands, not valid\n");
        break;
    case 3:
        strcpy(message, "No commas between operands or paramters\n");
        break;
    case 4:
        strcpy(message, "Illegal characters after end of valid operation instruction line\n");
        break;
    case 5:
        strcpy(message, "Invalid operand type for the operation in the instruction line\n");
        break;
    case 6:
        strcpy(message, "Invalid operand (not a valid label, register or number)\n");
        break;
    case 7:
        strcpy(message, "Invalid label, label above maximum label leangth (30 characters)\n");
        break;
    case 8:
        strcpy(message, "Invalid label, label starts with a number or contain illegeal character\n");
        break;
    case 9:
        strcpy(message, "Missing ')' in paramaters\n");
        break;
    case 10:
        strcpy(message, "No spaces allowed inside jump operation () parameters\n");
        break;
    case 11:
        strcpy(message, "Illegal characters in jump instruction line\n");
        break;
    case 12:
        strcpy(message, "too many parameters in line\n");
        break;
    case 13:
        strcpy(message, "Invalid paramter, not a label register or a number\n");
        break;
    case 14:
        strcpy(message, "Missing parameters on that line\n");
        break;
    case 15:
        strcpy(message, "a Symbol with that label name already exists\n");
        break;
    case 16:
        strcpy(message, "Illegal label name, matches operation name\n");
        break;
    case 17:
        strcpy(message, "Illegal label name, matches register name\n");
        break;
    case 18:
        strcpy(message, "Illegal character in data instruction line\n");
        break;
    case 19:
        strcpy(message, "Missing comma between data\n");
        break;
    case 20:
        strcpy(message, "Missing quotation Mark at the beginning of the string or the end of the string\n");
        break;
    case 21:
        strcpy(message, "Illegal character in string instruction line\n");
        break;
    case 22:
        strcpy(message, "Illegal comma at the end of line\n");
        break;
    case 23:
        strcpy(message, "Illegal character after expected number\n");
        break;
    case 24:
        strcpy(message, ".entry label defenition isn't found in file\n");
        break;
    case 25:
        strcpy(message, "Illegal line not an instruction line or a data line\n");
        break;
    case 26:
        strcpy(message, "failed to store binary to machine code block, can only hold 256 blocks\n");
        break;
    }
    strcat(errMessage, message);
    printException(errMessage);
}
