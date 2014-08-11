#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "global.h"
#include "pull_by_name.h"
#include "hash.h"
#include "file_read.h"
#include "size_filter.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

extern char const *progname;
extern int verbose_flag;

int pull_by_name(char *input_file, FILE *names_fp, int min, int max, int length, int exclude, int convert, int just_count) {
	gzFile fp;
	int i,l,capacity=80;
	int count=0,excluded=0;
	int is_fasta = -1;
	char *fasta_name;
	char *line;
	kseq_t *seq;

	/* get some space for the line */
	line = malloc(sizeof(char) * capacity); /* get memory allocated */
	if (!line) {
		fprintf(stderr, "%s - line malloc: %s\n",progname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while((i = getl(&line,names_fp)) != -1) {
		fasta_name = parse_name(line);
		if (fasta_name) {
			add_name(fasta_name);             /* add fasta_name to hash */
		}
	}

	free(line); /* free up line */

	if (verbose_flag) {
		fprintf(stderr,"\n");
		fprintf(stderr,"done reading from input (%d entries)\n", hash_key_count());
	}
	/*print_hash();*/


	/* open fasta file */
	fp = gzopen(input_file,"r");
	if (!fp) {
		fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,input_file);
		exit(EXIT_FAILURE);
	}

	seq = kseq_init(fp); /* initialize kseq */

	/* determine file type */
	l = kseq_read(seq); /* read the first sequence */
	is_fasta = seq->qual.s == NULL ? 1 : 0;
	gzrewind(fp); /* rewind to beginning for main loop */
	kseq_rewind(seq);

	if (verbose_flag) {
		if (is_fasta)
			fprintf(stderr, "Input is FASTA format\n");
		else
			fprintf(stderr, "Input is FASTQ format\n");
	}

	/* search through list and see if this header matches */
	while((l = kseq_read(seq)) >= 0) {
		if (exclude == 0) { /* INCLUDE names from names file */
			if (find_name(seq->name.s))            /* found name in list */
				count += size_filter(seq, is_fasta, min, max, length, convert, just_count);
			else
				excluded++;
		} else { /* EXCLUDE names from names file */
			if (find_name(seq->name.s))            /* found name in list */
				excluded++;
			else
				count += size_filter(seq, is_fasta, min, max, length, convert, just_count);
		}
	}
	kseq_destroy(seq);
	gzclose(fp); /* done reading file */

	delete_hash(); /* free the list nodes */

	if (just_count) {
		fprintf(stdout, "Total output: %i\n", count);
		if (exclude)
			fprintf(stdout, "Total excluded: %i\n", excluded);
	}

	if (verbose_flag) {
		fprintf(stderr,"Processed %i entries\n",count);
		if (exclude)
			fprintf(stderr,"Excluded %i entries\n",excluded);
	}
	return count;
}
