#ifndef GLOBAL_H
#define GLOBAL_H

#define _POSIX_C_SOURCE 200809L

#ifdef DEBUG
#define DEBUGP(x, args...) fprintf(stderr, " [%s(), %s:%u]\n" x, __FUNCTION__, __FILE__,__LINE__, ## args)
#else
#define DEBUGP(x, args...)
#endif

#include "kseq.h"
#include "zlib.h"
#define BUFFER_SIZE 4096

__KS_TYPE(gzFile)
__KS_BASIC(gzFile, BUFFER_SIZE)
__KSEQ_TYPE(gzFile)
__KSEQ_BASIC(gzFile)

char const *progname;
int verbose_flag;
#endif
