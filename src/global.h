#ifndef GLOBAL_H
#define GLOBAL_H

#define PULLSEQ_VERSION "1.0.2"

#define _POSIX_C_SOURCE 200809L
/*
#ifdef DEBUG
#define DEBUGP(x, args...) fprintf(stderr, " [%s(), %s:%u]\n" x, __FUNCTION__, __FILE__,__LINE__, ## args)
#else
#define DEBUGP(x, args...)
#endif
*/

#include "zlib.h"
#include "kseq.h"
#include "uthash.h"

#define BUFFER_SIZE 65535

__KS_TYPE(gzFile)
__KS_BASIC(gzFile, BUFFER_SIZE)
__KSEQ_TYPE(gzFile)
__KSEQ_BASIC(static, gzFile)

char const *progname;
int QUALITY_SCORE;
int verbose_flag;

#endif
