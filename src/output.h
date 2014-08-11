#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "global.h"

void print_fasta_seq(kseq_t *seq,int n);
void print_fastq_seq(kseq_t *seq);
void print_fasta(FILE *fp,char *name, char *comment, char *seq);
#endif
