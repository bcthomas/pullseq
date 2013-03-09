#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>
#include <getopt.h>

#include "global.h"
#include "cmpseq.h"
#include "seqdiff.h"

extern int verbose_flag;
#define VERSION 0.1

void show_usage(int status) {
	fprintf(stderr, "seqdiff - a bioinformatics tool for comparing sequences in two files\n");
	fprintf(stderr, "\n(Written by bct - 2013)     ");
	/*  fprintf(stderr, "search method: %s",PULLSEQ_SORTMETHOD); */
	fprintf(stderr, "\nUsage:\n"
			"%s <first input fasta/fastq file> <second fasta/fastq file>\n\n", progname);
	fprintf(stderr, "  Options:\n"
			"    -1, --first,      First sequence file\n"
			"    -2, --second,     Second sequence file\n");
	fprintf(stderr, "    -a, --a_output,   File name for uniques from first file\n"
			"    -b, --b_output,   File name for uniques from second file\n"
			"    -c, --c_output,   File name for common entries\n");

	fprintf(stderr, "    -d, --headers,    Compare headers instead of sequences (default: false)\n"
			"    -g, --include_gt, If comparing headers, include '>' in output? (default: false)\n");
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
	int use_headers = 0;
	int use_gt = 0;

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
			{"include_gt", no_argument, 0, 'g'},
			{"headers",    no_argument, 0, 'd'},
			{"verbose",    no_argument, 0, 'v'},
			{"version",    no_argument, 0, 'V'},
			{"help",       no_argument, 0, 'h'},
			{"first",      required_argument, 0, '1'},
			{"second",     required_argument, 0, '2'},
			{"a_output",   required_argument, 0, 'a'},
			{"b_output",   required_argument, 0, 'b'},
			{"c_output",   required_argument, 0, 'c'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "vVh?dg1:2:a:b:c:", long_options, &option_index);

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

			case 'd':
				use_headers = 1;
				break;

			case 'g':
				use_gt = 1;
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

		if (use_headers)
			fprintf(stderr,"Processing will be done using headers, not sequences\n");
		else
			fprintf(stderr,"Processing will be done using sequences\n");
		if (use_gt && use_headers)
			fprintf(stderr,"'>' will be included in output\n");
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

	if (use_headers == 0 && use_gt == 1) {
		fprintf (stderr, "WARNING: Ignoring -g option since -d option is not set.\n");
	}

	results->first_file = first_file;
	results->second_file = second_file;
	results->a_output_file = a_output_file;
	results->b_output_file = b_output_file;
	results->c_output_file = c_output_file;
	results->use_headers = use_headers;
	results->use_gt = use_gt;
	if (a_output_file != NULL && b_output_file != NULL && c_output_file != NULL) {
		cmpseq_with_output(results);
	} else {
		cmpseq(results);
	}

	/* report results */
	fprintf(stdout, "first_file_total = %d\n", results->first_file_total);
	fprintf(stdout, "first_file_uniq = %d\n",  results->first_file_uniq);
	fprintf(stdout, "second_file_total = %d\n",results->second_file_total);
	fprintf(stdout, "second_file_uniq = %d\n", results->second_file_uniq);
	fprintf(stdout, "common = %d\n",           results->common);

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
