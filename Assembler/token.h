#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/**
 * @brief a function to tokenize a line, and reset it's offsets
 * 
 * @param in_str the input string 
 * @param delim the delimiter characters
 * @param out_str the output string
 * @param delim_char the delimiter characters
 * @return the tokenized line 
 */
char *parse_token(char *in_str, char *delim, char **out_str, char *delim_char)
{
  int len;
  char *ptr, *tptr, *token;

  /* Bypass leading whitespace delimiters */
  len = strspn(in_str, delim);
  ptr = (in_str + len);

  /* Get end of token */
  tptr = strpbrk(ptr, delim);
  if (tptr == NULL)
    return (NULL);
  len = tptr - ptr;

  if (delim_char != NULL)
    *delim_char = *tptr;

  /* Create output string */
  *out_str = tptr + 1; /* go past the delimiter */

  /* Create token */
  token = (char *)malloc(len + 1);
  if (token == NULL)
    return (NULL);
  memcpy(token, ptr, len);
  token[len] = (char)0;
  return (token);
}
