#ifndef SEARCH_HEADER_H
#define SEARCH_HEADER_H

#include <pcre.h>

#define MAX_CAPTURE_COUNT 30

int search_header(pcre *re, pcre_extra *re_extra, char *str);

#endif
