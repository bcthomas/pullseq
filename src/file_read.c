#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "file_read.h"

char *parse_name(char *line)
{
  char *word;
  char *delims = " \t\n"; /* space, tab, newline */
  word = strtok(line,delims);
  return word;
}

#define BUFSIZE 80
int getl(char **lineptr, FILE *fp) {
  char ch;
  ssize_t buf_pos = 0;
  ssize_t count = 2; /* Always buf_pos + 2 (see below). */
  size_t new_length = 0;
  size_t n = BUFSIZE;
  char *temp;

  if ((lineptr == NULL) || (fp == NULL)) {
    errno = EINVAL;
    return -1;
  }

  if (errno != 0)
    errno = 0;

  if ((*lineptr == NULL)) {
    *lineptr = malloc(n * sizeof(char));

    if (*lineptr == NULL) {
      return -1; /* Out of memory. */
    }
  }

  /*
   * There are buf_pos characters in the buffer.  When we read another
   * character, we want to store it, and we also need enough
   * room for a nul string. So we need to realloc as soon as our capacity
   * becomes less than buf_pos + 2.
   * Hence the variable "count" which always equals buf_pos + 2.
   */

  while ((ch = getc(fp)) != EOF) {
    if (errno != 0)
      return -1;

    if (count > n) { /* current chars read is going to blow our buffer - add more */
      new_length = n * 2; /* double the current buffer size */
      if (new_length <= n) { /* Overflow. */
        errno = ENOMEM;
        /* We couldn't store the character, */
        /* so put it back on the stream. */
        ungetc(ch, fp);
        return -1;
      }
      temp = (char *)realloc(*lineptr, new_length * sizeof(char)); /* realloc to a temp */
      if (temp == NULL) {
        ungetc(ch, fp);
        return -1;
      }
      n = new_length; /* set n to the new length we were able to get from system */
      *lineptr = temp; /* set line to this new temp string */
    }

    (*lineptr)[buf_pos++] = ch; /* set this char in the string at buf_pos and THEN increment buf_pos */

    if (ch == '\n') /* eol */
      break;

    if (count == SSIZE_MAX) { /* SSIZE_MAX is 32767 - posix def */
      /* We'll overflow ssize_t on the next round, since the return
       * type is SSIZE_T */
      errno = ENOMEM;
      return -1;
    }
    count++; /* increment ch count */
  }

  (*lineptr)[buf_pos] = '\0'; /* set last position to \0 */

  if (buf_pos == 0) { /* nothing in the file? */
    buf_pos = -1;
  }
  return buf_pos;
}

int getlx(char **iline,FILE *fp)
{
  char *line = *iline;
  char *newline = NULL;
  char *buf = NULL;
  char *eol = NULL;
  size_t capacity = sizeof(line); /* reasonable starting point for line length */
  size_t remaining = capacity;
  size_t used = 0;

  buf = line; /* point buf -> line */
  line[0] = '\0';

  /* read file  into buf */
  while (fgets(buf, remaining, fp)) {
    eol = strchr(buf, '\n'); /* locate first occurrence of '\n' */
    if (eol) { /* found a newline in the string */
      *eol = '\0'; /* replace the newline with the null character */
      break;
    } else {
      /* buffer was too small - enlarge it */
      used = buf + remaining - line;
      
      newline = realloc(line, capacity * 2);
      if (!newline) {
        fprintf(stderr, "getl - realloc: %s\n", strerror(errno));
        return -1;
      } else {
        line = newline;
      }

      buf = line + used - 1;
      capacity *= 2;
      remaining = capacity - used;
    }
  }

  if (errno) {
    fprintf(stderr, "getl - fgets: %s\n", strerror(errno));
  } else if (line[0]) {
    char *eol = strchr(buf, '\n');
    if (eol)
      *eol = '\0';
    /*buf = line;*/
    return strlen(line);
  }
  return -1;
}
