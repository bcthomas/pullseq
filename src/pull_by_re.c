#define PCRE2_CODE_UNIT_WIDTH 8

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>
#include <pcre2.h>

#include "pull_by_re.h"
#include "file_read.h"
#include "global.h"
#include "size_filter.h"
#include "search_header.h"


__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ(static)

extern char const *progname;
extern int verbose_flag;

int pull_by_re(char *input_file, char *aStrRegex, int min, int max, int length, int exclude, int convert, int just_count) {
	gzFile fp;
	int count=0,l;
	int excluded = 0;
	int is_fasta = 0; /* assume fastq */
	kseq_t *seq;

	/* pcre2 variables */
	pcre2_code *re; // the regex object
	PCRE2_SIZE erroroffset;
	int errornumber;

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

	/* initialize the re */
	re = pcre2_compile(
			aStrRegex,             /* the pattern */
			PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
			0,                     /* default options */
			&errornumber,          /* for error num */
			&erroroffset,          /* err offset */
			NULL);                 /* default compile context */

	if (re == NULL) {
		  PCRE2_UCHAR buffer[256];
		  pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		  fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
		  exit(EXIT_FAILURE);
	}

	/* search through list and see if this header matches */
	while((l = kseq_read(seq)) >= 0) {
		if (exclude) {
			if (search_header(re, seq->name.s) || search_header(re, seq->comment.s))
				excluded++;
			else {
				/* regex doesn't match, so check size/print */
				count += size_filter(seq, is_fasta, min, max, length, convert, just_count);
			}
		} else {
			if (search_header(re, seq->name.s) || search_header(re, seq->comment.s)) {
				/* regex matches so check size/print */
				count += size_filter(seq, is_fasta, min, max, length, convert, just_count);
			} else
				excluded++;
		}
	} /* end of seq traversal */

	/* tear down re */
	pcre2_code_free(re); /* free up the re */

	kseq_destroy(seq);
	gzclose(fp); /* done reading file so close */

	if (just_count) {
		fprintf(stdout, "Total output: %i\n", count);
		fprintf(stdout, "Total excluded: %i\n", excluded);
	}
	return count;
}
