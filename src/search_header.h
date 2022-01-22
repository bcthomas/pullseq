#ifndef SEARCH_HEADER_H
#define SEARCH_HEADER_H

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#define MAX_CAPTURE_COUNT 30

int search_header(pcre2_code *re, char *str);

#endif
