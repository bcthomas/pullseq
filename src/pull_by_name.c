#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "pull_by_name.h"
#include "hash.h"
#include "file_read.h"
#include "global.h"
#include "output.h"
#include "kseq.h"
#include "uthash.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

extern char const *progname;
extern int verbose_flag;

int pull_by_name(char *input_file, char *names_file, int min, int max, int length, int exclude, int convert) {
	FILE *fp;
	int i,l,capacity=80;
	int count=0,excluded=0;
	int is_fasta = -1;
	char *fasta_name;
	char *line;
	kseq_t *seq;

	fp = fopen(names_file,"r");
	if (!fp) {
		fprintf(stderr,"%s - failed to open file %s\n",progname,names_file);
		exit(EXIT_FAILURE);
	}

	/* get some space for the line */
	line = malloc(sizeof(char) * capacity); /* get memory allocated */
	if (!line) {
		fprintf(stderr, "%s - line malloc: %s\n",progname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while((i = getl(&line,fp)) != -1) {
		fasta_name = parse_name(line);
		if (fasta_name) {
			add_name(fasta_name);             /* add fasta_name to hash */
		}
	}

	free(line); /* free up line */
	fclose(fp); /* close file */

	if (verbose_flag) {
		fprintf(stderr,"\n");
		fprintf(stderr,"done reading from %s ",names_file);
		fprintf(stderr, "(%d entries)\n", hash_key_count());
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
	kseq_rewind(seq); /* rewind to beginning for main loop */

	if (verbose_flag) {
		if (is_fasta)
			fprintf(stderr, "Input is FASTA format\n");
		else
			fprintf(stderr, "Input is FASTQ format\n");
	}

	/* search through list and see if this header matches */
	while((l = kseq_read(seq)) >= 0) {
		if (exclude == 0) { /* INCLUDE names from names file */
			if (find_name(seq->name.s)) {            /* found name in list */
				if (min > 0 && max > 0) { /* got a min and max */
					if (seq->seq.l >= min && seq->seq.l <= max) {
						count++;
						if (convert)
								is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
						else 
								is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
					}
				} else if (min > 0 || max > 0) { /* either  min or max is 0 */
					if (min > 0 && seq->seq.l >= min) {
						count++;
						if (convert)
								is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
						else 
								is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
					} else if (max > 0 && seq->seq.l <= max) {
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
		} else { /* EXCLUDE names from names file */
			if (find_name(seq->name.s)) {            /* found name in list */
				excluded++;
			} else {
				if (min > 0 && max > 0) { /* got a min and max */
					if (seq->seq.l >= min && seq->seq.l <= max) {
						count++;
						if (convert)
								is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
						else 
								is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
					}
				} else if (min > 0 || max > 0) { /* either  min or max is 0 */
					if (min > 0 && seq->seq.l >= min) {
						count++;
						if (convert)
								is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
						else 
								is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
					} else if (max > 0 && seq->seq.l <= max) {
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
		}
	}
	kseq_destroy(seq);
	gzclose(fp); /* done reading file */

	delete_hash(); /* free the list nodes */

	if (verbose_flag) {
		if (exclude)
			fprintf(stderr,"Excluded %i entries\n",excluded);
	}
	return count;
}
