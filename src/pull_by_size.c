#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "pull_by_size.h"
#include "file_read.h"
#include "global.h"
#include "output.h"
#include "kseq.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

extern char const *progname;
extern int verbose_flag;

int pull_by_size(char *input_file, int min, int max,int length, int convert) {
	FILE *fp;
	int count=0,l;
	int is_fasta = 0; /* assume fastq */
	kseq_t *seq;

	/* open fasta file */
	fp = gzopen(input_file,"r");
	if (!fp) {
		fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,input_file);
		exit(EXIT_FAILURE);
	}

	seq = kseq_init(fp);

	/* determine file type */
	l = kseq_read(seq); /* read the first sequence */
	is_fasta = seq->qual.s == NULL ? 1 : 0;
	gzrewind(fp); 
	kseq_rewind(seq); /* rewind to beginning for main loop */

    if (verbose_flag) {
        if (is_fasta)
            fprintf(stderr, "Input is FASTA format\n");
        else
            fprintf(stderr, "Input is FASTQ format\n");
    }

	/* search through list and see if this header matches */
	while((l = kseq_read(seq)) >= 0) {
		if (min > 0 && max > 0) { /* got a min and max */
			if (l >= min && l <= max) {
				count++;
				if (convert)
					is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
				else
					is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
			}
		} else if (min > 0 || max > 0) { /* either  min or max is 0 */
			if (min > 0 && l >= min) {
				count++;
				if (convert)
					is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
				else
					is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
			} else if (max > 0 && l <= max) {
				count++;
				if (convert)
					is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
				else
					is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
			}
		} else {
			count++;
			if (convert)
				is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
			else
				is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
		}
	}
	kseq_destroy(seq);
	gzclose(fp); /* done reading file */

	return count;
}
