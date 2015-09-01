#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>
#include <getopt.h>
#include <pcre.h>

#include "global.h"
#include "pullseq.h"
#include "pull_by_name.h"
#include "pull_by_size.h"
#include "pull_by_re.h"

extern int QUALITY_SCORE;
extern int verbose_flag;

void show_usage(int status) {
	fprintf(stderr, "pullseq - a bioinformatics tool for manipulating fasta and fastq files\n");
	fprintf(stderr, "\nVersion: %s              Name lookup method: %s", PULLSEQ_VERSION, PULLSEQ_SORTMETHOD);
	fprintf(stderr, "\n(Written by bct - copyright 2012-2015)\n");
	fprintf(stderr, "\nUsage:\n");
	fprintf(stderr, " %s -i <input fasta/fastq file> -n <header names to select>\n\n", progname);
	fprintf(stderr, " %s -i <input fasta/fastq file> -m <minimum sequence length>\n\n", progname);
	fprintf(stderr, " %s -i <input fasta/fastq file> -g <regex name to match>\n\n", progname);
	fprintf(stderr, " %s -i <input fasta/fastq file> -m <minimum sequence length> -a <max sequence length>\n\n", progname);
	fprintf(stderr, " %s -i <input fasta/fastq file> -t\n\n", progname);
	fprintf(stderr, " cat <names to select from STDIN> | %s -i <input fasta/fastq file> -N\n\n", progname);

	fprintf(stderr, "  Options:\n");
	fprintf(stderr, "    -i, --input,       Input fasta/fastq file (required)\n");
	fprintf(stderr, "    -n, --names,       File of header id names to search for\n");
	fprintf(stderr, "    -N, --names_stdin, Use STDIN for header id names\n");
	fprintf(stderr, "    -g, --regex,       Regular expression to match (PERL compatible; always case-insensitive)\n");
	fprintf(stderr, "    -m, --min,         Minimum sequence length\n");
	fprintf(stderr, "    -a, --max,         Maximum sequence length\n");
	fprintf(stderr, "    -l, --length,      Sequence characters per line (default 50)\n");
	fprintf(stderr, "    -c, --convert,     Convert input to fastq/fasta (e.g. if input is fastq, output will be fasta)\n");
	fprintf(stderr, "    -q, --quality,     ASCII code to use for fasta->fastq quality conversions\n");
	fprintf(stderr, "    -e, --excluded,    Exclude the header id names in the list (-n)\n");
	fprintf(stderr, "    -t, --count,       Just count the possible output, but don't write it\n");
	fprintf(stderr, "    -h, --help,        Display this help and exit\n");
	fprintf(stderr, "    -v, --verbose,     Print extra details during the run\n");
	fprintf(stderr, "    --version,         Output version information and exit\n\n");

	exit(status);
}

int main(int argc, char *argv[]) {
	int c;
	char *in = NULL,*names = NULL;
	FILE *names_fp = NULL;
	int min = -1, max = -1;
	int names_from_stdin = 0;
	int exclude = 0;
	int count = 0;
	int just_count = 0; /* flag for just counting the output */
	int convert = 0;
	int length = 50;
	long value;
	char *end;
	pcre *reCompiled = NULL;
	pcre_extra *pcreExtra;
	const char *pcreErrorStr;
	int pcreErrorOffset;
	char *aStrRegex = NULL;

	extern char *optarg; /* external from getopt */

	verbose_flag = 0; /* assume not verbose */

	progname = argv[0];
	if (argc < 4) { /* progname + at least 3 other args */
		show_usage(EXIT_FAILURE);
	}

	while(1) {
		static struct option long_options[] =
		{
			{"verbose",     no_argument,       0, 'v'},
			{"convert",     no_argument,       0, 'c'},
			{"exclude",     no_argument,       0, 'e'},
			{"count",       no_argument,       0, 't'},
			{"version",     no_argument,       0, 'V'},
			{"help",        no_argument,       0, 'h'},
			{"input",       required_argument, 0, 'i'},
			{"regex",       required_argument, 0, 'g'},
			{"names",       required_argument, 0, 'n'},
			{"names_stdin", no_argument,       0, 'N'},
			{"min",         required_argument, 0, 'm'},
			{"max",         required_argument, 0, 'a'},
			{"length",      required_argument, 0, 'l'},
			{"quality",     required_argument, 0, 'q'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "Vvh?cetq:i:g:Nn:m:a:l:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'v':
				verbose_flag = 1;
				break;

			case 'i':
				in = (char*) malloc(strlen(optarg)+1);
				strcpy(in,optarg);
				break;

			case 'g':
				aStrRegex = (char*) malloc(strlen(optarg)+1);
				strcpy(aStrRegex, optarg);
				break;

			case 'n':
				names = (char*) malloc(strlen(optarg)+1);
				strcpy(names, optarg);
				break;

			case 'N':
				names_from_stdin = 1;
				break;

			case 'm':
				value = strtol(optarg, &end, 0);
				if (*end == '\0' && errno == 0) {
					min = atoi(optarg);
				} else {
					fprintf(stderr, "Maximum value (-m) argument '%s' is not an integer\n", optarg);
					return EXIT_FAILURE;
				}
				break;

			case 'a':
				value = strtol(optarg, &end, 0);
				if (*end == '\0' && errno == 0) {
					max = atoi(optarg);
				} else {
					fprintf(stderr, "Maximum value (-a) argument '%s' is not an integer\n", optarg);
					return EXIT_FAILURE;
				}
				break;

			case 'c':
				convert = 1;
				QUALITY_SCORE = 61;
				break;

			case 't':
				just_count = 1;
				break;

			case 'q':
				value = strtol(optarg, &end, 0);
				if (*end == 0 && errno == 0) {
					QUALITY_SCORE = atoi(optarg);
				} else {
					fprintf(stderr, "Quality ASCII value (-q) is invalid - must be an ASCII code (e.g. 73, which is 'I')\n");
					return EXIT_FAILURE;
				}
				break;

			case 'e':
				exclude = 1;
				break;

			case 'l':
				value = strtol(optarg, &end,0);
				if (*end == '\0' && errno == 0) {
					length = atoi(optarg);
				} else {
					fprintf(stderr, "Sequence length value (-l) argument '%s' is not an integer\n", optarg);
					return EXIT_FAILURE;
				}
				break;

			case 'V':
				/* version */
				printf("Version is %s\n", PULLSEQ_VERSION);
				break;

			case 'h':
				show_usage(EXIT_FAILURE);
				break;

			case '?':
				/* getopt_long already printed an error message. */
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
		fprintf(stderr,"Input is %s\n", in);
		if (convert)
			fprintf(stderr,"Input will be converted between FASTQ and FASTA\n");
		if (names_from_stdin) {
			if (exclude)
				fprintf(stderr,"Names in STDIN will be excluded\n");
			else
				fprintf(stderr,"Names in STDIN will be included\n");

		} else if (names != NULL) {
			if (exclude)
				fprintf(stderr,"Names in %s will be excluded\n", names);
			else
				fprintf(stderr,"Names in %s will be included\n", names);
		}
		if (aStrRegex)
			if (exclude)
				fprintf(stderr,"Only sequences not matching %s will be output\n", aStrRegex);
			else
				fprintf(stderr,"Only sequences matching %s will be output\n", aStrRegex);
		if (max > 0)
			fprintf(stderr,"Only sequences less than %i will be output\n", max);
		if (min > 0)
			fprintf(stderr,"Only sequences greater than %i will be output\n", min);
		if (length > 0)
			fprintf(stderr,"Output will be %i columns long\n", length);
		if (just_count > 0)
			fprintf(stderr,"Output will be counted only\n");
	}

	/* check validity of given argument set */
	if (!in) {
		fprintf (stderr, "Error: Input file is required.\n");
		return EXIT_FAILURE;
	}

	if (names) {
		if (!strcmp(in, names)) {
			fprintf (stderr, "Error: Input file is same as names file.\n");
			return EXIT_FAILURE;
		}
	}

	if (names && names_from_stdin) {
		fprintf (stderr, "Error: Cannot use names from STDIN *and* names from a file.\n");
		return EXIT_FAILURE;
	}

	if (aStrRegex) {
		if (names || names_from_stdin) {
			fprintf (stderr, "Error: You can't use a names file or names from STDIN and a regex match.\n");
			return EXIT_FAILURE;
		}
	}

	if (min > 0 && max > 0) {
		if (max <= min) {
			fprintf (stderr, "Error: Max is less than or equal to min.\n");
			return EXIT_FAILURE;
		}
	}

	/* if a regex search was requested, set up the pcre engine */
	if (aStrRegex) {
		// First, the regex string must be compiled. Support extended
		// regex strings (e.g. m//x)
		reCompiled = pcre_compile(aStrRegex, PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);

		if (reCompiled == NULL) {
			fprintf(stderr, "ERROR: Could not compile '%s': %s\n", aStrRegex, pcreErrorStr);
			exit(EXIT_FAILURE);
		} else {
			if (verbose_flag)
				fprintf(stderr, "Successfully compiled regex from string '%s'\n", aStrRegex);
		}

		// Optimize the regex
		pcreExtra = pcre_study(reCompiled, 0, &pcreErrorStr);

		/* pcre_study() returns NULL for both errors and when it can not optimize the regex.
		   The last argument is how one checks for errors (it is NULL if everything works,
		   and points to an error string otherwise. */
		if(pcreErrorStr != NULL) {
			printf("ERROR: Could not study regex string '%s': %s\n", aStrRegex, pcreErrorStr);
			exit(EXIT_FAILURE);
		}
	}

	if (names || names_from_stdin) {
		if (names) {
			names_fp = fopen(names,"r");
			if (!names_fp) {
				fprintf(stderr,"%s - failed to open names file %s\n",progname, names);
				exit(EXIT_FAILURE);
			}
		} else {
			names_fp = stdin;
		}
		count = pull_by_name(in, names_fp, min, max, length, exclude, convert, just_count);
	} else if (reCompiled) {
		count = pull_by_re(in, reCompiled, pcreExtra, min, max, length, exclude, convert, just_count);
	} else {
		count = pull_by_size(in, min, max, length, convert, just_count);
	}

	/* free up memory */
	free(in);

	if (names)
		free(names);

	if (names_fp)
		fclose(names_fp);

	if (aStrRegex) {
		free(aStrRegex);
		// Free up the regular expression.
		pcre_free(reCompiled);

		// Free up the EXTRA PCRE value (may be NULL at this point)
		if (pcreExtra != NULL)
			pcre_free(pcreExtra);
	}

	if (verbose_flag)
		fprintf(stderr,"Pulled %i entries\n",count);

	/* close streams */
	fclose(stderr);
	fclose(stdout);
	fclose(stdin);
	return EXIT_SUCCESS;
}
