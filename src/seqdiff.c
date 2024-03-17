#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>
#include <getopt.h>

#include "global.h"
#include "cmpseq.h"
#include "seqdiff.h"

int verbose_flag;
char const *progname;

#define VERSION 0.1

void show_usage(int status) {
	fprintf(stderr, "seqdiff - a bioinformatics tool for comparing sequences in two files\n");
	fprintf(stderr, "\n(Written by bct - 2013)     ");
	/*  fprintf(stderr, "search method: %s",PULLSEQ_SORTMETHOD); */
	fprintf(stderr, "\nUsage:\n"
			"%s -1 <first input fasta/fastq file> -2 <second fasta/fastq file>\n\n", progname);
	fprintf(stderr, "  Options:\n"
			"    -1, --first,      First sequence file (required)\n"
			"    -2, --second,     Second sequence file (required)\n");
	fprintf(stderr, "    -a, --a_output,   File name for uniques from first file\n"
			"    -b, --b_output,   File name for uniques from second file\n"
			"    -c, --c_output,   File name for common entries\n");

	fprintf(stderr, "    -d, --headers,    Compare headers instead of sequences (default: false)\n"
			"    -s, --summary, Just show summary stats? (default: false)\n");
	fprintf(stderr, "    -h, --help,       Display this help and exit\n"
			"    -v, --verbose,    Print extra details during the run\n"
			"    --version,        Output version information and exit\n\n");
	exit(status);
}

int main(int argc, char *argv[]) {
	int c;                          /* character for getopt processing */
	/* command argument variables */
	char *first_file = NULL;
	char *second_file = NULL;
	char *a_output_file = NULL;
	char *b_output_file = NULL;
	char *c_output_file = NULL;
	FILE *a_output_fp = NULL;
	FILE *b_output_fp = NULL;
	FILE *c_output_fp = NULL;
	int use_header = 0;
	int only_summarize = 0;

	/* internal variables */
	seqdiff_results_t *results;
	results = seqdiff_results_init();

	extern char *optarg; /* external from getopt */

	verbose_flag = 0; /* assume not verbose */
	progname = argv[0]; /* capture the program name */
	if (argc < 2) {
		show_usage(EXIT_FAILURE);
	}

	while(1) {
		static struct option long_options[] =
		{
			{"verbose",    no_argument, 0, 'v'},
			{"version",    no_argument, 0, 'V'},
			{"help",       no_argument, 0, 'h'},
			{"summary",    no_argument, 0, 's'},
			{"headers",    no_argument, 0, 'd'},
			{"first",      required_argument, 0, '1'},
			{"second",     required_argument, 0, '2'},
			{"a_output",   required_argument, 0, 'a'},
			{"b_output",   required_argument, 0, 'b'},
			{"c_output",   required_argument, 0, 'c'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "vVh?sd1:2:a:b:c:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'v':
				verbose_flag = 1;
				break;

			case 'V':
				/* version */
				printf("Version is %f\n",VERSION);
				break;

			case 'h':
				show_usage(EXIT_FAILURE);
				break;

			case '?':
				/* getopt_long already printed an error message. */
				break;

			case 's':
				only_summarize = 1;
				break;

			case 'd':
				use_header = 1;
				break;

			case '1':
				first_file = (char*) malloc(strlen(optarg)+1);
				strcpy(first_file,optarg);
				break;

			case '2':
				second_file = (char*) malloc(strlen(optarg)+1);
				strcpy(second_file,optarg);
				break;

			case 'a':
				a_output_file = (char*) malloc(strlen(optarg)+1);
				strcpy(a_output_file,optarg);
				break;

			case 'b':
				b_output_file = (char*) malloc(strlen(optarg)+1);
				strcpy(b_output_file,optarg);
				break;

			case 'c':
				c_output_file = (char*) malloc(strlen(optarg)+1);
				strcpy(c_output_file,optarg);
				break;

			default:
				abort ();
		}
	}

	/* Instead of reporting '--verbose'
	   and '--brief' as they are encountered,
	   we report the final status resulting from them. */
	if (verbose_flag) {
		fprintf(stderr, "verbose flag is set\n");
		fprintf(stderr,"First file is %s\n", first_file);
		fprintf(stderr,"Second file is %s\n", second_file);
		if (a_output_file != NULL && a_output_file != NULL && a_output_file != NULL) {
			fprintf(stderr, "Output will be written to files:\n");
			fprintf(stderr, "  first file uniques: %s\n", a_output_file);
			fprintf(stderr, "  second file uniques: %s\n", b_output_file);
			fprintf(stderr, "  common to both input files: %s\n", c_output_file);
		} else
			fprintf(stderr,"No output files will be generated\n");

		if (use_header)
			fprintf(stderr,"Processing will be done using headers, not sequences\n");
		else
			fprintf(stderr,"Processing will be done using sequences\n");
		if (only_summarize)
			fprintf(stderr,"Only showing summary information\n");
	}

	/* check validity of given argument combination */
	if (! first_file) {
		fprintf (stderr, "Error: First sequence file is required.\n");
		return EXIT_FAILURE;
	}

	if (! second_file) {
		fprintf (stderr, "Error: First sequence file is required.\n");
		return EXIT_FAILURE;
	}

	results->first_file = first_file;
	results->second_file = second_file;
	results->use_header = use_header;
	results->only_summarize = only_summarize;
	if (a_output_file != NULL && b_output_file != NULL && c_output_file != NULL) {
		results->a_output_fp = fopen(a_output_file,"w+");
		if (!results->a_output_fp) {
			fprintf(stderr,"%s - failed to open file %s\n",progname,a_output_file);
			exit(EXIT_FAILURE);
		}
		results->b_output_fp = fopen(b_output_file,"w+");
		if (!results->b_output_fp) {
			fprintf(stderr,"%s - failed to open file %s\n",progname,b_output_file);
			exit(EXIT_FAILURE);
		}
		results->c_output_fp = fopen(c_output_file,"w+");
		if (!results->c_output_fp) {
			fprintf(stderr,"%s - failed to open file %s\n",progname,c_output_file);
			exit(EXIT_FAILURE);
		}
	}

	/* do the comparison */
	cmpseq(results);

	if (a_output_file != NULL && b_output_file != NULL && c_output_file != NULL) {
		fclose(results->a_output_fp);
		fclose(results->b_output_fp);
		fclose(results->c_output_fp);
	}

	/* report results */
	fprintf(stderr, " first_file_total = %d\n", results->first_file_total);
	fprintf(stderr, "  first_file_uniq = %d\n", results->first_file_uniq);
	fprintf(stderr, "second_file_total = %d\n", results->second_file_total);
	fprintf(stderr, " second_file_uniq = %d\n", results->second_file_uniq);
	fprintf(stderr, "           common = %d\n",           results->common);

	free(first_file);
	free(second_file);
	if (a_output_file != NULL)
		free(a_output_file);
	if (b_output_file != NULL)
		free(b_output_file);
	if (c_output_file != NULL)
		free(c_output_file);

	/* clean up */
	seqdiff_results_destroy(results);
	fclose(stderr);
	fclose(stdout);
	fclose(stdin);
	return EXIT_SUCCESS;
}
