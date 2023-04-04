#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "phaser.h"
#include "symbol.h"
#include "token.h"
#include "exception.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief a struct that contains the list of valid operations used in the given assembly file and their parameters\operands,
 * and their corresponding codes
 */
struct
{
    const char *opCodeName;
    int adress;
    int valid_source_operand_types;
    int valid_destanition_operand_types;
    int numOfOperands;
    /*7=0111, 4=0100, 3=0011, 2=0010
      Bit at index 0 (LSB): Immediate operand (starting with #)
      Bit at index 1: Register operand (starting with r)
      Bit at index 2: Label operand (not starting with # or r)*/
} opMap[] = {
    {"mov", 0, 7, 7, 2}, /* valid operand types: 111 for both source and destination*/
    {"cmp", 1, 7, 7, 2},
    {"add", 2, 7, 7, 2},
    {"sub", 3, 7, 7, 2},
    {"not", 4, 0, 3, 1},
    {"clr", 5, 0, 2, 1},
    {"lea", 6, 4, 2, 2},
    {"inc", 7, 0, 2, 1},
    {"dec", 8, 0, 2, 1},
    {"jmp", 9, 0, 4, 1},
    {"bne", 10, 0, 4, 1},
    {"red", 11, 0, 2, 1},
    {"prn", 12, 0, 7, 1},
    {"jsr", 13, 0, 4, 1},
    {"rts", 14, 0, 0, 0},
    {"stop", 15, 0, 0, 0}};

/**
 * @brief a struct that contains the list of valid registers and their corresponding codes
 */
struct
{
    const char *registerName;
    int adress;
} rMap[] = {
    {"r0", 0},
    {"r1", 1},
    {"r2", 2},
    {"r3", 3},
    {"r4", 4},
    {"r5", 5},
    {"r6", 6},
    {"r7", 7},
    {NULL, 8}};

int isascii(int c)
{
    return (c >= 0 && c < 128);
}

int validateString(char *line, machine_code map[], int DC, int exception[])
{
    int i = 0, e = 0, c = 0; /*e is flag for the end of the string, and c is for it's index*/
    if (line[i] != '"')
    {
        /*throw exception missing " at the beginning of the string*/
        exception[0] = 20;
        return DC;
    }
    else
    {

        for (i = 1; i < strlen(line); i++)
        {
            if (line[i] == '"')
            {
                e = 1;
                c = i;
                continue;
            }
            if (isspace(line[i]) != 0)
            {
                continue;
            }
            if (isascii(line[i]) != 0)
            {
                e = 0;
            }
            else
            {
                /*throw exception illegal character in string*/
                exception[0] = 21;
                return DC;
            }
        }
        if (e == 0)
        {
            /*throw exception missing " at the end of the string*/
            exception[0] = 20;
            return DC;
        }
    }
    for (i = 1; i < c; i++)
    {
        if (DC < 255)
        {
            map[DC++].bin_code = line[i];
        }
        else
        {
            /*Data counter is over the maximum Data*/
            exception[0] = 27;
        }
    }
    if (DC < 255)
    {
        map[DC++].bin_code = '\0';
    }
    else
    {
        exception[0] = 27;
    }
    return DC;
}

int validateData(char *line, machine_code map[], int DC, int exception[])
{
    int n = 0, c = 0, i = 0;
    char *token, *delim = " ,\t\n";
    for (; i < strlen(line); i++)
    {
        if (i == 0)
        {
            if (line[i] == '+' || line[i] == '-' || isdigit(line[i]) != 0)
            {
                n = 1;
            }
            if (i == 0 && n != 1)
            {
                /*throw exception illegal character for data*/
                exception[0] = 18;
                break;
            }
        }
        else if (n == 1)
        {
            if (isdigit(line[i]) != 0)
            {
                continue;
            }
            else if (isspace(line[i]) != 0)
            {
                n = 0;
            }
            else if (line[i] == ',')
            {
                n = 0;
                c = 1;
            }
            else
            {
                /*throw exception illegal character for data*/
                exception[0] = 18;
                break;
            }
        }
        else if (n == 0)
        {
            if (isspace(line[i]) != 0)
            {
                continue;
            }
            if ((line[i] == '+' || line[i] == '-' || isdigit(line[i]) != 0))
            {
                if (c == 0)
                {
                    /*throw exception missing comma*/
                    exception[0] = 19;
                }
                else
                {
                    n = 1;
                    c = 0;
                }
                continue;
            }
            else if (line[i] == ',' && c == 0)
            {
                c = 1;
            }
            else
            {
                /*throw exception illegal character for data*/
                exception[0] = 18;
                break;
            }
        }
    }
    if (n == 0 && c == 1)
    {
        /*throw exception illigeal comma at end of line*/
        exception[0] = 22;
    }
    else
    {
        token = strtok(line, delim);
        while (token != NULL)
        {
            map[DC].bin_code = atoi(token);
            if (DC < 255)
            {
                DC++;
            }
            token = strtok(NULL, delim);
        }
    }
    return DC;
}

int phraseOperation(char *line, int opNum, symbol_t **head_ref, machine_code map[], int IC, int exception[], int lineNum)
{
    int validation_step = -1, validation_assigment, i, srcOperand = 1, destOperand = 0, t = IC, counter = 0;
    symbol_t *result_ref = NULL;
    char *token, *delim = " ,\t\n", tempLine[30];
    strcpy(tempLine, line);                       /* we create tempLine to make sure we dont use strtok*/
    map[t].bin_code = (opMap[opNum].adress << 6); /*the machine code of the instruction line of a opNum operation.*/
    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == ',')
        {
            if ((i == 0) || (i == (strlen(line) - 1)))
            {
                /*throw exception, comma is not between operands*/
                exception[0] = 1;
                break;
            }
            counter++;
        }
        if (counter > 1)
        {
            /*throw exception more than one comma, not a valid character;*/
            exception[0] = 2;
            break;
        }
    }
    if ((opMap[opNum].numOfOperands == 2 && counter == 0) || exception[0] == 1)
    {
        /*throw exception not enough commas*/
        exception[0] = 3;
        return IC;
    }
    while (1)
    {
        /*the program might not check the next if, if that happens all we do is nest those 3 validation steps in a if statement that is always 1 and so it will always go inside it.*/
        if (opMap[opNum].numOfOperands == 2)
        {
            if (validation_step == -1)
            {
                validation_assigment = opMap[opNum].valid_source_operand_types;
                validation_step = 1;
                if (opMap[opNum].valid_destanition_operand_types != 4)
                {
                    token = strtok(line, delim);
                }
            }
            else if (validation_step == 1)
            {
                validation_assigment = opMap[opNum].valid_destanition_operand_types;
                validation_step = 2;
                if (opMap[opNum].valid_destanition_operand_types != 4)
                {
                    token = strtok(NULL, delim);
                }
            }
            else
            {
                token = strtok(NULL, delim);
                /*illegal characters after end of valid operation instruction line*/
                if (token != NULL)
                {
                    exception[0] = 4;
                }
                break;
            }
        }
        if (opMap[opNum].numOfOperands == 1)
        {
            if (validation_step == -1)
            {
                validation_assigment = opMap[opNum].valid_destanition_operand_types;
                validation_step = 2;
                if (opMap[opNum].valid_destanition_operand_types != 4)
                {
                    token = strtok(line, delim);
                }
            }
            else
            {
                token = strtok(NULL, delim);
                /*illegal characters after end of valid operation instruction line*/
                if (token != NULL)
                {
                    exception[0] = 4;
                }
                break;
            }
        }
        if (opMap[opNum].numOfOperands == 0)
        {
            token = strtok(line, delim);
            /*illegal characters after end of valid operation instruction line*/
            if (token != NULL)
            {
                exception[0] = 4;
            }
            break;
        }
        if (validation_assigment == 2 || validation_assigment == 3 || validation_assigment == 7)
        {
            IC++;
            switch (sortingValidation(token, exception))
            {
            /*token is register*/
            case 1:
                /*we turn the register number from char to int*/
                i = token[1] - '0';
                if (validation_step == 1)
                {
                    srcOperand = (i << 8); /*register adress of source*/
                    map[IC].bin_code = srcOperand;
                    map[t].bin_code |= (3 << 4); /* the machine code of the instruction line, that takes register as it's source operand*/
                }
                else if (validation_step == 2)
                {
                    destOperand = (i << 2); /*register adress of dest*/
                    map[IC].bin_code = destOperand;
                    map[t].bin_code |= (3 << 2); /* the machine code of the instruction line, that takes register as it's dest operand*/
                }
                break;
            /*token is label*/
            case 2:
                strcpy(map[IC].word, token);
                if (validation_assigment == 7)
                {
                    map[IC].bin_code |= (1 << 1);
                    map[IC].lineNum = lineNum;
                    /*it might not find symbol, most likely it wont find it and so srcOperand and destOperand are not defined*/
                    if (validation_step == 1)
                    {
                        searchSymbol(token, head_ref, &result_ref);
                        if (result_ref != NULL)
                        {
                            srcOperand = result_ref->value << 2; /*label adress of source*/
                            map[IC].bin_code |= srcOperand;
                            if (result_ref->type == external)
                            {
                                createSymbol(token, IC, external, head_ref);
                            }
                        }
                        map[t].bin_code |= (1 << 4); /* the machine code of the instruction line, that takes label as it's source operand*/
                    }
                    else if (validation_step == 2)
                    {
                        searchSymbol(token, head_ref, &result_ref);
                        if (result_ref != NULL)
                        {
                            destOperand = result_ref->value << 2; /*label adress of dest */
                            map[IC].bin_code |= destOperand;
                            if (result_ref->type == external)
                            {
                                createSymbol(token, IC, external, head_ref);
                            }
                        }
                        map[t].bin_code |= (1 << 2); /* the machine code of the instruction line, that takes label as it's dest operand*/
                    }
                    break;
                }
                else
                {
                    /*throw exception invalid operand type for the instruction line.*/
                    exception[0] = 5;
                    break;
                }
                break;
            /*token is number*/
            case 3:
                if (validation_assigment == 3 || validation_assigment == 7)
                {
                    i = atoi(token + 1); /*if token is number than token starts with # so we skip it*/
                    if (validation_step == 1)
                    {
                        srcOperand = (i << 2); /*number adress of source*/
                        map[IC].bin_code = srcOperand;
                    }
                    else if (validation_step == 2)
                    {
                        destOperand = (i << 2); /*number adress of dest*/
                        map[IC].bin_code = destOperand;
                    }
                    break;
                }
                else
                {
                    /*throw exception invalid operand type for the instruction line.*/
                    exception[0] = 5;
                    break;
                }
            case -1:
                if (exception[0] == 13)
                {
                    exception[0] = 6;
                }
                break;
            }
        }
        else if (validation_assigment == 4)
        {
            IC++;
            /*lea operation*/
            if (validation_step == 1)
            {
                /*check if first operand is label*/
                if ((strlen(token) - 1) > 30)
                {
                    /*throw exception label is above maximum label length*/
                    exception[0] = 7;
                    break;
                }
                for (i = (strlen(token) - 1); i >= 0; i--)
                {
                    /*if token doesn't start with an alpahbet charcter or if every other character in token is neither an alpahbet character or a digit*/
                    if ((i == 0 && !isalpha(token[i])) || (i > 0 && (!isalpha(token[i]) && !isdigit(token[i]))))
                    {
                        /*throw exception*/
                        free(token);
                        exception[0] = 8;
                        return IC;
                    }
                }
                map[IC].bin_code |= (1 << 1);
                strcpy(map[IC].word, token);
                map[IC].lineNum = lineNum;
                searchSymbol(token, head_ref, &result_ref);
                if (result_ref != NULL)
                {
                    srcOperand = result_ref->value << 2;
                    map[IC].bin_code |= srcOperand;
                    if (result_ref->type == external)
                    {
                        createSymbol(token, IC, external, head_ref);
                    }
                }
                IC++;
                token = strtok(NULL, delim);
                for (i = 0; i < 8; i++)
                {
                    if (strcmp(token, rMap[i].registerName) == 0)
                    {
                        destOperand = (i << 2); /*register adress of dest*/
                        map[IC].bin_code = destOperand;
                    }
                }
                token = strtok(NULL, delim);
                if (token != NULL)
                {
                    /*throw exception too many words*/
                    exception[0] = 4;
                    break;
                }
                else
                {
                    map[t].bin_code = 412; /*412=00 00 0110 01 11 00*/
                }
            }
            /*jump operations only one token */
            else if (validation_step == 2)
            {
                map[t].bin_code |= (1 << 3); /*we transform the current instruction line of a jump operation into a Machine code, by the given project rules that's always have label as it's dest operand*/
                token = strtok(tempLine, " (\t\n");
                if ((strlen(token) - 1) > 30)
                {
                    /*throw exception label is above maximum label length or if it's not a char and just a illegal char */
                    exception[0] = 7;
                    break;
                }
                for (i = strlen(token) - 1; i >= 0; i--)
                {
                    /*if token doesn't start with an alpahbet charcter or if every other character in token is neither an alpahbet character or a digit*/
                    if ((i == 0 && !isalpha(token[i])) || (i > 0 && (!isalpha(token[i]) && !isdigit(token[i]))))
                    {
                        /*throw exception*/
                        free(token);
                        exception[0] = 8;
                        return IC;
                    }
                }
                map[IC].bin_code |= (1 << 1);
                strcpy(map[IC].word, token);
                map[IC].lineNum = lineNum;
                searchSymbol(token, head_ref, &result_ref);
                if (result_ref != NULL)
                {
                    destOperand = result_ref->value << 2;
                    map[IC].bin_code |= destOperand;
                    if (result_ref->type == external)
                    {
                        createSymbol(token, IC, external, head_ref);
                    }
                }
                IC++;
                /*we reset counter to 0 so we can reuse it to count the symbol '(', if it's equal to 1, than we throw exception */
                counter = 0;
                destOperand = 0;
                for (i = (strlen(token)); i < strlen(line); i++)
                {
                    if (isspace(line[i]) != 0)
                    {
                        destOperand++; /*we use destOperand as a counter for the number of spaces after the token in the line since we don't use destOperand in this case that is already defined
                         that way we can check if there if there's only spaces after the token so the jump is with no parameters*/
                    }
                    if (line[i] == '(')
                    {
                        counter++;
                    }
                    if (line[i] == ')')
                    {
                        srcOperand--; /*we use srcOperand as a counter for the number of closing pernthsis ')' in the line since we don't use srcOperand in this case*/
                    }                 /*
                                                  לפי עמ 31, מיעון קפיצה עם פרמטרים
                                      "האופרנד מורכב משם של תווית אליה רוצים לקפוץ, ואחריה בתוך סוגריים שני הפרמטרים, מופרדים זה מזה בפסיק. אין רווחים בין מרכיבי האופרנד."*/
                    if (counter == 1 && (isspace(line[i]) != 0))
                    {
                        /*throw exception no spaces allowed inside jump operation () parameters.*/
                        exception[0] = 10;
                    }
                    if (counter > 1)
                    {
                        /*throw exception illigeal character '('*/
                        exception[0] = 11;
                    }
                    /*we need to check if there's a comma between the two parameters if there is more than one or less than one, and there is (), than we throw exception*/
                    if (counter == 1 && line[i] == ',')
                    {
                        counter--;
                    }
                    if (counter < 0)
                    {
                        /*throw exception illegal character ','*/
                        exception[0] = 11;
                    }
                }
                if (counter != 0 || exception[0] != 0)
                {
                    /*throw exception illegal characters.*/
                    exception[0] = 11;
                    break;
                }
                /*if the jump operand doesn't include any parameters*/
                if ((strlen(token) + destOperand) == strlen(line))
                {
                    IC--;
                    break;
                }
                else if (srcOperand != 0)
                {
                    /*throw exception too many closing perenthesis ')'*/
                    exception[0] = 9;
                    break;
                }
                else
                {
                    /*the jump operand does have paramaters, we need to validate them, we will reset counter agian to make sure we map the right parameters to the right places*/
                    counter = -1;
                    while (counter < 1)
                    {
                        counter++;
                        token = strtok(NULL, " ,\t\n");
                        if (counter > 1)
                        {
                            /*throw exception too many parameters*/
                            exception[0] = 12;
                            break;
                        }
                        /*increasing IC to store the Machine code of the second parameter*/
                        if (counter == 1)
                        {
                            IC++;
                            token[strlen(token) - 1] = 0;
                        }
                        switch (sortingValidation(token, exception))
                        {
                        /*token is register*/
                        case 1:
                            /*converting register number to int*/
                            i = token[1] - '0';
                            if (counter == 0)
                            {
                                map[t].bin_code = (map[t].bin_code | (3 << 12));
                                map[IC].bin_code = (i << 8);
                            }
                            else if (counter == 1)
                            {
                                map[t].bin_code = (map[t].bin_code | (3 << 12));
                                map[IC].bin_code = (i << 2);
                            }
                            break;
                        /*token is label*/
                        case 2:
                            map[IC].bin_code |= (1 << 1);
                            strcpy(map[IC].word, token);
                            map[IC].lineNum = lineNum;
                            searchSymbol(token, head_ref, &result_ref);
                            if (result_ref != NULL)
                            {
                                if (result_ref->type == external)
                                {
                                    createSymbol(token, IC, external, head_ref);
                                }
                                if (counter == 0)
                                {
                                    map[t].bin_code = (map[t].bin_code | (1 << 12));
                                    srcOperand = result_ref->value << 2; /*register adress of source*/
                                    map[IC].bin_code = (srcOperand | (1 << 1));
                                }
                                else if (counter == 1)
                                {
                                    map[t].bin_code = (map[t].bin_code | (1 << 10));
                                    destOperand = result_ref->value << 2; /*register adress of dest */
                                    map[IC].bin_code = (destOperand | (1 << 1));
                                }
                            }
                            break;
                        /*token is number*/
                        case 3:
                            if (counter == 0)
                            {
                                srcOperand = (atoi(token + 1) << 2);
                                map[IC].bin_code = srcOperand;
                            }
                            else if (counter == 1)
                            {
                                destOperand = (atoi(token + 1) << 2);
                                map[IC].bin_code = destOperand;
                            }
                            break;
                        case -1:
                            break;
                        }
                    }
                }
            }
        }
    }
    if (token == NULL)
    {
        free(token);
    }
    return IC;
}

int sortingValidation(char *token, int exception[])
{
    int i;
    /*check if token is a register*/
    if (token[0] == 'r' && strlen(token) == 2)
    {
        if (isdigit(token[1]))
        {
            if (token[1] < '8' && token[1] >= '0')
            {
                return 1;
            }
        }
    }
    /*check if token is label*/
    if ((strlen(token) - 1) < 30)
    {
        if (token[0] != '#')
        {
            for (i = strlen(token) - 2; i >= 0; i--)
            {
                /*if token doesn't start with an alpahbet charcter or if every other character in token is neither an alpahbet character or a digit*/
                if ((i == 0 && !isalpha(token[i])) || (i > 0 && (!isalpha(token[i]) && !isdigit(token[i]))))
                {
                    /*throw exception*/
                    exception[0] = 8;
                    return -1;
                }
            }
            return 2;
        }
    }
    /*check if token is a number*/
    if (token[0] == '#')
    {
        if (strlen(token) > 1)
        {
            for (i = 1; i < strlen(token) - 1; i++)
            {
                if (i == 1 && (token[i] == '+' || token[i] == '-'))
                {
                    continue;
                }
                else if (!isdigit(token[i]))
                {
                    /*throw exception, illegal character after expected number*/
                    exception[0] = 23;
                    return -1;
                }
            }
            return 3;
        }
    }
    else
    {
        exception[0] = 13;
        return -1;
    }
    return -1;
}

char *validateLabel(char *label, symbol_t **head, int pass, int exception[])
{
    int i = 0;
    symbol_t *result_ref = NULL;
    if ((strlen(label) - 1) > 30)
    {
        /*throw exception, label is too long*/
        exception[0] = 18;
    }
    if (!isalpha(label[0]))
    {
        /*throw exception, label doesn't start with an alpahbet character*/
        exception[0] = 8;
    }
    for (; i < (strlen(label) - 1); i++)
    {
        if (!isalpha(label[i]) && !isdigit(label[i]))
        {
            /*throw exception, label contains illegal character*/
            exception[0] = 8;
        }
        if ((i + 1) == (strlen(label) - 1) && pass == 1)
        {
            label[i + 1] = 0;
        }
    }
    searchSymbol(label, head, &result_ref);
    if ((pass == 1) && result_ref != NULL)
    {
        /*throw exception, label already exists*/
        exception[0] = 15;
    }
    /*validate that Label name isn't equal to any operation name*/
    for (i = 0; i < sizeof(opMap) / sizeof(opMap[0]); i++)
    {
        if (!memcmp(label, opMap[i].opCodeName, MIN(strlen(opMap[i].opCodeName), strlen(label))))
        {
            /*throw exception on that line, illegal label name*/
            exception[0] = 16;
            break;
        }
    }
    /*check that label is not a register*/
    if (label[0] == 'r' && strlen(label) == 2)
    {
        if (isdigit(label[1]))
        {
            if (label[1] < '8' && label[1] >= '0')
            {
                exception[0] = 17;
            }
        }
    }
    if (exception[0] != 0)
    {
        return NULL;
    }
    return label;
}

void validateEntry(char *line, symbol_t **head_ref, int exception[])
{
    char *token, *delim = " \t\n";
    token = strtok(line, delim);
    if (modifySymbol(token, head_ref, entry) != 0)
    {
        /*throw exception, .entry label defenition isn't found in file*/
        exception[0] = 24;
    }
    token = strtok(NULL, delim);
    if (token != NULL)
    {
        /*throw exception, too many operands*/
        exception[0] = 12;
    }
    else
    {
        free(token);
    }
}

void phraseExtern(char *line, symbol_t **head_ref, int exception[])
{
    char *token, *label, *delim = " \t\n";
    token = strtok(line, delim);
    label = validateLabel(line, head_ref, 2, exception);
    if (label == NULL)
    {
        free(label);
    }
    else
    {
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            /*throw exception, too many operands*/
            exception[0] = 12;
        }
        else
        {
            free(token);
        }
    }
    if (token == NULL)
    {
        free(token);
    }
}

int parse_file(FILE *fptr, int pass, symbol_t **head_ref, machine_code map[], int IC, int exception[])
{
    char line[80 + 1];
    char *tok_ptr, *ret, *label, *token = NULL;
    int isLabel, isException, opNum, line_num = 0, DC = 0, wordCount = 0, i = 0;
    if (pass == 2)
    {
        DC = IC;
    }
    while (1)
    {
        if (isLabel == 1 && pass == 2)
        {
            free(label);
        }
        isLabel = 0;
        wordCount = 0;
        line_num++;
        if ((ret = fgets(line, 80, fptr)) == NULL)
            break;
        line[80] = 0;

        tok_ptr = line;
        if (strlen(line) == 80)
        {
            fprintf(stderr,
                    "line %d: line is too long. ignoring line ...\n", line_num);
            continue;
        }
        while (1)
        {

            token = parse_token(tok_ptr, " \n\t$,", &tok_ptr, NULL);
            wordCount++;
            opNum = -1;
            /* blank line or comment begins here. go to the next line */
            if (token == NULL || *token == '#' || *token == ';')
            {
                if (isLabel == 1)
                {
                    isException = 1;
                    /*throw exception, missing parameters on that line*/
                    exception[0] = 14;
                    getException(exception[0], line_num);
                    exception[0] = 0;
                }
                free(token);
                break;
            }
            if (pass == 1)
            {
                if (wordCount == 1)
                {
                    if (token[(strlen(token) - 1)] == ':')
                    {
                        isLabel = 1;
                        label = validateLabel(token, head_ref, pass, exception);
                        if (label == NULL)
                        {
                            isException = 1;
                            getException(exception[0], line_num);
                            exception[0] = 0;
                            free(token);
                            break;
                        }
                    }
                }
                if ((isLabel == 1 && wordCount == 2) || (isLabel == 0 && wordCount == 1))
                {
                    /*get operation number*/
                    for (i = 0; i < sizeof(opMap) / sizeof(opMap[0]); i++)
                    {
                        if (!memcmp(token, opMap[i].opCodeName, MIN(strlen(opMap[i].opCodeName), strlen(label))))
                        {
                            opNum = i;
                            break;
                        }
                    }

                    if ((strlen(token) > 2) && token[0] == '.')
                    {
                        switch (token[1])
                        {
                        case 'e':
                            if (token[2] == 'x')
                            {
                                if (!memcmp(token, ".extern", MIN(strlen(token), 7)))
                                {
                                    phraseExtern(tok_ptr, head_ref, exception);
                                    if (exception[0] == 0)
                                    {
                                        createSymbol(tok_ptr, line_num, external, head_ref);
                                    }
                                    if (isLabel == 1)
                                    {
                                        printf("Warning, defining label followed by .extern may cause unexpected behavior, ignoring that label.\n");
                                        free(label);
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    /*invalid format not equal .extern and not an operation*/
                                    exception[0] = 25;
                                }
                            }
                            else if (token[2] == 'n')
                            {
                                if (!memcmp(token, ".entry", MIN(strlen(token), 6)))
                                {
                                    if (1)
                                    {
                                        if (isLabel == 1)
                                        {
                                            printf("Warning, defining label followed by .entry may cause unexpected behavior, ignoring that label.\n");
                                            free(label);
                                            break;
                                        }
                                    }
                                    break;
                                }
                                else
                                {
                                    /*invalid format not equal .entry and not an operation*/
                                    exception[0] = 25;
                                }
                            }
                        default:
                            if (!memcmp(token, ".data", MIN(strlen(token), 5)) || !memcmp(token, ".string", MIN(strlen(token), 7)))
                            {
                                if (isLabel == 1)
                                {
                                    createSymbol(label, DC, relocatable, head_ref);
                                    free(label);
                                }
                                break;
                            }
                            /*invalid format not an instruction line or an operation line*/
                            exception[0] = 25;
                            break;
                        }
                        if (exception[0] == 0)
                        {
                            break;
                        }
                    }
                    if (opNum > -1)
                    {
                        if (isLabel == 1)
                        {
                            createSymbol(label, IC, relocatable, head_ref);
                        }
                        IC = (1 + phraseOperation(tok_ptr, opNum, head_ref, map, IC, exception, line_num));
                    }
                    if (exception[0] != 0)
                    {
                        isException = 1;
                        getException(exception[0], line_num);
                        if (isLabel == 1)
                        {
                            free(label);
                        }
                        exception[0] = 0;
                        free(token);
                        break;
                    }
                    if (opNum == -1 && exception[0] == 0)
                    {
                        /*invalid format not an operation and not an instruction line*/
                        exception[0] = 25;
                        getException(exception[0], line_num);
                        if (isLabel == 1)
                        {
                            free(label);
                        }
                        exception[0] = 0;
                        free(token);
                        break;
                    }
                    break;
                }
            }
            if (pass == 2)
            {
                if (token[(strlen(token) - 1)] == ':')
                {
                    isLabel = 1;
                    label = malloc(strlen(token));
                    token[(strlen(token) - 1)] = 0;
                    strcpy(label, token);
                }
                if (!memcmp(token, ".entry", MIN(strlen(token), 6)))
                {
                    validateEntry(tok_ptr, head_ref, exception);
                    if (exception[0] != 0)
                    {
                        isException = 1;
                        getException(exception[0], line_num);
                        exception[0] = 0;
                        free(token);
                        break;
                    }
                    break;
                }
                if (strlen(token) > 2 && token[0] == '.')
                {
                    switch (token[1])
                    {
                    case 'd':
                        if (!memcmp(token, ".data", MIN(strlen(token), 5)))
                        {
                            DC = (validateData(tok_ptr, map, DC, exception));
                            if (isLabel == 1)
                            {
                                updateSymbol(label, DC, head_ref);
                            }
                        }
                        break;
                    case 's':
                        if (!memcmp(token, ".string", MIN(strlen(token), 7)))
                        {
                            DC = (validateString(tok_ptr, map, DC, exception));
                            if (isLabel == 1)
                            {
                                updateSymbol(label, DC, head_ref);
                            }
                        }
                        break;
                    default:
                        if (!memcmp(token, ".extern", MIN(strlen(token), 7)))
                        {
                            break;
                        }
                        else
                        {
                            exception[0] = 25;
                            break;
                        }
                    }
                }
                if (exception[0] != 0)
                {
                    isException = 1;
                    getException(exception[0], line_num);
                    exception[0] = 0;
                    free(token);
                    break;
                }
                if (wordCount == 2)
                {
                    break;
                }
            }
        }
    }
    if (isException == 1)
    {
        exception[0] = 1;
    }
    if (pass == 1)
    {
        return IC;
    }
    return DC;
}
