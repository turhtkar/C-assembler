#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preAssembler.h"
#include "exception.h"
#include "macro.h"

char *readline(FILE *fp, char **buffer)
{
    int ch, counter = 0;
    size_t buflen = 0, nchar = 80;
    char *tmp;

    *buffer = malloc(nchar); /* allocate buffer nchar in length */
    if (*buffer == NULL)
    {
        fprintf(stderr, "readline() error: virtual memory exhausted.\n");
        return NULL;
    }

    if (!*buffer)
    {
        fprintf(stderr, "readline() error: virtual memory exhausted.\n");
        return NULL;
    }

    while ((ch = fgetc(fp)) != '\n' && ch != EOF && ch != '\r')
    {
        counter++;
        (*buffer)[buflen++] = ch;
        if (buflen + 1 >= nchar)
        { /* realloc */
            nchar *= 2;
            tmp = realloc(*buffer, nchar);
            if (!tmp)
            {
                fprintf(stderr, "error: realloc failed, returning partial buffer.\n");
                break;
            }
            *buffer = tmp;
        }
    }
    (*buffer)[buflen] = '\0'; /* nul-terminate */

    if (buflen == 0 && ch == EOF)
    { /* return NULL if nothing read */
        if (counter == 0)
        {
            *buffer = NULL;
            return NULL;
        }
        else
        {
            free(*buffer);
            *buffer = NULL;
        }
        return NULL;
    }

    return *buffer;
}

void preAssmble(char *input_file_name, char *output_file_name, int exception[])
{
    FILE *input_file, *output_file;
    macro_t *head = NULL;
    macro_t *found = NULL;
    char *line = NULL;
    int isMcr = 0, counter = 0, i = 0;
    char *mcr = "mcr";
    char *endmcr = "endmcr";
    char *tmp_line = NULL;
    char *word = NULL;
    head = create_macro(head);

    input_file = fopen(input_file_name, "r"); /* Open the file in read mode*/
    if (input_file == NULL)
    { /* Check for errors*/
        exception[0] = 1;
        fclose(input_file);
        return;
    }
    output_file = fopen(output_file_name, "w+"); /* Open the file in write mode*/
    if (output_file == NULL)
    { /* Check for errors*/
        exception[0] = 2;
        fclose(output_file);
        return;
    }
    while (readline(input_file, &line) != NULL)
    { /* Read a line of the file*/
        counter++;
        if (line != NULL)
        {
            tmp_line = malloc(strlen(line));
            if (tmp_line == NULL)
            {
                fprintf(stderr, "failed to allocate memory for string");
                return;
            }
        }
        strcpy(tmp_line, line);
        word = strtok(tmp_line, " \r\n");
        while (word != NULL)
        {
            if (isMcr == 1)
            {
                if (strncmp(word, endmcr, strlen(mcr)) == 0)
                {
                    isMcr = 0;
                    head = create_macro(head);
                    break;
                }
                else
                {
                    add_line(head, line);
                    break;
                }
            }
            else if ((found = search_macro(head, word)) != NULL)
            {
                for (i = 0; i < found->lines; i++)
                {
                    fprintf(output_file, "\n");
                    fputs(found->text[i], output_file);
                }
                break;
            }
            else if (strncmp(word, mcr, strlen(mcr)) == 0)
            {
                word = strtok(NULL, " \r\n");
                strcpy(head->name, word);
                isMcr = 1;
                break;
            }
            else
            {
                fprintf(output_file, "\n");
                fputs(line, output_file);
                break;
            }
        }
    }
    free(tmp_line);
    fclose(input_file);
    fclose(output_file);
}
