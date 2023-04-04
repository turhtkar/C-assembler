/**
 * @file Assembler.c
 * @author your name (https://github.com/turhtkar)
 * @brief The main file of the assembler, it gets an assembly file and creates the object,
 * entry and external files in accordance with the assembly file for the given instructions
 * @version 0.1
 * @date 2023-03-14
 *
 * @copyright Copyright (c) 2023
 *
 * @note this version is made to be supported by ansi 90 standard and it also comes with a makefile and 2 assembly files to test the assembler
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <malloc.h>
#include "Assembler.h"
#include "preAssembler.h"
#include "phaser.h"
#include "symbol.h"
#include "machineCode.h"
#include "exception.h"
#include "macro.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void strrev(char *str)
{
    int len = strlen(str), i = 0, j = len - 1;
    if (str == NULL)
    {
        return;
    }

    if (len <= 1)
    {
        return;
    }

    while (i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int getFile(char *fileName, char *file, int code)
{
    if (strlen(fileName) < 253)
    {
        strcpy(file, fileName);
        switch (code)
        {
        case 1:
            strcat(file, ".as");
            break;
        case 2:
            strcat(file, ".am");
            break;
        case 3:
            strcat(file, ".ob");
            break;
        case 4:
            strcat(file, ".ent");
            break;
        case 5:
            strcat(file, ".ext");
            break;
        }
    }
    else
    {
        return 1;
    }
    return 0;
}

int checkEntExt(symbol_t **head_ref)
{
    symbol_t *current = *head_ref;
    int res = 0;
    while (current != NULL)
    {
        if (current->type == entry)
        {
            if (res == 2)
            {
                return 3;
            }
            else
            {
                res = 1;
            }
        }
        if (current->type == external)
        {
            if (res == 1)
            {
                return 3;
            }
            else
            {
                res = 2;
            }
        }
        current = current->next;
    }
    return res;
}

void writeObjectFile(char *fileName, int IC, machine_code map[])
{
    int i = 0, maxLines = IC;
    char machineCode[14];
    FILE *file = fopen(fileName, "w+"); /* Open the file in write mode*/
    if (file == NULL)
    {
        fprintf(stderr, "error opening output file %s", fileName);
    }
    for (; i < maxLines; i++)
    {
        fprintf(file, "%d", i + 100);
        fputs("\t", file);
        translateToMc(map[i].bin_code, machineCode);
        strrev(machineCode);
        fputs(machineCode, file);
        fputs("\n", file);
    }
    fclose(file);
}

void writeEntryFile(char *fileName, symbol_t **head)
{
    symbol_t *current = *head;
    FILE *file = fopen(fileName, "w+"); /* Open the file in write mode*/
    if (file == NULL)
    {
        fprintf(stderr, "error opening output file %s", fileName);
    }
    while (current != NULL)
    {
        if (current->type == entry)
        {
            /*I wasn't sure if the incrementing by 100 to get the line in the ob file or the actual postion from the symbols map, so I printed the value + 100 and wrote this note*/
            fprintf(file, "%s\t%d\n", current->name, (current->value + 100));
        }
        current = current->next;
    }
    fclose(file);
}

void writeExternFile(char *fileName, symbol_t **head, machine_code map[])
{
    symbol_t *current = *head;
    FILE *file = fopen(fileName, "w+"); /* Open the file in write mode*/
    if (file == NULL)
    {
        fprintf(stderr, "error opening output file %s", fileName);
    }
    while (current != NULL)
    {
        if (current->type == external)
        {
            /*we go over this conditon to see that this external label is not the external labed def, since those labels who werent defined does not   		     have the line number as the label, but their position in the map which is used to write the ob file*/
            if (memcmp(current->name, map[current->value].word, MAX(strlen(current->name), strlen(map[current->value].word))) == 0)
            {
                fprintf(file, "%s\t%d\n", current->name, map[current->value].lineNum);
            }
            else
            {
                /*this is the external label defention, since it's not included on the map and so the map at postion of it's value doesn't have the 			same label name aka map[].word, in fact it isn't neccsirly defined.*/
                fprintf(file, "%s\t%d\n", current->name, current->value);
            }
        }
        current = current->next;
    }
    fclose(file);
}
/**
 * @brief write the ob, ent and ext files
 *
 * @param fileName the file name
 * @param IC the instruction counter
 * @param head the head of the symbol table
 * @param map the map of the machine code
 */
int main(int argc, char *argv[])
{
    FILE *input_file;          /* Pointer to the input file*/
    char input_file_name[256]; /* Pointer to the input_file_name*/
    char output_file_name[256];
    int exception[1] = {0}, IC = 0, i = 0, isEntry = 0, isExteren = 0;
    symbol_t *head_ref = NULL;
    machine_code map[255];
    for (i = 1; i < argc; i++)
    {
        IC = 0;
        head_ref = NULL;
        isEntry = 0;
        isExteren = 0;
        memset(map, 0, sizeof(map));
        if (getFile(argv[i], input_file_name, 1) == 1)
        {
            fprintf(stderr, "file name can't be longer than 256 characters");
            continue;
        }
        if (exception[0] == 0)
        {
            switch (getFile(argv[i], output_file_name, 2))
            {
            case 1:
                fprintf(stderr, "error opening input file %s", input_file_name);
                break;
            case 2:
                fprintf(stderr, "error opening output file %s", output_file_name);
                break;
            }
        }
        /*I'm adding this print, just to make it easier on the developer to understand on which file the exception appeard*/
        printf("\n Assembling file %s \n", input_file_name);
        preAssmble(input_file_name, output_file_name, exception);
        if (i >= 1)
        {
            /*opening the first file in the arguments*/
            input_file = fopen(output_file_name, "r"); /* Open the file in read mode*/
        }
        else
        {
            /*opening the next file in the arguments*/
            freopen(output_file_name, "r", input_file);
        }
        if (input_file == NULL)
        {
            fprintf(stderr, "error opening input file %s", input_file_name);
            continue;
        }
        /*pass 1
        IC=parse_file(input_file,1,output_file, exception);*/
        IC = parse_file(input_file, 1, &head_ref, map, IC, exception);
        if (exception[0] != 0)
        {
            exception[0] = 0;
            fclose(input_file);
            continue;
        } /*pass 2*/
        rewind(input_file);
        IC = parse_file(input_file, 2, &head_ref, map, IC, exception);
        if (exception[0] != 0)
        {
            exception[0] = 0;
            fclose(input_file);
            continue;
        }
        else
        {
            /*close .am file since we don't read or write into it anymore*/
            fclose(input_file);
            /*check if there is an entry or extern for file writing
              and then relocate all the symbols*/
            switch (checkEntExt(&head_ref))
            {
            case 1:
                isEntry = 1;
                break;
            case 2:
                isExteren = 1;
                break;
            case 3:
                isEntry = 1;
                isExteren = 1;
                break;
            }
            relocateSymbol(&head_ref, map, IC, exception);
            if (exception[0] != 0)
            {
                continue;
            }
        }
        /*write the files
         write the object file*/
        if (getFile(argv[i], output_file_name, 3) == 1)
        {
            fprintf(stderr, "file name can't be longer than 256 characters");
            continue;
        }

        writeObjectFile(output_file_name, IC, map);

        if (isEntry == 1)
        {
            if (getFile(argv[i], output_file_name, 4) == 1)
            {
                fprintf(stderr, "file name can't be longer than 256 characters");
                continue;
            }
            writeEntryFile(output_file_name, &head_ref);
        }

        if (isExteren == 1)
        {
            if (getFile(argv[i], output_file_name, 5) == 1)
            {
                fprintf(stderr, "file name can't be longer than 256 characters");
                continue;
            }
            writeExternFile(output_file_name, &head_ref, map);
        }
    }
    return 0;
}
