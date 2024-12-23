#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "pull_by_size.h"
#include "file_read.h"
#include "global.h"
#include "size_filter.h"


__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ(static)


char const *progname;
int verbose_flag;


int pull_by_size(char *input_file, int min, int max,int length, int convert, int just_count) {
	gzFile fp;
	int count=0,l;
	int hit = 0;
	int excluded = 0;
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
		hit = size_filter(seq, is_fasta, min, max, length, convert, just_count);
		if (hit)
			count++;
		else
			excluded++;
	}
	kseq_destroy(seq);
	gzclose(fp); /* done reading file */

	if (just_count) {
		fprintf(stdout, "Total output: %i\n", count);
		fprintf(stdout, "Total excluded: %i\n", excluded);
	}
	return count;
}
