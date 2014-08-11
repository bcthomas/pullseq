#ifndef PULL_BY_SIZE_H
#define PULL_BY_SIZE_H

#include <pcre.h>

int pull_by_re(char *input_file, pcre *re, pcre_extra *re_extra, int min, int max, int length, int exclude, int convert, int just_count);

#endif
