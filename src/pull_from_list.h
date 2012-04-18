#ifndef __PULL_FROM_LIST_H__
#define __PULL_FROM_LIST_H__
#endif

#define _POSIX_C_SOURCE 200809L

#include "kseq.h"

#define BUFFER_SIZE 4096
__KS_TYPE(gzFile)
__KS_BASIC(gzFile, BUFFER_SIZE)
__KSEQ_TYPE(gzFile)
__KSEQ_BASIC(gzFile)
__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

void pull_from_list(char *input_fasta, char *names_file, int min, int max);

